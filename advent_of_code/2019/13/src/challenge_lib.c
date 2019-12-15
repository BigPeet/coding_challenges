/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-15
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "stdio.h"
#include "stdlib.h"

static void resize_game(Game* const game, const Direction dir);
static void read_tile(const Builder* const builder, Tile* const tile);
static int read_output(const Builder* const builder);


void* builder_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    Builder* builder = (Builder*) args;
    int ret          = execute(builder->brain);
    if (ret != INT_CODE_HALT)
    {
        printf("Programm did not halt as expected. Err code: %d\n", ret);
    }
    else
    {
        /*Only writing access*/
        builder->finished = 1;
    }
    return NULL;
}

void* game_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    Game* game = (Game*) args;

    while (!game->builder->finished)
    {
        Tile tile;
        read_tile(game->builder, &tile);
        add_tile(game, &tile);
    }
    return NULL;
}

Game* create_game(const int height, const int width)
{
    Game* game = (Game*) malloc(sizeof(Game));
    if (game == NULL)
    {
        return NULL;
    }

    TileType* game_area = (TileType*) calloc(height, width * sizeof(TileType));
    if (game_area == NULL)
    {
        return NULL;
    }

    game->game_area = game_area;
    game->height    = height;
    game->width     = width;
    return game;
}

void destroy_game(Game* const game)
{
    if (game != NULL)
    {
        if (game->game_area != NULL)
        {
            free(game->game_area);
        }
        free(game);
    }
}

void add_tile(Game* const game, const Tile* const tile)
{
    if ((game != NULL) && (tile != NULL) && (game->game_area != NULL))
    {
        /*Resize*/
        if (tile->x >= game->width)
        {
            resize_game(game, RIGHT);
        }
        if (tile->y >= game->height)
        {
            resize_game(game, DOWN);
        }
        if (tile->x < 0 || tile->y < 0)
        {
            printf("Does that even happen?\n");
        }

        game->game_area[(tile->y * game->width) + tile->x] = tile->type;
    }
}

int count_tiles(const Game* const game, const TileType type)
{
    int count = 0;
    if ((game != NULL) && (game->game_area != NULL))
    {
        for (int i = 0; i < game->height; ++i)
        {
            for (int j = 0; j < game->width; ++j)
            {
                if (game->game_area[(i * game->width) + j] == type)
                {
                    count++;
                }
            }
        }
    }
    return count;
}

void print_game(const Game* const game);

static void resize_game(Game* const game, const Direction dir)
{
    assert(game != NULL);
    assert(game->game_area != NULL);
    int new_height =
        ((dir == UP) || (dir == DOWN)) ? game->height + 10 : game->height;
    int new_width =
        ((dir == RIGHT) || (dir == LEFT)) ? game->width + 10 : game->width;

    TileType* new_area =
        (TileType*) calloc(new_height, new_width * sizeof(TileType));

    if (new_area != NULL)
    {
        int start_y = (dir == UP) ? 10 : 0;
        int start_x = (dir == LEFT) ? 10 : 0;
        for (int i = start_y; i < game->height; ++i)
        {
            for (int j = start_x; j < game->width; ++j)
            {
                int old_index       = (i * game->width) + j;
                int new_index       = (i * new_width) + j;
                new_area[new_index] = game->game_area[old_index];
            }
        }
        free(game->game_area);
        game->game_area = new_area;
        game->height    = new_height;
        game->width     = new_width;
    }
}

static void read_tile(const Builder* const builder, Tile* const tile)
{
    assert(builder != NULL);
    assert(tile != NULL);

    int value = read_output(builder);
    if (value != -1)
    {
        tile->x = value;
    }

    value = read_output(builder);
    if (value != -1)
    {
        tile->y = value;
    }

    value = read_output(builder);
    if (value != -1)
    {
        tile->type = value;
    }
}

static int read_output(const Builder* const builder)
{
    assert(builder != NULL);
    assert(builder->brain != NULL);
    assert(builder->brain->mem_io_out != NULL);

    int read_value = -1;

    intcode_io_mem_t* output = builder->brain->mem_io_out;
    pthread_mutex_lock(&output->mut);
    while (output->consumed && !builder->finished)
    {
        pthread_cond_wait(&output->cond, &output->mut);
    }

    if (!builder->finished)
    {
        read_value       = output->value;
        output->consumed = 1;
    }
    pthread_cond_signal(&output->cond);
    pthread_mutex_unlock(&output->mut);
    return read_value;
}
