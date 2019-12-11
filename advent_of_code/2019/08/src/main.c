/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-11
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
    if (argc != 4)
    {
        printf("This executabel takes exactly three arguments.\n");
        printf("Usage: aoc2019_08 FILE_PATH HEIGHT WIDTH.\n");
        return 0;
    }

    int input[2];
    read_input_numbers(argc - 1, argv+1, input);
    image_t* img = parse_image(argv[1], input[0], input[1]);

    if (img != NULL)
    {
        printf("Image has %zu layers.\n", img->num_layers);
        print_image(img);
        free(img);
    }


    return 0;
}
