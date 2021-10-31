/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "assert.h"
#include "challenge/intcode.h"
#include "stdio.h"
#include "unistd.h"

#include "challenge/challenge_lib.h"
#include "challenge/packet.h"
#include "challenge/queue.h"

#define NAT_ADDRESS (255)
#define IDLE_ROUNDS (5)

static void provide_input(const ASCII* const system, const int64_t value)
{
    assert(system != NULL);
    assert(system->brain != NULL);
    assert(system->brain->mem_io_in != NULL);

    intcode_io_mem_t* input = system->brain->mem_io_in;
    pthread_mutex_lock(&input->mut);
    while (!input->consumed && !system->finished)
    {
        pthread_cond_wait(&input->cond, &input->mut);
    }
    input->value    = value;
    input->consumed = 0;
    pthread_cond_signal(&input->cond);
    pthread_mutex_unlock(&input->mut);
    usleep(10);
}

static int64_t read_output(const ASCII* const system)
{
    assert(robot != NULL);
    assert(robot->brain != NULL);
    assert(robot->brain->mem_io_out != NULL);

    int64_t read_value = -1;

    intcode_io_mem_t* output = system->brain->mem_io_out;
    pthread_mutex_lock(&output->mut);
    while (output->consumed && !system->finished)
    {
        pthread_cond_wait(&output->cond, &output->mut);
    }

    if (!system->finished)
    {
        read_value       = output->value;
        output->consumed = 1;
    }
    pthread_cond_signal(&output->cond);
    pthread_mutex_unlock(&output->mut);


    return read_value;
}

static void assign_address(ASCII* nic, const int address)
{
    assert(system != NULL);
    assert(system->brain != NULL);
    while (!waiting_for_input(nic->brain))
    {
        usleep(10);
    }
    provide_input(nic, address);
}

static void start_up(ControllerParams* params)
{
    for (int i = 0; i < params->num_of_nics; ++i)
    {
        assign_address(&params->nics[i], i);
    }
}

static Packet read_packet(const ASCII* const nic_system)
{
    int64_t read_x = read_output(nic_system);
    int64_t read_y = read_output(nic_system);
    Packet ret     = {.x = read_x, .y = read_y};
    return ret;
}

static void send_packet(const ASCII* const nic_system, Packet p)
{
    provide_input(nic_system, p.x);
    provide_input(nic_system, p.y);
}

static void send_receive_phase(ControllerParams* params, PacketQueue* queues)
{
    Packet nat_packet        = {0, 0};
    int64_t last_nat_value   = 0;
    int all_systems_finished = 0;
    int idle_count           = 0;
    while (!all_systems_finished)
    {
        all_systems_finished    = 1;
        int no_output_available = 1;
        int all_queues_empty    = 1;
        for (int i = 0; i < params->num_of_nics; ++i)
        {
            all_systems_finished = all_systems_finished && params->nics[i].finished;
            all_queues_empty     = all_queues_empty && queue_is_empty(&queues[i]);
            if (providing_ouput(params->nics[i].brain))
            {
                no_output_available = 0;
                /*Read address and packet, then add packet to queue for receiver*/
                int64_t address = read_output(&params->nics[i]);
                Packet packet   = read_packet(&params->nics[i]);
                if (address == NAT_ADDRESS)
                {
                    /*Received packet for NAT*/
                    nat_packet = packet;

                    /*Enable for part 1*/
                    /*printf("Packet for address 255: (X=%ld, Y=%ld).\n", packet.x, packet.y);*/
                    /*return;*/
                }
                else
                {
                    assert(address < params->num_of_nics);
                    queue_push(&queues[address], packet);
                }
            }
            if (waiting_for_input(params->nics[i].brain))
            {
                if (queue_is_empty(&queues[i]))
                {
                    provide_input(&params->nics[i], -1);
                }
                else
                {
                    Packet p = queue_pop(&queues[i]);
                    send_packet(&params->nics[i], p);
                }
            }
        }

        if (no_output_available && all_queues_empty)
        {
            idle_count++;
            if (idle_count == IDLE_ROUNDS)
            {
                // Send NAT packet to 0
                printf("Send NAT Package (X=%ld, Y=%ld) to address 0.\n",
                       nat_packet.x,
                       nat_packet.y);
                send_packet(&params->nics[0], nat_packet);
                idle_count = 0;

                /*Part 2 output*/
                if (nat_packet.y == last_nat_value)
                {
                    printf("The following value was sent twice in a row: %ld\n", last_nat_value);
                }
                last_nat_value = nat_packet.y;
            }
        }
        else
        {
            idle_count = 0;
        }
    }
}

void* nic_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    ASCII* system = (ASCII*) args;
    int ret       = execute(system->brain);
    if (ret != INT_CODE_HALT)
    {
        printf("Programm did not halt as expected. Err code: %d\n", ret);
    }
    else
    {
        usleep(1000);
        system->finished = 1;
    }
    return NULL;
}

void* nat_control(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    ControllerParams* params = (ControllerParams*) args;
    /*Give robot some time to setup.*/
    usleep(3000);

    /*Setup packet queues*/
    PacketQueue queues[params->num_of_nics];
    for (int i = 0; i < params->num_of_nics; i++)
    {
        queues[i] = queue_create();
    }

    /*Assign addresses*/
    start_up(params);

    /*Give robot some time to setup.*/
    usleep(3000);

    /*Send and receive packages*/
    send_receive_phase(params, queues);

    /*Clean up*/
    for (int i = 0; i < params->num_of_nics; i++)
    {
        queue_destroy(&queues[i]);
    }

    return NULL;
}
