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

    /*Part 01*/
    int sum = 0;
    for (int y = 0; y < 50; ++y)
    {
        for (int x = 0; x < 50; ++x)
        {
            int64_t result = scan_coordinate(prog, x, y);
            /*printf("Scan result for (%d, %d): %ld\n", x, y, result);*/
            sum += result;
        }
    }
    printf("Total number of affected points: %d\n", sum);

    /*Part 02*/

    /*Calculate slope of the beam.*/
    int row   = 300;
    int cols  = 500;
    int beam  = 0;
    int first = -1;
    int last  = -1;
    for (int x = 0; x < cols; ++x)
    {
        int64_t result = scan_coordinate(prog, x, row);
        if (beam ^ result)
        {
            if (!beam)
            {
                first = x;
            }
            else
            {
                last = x - 1;
            }
            beam = !beam;
        }
    }
    if (beam)
    {
        printf("Didn't find end of beam, not enough columns.\n");
    }
    float slope = ((first + last) / 2.0) / (row + 1);
    printf("Slope for row #%d is %f.\n", row, slope);

    /*Since the width of the beam increases about every 3rd row,
     * the beam will hit a width of 100 at roughly this offset.
    * Looking at earlier rows is not really useful.*/
    row = 300;

    /*The beam should be between 100-200 fields wide in the area we are looking
     * for.*/
    cols = 250;

    int square_size = 100;

    int found = 0;
    while (!found)
    {
        /*We are only interested in the beam area, so we offset the column*/
        /*based on our estimate of the beam center.*/
        int col_offset = (int) (row * slope);
        beam           = 1;

        /*Find the right most field affected by the beam.*/
        /*We start looking roughly in the center of the beam.*/
        for (int x = 0; x < cols; ++x)
        {
            int64_t result = scan_coordinate(prog, x + col_offset, row);
            if (beam ^ result)
            {
                if (beam)
                {
                    /*End of beam found.*/
                    /*last field of beam is in x-1.*/

                    int right_x  = x - 1 + col_offset;
                    int top_y    = row;
                    int left_x   = right_x - (square_size - 1);
                    int bottom_y = top_y + (square_size - 1);

                    if (left_x >= 0)
                    {
                        /*check the corner coordinates.*/
                        int64_t top_left = scan_coordinate(prog, left_x, top_y);
                        int64_t bottom_left =
                            scan_coordinate(prog, left_x, bottom_y);
                        int64_t bottom_right =
                            scan_coordinate(prog, right_x, bottom_y);

                        if (top_left && bottom_left && bottom_right)
                        {
                            printf("Top Left: (%d, %d)\n", left_x, top_y);
                            found = 1;
                        }
                    }

                    break;
                }
            }
        }
        row++;
    }

    destroy_intcode(prog);
    return 0;
}
