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

    Map* map = read_map(argv[1], height, width);
    if (map == NULL)
    {
        printf("Reading map failed.\n");
        return 0;
    }
    print_map(map);

    Point best;
    int max = get_max_visible_asteroids(map, &best);
    printf("Maximal visible asteroids %d from (%d, %d)\n", max, best.x, best.y);

    fire_laser(map, &best);

    destroy_map(map);

    return 0;
}
