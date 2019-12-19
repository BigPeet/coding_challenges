/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-17
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "challenge/intcode.h"

typedef enum
{
    UP    = 0,
    DOWN  = 1,
    RIGHT = 2,
    LEFT  = 3,
} Direction;
#define NUM_OF_DIRECTIONS 4

typedef struct
{
    int x;
    int y;
} Position;

typedef struct
{
    Position pos;
    Direction direction;
} Robot;

typedef struct
{
    intcode_t* brain;
    int finished;
} ASCII;

typedef struct
{
    ASCII* system;
    Robot* robot;
    int* area;
    int initialized;
    Position output_pos;
    int height;
    int width;
} Overview;


void print_overview(const Overview* const overview);

/*Thread functions*/
void* system_func(void* args);
void* control_func(void* args);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
