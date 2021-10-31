/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "pthread.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"

#define DEFAULT_INTERACTIVE (1)

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("This executabel takes at least one argument.\n");
        printf("Usage: aoc2019_21 FILE_PATH [INTERACTIVE_INDICATOR].\n");
        return 0;
    }

    int interactive = DEFAULT_INTERACTIVE;
    if (argc >= 3)
    {
        interactive = (int) strtoll(argv[2], NULL, 10);
    }

    /*Initialize the program with IO memory.*/
    intcode_t* prog          = read_intcode(argv[1]);
    intcode_io_mem_t* io_in  = create_io_mem();
    intcode_io_mem_t* io_out = create_io_mem();
    if ((prog == NULL) || (io_in == NULL) || (io_out == NULL))
    {
        printf("Error reading programm or allocating IO memory\n");
        return 0;
    }

    set_io_mode(prog, INT_CODE_MEM_IO);
    set_mem_io_in(prog, io_in);
    set_mem_io_out(prog, io_out);

    /*Setup threads etc.*/
    ASCII drone = {.brain = prog, .finished = 0};

    pthread_t drone_thread   = 0;
    pthread_t control_thread = 0;
    pthread_create(&drone_thread, NULL, drone_func, &drone);
    pthread_create(&control_thread, NULL, control_func, &drone);

    /*Wait for threads to finish.*/
    pthread_join(drone_thread, NULL);
    pthread_join(control_thread, NULL);

    /*Clean Up*/
    destroy_intcode(prog);

    return 0;
}
