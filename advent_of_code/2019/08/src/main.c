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
    read_input_numbers(argc - 1, argv + 1, input);

    image_t* img = parse_image(argv[1], input[0], input[1]);

    if (img != NULL)
    {
        printf("Image has %zu layers.\n", img->num_layers);

        /*Find layer with fewest 0s*/
        size_t count;
        size_t layer_index;
        if (find_layer_with_fewest(img, 0, &count, &layer_index))
        {
            printf("Layer %zu has %zu 0s.\n", layer_index + 1, count);

            size_t num_of_ones =
                layer_count_occurences(img->layers[layer_index], 1);
            size_t num_of_twos =
                layer_count_occurences(img->layers[layer_index], 2);

            printf("Layer %zu has %zu 1s and %zu 2s: %zu * %zu = %zu\n",
                   layer_index + 1,
                   num_of_ones,
                   num_of_twos,
                   num_of_ones,
                   num_of_twos,
                   num_of_ones * num_of_twos);
        }

        /*Decode image*/
        image_t* decoded_img = decode_image(img);

        show_image(decoded_img);

        destroy_image(img);
        destroy_image(decoded_img);
    }


    return 0;
}
