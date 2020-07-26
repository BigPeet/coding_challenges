/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-15
 *
 */

#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#define NUM_OF_MOONS 4

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("This executabel takes exactly two arguments.\n");
        printf("Usage: aoc2019_12 FILE_PATH MAX_ITERATIONS.\n");
        return 0;
    }

    Moon** moons = read_moons(argv[1], NUM_OF_MOONS);
    if (moons == NULL)
    {
        printf("Error reading moons.\n");
        return 0;
    }

    /*Part 1*/
    Moon** copy        = copy_system(moons, NUM_OF_MOONS);
    int num_iterations = atoi(argv[2]);
    simulate_iterations(copy, NUM_OF_MOONS, num_iterations);

    int system_energy = total_system_energy(copy, NUM_OF_MOONS);
    printf("After simulating %d iterations, the total energy of the system is: "
           "%d.\n",
           num_iterations,
           system_energy);
    destroy_system(copy, NUM_OF_MOONS);

    /*Part 2*/
    /*Calculate the length of a repeating cycle for each dimension individually.*/
    /*Then find the least common multiple.*/
    int cycle_lengths[3];
    cycle_lengths[0] = steps_for_cycle(moons, NUM_OF_MOONS, 0);
    cycle_lengths[1] = steps_for_cycle(moons, NUM_OF_MOONS, 1);
    cycle_lengths[2] = steps_for_cycle(moons, NUM_OF_MOONS, 2);

    int64_t result = least_common_multiple(cycle_lengths, 3);
    printf("Steps to reach a previous state: %ld\n", result);

    /*clean up*/
    destroy_system(moons, NUM_OF_MOONS);

    return 0;
}
