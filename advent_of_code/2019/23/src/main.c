/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

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
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_23 FILE_PATH.\n");
        return 0;
    }


    return 0;
}
