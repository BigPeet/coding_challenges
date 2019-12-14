/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-12
 *
 */

#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

void read_input_numbers(const int argc, char** argv, int* input)
{
    for (int i = 0; i < argc; i++)
    {
        input[i] = atoi(argv[i]);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("This executabel takes exactly three arguments.\n");
        printf("Usage: aoc2019_10 FILE_PATH HEIGHT WIDTH.\n");
        return 0;
    }

    int amount_of_numbers = argc - 2;
    int dimensions[amount_of_numbers];
    read_input_numbers(amount_of_numbers, argv + 2, dimensions);

    int height = dimensions[0];
    int width  = dimensions[1];

    int data[height][width];
    Map map = {.height = height, .width = width, .data = &data[0][0]};
    read_map(argv[1], &map);

    print_map(&map);

    Point best;
    int max = get_max_visible_asteroids(&map, &best);

    printf("Maximal visible asteroids %d from (%d, %d)\n", max, best.x, best.y);

    fire_laser(&map, &best);

    /*Point solution = {.x = 5, .y = 8};*/
    /*int count = count_visible_asteroids(&solution, &map);*/
    /*printf("%d visible asteroids from (%d, %d)\n", count, solution.x, solution.y);*/

    /*Point debug = {.x = 3, .y = 4};*/
    /*int blocked[map.height][map.width];*/
    /*get_blocked_map(&map, &debug, &blocked[0][0]);*/
    /*printf("Blocked:\n");*/
    /*for (int row = 0; row < map.height; ++row)*/
    /*{*/
        /*for (int col = 0; col < map.width; col++)*/
        /*{*/
            /*printf("%d ", blocked[row][col]);*/
        /*}*/
        /*printf("\n");*/
    /*}*/


    /*int count = count_visible_asteroids(&debug, &map);*/
    /*printf("%d visible asteroids from (%d, %d)\n", count, debug.x, debug.y);*/


    return 0;
}
