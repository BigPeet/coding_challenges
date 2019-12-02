#include "challenge/challenge_lib.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_01 FILE_PATH.\n");
        return 0;
    }

    size_t amount_modules  = 0;
    size_t* module_masses = read_modules(argv[1], &amount_modules);

    if (module_masses != NULL)
    {
        printf("Amount of modules: %zu\n", amount_modules);
        size_t fuel = total_fuel(module_masses, amount_modules);
        printf("Fuel for module mass needed: %zu\n", fuel);

        fuel = total_fuel_complete(module_masses, amount_modules);
        printf("Complete fuel needed: %zu\n", fuel);
        free(module_masses);
    }

    return 0;
}
