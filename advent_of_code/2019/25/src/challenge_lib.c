/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"

#define MAX_COMMAND_LENGTH (100)

static int64_t read_output(const ASCII* const system)
{
    assert(system != NULL);
    assert(system->brain != NULL);
    assert(system->brain->mem_io_out != NULL);

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


static void read_prompt(const ASCII* const system)
{
    /*Only reading access*/
    while (!waiting_for_input(system->brain) && !system->finished && providing_ouput(system->brain))
    {
        /*Read Response*/
        int64_t resp = read_output(system);
        printf("%c", (char) resp);
    }
}

static void provide_line(const ASCII* const system, const char* const line)
{
    assert(system != NULL);
    assert(system->brain != NULL);
    assert(line != NULL);
    for (int i = 0; i < strlen(line); ++i)
    {
        provide_input(system, (int64_t) line[i]);
    }
}

static void run_interactive_program(const ASCII* const system)
{
    /*Prompt and Command loop*/
    char cmd_buffer[MAX_COMMAND_LENGTH];
    while (!system->finished)
    {
        if (providing_ouput(system->brain))
        {
            read_prompt(system);
        }
        else if (waiting_for_input(system->brain))
        {
            fgets(cmd_buffer, MAX_COMMAND_LENGTH, stdin);
            provide_line(system, cmd_buffer);
        }
    }
}

static void run_hardcoded_commands(const ASCII* const system, char const* command_file)
{
    FILE* fp = fopen(command_file, "r");
    if (fp)
    {
        /*Prompt and Command loop*/
        char cmd_buffer[MAX_COMMAND_LENGTH];
        while (!system->finished)
        {
            if (providing_ouput(system->brain))
            {
                read_prompt(system);
            }
            else if (waiting_for_input(system->brain))
            {
                fgets(cmd_buffer, MAX_COMMAND_LENGTH, fp);
                provide_line(system, cmd_buffer);
            }
        }
        fclose(fp);
    }
}


void* drone_func(void* args)
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
        /*Only writing access*/
        usleep(1000);
        system->finished = 1;
    }
    return NULL;
}

void* control_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    ControlParams* control_params = (ControlParams*) args;

    /*Give robot some time to setup.*/
    usleep(3000);

    if (control_params->interactive)
    {
        run_interactive_program(control_params->drone);
    }
    else
    {
        run_hardcoded_commands(control_params->drone, control_params->command_file);
    }
    return NULL;
}
