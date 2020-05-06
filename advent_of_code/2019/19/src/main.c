/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-05-06
 *
 */

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"
#include "pthread.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

/*Give the robot a starting area*/
/*area is resized dynamically*/
#define INITIAL_HEIGHT 25
#define INITIAL_WIDTH 25


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_19 FILE_PATH.\n");
        return 0;
    }

    /*Initialize the program with IO memory.*/
    intcode_t* prog          = read_intcode(argv[1]);
    intcode_io_mem_t* io_in  = create_io_mem();
    intcode_io_mem_t* io_out = create_io_mem();
    set_io_mode(prog, INT_CODE_MEM_IO);
    set_mem_io_in(prog, io_in);
    set_mem_io_out(prog, io_out);

    if ((prog == NULL) || (io_in == NULL) || (io_out == NULL))
    {
        printf("Error reading programm or allocating IO memory\n");
        return 0;
    }

    pthread_t system_thread;
    pthread_create(&system_thread, NULL, system_func, prog);

    pthread_t control_thread;
    pthread_create(&control_thread, NULL, control_func, prog);

    /*Wait for threads to finish.*/
    pthread_join(control_thread, NULL);
    pthread_join(system_thread, NULL);

    /*Clean up*/
    destroy_io_mem(io_in);
    destroy_io_mem(io_out);
    destroy_intcode(prog);
    return 0;
}
