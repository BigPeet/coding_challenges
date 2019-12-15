/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-15
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/*Assuming each line is less than 100 characters*/
#define LINE_BUFFER_LENGTH 100
#define COORD_DELIM ","

static void step(Moon** const moons, const int num_moons, const int dim);
static void update_velocities(Moon** const moons,
                              const int num_moons,
                              const int dim);
static void update_velocity(Moon* const a, const Moon* const b, const int dim);
static void update_position(Moon* const moon, const int dim);
static void get_system_state(Moon** const moons,
                             const int num_moons,
                             const int dim,
                             int* state);
static int are_equal(const int* state_a,
                     const int* state_b,
                     const int num_moons);

static int get_max(const int* const cycle_lengths, const int amount);

Moon** read_moons(const char* const file_path, const int amount)
{
    Moon** moons = NULL;
    if (file_path == NULL || amount <= 0)
    {
        return NULL;
    }

    FILE* fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        return NULL;
    }

    moons = (Moon**) malloc(sizeof(Moon*) * amount);
    if (moons == NULL)
    {
        return NULL;
    }

    int line_index = 0;
    char line_buffer[LINE_BUFFER_LENGTH];

    while (line_index < amount && !feof(fp))
    {
        Moon* moon      = (Moon*) malloc(sizeof(Moon));
        int coord_index = 0;
        if (moon == NULL)
        {
            return NULL;
        }
        if (fgets(line_buffer, LINE_BUFFER_LENGTH, fp) != NULL)
        {
            char* token = strtok(line_buffer, COORD_DELIM);
            while ((token != NULL) && (coord_index < DIMENSIONS))
            {
                moon->position[coord_index] = atoi(token);
                moon->velocity[coord_index] = 0;
                coord_index++;
                token = strtok(NULL, COORD_DELIM);
            }
            moons[line_index] = moon;
            line_index++;
        }
    }

    fclose(fp);
    return moons;
}

Moon** copy_system(Moon** moons, const int num_moons)
{
    Moon** copy = NULL;

    if (moons != NULL)
    {
        copy = (Moon**) malloc(sizeof(Moon*) * num_moons);
        if (copy != NULL)
        {
            for (int i = 0; i < num_moons; ++i)
            {
                Moon* moon_cpy = (Moon*) malloc(sizeof(Moon));
                if (moon_cpy != NULL)
                {
                    for (int d = 0; d < DIMENSIONS; ++d)
                    {
                        moon_cpy->position[d] = moons[i]->position[d];
                        moon_cpy->velocity[d] = moons[i]->velocity[d];
                    }
                    copy[i] = moon_cpy;
                }
            }
        }
    }
    return copy;
}

void destroy_system(Moon** const moons, const int num_moons)
{
    if (moons != NULL)
    {
        for (int i = 0; i < num_moons; ++i)
        {
            destroy_moon(moons[i]);
        }
        free(moons);
    }
}

void destroy_moon(Moon* const moon)
{
    if (moon != NULL)
    {
        free(moon);
    }
}

void print_moon(const Moon* const moon)
{
    if (moon != NULL)
    {
        printf("Moon:\n");
        for (int i = 0; i < DIMENSIONS; ++i)
        {
            printf("%d\t%d\n", moon->position[i], moon->velocity[i]);
        }
    }
    else
    {
        printf("Moon is NULL\n");
    }
}

void simulate_iterations(Moon** const moons,
                         const int num_moons,
                         const int num_iterations)
{
    for (int i = 0; i < num_iterations; ++i)
    {
        for (int d = 0; d < DIMENSIONS; ++d)
        {
            step(moons, num_moons, d);
        }
    }
}

int potential_energy(const Moon* const moon)
{
    int pot_eng = 0;
    if (moon != NULL)
    {
        for (int i = 0; i < DIMENSIONS; ++i)
        {
            pot_eng += abs(moon->position[i]);
        }
    }
    return pot_eng;
}

