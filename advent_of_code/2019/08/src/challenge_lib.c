/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-11
 *
 */

#include "challenge/challenge_lib.h"
#include "stdio.h"

static void layer_init(layer_t* const layer,
                       const size_t height,
                       const size_t width);

static uint8_t layer_get_value(const layer_t* const layer,
                               const size_t col,
                               const size_t row);

static int image_add_layer(image_t* const img);

image_t* parse_image(const char* const file_path,
                     const size_t height,
                     const size_t width)
{
    image_t* img = NULL;
    if (file_path == NULL)
    {
        return NULL;
    }

    FILE* fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        printf("Error opening the file: %s\n", file_path);
        return NULL;
    }

    /*Init image and first layer.*/
    img             = (image_t*) malloc(sizeof(image_t));
    img->layers     = (layer_t*) malloc(0);
    img->height     = height;
    img->width      = width;
    img->num_layers = 0;
    image_add_layer(img);

    size_t layer_index      = 0;
    size_t layer_data_index = 0;

    char c = fgetc(fp);
    while (!feof(fp) && c != '\n')
    {
        uint8_t x;
        sscanf(&c, "%hhu", &x);
        /*Alternatively: */
        /*x = c - '0';*/
        printf("%hhu", x);

        if (layer_data_index == img->layers[layer_index].size)
        {
            /*Layer is full. Add a new layer*/
            if (image_add_layer(img))
            {
                layer_data_index = 0;
                layer_index++;
            }
            else
            {
                printf("Error when adding a new layer.\n");
                return NULL;
            }
        }

        if (layer_data_index < img->layers[layer_index].size)
        {
            img->layers[layer_index].data[layer_data_index] = x;
            layer_data_index++;
        }

        c = fgetc(fp);
    }
    printf("\n");
    fclose(fp);
    return img;
}

void destroy_image(image_t* const img)
{
    if (img != NULL)
    {
        if (img->layers != NULL)
        {
            for (size_t i = 0; i < img->num_layers; i++)
            {
                if (img->layers[i].data != NULL)
                {
                    free(img->layers[i].data);
                }
            }
            free(img->layers);
        }
        free(img);
    }
}

void print_image(const image_t* const img)
{
    if (img != NULL)
    {
        for (size_t i = 0; i < img->num_layers; i++)
        {
            printf("Layer %lu\n", i + 1);
            layer_t layer = img->layers[i];
            for (size_t r = 0; r < layer.height; r++)
            {
                for (size_t c = 0; c < layer.width; c++)
                {
                    printf("%hhu", layer_get_value(&layer, c, r));
                }
                printf("\n");
            }
        }
    }
}

static void layer_init(layer_t* const layer,
                       const size_t height,
                       const size_t width)
{
    if (layer != NULL)
    {
        size_t size   = height * width;
        layer->size   = size;
        layer->height = height;
        layer->width  = width;

        layer->data = (uint8_t*) malloc(sizeof(uint8_t) * size);
        for (size_t i = 0; i < size; i++)
        {
            layer->data[i] = 0;
        }
    }
}

static uint8_t layer_get_value(const layer_t* const layer,
                               const size_t col,
                               const size_t row)
{
    uint8_t value = 0;
    if (layer != NULL)
    {
        if ((col < layer->width) && (row < layer->height))
        {
            value = layer->data[(row * layer->width) + col];
        }
    }
    return value;
}

static int image_add_layer(image_t* const img)
{
    int success = 0;
    if (img != NULL)
    {
        layer_t layer;
        layer_init(&layer, img->height, img->width);
        img->num_layers++;

        img->layers =
            (layer_t*) realloc(img->layers, sizeof(layer_t) * img->num_layers);
        if (img->layers != NULL)
        {
            img->layers[img->num_layers - 1] = layer;
            success                          = 1;
        }
    }
    return success;
}
