/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-12
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

static int check_rectangle(const Point* const origin,
                           const int steps,
                           const Map* const map,
                           int* const blocked,
                           int* const count);
static void block_from(const Point* const origin,
                       const Point* const blocking,
                       const int height,
                       const int width,
                       int* const blocked);

static int max(int a, int b)
{
    return (a > b) ? a : b;
}

static int min(int a, int b)
{
    return (a < b) ? a : b;
}

void read_map(const char* const file_path, Map* map)
{
    if ((file_path != NULL) && (map != NULL))
    {
        FILE* fp = fopen(file_path, "r");
        if (fp == NULL)
        {
            return;
        }
        for (int row = 0; row < map->height; row++)
        {
            for (int col = 0; col < map->width;)
            {
                char c = fgetc(fp);
                if (c != '\n')
                {
                    map->data[(row * map->width) + col] = (c == '#') ? 1 : 0;
                    col++;
                }
                if (feof(fp))
                {
                    break;
                }
            }
        }
        fclose(fp);
    }
}

void print_map(const Map* const map)
{
    if ((map != NULL) && (map->data != NULL))
    {
        for (int row = 0; row < map->height; row++)
        {
            for (int col = 0; col < map->width; col++)
            {
                printf("%d ", map->data[(row * map->width) + col]);
            }
            printf("\n");
        }
    }
}

int get_max_visible_asteroids(const Map* const map, Point* const pos)
{
    int max = 0;
    if ((map != NULL) && (pos != NULL))
    {
        Point tmp;
        for (int row = 0; row < map->height; row++)
        {
            for (int col = 0; col < map->width; col++)
            {
                if (map->data[(row * map->width) + col] == 1)
                {
                    Point p = {.x = col, .y = row};
                    int count = count_visible_asteroids(&p, map);
                    if (count > max)
                    {
                        max   = count;
                        tmp.x = p.x;
                        tmp.y = p.y;
                    }
                }
            }
        }
        pos->x = tmp.x;
        pos->y = tmp.y;
    }
    return max;
}

int count_visible_asteroids(const Point* const ast_pos, const Map* const map)
{
    int count = 0;
    if ((ast_pos != NULL) && (map != NULL))
    {
        /*Zero initialized*/
        int* blocked = (int*) calloc(map->height, map->width * sizeof(int));
        if (blocked == NULL)
        {
            return 0;
        }

        /*Block the origin*/
        blocked[(ast_pos->y * map->width) + ast_pos->x] = 1;

        int ret   = 1;
        int steps = 1;
        /*printf("Origin: (%d, %d)\n", ast_pos->x, ast_pos->y);*/
        while (ret)
        {
            ret = check_rectangle(ast_pos, steps, map, blocked, &count);
            /*printf("Step %d, currently at %d asteroids\n", steps, count);*/
            steps++;
        }
        free(blocked);
    }
    return count;
}

static void block_from(const Point* const origin,
                       const Point* const blocking,
                       const int height,
                       const int width,
                       int* const blocked)
{
    /*static function assumes clean input*/
    assert(origin != NULL);
    assert(blocked != NULL);
    assert(blocking != NULL);

    /*Assuming blocking is in relative coordinates to origin.*/

    /*printf("\tDiff is (%d, %d)\n", blocking->x, blocking->y);*/

    /*Blocking is at origin*/
    if ((blocking->x == 0) && (blocking->y == 0))
    {
        return;
    }

    /*Blocking is on horizontal line to origin*/
    if (blocking->y == 0)
    {
        int row   = origin->y;
        int index = origin->x + blocking->x;
        int inc   = (blocking->x > 0) ? 1 : -1;
        while ((index >= 0) && (index < width))
        {
            /*printf("\tBlocking (%d, %d)\n", index, row);*/
            blocked[(row * width) + index] = 1;
            index += inc;
        }
    }
    /*Blocking is on vertical line to origin*/
    else if (blocking->x == 0)
    {
        int col   = origin->x;
        int index = origin->y + blocking->y;
        int inc   = (blocking->y > 0) ? 1 : -1;
        while ((index >= 0) && (index < height))
        {
            /*printf("\tBlocking (%d, %d)\n", col, index);*/
            blocked[(index * width) + col] = 1;
            index += inc;
        }
    }
    /*Blocking is on diagonal line to origin*/
    else if (abs(blocking->x) == abs(blocking->y))
    {
        int col_index = origin->x + blocking->x;
        int row_index = origin->y + blocking->y;
        int col_inc   = (blocking->y > 0) ? 1 : -1;
        int row_inc   = (blocking->x > 0) ? 1 : -1;

        while ((col_index >= 0) && (col_index < height) && (row_index >= 0) &&
               (row_index < width))
        {
            /*printf("\tBlocking (%d, %d)\n", col_index, row_index);*/
            blocked[(row_index * width) + col_index] = 1;
            col_index += col_inc;
            row_index += row_inc;
        }
    }
    /*Fancy block*/
    else
    {
        int col_index = origin->x + blocking->x;
        int row_index = origin->y + blocking->y;
        int col_inc   = blocking->x;
        int row_inc   = blocking->y;

        while ((col_index >= 0) && (col_index < height) && (row_index >= 0) &&
               (row_index < width))
        {
            /*printf("\tFancy Blocking (%d, %d)\n", col_index, row_index);*/
            blocked[(row_index * width) + col_index] = 1;
            col_index += col_inc;
            row_index += row_inc;
        }
    }
}


