/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-19
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
    if (argc != 6)
    {
        printf("This executabel takes exactly 4 arguments.\n");
        printf("Usage: aoc2019_18 FILE_PATH HEIGHT WIDTH NUM_KEYS NUM_DOORS.\n");
        return 0;
    }

    int amount_of_numbers = argc - 2;
    int numbers[amount_of_numbers];
    read_input_numbers(amount_of_numbers, argv + 2, numbers);

    Overview* overview = read_input(argv[1], numbers[0], numbers[1], numbers[2], numbers[3]);
    if (overview == NULL)
    {
        return 0;
    }
    printf("Overview size: %d, %d. Keys: %d\n",
           overview->map->width,
           overview->map->height,
           overview->num_keys);

    print_map(overview->map);

    int step_count = minimal_steps(overview);
    printf("Minimal step count for all keys: %d\n", step_count);

    destroy_overview(overview);
    return 0;
}
