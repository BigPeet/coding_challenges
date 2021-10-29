/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"

#define NUMBER_OF_NICS (50)

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_23 FILE_PATH.\n");
        return 0;
    }

    /*Initialize the program.*/
    intcode_t* prog = read_intcode(argv[1]);

    ASCII nics[NUMBER_OF_NICS];
    nics[0].brain = prog;

    /*Create 49 other copies*/
    for (int i = 0; i < NUMBER_OF_NICS; ++i)
    {
        if (i != 0)
        {
            nics[i].brain = copy_intcode(prog);
        }
        nics[i].finished = 0;

        /*Create IO memory*/
        intcode_io_mem_t* io_in  = create_io_mem();
        intcode_io_mem_t* io_out = create_io_mem();
        if ((nics[i].brain == NULL) || (io_in == NULL) || (io_out == NULL))
        {
            printf("Error reading programm or allocating IO memory\n");
            return 0;
        }
        set_io_mode(nics[i].brain, INT_CODE_MEM_IO);
        set_mem_io_in(nics[i].brain, io_in);
        set_mem_io_out(nics[i].brain, io_out);
    }

    // Start NIC threads
    pthread_t nic_threads[NUMBER_OF_NICS];
    for (int i = 0; i < NUMBER_OF_NICS; ++i)
    {
        nic_threads[i] = 0;
        pthread_create(&nic_threads[i], NULL, nic_func, &nics[i]);
    }

    /*Wait for threads to finish.*/
    for (int i = 0; i < NUMBER_OF_NICS; ++i)
    {
        pthread_join(nic_threads[i], NULL);
    }


    /*Clean Up*/
    for (int i = 0; i < NUMBER_OF_NICS; ++i)
    {
        destroy_intcode(nics[i].brain);
    }

    return 0;
}
