/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-19
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdlib.h"

typedef struct
{
    int x;
    int y;
} Position;

typedef struct
{
    Position pos;
    int opened;
    char id;
} Door;

typedef struct
{
    Position pos;
    Door* door;
    int picked_up;
    char id;
} Key;

typedef struct
{
    char* data;
    int width;
    int height;
} Map;

typedef struct
{
    Map* map;
    Door** doors;
    Key** keys;
    Position entrance;
    int num_keys;
    int num_doors;
} Overview;


Overview* read_input(const char* const file_path,
                     const int width,
                     const int height,
                     const int num_keys,
                     const int num_doors);
void destroy_overview(Overview* const overview);
void print_map(const Map* const map);

int minimal_steps(const Overview* const overview);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
