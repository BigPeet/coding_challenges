/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-05-07
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"

#define RESIZE_AMOUNT 10

static void provide_input(const ASCII* const system, const int value);
static int64_t read_output(const ASCII* const system);
static void read_prompt(const ASCII* const system);
static void provide_line(const ASCII* const system, const char* const line);

void* system_func(void* args)
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
    ASCII* system    = (ASCII*) args;

    /*Give robot some time to setup.*/
    usleep(3000);

    read_prompt(system);

    /*Robot is expecting input instructions.*/
    /*Part 01: Jump if there is a hole in A, B or C, but not in D*/
    provide_line(system, "NOT A J\n");
    provide_line(system, "NOT B T\n");
    provide_line(system, "OR T J\n");
    provide_line(system, "NOT C T\n");
    provide_line(system, "OR T J\n");
    provide_line(system, "AND D J\n");
    provide_line(system, "WALK\n");

    read_prompt(system);
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

static void read_prompt(const ASCII* const system)
{
    /*Only reading access*/
    while (!waiting_for_input(system->brain) && !system->finished)
    {
        /*Read Response*/
        int64_t resp = read_output(system);
        if (resp < 150)
        {
          printf("%c", (char)resp);
        }
        else
        {
          printf("%ld\n", resp);
        }
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
