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

typedef struct
{
    int x;
    int y;
} Position;

typedef struct
{
    intcode_t* brain;
    Position pos;
    Direction direction;
    int finished;
} Robot;

typedef struct
{
    Robot* robot;
    int* hull;
    int height;
    int width;
} Overview;

Direction turn(const Direction current, const int command);
void move(Robot* const robot);

void print_overview(const Overview* const overview);
int count_painted_fields(const Overview* const overview);

/*Thread functions*/
void* robot_func(void* args);
void* control_func(void* args);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
