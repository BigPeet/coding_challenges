/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-18
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
        printf("Usage: aoc2019_17 FILE_PATH.\n");
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

    /*Setup area, overview and robot with intial size and values.*/
    int* area          = (int*)calloc(INITIAL_HEIGHT, INITIAL_WIDTH * sizeof(int));
    Robot* robot       = (Robot*)malloc(sizeof(Robot));
    ASCII* system      = (ASCII*)malloc(sizeof(ASCII));
    Overview* overview = (Overview*)malloc(sizeof(Overview));
    if ((area == NULL) || (robot == NULL) || (overview == NULL) || (system == NULL))
    {
        printf("Error allocating memory for area, overview or robot.\n");
        return 0;
    }

    /*Initialize everything*/
    system->brain    = prog;
    system->finished = 0;

    overview->height     = INITIAL_HEIGHT;
    overview->width      = INITIAL_WIDTH;
    overview->area       = area;
    overview->system     = system;
    overview->robot      = robot;
    overview->output_pos = (Position){.x = 0, .y = 0};

    /*Start robot and control threads*/
    pthread_t system_thread;
    pthread_create(&system_thread, NULL, system_func, system);

    pthread_t control_thread;
    pthread_create(&control_thread, NULL, control_func, overview);


    /*Wait for threads to finish.*/
    pthread_join(control_thread, NULL);
    pthread_join(system_thread, NULL);

    /*Print results*/

    /*Clean up*/
    destroy_io_mem(io_in);
    destroy_io_mem(io_out);
    destroy_intcode(prog);
    if (robot != NULL)
    {
        free(robot);
    }
    if (system != NULL)
    {
        free(system);
    }
    if (overview->area != NULL)
    {
        free(overview->area);
    }
    if (overview != NULL)
    {
        free(overview);
    }
    return 0;
}
