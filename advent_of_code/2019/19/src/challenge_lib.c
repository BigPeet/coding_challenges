/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-05-06
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"

static void provide_input(const intcode_t* const prog, const int value)
{
    assert(prog != NULL);
    assert(prog->mem_io_in != NULL);

    intcode_io_mem_t* input = prog->mem_io_in;
    pthread_mutex_lock(&input->mut);
    while (!input->consumed)
    {
        pthread_cond_wait(&input->cond, &input->mut);
    }
    input->value    = value;
    input->consumed = 0;
    pthread_cond_signal(&input->cond);
    pthread_mutex_unlock(&input->mut);
    usleep(10);
}

static int64_t read_output(const intcode_t* const prog)
{
    assert(prog != NULL);
    assert(prog->mem_io_out != NULL);

    int64_t read_value = -1;

    intcode_io_mem_t* output = prog->mem_io_out;
    pthread_mutex_lock(&output->mut);
    while (output->consumed)
    {
        pthread_cond_wait(&output->cond, &output->mut);
    }

    read_value       = output->value;
    output->consumed = 1;
    pthread_cond_signal(&output->cond);
    pthread_mutex_unlock(&output->mut);

    return read_value;
}

void* system_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    intcode_t* prog = (intcode_t*) args;
    int ret       = execute(prog);
    if (ret != INT_CODE_HALT)
    {
        printf("Programm did not halt as expected. Err code: %d\n", ret);
    }
    return NULL;
}

void* control_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    intcode_t* prog    = (intcode_t*) args;

    /*Only reading access*/
    while (!waiting_for_input(prog))
    {
        /*Read Response*/
        int64_t resp = read_output(prog);
        printf("Response: %d\n", resp);
        usleep(10);
    }
    provide_input(prog, -1);
    return NULL;
}
