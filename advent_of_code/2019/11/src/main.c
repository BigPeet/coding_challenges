/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-15
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "challenge/intcode.h"
#include "challenge/challenge_lib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_11 FILE_PATH.\n");
        return 0;
    }

    intcode_t* prog = read_intcode(argv[1]);
    destroy_intcode(prog);


    return 0;
}
