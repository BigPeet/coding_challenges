/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-09
 *
 */

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"
#include "pthread.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#define BUFFER_LENGTH (size_t)(32)

typedef struct
{
    int phase;
    intcode_io_mem_t* io_mem;
} phase_thread_args;

typedef struct
{
    intcode_io_mem_t** io_mems;
    int* phases;
    size_t k;
} input_thread_args;

void* input_phases(void* args)
{
    phase_thread_args* thread_args = (phase_thread_args*)args;
    int phase                      = thread_args->phase;
    intcode_io_mem_t* io_mem       = thread_args->io_mem;

    printf("Thread for phases input started. Distributing phase val %d\n", phase);

    for (;;)
    {
        pthread_mutex_lock(&io_mem->mut);
        while (io_mem->consumed != 0)
        {
            /*spin*/
            pthread_cond_signal(&io_mem->cond);
            pthread_cond_wait(&io_mem->cond, &io_mem->mut);
        }

        printf("Set phase: %d\n", phase);
        io_mem->value    = phase;
        io_mem->consumed = 3;
        pthread_cond_signal(&io_mem->cond);
        pthread_mutex_unlock(&io_mem->mut);
    }
    return NULL;
}

void* input_thread(void* args)
{
    input_thread_args* thread_args = (input_thread_args*)args;
    size_t k                       = thread_args->k;
    intcode_io_mem_t** io_mems     = thread_args->io_mems;
    int* phases                    = thread_args->phases;
    pthread_t threads[k];

    /*Initial input*/
    pthread_mutex_lock(&io_mems[0]->mut);
    io_mems[0]->value    = phases[0];
    io_mems[0]->consumed = 3;
    pthread_cond_signal(&io_mems[0]->cond);
    pthread_mutex_unlock(&io_mems[0]->mut);

    printf("Set initial phase value to %d\n", io_mems[0]->value);

    pthread_mutex_lock(&io_mems[0]->mut);
    while (io_mems[0]->consumed != 2)
    {
        /*spin*/
        pthread_cond_signal(&io_mems[0]->cond);
        pthread_cond_wait(&io_mems[0]->cond, &io_mems[0]->mut);
    }

    printf("Set initial input value to %d\n", 0);
    io_mems[0]->value    = 0;
    io_mems[0]->consumed = 1;
    pthread_cond_signal(&io_mems[0]->cond);
    pthread_mutex_unlock(&io_mems[0]->mut);

    phase_thread_args ptas[k];

    for (size_t i = 0; i < k; i++)
    {
        ptas[i].io_mem = io_mems[i];
        ptas[i].phase  = phases[i];
        pthread_create(&threads[i], NULL, input_phases, &ptas[i]);
    }
    for (size_t i = 0; i < k; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return NULL;
}

void* compute_thread(void* args)
{
    intcode_t* prog = (intcode_t*)args;

    int ret = execute(prog);
    if (ret == INT_CODE_HALT)
    {
        printf("Programm halted.\n");
    }
    else
    {
        printf("Programm did not halt as expected. Err code: %d\n", ret);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_07 FILE_PATH.\n");
        return 0;
    }

    /*TODO: check return values of the function calls here */

    size_t num_amplifiers = 2;
    size_t offset         = 5;

    intcode_t* amplifier_progs[num_amplifiers];
    intcode_io_mem_t* amp_io_mems[num_amplifiers];

    for (size_t i = 0; i < num_amplifiers; i++)
    {
        amp_io_mems[i] = (intcode_io_mem_t*)malloc(sizeof(intcode_io_mem_t));
        pthread_mutex_init(&amp_io_mems[i]->mut, NULL);
        pthread_cond_init(&amp_io_mems[i]->cond, NULL);
    }

    intcode_t* prog = read_intcode(argv[1]);
    if (prog == NULL)
    {
        return 0;
    }

    size_t num_perms = 0;
    int** perms      = create_permutations_with_offset(num_amplifiers, offset, &num_perms);

    if (perms == NULL)
    {
        return 0;
    }

    int max_output = -1; /*should be min-int*/

    /*for (int j = 0; j < num_perms; j++)*/
    for (int j = 0; j < 1; j++)
    {
        int* phases = perms[j];

        for (size_t i = 0; i < num_amplifiers; i++)
        {
            printf("%d ", phases[i]);
        }
        printf("\n");

        /*Setup intcodes and memory for all amplifiers*/
        for (size_t i = 0; i < num_amplifiers; ++i)
        {
            /*Create copy*/
            intcode_t* copy = copy_intcode(prog);
            if (copy == NULL)
            {
                return 0;
            }

            amplifier_progs[i] = copy;
            set_io_mode(copy, INT_CODE_MEM_IO);

            amp_io_mems[i]->value    = 0;
            amp_io_mems[i]->consumed = 0;
            set_mem_io_in(copy, amp_io_mems[i]);

            if (i < (num_amplifiers - 1))
            {
                amp_io_mems[i + 1]->value    = 0;
                amp_io_mems[i + 1]->consumed = 0;
                set_mem_io_out(copy, amp_io_mems[i + 1]);
            }
            else
            {
                set_mem_io_out(copy, amp_io_mems[0]);
            }
        }


        int input = 0; /*as described by task*/

        /*One thread to provide the phases input and one thread for each amplifier.*/
        pthread_t init_thread;
        input_thread_args ita;
        ita.io_mems = amp_io_mems;
        ita.k       = num_amplifiers;
        ita.phases  = phases;
        pthread_create(&init_thread, NULL, input_thread, &ita);

        pthread_t compute_threads[num_amplifiers];
        for (size_t i = 0; i < num_amplifiers; i++)
        {
            pthread_create(&compute_threads[i], NULL, compute_thread, amplifier_progs[i]);
        }

        for (size_t i = 0; i < num_amplifiers; i++)
        {
            pthread_join(compute_threads[i], NULL);
        }

        printf("All compute threads have finished!\n");

        /*if (max_output < input)*/
        /*{*/
        /*max_output = input;*/
        /*}*/
    }

    printf("Max output: %d\n", max_output);

    for (size_t i = 0; i < num_amplifiers; ++i)
    {
        destroy_intcode(amplifier_progs[i]);
    }

    for (size_t i = 0; i < num_amplifiers + 1; ++i)
    {
        pthread_mutex_destroy(&amp_io_mems[i]->mut);
        pthread_cond_destroy(&amp_io_mems[i]->cond);
        free(amp_io_mems[i]);
    }

    destroy_permutations(perms, num_perms);
    destroy_intcode(prog);
    return 0;
}
