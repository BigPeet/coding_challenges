/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-15
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "ncurses.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

static void resize_game(Game* const game, const Direction dir);
static void read_tile(const Engine* const engine, Tile* const tile);
static int read_output(const Engine* const engine);
static char type_to_char(const TileType type);
static void send_control_cmd(const Game* const game);
static int get_x_coord(const Game* const game, const TileType type);


void* engine_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    Engine* engine = (Engine*) args;
    int ret        = execute(engine->brain);
    if (ret != INT_CODE_HALT)
    {
        printf("Programm did not halt as expected. Err code: %d\n", ret);
    }
    else
    {
        /*Give game thread time to parse remaining output.*/
        usleep(1000);

        /*End the game*/
        engine->finished = 1;

        /*Wake up potentially waiting game thread.*/
        pthread_cond_signal(&engine->brain->mem_io_out->cond);
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

    initscr();
    curs_set(0);

    while (!game->engine->finished)
    {

        Tile tile;
        read_tile(game->engine, &tile);
        if ((tile.x == -1) && (tile.y == 0))
        {
            game->score = tile.type;
        }
        else
        {
            add_tile(game, &tile);
        }

        /*Without this sleep, a deadlock might occur.*/
        usleep(1);

        if (waiting_for_input(game->engine->brain))
        {
            send_control_cmd(game);
        }

        display_game(game);
    }

    endwin();
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
    game->score     = 0;
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

void display_game(const Game* const game)
{
    /*Print using ncurses*/
    /*Install libncurses5-dev*/

    /*Assuming the curse screen has already been initialized.*/
    mvprintw(0, 0, "Game Score: %d", game->score);
    for (int i = 0; i < game->height; ++i)
    {
        for (int j = 0; j < game->width; ++j)
        {
            TileType type = game->game_area[(i * game->width) + j];
            mvprintw(i + 1, j, "%c", type_to_char(type));
        }
    }
    refresh();
}

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

static void read_tile(const Engine* const engine, Tile* const tile)
{
    assert(engine != NULL);
    assert(tile != NULL);

    int value = read_output(engine);
    if (value != -10)
    {
        tile->x = value;
    }

    value = read_output(engine);
    if (value != -10)
    {
        tile->y = value;
    }

    value = read_output(engine);
    if (value != -10)
    {
        tile->type = value;
    }
}

static int read_output(const Engine* const engine)
{
    assert(engine != NULL);
    assert(engine->brain != NULL);
    assert(engine->brain->mem_io_out != NULL);

    int read_value = -10;

    intcode_io_mem_t* output = engine->brain->mem_io_out;
    pthread_mutex_lock(&output->mut);
    while (output->consumed && !engine->finished)
    {
        pthread_cond_wait(&output->cond, &output->mut);
    }

    if (!engine->finished)
    {
        read_value       = output->value;
        output->consumed = 1;
    }
    pthread_cond_signal(&output->cond);
    pthread_mutex_unlock(&output->mut);
    return read_value;
}

static int get_x_coord(const Game* const game, const TileType type)
{
    for (int i = 0; i < game->height; ++i)
    {
        for (int j = 0; j < game->width; ++j)
        {
            if (game->game_area[(i * game->width) + j] == type)
            {
                return j;
            }
        }
    }
    return -1;
}

static void send_control_cmd(const Game* const game)
{
    assert(game != NULL);
    assert(game->engine != NULL);
    assert(game->engine->brain != NULL);
    assert(game->engine->brain->mem_io_in != NULL);

    intcode_io_mem_t* input = game->engine->brain->mem_io_in;
    pthread_mutex_lock(&input->mut);
    while (!input->consumed && !game->engine->finished)
    {
        pthread_cond_wait(&input->cond, &input->mut);
    }

    int ball_x   = get_x_coord(game, BALL);
    int paddle_x = get_x_coord(game, PADDLE);
    int value    = 0;
    if ((ball_x != -1) && (paddle_x != -1))
    {
        value = (ball_x > paddle_x) ? 1 : (ball_x < paddle_x) ? -1 : 0;
    }
    input->value    = value;
    input->consumed = 0;
    pthread_cond_signal(&input->cond);
    pthread_mutex_unlock(&input->mut);
}

static char type_to_char(const TileType type)
{
    switch (type)
    {
        case EMPTY:
            return ' ';
        case WALL:
            return '|';
        case BLOCK:
            return '#';
        case PADDLE:
            return '_';
        case BALL:
            return 'o';
        default:
            return ' ';
    }
}
