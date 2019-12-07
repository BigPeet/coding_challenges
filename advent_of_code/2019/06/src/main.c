/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-06
 *
 */

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "challenge/challenge_lib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_06 FILE_PATH.\n");
        return 0;
    }

    map_t map;
    parse_map(argv[1], &map);
    return 0;
}