int kinetic_energy(const Moon* const moon)
{
    int kin_eng = 0;
    if (moon != NULL)
    {
        for (int i = 0; i < DIMENSIONS; ++i)
        {
            kin_eng += abs(moon->velocity[i]);
        }
    }
    return kin_eng;
}

int total_energy(const Moon* const moon)
{
    return kinetic_energy(moon) * potential_energy(moon);
}

int total_system_energy(Moon** moons, const int num_moons)
{
    int system_energy = 0;
    if (moons != NULL)
    {
        for (int i = 0; i < num_moons; ++i)
        {
            system_energy += total_energy(moons[i]);
        }
    }
    return system_energy;
}

int steps_for_cycle(Moon** moons, const int num_moons, const int dim)
{
    int steps = 0;
    int initial_state[2 * num_moons];
    get_system_state(moons, num_moons, dim, initial_state);
    int tmp_state[2 * num_moons];

    while (!are_equal(initial_state, tmp_state, num_moons))
    {
        step(moons, num_moons, dim);
        get_system_state(moons, num_moons, dim, tmp_state);
        steps++;
    }
    return steps;
}

int64_t least_common_multiple(const int* const cycle_lengths, const int amount)
{
    /*This is a brute force approach.*/
    /*An faster and smarter alternative would be to use primes,
     * e.g. "Division Method".*/
    int max      = get_max(cycle_lengths, amount);
    int64_t mult = 0;
    if (cycle_lengths != NULL)
    {
        int found = 0;
        while (!found)
        {
            mult += max;
            found = 1;
            for (int i = 0; i < amount; ++i)
            {
                found = found && (cycle_lengths[i] > 0) &&
                        (mult / cycle_lengths[i] != 0) &&
                        (mult % cycle_lengths[i] == 0);
            }
        }
    }
    return mult;
}

static void get_system_state(Moon** const moons,
                             const int num_moons,
                             const int dim,
                             int* state)
{
    assert(moons != NULL);
    assert(state != NULL);
    for (int i = 0; i < num_moons; ++i)
    {
        assert(moons[i] != NULL);
        state[i * 2]       = moons[i]->position[dim];
        state[(i * 2) + 1] = moons[i]->velocity[dim];
    }
}

static int are_equal(const int* state_a,
                     const int* state_b,
                     const int num_moons)
{
    assert(state_a != NULL);
    assert(state_b != NULL);
    for (int i = 0; i < num_moons * 2; ++i)
    {
        if (state_a[i] != state_b[i])
        {
            return 0;
        }
    }
    return 1;
}

static void step(Moon** const moons, const int num_moons, const int dim)
{
    assert(moons != NULL);
    update_velocities(moons, num_moons, dim);
    for (int i = 0; i < num_moons; ++i)
    {
        update_position(moons[i], dim);
    }
}

static void update_velocities(Moon** const moons,
                              const int num_moons,
                              const int dim)
{
    assert(moons != NULL);
    for (int i = 0; i < num_moons; ++i)
    {
        for (int j = 0; j < num_moons; ++j)
        {
            if (i == j)
            {
                continue;
            }
            update_velocity(moons[i], moons[j], dim);
        }
    }
}

static void update_velocity(Moon* const a, const Moon* const b, const int dim)
{
    assert(a != NULL);
    assert(b != NULL);
    if (a->position[dim] < b->position[dim])
    {
        a->velocity[dim] += 1;
    }
    else if (a->position[dim] > b->position[dim])
    {
        a->velocity[dim] -= 1;
    }
}

static void update_position(Moon* const moon, const int dim)
{
    assert(moon != NULL);
    moon->position[dim] += moon->velocity[dim];
}

static int get_max(const int* const cycle_lengths, const int amount)
{
    assert(cycle_lengths != NULL);
    int max = 0;
    for (int i = 0; i < amount; ++i)
    {
        if (cycle_lengths[i] > max)
        {
            max = cycle_lengths[i];
        }
    }
    return max;
}
