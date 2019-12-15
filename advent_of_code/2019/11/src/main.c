/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-15
 *
 */

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"
#include "pthread.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

/*Give the robot a (hopefully) large enough starting field*/
/*Might want to resize dynamically, if necessary*/
#define INITIAL_HEIGHT 101
#define INITIAL_WIDTH  101


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_11 FILE_PATH.\n");
        return 0;
    }

    /*Initialize the program with IO memory.*/
    intcode_t* prog = read_intcode(argv[1]);
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
    int* hull    = (int*) calloc(INITIAL_HEIGHT, INITIAL_WIDTH * sizeof(int));
    Robot* robot = (Robot*) malloc(sizeof(Robot));
    Overview* overview = (Overview*) malloc(sizeof(Overview));
    if ((hull == NULL) || (robot == NULL) || (overview == NULL))
    {
        printf("Error allocating memory for hull, overview or robot.\n");
        return 0;
    }

    /*TODO using the stack might be an issue here*/
    Position starting_pos = {.x = INITIAL_WIDTH / 2, .y = INITIAL_HEIGHT / 2};
    robot->direction = UP;
    robot->finished  = 0;
    robot->brain     = prog;
    robot->pos       = starting_pos;
    overview->height = INITIAL_HEIGHT;
    overview->width  = INITIAL_WIDTH;
    overview->hull   = hull;
    overview->robot  = robot;

    /*Start robot and control threads*/
    pthread_t robot_thread;
    pthread_create(&robot_thread, NULL, robot_func, robot);

    pthread_t control_thread;
    pthread_create(&control_thread, NULL, control_func, overview);


    /*Wait for threads to finish.*/
    pthread_join(robot_thread, NULL);
    pthread_join(control_thread, NULL);

    int total_painted_once = count_painted_fields(overview);
    printf("Fields painted at least once: %d\n", total_painted_once);


    /*TODO additional clean up*/

    destroy_io_mem(io_in);
    destroy_io_mem(io_out);
    destroy_intcode(prog);


    return 0;
}
