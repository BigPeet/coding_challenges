/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-11
 *
 */

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#define BUFFER_LENGTH (size_t)(32)

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_09 FILE_PATH.\n");
        return 0;
    }

    /*TODO: check return values of the function calls here */

    intcode_t* prog = read_intcode(argv[1]);
    if (prog == NULL)
    {
        return 0;
    }

    int ret = execute(prog);
    if (ret != INT_CODE_HALT)
    {
        printf("Program stopped with an error.\n");
    }

    destroy_intcode(prog);
    return 0;
}
