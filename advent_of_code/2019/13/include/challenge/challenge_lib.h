/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-15
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "challenge/intcode.h"

typedef enum
{
    UP    = 0,
    RIGHT = 1,
    DOWN  = 2,
    LEFT  = 3,
} Direction;
#define NUM_OF_DIRECTIONS 4

typedef enum
{
    EMPTY  = 0,
    WALL   = 1,
    BLOCK  = 2,
    PADDLE = 3,
    BALL   = 4,
} TileType;

typedef struct
{
    int x;
    int y;
    int type;
} Tile;

typedef struct
{
    intcode_t* brain;
    int finished;
} Engine;

typedef struct
{
    TileType* game_area;
    Engine* engine;
    int height;
    int width;
    int score;
} Game;

/*Thread functions*/
void* engine_func(void* args);
void* game_func(void* args);

Game* create_game(const int height, const int width);
void destroy_game(Game* const game);
void add_tile(Game* const game, const Tile* const tile);

int count_tiles(const Game* const game, const TileType type);
void display_game(const Game* const game);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
