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

/*Starting size for the game*/
/*will be resized dynamically*/
#define INITIAL_HEIGHT 25
#define INITIAL_WIDTH 25


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_13 FILE_PATH.\n");
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

    /*Setup area with intial size and values.*/
    Game* game = create_game(INITIAL_HEIGHT, INITIAL_WIDTH);
    if (game == NULL)
    {
        printf("Error allocating memory for game.\n");
        return 0;
    }

    /*Setup builder*/
    Builder builder;
    builder.brain = prog;
    builder.finished = 0;
    game->builder = &builder;

    /*Start builder and game threads*/
    pthread_t builder_thread;
    pthread_create(&builder_thread, NULL, builder_func, &builder);

    pthread_t game_thread;
    pthread_create(&game_thread, NULL, game_func, game);


    /*Wait for threads to finish.*/
    pthread_join(builder_thread, NULL);
    pthread_join(game_thread, NULL);

    /*Print results*/
    int total_blocks = count_tiles(game, BLOCK);
    printf("Number of Block tiles: %d\n", total_blocks);

    /*Clean up*/
    destroy_io_mem(io_in);
    destroy_io_mem(io_out);
    destroy_intcode(prog);
    destroy_game(game);
    return 0;
}
