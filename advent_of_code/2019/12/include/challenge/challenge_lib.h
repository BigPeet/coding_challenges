/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-15
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#define DIMENSIONS 3

#include "stdint.h"

typedef struct
{
    int position[DIMENSIONS];
    int velocity[DIMENSIONS];
} Moon;


Moon** read_moons(const char* const file_path, const int amount);
Moon** copy_system(Moon** moons, const int num_moons);
void destroy_system(Moon** const moons, const int num_moons);
void destroy_moon(Moon* const moon);
void print_moon(const Moon* const moon);
void simulate_iterations(Moon** const moons,
                         const int num_moons,
                         const int num_iterations);
int steps_for_cycle(Moon** moons, const int num_moons, const int dim);

int64_t least_common_multiple(const int* const cycle_lengths, const int amount);

int potential_energy(const Moon* const moon);
int kinetic_energy(const Moon* const moon);
int total_energy(const Moon* const moon);
int total_system_energy(Moon** moons, const int num_moons);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
