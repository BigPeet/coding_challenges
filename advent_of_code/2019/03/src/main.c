/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-03
 *
 */

#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_03 FILE_PATH.\n");
        return 0;
    }

    size_t num_wires = 0;
    wire_t** wires   = read_wires(argv[1], &num_wires);
    if (wires != NULL)
    {
        if (num_wires == 2)
        {
            point_t pos;
            int distance = intersect_wires_manhattan(wires[0], wires[1], &pos);
            printf("Manhattan Distance: %d\n", distance);
            distance = intersect_wires_steps(wires[0], wires[1], &pos);
            printf("Steps Distance: %d\n", distance);
        }

        /*Clean up*/
        for (size_t i = 0; i < num_wires; i++)
        {
            destroy_wire(wires[i]);
        }
        free(wires);
    }
    return 0;
}
