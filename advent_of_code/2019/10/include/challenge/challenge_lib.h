/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-12
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

typedef struct
{
    int x;
    int y;
} Point;

typedef struct
{
    int height;
    int width;
    int* data;
} Map;

Map* read_map(const char* const file_path, const int height, const int width);
void print_map(const Map* const map);
void destroy_map(Map* const map);

int get_max_visible_asteroids(const Map* const map, Point* const pos);
int count_visible_asteroids(const Point* const ast_pos, const Map* const map);
void fire_laser(Map* const map, const Point* const laser_pos);
void get_blocked_map(const Map* const map, const Point* const origin, int* blocked);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
