/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-11
 *
 */

#include "challenge/challenge_lib.h"
#include "stdio.h"

static void layer_init(layer_t* const layer, const size_t height, const size_t width);

static uint8_t layer_get_value(const layer_t* const layer, const size_t col, const size_t row);
static void layer_set_value(layer_t* const layer,
                            const uint8_t value,
                            const size_t col,
                            const size_t row);

static int image_add_layer(image_t* const img);

typedef enum
{
    IMAGE_COLOR_BLACK       = 0,
    IMAGE_COLOR_WHITE       = 1,
    IMAGE_COLOR_TRANSPARENT = 2,
} image_color_t;

image_t* parse_image(const char* const file_path, const size_t height, const size_t width)
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
    img->layers     = NULL;
    img->height     = height;
    img->width      = width;
    img->num_layers = 0;
    image_add_layer(img);

    size_t layer_index      = 0;
    size_t layer_data_index = 0;
    size_t counter          = 0;

    char c = fgetc(fp);
    while (!feof(fp) && c != '\n')
    {
        uint8_t x;
        sscanf(&c, "%hhu", &x);
        /*Alternatively: */
        /*x = c - '0';*/
        counter++;

        if (layer_data_index == img->layers[layer_index]->size)
        {
            /*Layer is full. Add a new layer*/
            if (image_add_layer(img))
            {
                layer_data_index = 0;
                layer_index++;
            }
            else
            {
                destroy_image(img);
                printf("Error when adding a new layer.\n");
                return NULL;
            }
        }

        if (layer_data_index < img->layers[layer_index]->size)
        {
            img->layers[layer_index]->data[layer_data_index] = x;
            layer_data_index++;
        }

        c = fgetc(fp);
    }
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
                if (img->layers[i] != NULL)
                {
                    if (img->layers[i]->data != NULL)
                    {
                        free(img->layers[i]->data);
                    }
                }
                free(img->layers[i]);
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
            layer_t* layer = img->layers[i];
            for (size_t r = 0; r < layer->height; r++)
            {
                for (size_t c = 0; c < layer->width; c++)
                {
                    printf("%hhu", layer_get_value(layer, c, r));
                }
                printf("\n");
            }
        }
    }
}

void show_image(const image_t* const img)
{
    if (img != NULL)
    {
        for (size_t i = 0; i < img->num_layers; i++)
        {
            layer_t* layer = img->layers[i];
            for (size_t r = 0; r < layer->height; r++)
            {
                for (size_t c = 0; c < layer->width; c++)
                {
                    uint8_t value = layer_get_value(layer, c, r);
                    if (value == IMAGE_COLOR_WHITE)
                    {
                        printf("%hhu", value);
                    }
                    else
                    {
                        /*Assuming your background is "black"*/
                        printf(" ");
                    }
                }
                printf("\n");
            }
        }
    }
}

void output_image(const image_t* const img)
{
    if (img != NULL)
    {
        for (size_t i = 0; i < img->num_layers; i++)
        {
            layer_t* layer = img->layers[i];
            for (size_t r = 0; r < layer->height; r++)
            {
                for (size_t c = 0; c < layer->width; c++)
                {
                    printf("%hhu", layer_get_value(layer, c, r));
                }
            }
        }
        printf("\n");
    }
}

image_t* decode_image(const image_t* const img)
{
    image_t* decoded_img = NULL;
    if (img != NULL)
    {
        decoded_img             = (image_t*) malloc(sizeof(image_t));
        decoded_img->layers     = NULL;
        decoded_img->height     = img->height;
        decoded_img->width      = img->width;
        decoded_img->num_layers = 0;
        image_add_layer(decoded_img);

        layer_t* decoded_layer = decoded_img->layers[0];
        if (decoded_layer != NULL)
        {
            for (size_t r = 0; r < img->height; r++)
            {
                for (size_t c = 0; c < img->width; c++)
                {
                    for (size_t i = 0; i < img->num_layers; i++)
                    {
                        uint8_t layer_val = layer_get_value(img->layers[i], c, r);
                        if (layer_val != IMAGE_COLOR_TRANSPARENT)
                        {
                            layer_set_value(decoded_layer, layer_val, c, r);
                            break;
                        }
                    }
                }
            }
        }
    }
    return decoded_img;
}

int find_layer_with_fewest(const image_t* const img,
                           const uint8_t value,
                           size_t* const count,
                           size_t* const layer_index)
{
    int success = 0;
    if ((img != NULL) && (count != NULL) && (layer_index != NULL))
    {
        size_t min_count = 256; /*larger than any uint8_t*/
        size_t layer     = 0;
        for (size_t i = 0; i < img->num_layers; i++)
        {
            size_t layer_count = layer_count_occurences(img->layers[i], value);
            if (min_count > layer_count)
            {
                min_count = layer_count;
                layer     = i;
            }
        }
        *count       = min_count;
        *layer_index = layer;
        success      = 1;
    }
    return success;
}

size_t layer_count_occurences(const layer_t* const layer, const uint8_t value)
{
    size_t count = 0;
    for (size_t i = 0; i < layer->size; i++)
    {
        if (layer->data[i] == value)
        {
            count++;
        }
    }
    return count;
}

static void layer_init(layer_t* const layer, const size_t height, const size_t width)
{
    if (layer != NULL)
    {
        size_t size   = height * width;
        layer->size   = size;
        layer->height = height;
        layer->width  = width;

        layer->data = (uint8_t*) malloc(sizeof(uint8_t) * size);
        if (layer->data != NULL)
        {
            for (size_t i = 0; i < size; i++)
            {
                layer->data[i] = IMAGE_COLOR_TRANSPARENT;
            }
        }
        else
        {
            printf("Error allocating data for layer.\n");
        }
    }
}

static uint8_t layer_get_value(const layer_t* const layer, const size_t col, const size_t row)
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

static void layer_set_value(layer_t* const layer,
                            const uint8_t value,
                            const size_t col,
                            const size_t row)
{
    if (layer != NULL)
    {
        if ((col < layer->width) && (row < layer->height))
        {
            layer->data[(row * layer->width) + col] = value;
        }
    }
}

static int image_add_layer(image_t* const img)
{
    int success = 0;
    if (img != NULL)
    {
        layer_t* layer = (layer_t*) malloc(sizeof(layer_t));
        if (layer != NULL)
        {
            layer_init(layer, img->height, img->width);

            if (img->layers == NULL)
            {
                img->layers = (layer_t**) malloc(sizeof(layer_t*));
            }
            else
            {
                img->layers =
                    (layer_t**) realloc(img->layers, sizeof(layer_t*) * (img->num_layers + 1));
            }

            if (img->layers != NULL)
            {
                img->layers[img->num_layers++] = layer;
                success                        = 1;
            }
            else
            {
                success = 0;
            }
        }
    }
    return success;
}
