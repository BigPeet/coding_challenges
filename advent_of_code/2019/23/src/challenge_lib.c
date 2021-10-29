/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "assert.h"
#include "unistd.h"
#include "stdio.h"

#include "challenge/challenge_lib.h"

void* nic_func(void* args)
{
    printf("Entered nic func\n");
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
        /*Only writing access*/
        usleep(1000);
        system->finished = 1;
    }
    return NULL;
}

void* control_part01(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    ASCII* system    = (ASCII*) args;

    /*Give robot some time to setup.*/
    usleep(3000);

    return NULL;
}

static void provide_input(const ASCII* const system, const int value)
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
