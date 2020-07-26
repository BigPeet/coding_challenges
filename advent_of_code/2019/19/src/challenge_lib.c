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

static int64_t provide_coordinate(const intcode_t* const prog, const int x, const int y)
{
    int64_t resp = -1;
    while (!waiting_for_input(prog))
    {
        usleep(10);
    }
    provide_input(prog, x);
    while (!waiting_for_input(prog))
    {
        usleep(10);
    }
    provide_input(prog, y);
    resp = read_output(prog);
    return resp;
}

void* system_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    intcode_t* prog = (intcode_t*) args;
    int ret         = execute(prog);
    if (ret != INT_CODE_HALT)
    {
        printf("Programm did not halt as expected. Err code: %d\n", ret);
    }
    return NULL;
}

int64_t scan_coordinate(const intcode_t* const prog, const int x, const int y)
{
    int64_t result = -1;
    if (prog == NULL)
    {
        return result;
    }

    intcode_t* exe_code      = copy_intcode(prog);
    intcode_io_mem_t* io_in  = create_io_mem();
    intcode_io_mem_t* io_out = create_io_mem();
    set_io_mode(exe_code, INT_CODE_MEM_IO);
    set_mem_io_in(exe_code, io_in);
    set_mem_io_out(exe_code, io_out);

    if ((exe_code == NULL) || (io_in == NULL) || (io_out == NULL))
    {
        printf("Error reading programm or allocating IO memory\n");
        return -1;
    }

    pthread_t system_thread;
    pthread_create(&system_thread, NULL, system_func, exe_code);

    result = provide_coordinate(exe_code, x, y);

    /*Wait for threads to finish.*/
    pthread_join(system_thread, NULL);

    destroy_io_mem(io_in);
    destroy_io_mem(io_out);
    destroy_intcode(exe_code);

    return result;
}
