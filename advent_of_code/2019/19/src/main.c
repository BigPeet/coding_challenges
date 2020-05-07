/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-05-06
 *
 */

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"
#include "pthread.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

/*scan area */
#define HEIGHT 50
#define WIDTH 50


int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_19 FILE_PATH.\n");
        return 0;
    }

    /*Initialize the program with IO memory.*/
    intcode_t* prog = read_intcode(argv[1]);
    if (prog == NULL)
    {
        printf("Error reading programm or allocating IO memory\n");
        return 0;
    }

    int sum = 0;

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            int64_t result = scan_coordinate(prog, x, y);
            /*printf("Scan result for (%d, %d): %ld\n", x, y, result);*/
            sum += result;
        }
    }

    printf("Total number of affected points: %d\n", sum);

    destroy_intcode(prog);
    return 0;
}
