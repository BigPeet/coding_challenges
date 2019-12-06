/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-02
 *
 */

#include "challenge/challenge_lib.h"
#include "stdio.h"

/*
 * @brief Counts the lines of the given file.
 *
 * Assumes the file is already opened.
 */
static size_t count_lines(const char* const file_path);

size_t* read_modules(const char* const file_path, size_t* const amount_modules)
{
    size_t* modules = NULL;
    if ((file_path != NULL) && (amount_modules != NULL))
    {
        *amount_modules = count_lines(file_path);
        FILE* fp;
        fp = fopen(file_path, "r");
        if (fp != NULL)
        {
            size_t index = 0;
            size_t mass  = 0;
            modules      = (size_t*)malloc(sizeof(size_t) * *amount_modules);

            while (fscanf(fp, "%zd", &mass) != EOF)
            {
                modules[index++] = mass;
            }
            fclose(fp);
        }
    }
    return modules;
}

size_t total_fuel(const size_t* const module_masses, const size_t num_of_modules)
{
    /*Used for Part 01*/
    size_t fuel = 0;
    if (module_masses != NULL)
    {
        for (size_t i = 0; i < num_of_modules; i++)
        {
            fuel += fuel_by_mass(module_masses[i]);
        }
    }
    return fuel;
}

size_t total_fuel_complete(const size_t* const module_masses, const size_t num_of_modules)
{
    /*Used for Part 02*/
    size_t total = 0;
    if (module_masses != NULL)
    {
        for (size_t i = 0; i < num_of_modules; i++)
        {
            total += fuel_for_module(module_masses[i]);
        }
    }
    return total;
}

size_t fuel_for_module(const size_t module_mass)
{
    size_t fuel_for_module = fuel_by_mass(module_mass);
    size_t fuel_for_fuel   = fuel_by_mass(fuel_for_module);
    do
    {
        fuel_for_module += fuel_for_fuel;
        fuel_for_fuel = fuel_by_mass(fuel_for_fuel);
    } while (fuel_for_fuel > 0);
    return fuel_for_module;
}

size_t fuel_by_mass(const size_t mass)
{
    size_t fuel = 0;
    if (mass > 8)
    {
        fuel = (mass / 3) - 2;
    }
    return fuel;
}

static size_t count_lines(const char* const file_path)
{
    /*Ugly and inefficient to read the same file twice, but I'm*/
    /*currently too lazy to dynamically adjust the memory needed.
     *And I don't want to hardcode the amount either.*/
    size_t num_lines = 0;
    FILE* fp;
    fp = fopen(file_path, "r");
    if (fp != NULL)
    {
        int tmp;
        while (fscanf(fp, "%d", &tmp) != EOF)
        {
            num_lines++;
        }
        fclose(fp);
    }
    return num_lines;
}
