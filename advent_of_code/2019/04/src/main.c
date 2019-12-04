#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

void read_input_numbers(const int argc, char** argv, int* input)
{
    for (int i = 1; i < argc; i++)
    {
        input[i - 1] = atoi(argv[i]);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("This executabel takes exactly two arguments.\n");
        printf("Usage: aoc2019_04 LOWER_RANGE UPPER_RANGE.\n");
        return 0;
    }

    int amount_of_numbers = argc - 1;
    int numbers[amount_of_numbers];
    read_input_numbers(argc, argv, numbers);

    if (amount_of_numbers == 2)
    {
        int lower     = numbers[0];
        int upper     = numbers[1];

        int solutions = num_of_solutions(lower, upper, fits_first_set_criteria);
        printf("Part 1: Between %d and %d there are %d solutions.\n", numbers[0], upper, solutions);

        solutions = num_of_solutions(lower, upper, fits_second_set_criteria);
        printf("Part 2: Between %d and %d there are %d solutions.\n", lower, upper, solutions);
    }

    return 0;
}
