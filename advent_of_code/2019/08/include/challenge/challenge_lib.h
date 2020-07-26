/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-11
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdint.h"
#include "stdlib.h"

typedef struct
{
    uint8_t* data;
    size_t size;
    size_t height;
    size_t width;
} layer_t;

typedef struct
{
    layer_t** layers;
    size_t num_layers;
    size_t height;
    size_t width;
} image_t;

image_t* parse_image(const char* const file_path, const size_t height, const size_t width);
void destroy_image(image_t* const img);
void print_image(const image_t* const img);
void show_image(const image_t* const img);
void output_image(const image_t* const img);

image_t* decode_image(const image_t* const img);

int find_layer_with_fewest(const image_t* const img,
                           const uint8_t value,
                           size_t* const count,
                           size_t* const layer_index);
size_t layer_count_occurences(const layer_t* const layer, const uint8_t value);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