static int check_rectangle(const Point* const origin,
                           const int steps,
                           const Map* const map,
                           int* const blocked,
                           int* const count)
{
    /*static function assumes clean input*/
    assert(origin != NULL);
    assert(blocked != NULL);
    assert(map != NULL);
    assert(count != NULL);

    int not_all_blocked = 0;

    int left       = max(0, origin->x - steps);
    int right      = min(map->width - 1, origin->x + steps);
    int top        = max(0, origin->y - steps);
    int bottom     = min(map->height - 1, origin->y + steps);
    int rect_count = 0;

    int fully_expanded = (left == 0) && (right == map->width - 1) &&
                         (top == 0) && (bottom == map->height - 1);

    Point top_left = {.x = left, .y = top};
    Point top_right = {.x = right, .y = top};
    Point bottom_left = {.x = left, .y = bottom};
    Point bottom_right = {.x = right, .y = bottom};

    /*left to right*/
    /*printf("\t Left to Right\n");*/
    for (int i = top_left.x; i <= top_right.x; i++)
    {
        int top_index = (top_left.y * map->width) + i;
        if (!blocked[top_index])
        {
            not_all_blocked = 1;
            int value       = map->data[top_index];
            if (value == 1)
            {
                /*printf("\tFound visible asteroid at (%d, %d)\n", i, top_left.y);*/
                rect_count++;
                /*Block line of sight*/
                Point blocking = {.x = (i - origin->x),
                                  .y = (top_left.y - origin->y)};
                block_from(origin, &blocking, map->height, map->width, blocked);
            }
        }
        int bottom_index = (bottom_left.y * map->width) + i;
        if (!blocked[bottom_index])
        {
            not_all_blocked = 1;
            int value       = map->data[bottom_index];
            if (value == 1)
            {
                /*printf("\tFound visible asteroid at (%d, %d)\n", i, bottom_left.y);*/
                rect_count++;
                /*Block line of sight*/
                Point blocking = {.x = (i - origin->x),
                                  .y = (bottom_left.y - origin->y)};
                block_from(origin, &blocking, map->height, map->width, blocked);
            }
        }
    }

    /*top to bottom*/
    /*printf("\t Top to bottom\n");*/
    for (int i = top_left.y + 1; i <= bottom_left.y - 1; i++)
    {
        int left_index = (i * map->width) + top_left.x;
        if (!blocked[left_index])
        {
            not_all_blocked = 1;
            int value       = map->data[left_index];
            if (value == 1)
            {
                /*printf("\tFound visible asteroid at (%d, %d)\n", top_left.x, i);*/
                rect_count++;
                /*Block line of sight*/
                Point blocking = {.x = (top_left.x - origin->x),
                                  .y = (i - origin->y)};
                block_from(origin, &blocking, map->height, map->width, blocked);
            }
        }
        int right_index = (i * map->width) + top_right.x;
        if (!blocked[right_index])
        {
            not_all_blocked = 1;
            int value       = map->data[right_index];
            if (value == 1)
            {
                /*printf("\tFound visible asteroid at (%d, %d)\n", top_right.x, i);*/
                /*printf("\tright_index= %d, i= %d, Top Right (%d, %d)\n", right_index, i, top_right.x, top_right.y);*/
                rect_count++;
                /*Block line of sight*/
                Point blocking = {.x = (top_right.x - origin->x),
                                  .y = (i - origin->y)};
                block_from(origin, &blocking, map->height, map->width, blocked);
            }
        }
    }

    *count += rect_count;

    /*Should we continue with next expansion?*/
    return not_all_blocked && !fully_expanded;
}
