/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-11
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdlib.h"
#include "stdint.h"

typedef struct
{
    uint8_t* data;
    size_t size;
    size_t height;
    size_t width;
} layer_t;

typedef struct
{
    layer_t* layers;
    size_t num_layers;
    size_t height;
    size_t width;
} image_t;

image_t* parse_image(const char* const file_path, const size_t height, const size_t width);
void destroy_image(image_t* const img);
void print_image(const image_t* const img);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
