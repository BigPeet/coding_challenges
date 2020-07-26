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

#define BUFFER_LENGTH (size_t)(32)

void* input_thread(void* args)
{
    intcode_io_mem_t* io_mem = (intcode_io_mem_t*) args;

    /*Initial input*/
    pthread_mutex_lock(&io_mem->mut);
    while (!io_mem->consumed)
    {
        pthread_cond_wait(&io_mem->cond, &io_mem->mut);
    }
    io_mem->value    = 0;
    io_mem->consumed = 0;
    pthread_cond_signal(&io_mem->cond);
    pthread_mutex_unlock(&io_mem->mut);

    return NULL;
}

void* compute_thread(void* args)
{
    intcode_t* prog = (intcode_t*) args;

    int ret = execute(prog);
    if (ret != INT_CODE_HALT)
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

    size_t num_amplifiers = 5;
    size_t offset         = 5;

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
    int* max_phases;

    for (int j = 0; j < num_perms; j++)
    {
        int* phases = perms[j];

        intcode_t** amplifier_progs = (intcode_t**) malloc(sizeof(intcode_t*) * num_amplifiers);
        intcode_io_mem_t** amp_io_mems =
            (intcode_io_mem_t**) malloc(sizeof(intcode_io_mem_t*) * num_amplifiers);

        for (size_t i = 0; i < num_amplifiers; i++)
        {
            amp_io_mems[i] = (intcode_io_mem_t*) malloc(sizeof(intcode_io_mem_t));
            pthread_mutex_init(&amp_io_mems[i]->mut, NULL);
            pthread_cond_init(&amp_io_mems[i]->cond, NULL);
        }

        /*Setup intcodes and memory for all amplifiers*/
        for (size_t i = 0; i < num_amplifiers; ++i)
        {
            /*Create copy*/
            intcode_t* copy = copy_intcode(prog);
            if (copy == NULL)
            {
                return 0;
            }

            set_io_mode(copy, INT_CODE_MEM_IO);

            amp_io_mems[i]->value    = phases[i];
            amp_io_mems[i]->consumed = 0;
            set_mem_io_in(copy, amp_io_mems[i]);

            if (i < (num_amplifiers - 1))
            {
                set_mem_io_out(copy, amp_io_mems[i + 1]);
            }
            else
            {
                set_mem_io_out(copy, amp_io_mems[0]);
            }
            amplifier_progs[i] = copy;
        }

        /*One thread to provide the initial input and one thread for each
         * amplifier.*/
        pthread_t init_thread;
        pthread_create(&init_thread, NULL, input_thread, amp_io_mems[0]);

        pthread_t compute_threads[num_amplifiers];
        for (size_t i = 0; i < num_amplifiers; i++)
        {
            pthread_create(&compute_threads[i], NULL, compute_thread, amplifier_progs[i]);
        }

        pthread_join(init_thread, NULL);
        for (size_t i = 0; i < num_amplifiers; i++)
        {
            pthread_join(compute_threads[i], NULL);
        }

        int program_output = amp_io_mems[0]->value;

        if (max_output < program_output)
        {
            max_output = program_output;
            max_phases = phases;
        }

        /*Clean up*/
        for (size_t i = 0; i < num_amplifiers; i++)
        {
            pthread_cond_destroy(&amp_io_mems[i]->cond);
            pthread_mutex_destroy(&amp_io_mems[i]->mut);
            free(amp_io_mems[i]);
            destroy_intcode(amplifier_progs[i]);
        }
        free(amp_io_mems);
        free(amplifier_progs);
    }

    printf("Max output: %d\n", max_output);
    if (max_phases != NULL)
    {
        printf("Permutation: ");
        for (size_t i = 0; i < num_amplifiers; i++)
        {
            printf("%d ", max_phases[i]);
        }
        printf("\n");
    }

    destroy_permutations(perms, num_perms);
    destroy_intcode(prog);
    return 0;
}
