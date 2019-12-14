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
                           int* const counted,
                           int* const blocked,
                           int* const count);
static void block_from(const Point* const origin,
                       const Point* const blocking,
                       const int height,
                       const int width,
                       int* const blocked);
static void reduce(int* const a, int* const b);
static int count_asteroids(const Map* const map);
static double point_slope(const Point* const p);
static int compare_points(const void* const a, const void* const b);

static int max(int a, int b)
{
    return (a > b) ? a : b;
}

static int min(int a, int b)
{
    return (a < b) ? a : b;
}

static void rotate_counter_clockwise(Point* const p, int amount);
static void rotate_clockwise(Point* const p, int amount);

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
                    Point p   = {.x = col, .y = row};
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
        int* blocked = (int*)calloc(map->height, map->width * sizeof(int));
        if (blocked == NULL)
        {
            return 0;
        }
        int* counted = (int*)calloc(map->height, map->width * sizeof(int));
        if (counted == NULL)
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
            ret = check_rectangle(ast_pos, steps, map, counted, blocked, &count);
            /*printf("Step %d, currently at %d asteroids\n", steps, count);*/
            steps++;
        }
        free(counted);
        free(blocked);
    }
    return count;
}

void get_blocked_map(const Map* const map, const Point* const origin, int* blocked)
{
    assert(map != NULL);
    assert(origin != NULL);
    assert(blocked != NULL);

    for (int i = 0; i < map->height; ++i)
    {
        for (int j = 0; j < map->width; ++j)
        {
            blocked[(i * map->width) + j] = 0;
        }
    }

    blocked[(origin->y * map->width) + origin->x] = 1;

    /*Quick and dirty reuse of other function*/
    int ret      = 1;
    int steps    = 1;
    int count    = 0;
    int* counted = (int*)calloc(map->height, map->width * sizeof(int));
    if (counted == NULL)
    {
        return;
    }
    while (ret)
    {
        ret = check_rectangle(origin, steps, map, counted, blocked, &count);
        steps++;
    }
    free(counted);
}

void fire_laser(Map* const map, const Point* const laser_pos)
{
    /*Fire laser until only the asteroid the laser is mounted on is left.*/

    if ((map != NULL) && (laser_pos != NULL))
    {
        int rotation = 0;
        /*TODO prevent overlapping*/
        Point corners[4][2] = {
          (Point){.x = laser_pos->x, .y = min(0, laser_pos->y - 1)},
          (Point){.x = max(map->width - 1, laser_pos->x + 1), .y = laser_pos->y - 1},

          (Point){.x = max(map->width - 1, laser_pos->x + 1), .y = laser_pos->y},
          (Point){.x = laser_pos->x + 1, .y = max(map->height - 1, laser_pos->y + 1)},

          (Point){.x = laser_pos->x, .y = max(map->height - 1, laser_pos->y + 1)},
          (Point){.x = min(0, laser_pos->x - 1), .y = laser_pos->y + 1},

          (Point){.x = min(0, laser_pos->x - 1), .y = laser_pos->y},
          (Point){.x = laser_pos->x - 1, .y = min(0, laser_pos->y - 1)},
        };

        int vaporize_count = 0;
        int* blocked       = (int*)calloc(map->height, map->width * sizeof(int));
        if (blocked == NULL)
        {
            return;
        }

        while (count_asteroids(map) > 1)
        {
            /*printf("Remaining asteroids: %d\n", count_asteroids(map));*/
            /*printf("Starting %d. rotation.\n", (rotation + 1));*/
            /*Look at the quadrants of the map individually First top left, then bottom left,*/
            /*bottom right and finally top right.*/

            get_blocked_map(map, laser_pos, blocked);
            /*printf("Blocked:\n");*/
            /*for (int row = 0; row < map->height; ++row)*/
            /*{*/
            /*for (int col = 0; col < map->width; col++)*/
            /*{*/
            /*printf("%d ", blocked[(row * map->width) + col]);*/
            /*}*/
            /*printf("\n");*/
            /*}*/
            /*printf("Map:\n");*/
            /*print_map(map);*/

            for (int q = 0; q < 4; ++q)
            {
                /*TODO: rotate*/
                Point start = corners[q][0];
                Point end   = corners[q][1];
                Point inc   = {.x = 1, .y = 1};
                rotate_clockwise(&inc, q);

                /*Maximum possible asteroids to be found in this quadrant.*/
                int astcount      = 0;
                int max_asteroids = (abs(end.y - start.y + 1) * abs(end.x - start.x + 1));
                Point asteroids[max_asteroids];
                /*printf("Max %d fields.\n", max_asteroids);*/

                /*printf("Start (%d, %d), End (%d, %d) and Inc (%d, %d).\n",*/
                /*start.x,*/
                /*start.y,*/
                /*end.x,*/
                /*end.y,*/
                /*inc.x,*/
                /*inc.y);*/
                /*Iterate over quadrant, extract visible asteroids.*/
                /*calculate slope of visible asteroids and sort in ascending order*/
                /*remove asteroids in that order from map*/

                for (int row = start.y; (row >= 0) && (row < map->height); row += inc.y)
                {
                    for (int col = start.x; (col >= 0) && (col < map->width); col += inc.x)
                    {
                        Point check = {.x = col, .y = row};
                        int index   = (check.y * map->width) + check.x;
                        /*printf("Check (%d, %d) (index=%d): ", check.x, check.y, index);*/
                        if (!blocked[index] && map->data[index] == 1)
                        {
                            /*printf("Hit\n");*/
                            Point ast             = {.x = (check.x - laser_pos->x),
                                         .y = (check.y - laser_pos->y)};
                            asteroids[astcount++] = ast;
                        }
                        /*else*/
                        /*{*/
                        /*printf(*/
                        /*"empty=(%d) or blocked=(%d)\n", map->data[index], blocked[index]);*/
                        /*}*/
                        if (col == end.x)
                        {
                            break;
                        }
                    }
                    if (row == end.y)
                    {
                        break;
                    }
                }

                /*free(blocked);*/

                /*printf("Found %d asteroids in quadrant %d\n", astcount, q + 1);*/
                for (int i = 0; i < astcount; ++i)
                {
                    rotate_counter_clockwise(&asteroids[i], q);
                }
                qsort(asteroids, astcount, sizeof(Point), compare_points);
                for (int i = 0; i < astcount; ++i)
                {
                    rotate_clockwise(&asteroids[i], q);
                }
                for (int i = 0; i < astcount; ++i)
                {
                    printf("Asteroid %d: (%d, %d)\n", i + 1, asteroids[i].x, asteroids[i].y);
                    Point ast                               = {.x = (laser_pos->x + asteroids[i].x),
                                 .y = (laser_pos->y + asteroids[i].y)};
                    map->data[(ast.y * map->width) + ast.x] = 0;
                    vaporize_count++;
                    /*if (vaporize_count < 4 || vaporize_count == 10 || vaporize_count == 20 ||*/
                        /*vaporize_count == 50 || vaporize_count == 100 || vaporize_count == 199 ||*/
                        /*vaporize_count == 200 || vaporize_count == 201 || vaporize_count == 299)*/
                    /*{*/
                        printf("The %d. asteroid to be vaporized is at (%d, %d)\n",
                               vaporize_count,
                               ast.x,
                               ast.y);
                    /*}*/
                }
            }

            rotation++;
        }
    }
}

static int count_asteroids(const Map* const map)
{
    assert(map != NULL);
    int count = 0;
    for (int i = 0; i < map->height; ++i)
    {
        for (int j = 0; j < map->width; ++j)
        {
            if (map->data[(i * map->width) + j] == 1)
            {
                count++;
            }
        }
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
        index += inc;
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
        index += inc;
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
        int col_inc   = (blocking->x > 0) ? 1 : -1;
        int row_inc   = (blocking->y > 0) ? 1 : -1;
        col_index += col_inc;
        row_index += row_inc;

        while ((col_index >= 0) && (col_index < height) && (row_index >= 0) && (row_index < width))
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
        reduce(&row_inc, &col_inc);
        col_index += col_inc;
        row_index += row_inc;
        while ((col_index >= 0) && (col_index < height) && (row_index >= 0) && (row_index < width))
        {
            /*printf("\tFancy Blocking (%d, %d)\n", col_index, row_index);*/
            blocked[(row_index * width) + col_index] = 1;
            col_index += col_inc;
            row_index += row_inc;
        }
    }
}

static void reduce(int* const a, int* const b)
{
    /*Find largest common denominator*/
    int d = 1;
    for (int i = d + 1; i <= abs(*a) && i <= abs(*b); ++i)
    {
        if ((*a % i == 0) && (*b % i == 0))
        {
            d = i;
        }
    }
    *a = *a / d;
    *b = *b / d;
}


static int check_rectangle(const Point* const origin,
                           const int steps,
                           const Map* const map,
                           int* const counted,
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

    int fully_expanded =
      (left == 0) && (right == map->width - 1) && (top == 0) && (bottom == map->height - 1);

    Point top_left     = {.x = left, .y = top};
    Point top_right    = {.x = right, .y = top};
    Point bottom_left  = {.x = left, .y = bottom};
    Point bottom_right = {.x = right, .y = bottom};

    /*left to right*/
    /*printf("\t Left to Right\n");*/
    for (int i = top_left.x; i <= top_right.x; i++)
    {
        int top_index = (top_left.y * map->width) + i;
        if (!blocked[top_index] && !counted[top_index])
        {
            not_all_blocked = 1;
            int value       = map->data[top_index];
            if (value == 1)
            {
                /*printf("\tFound visible asteroid at (%d, %d)\n", i, top_left.y);*/
                rect_count++;
                /*Block line of sight*/
                Point blocking = {.x = (i - origin->x), .y = (top_left.y - origin->y)};
                block_from(origin, &blocking, map->height, map->width, blocked);
                counted[top_index] = 1;
            }
        }
        int bottom_index = (bottom_left.y * map->width) + i;
        if (!blocked[bottom_index] && !counted[bottom_index])
        {
            not_all_blocked = 1;
            int value       = map->data[bottom_index];
            if (value == 1)
            {
                /*printf("\tFound visible asteroid at (%d, %d)\n", i, bottom_left.y);*/
                rect_count++;
                /*Block line of sight*/
                Point blocking = {.x = (i - origin->x), .y = (bottom_left.y - origin->y)};
                block_from(origin, &blocking, map->height, map->width, blocked);
                counted[bottom_index] = 1;
            }
        }
    }

    /*top to bottom*/
    /*printf("\t Top to bottom\n");*/
    for (int i = top_left.y + 1; i <= bottom_left.y - 1; i++)
    {
        int left_index = (i * map->width) + top_left.x;
        if (!blocked[left_index] && !counted[left_index])
        {
            not_all_blocked = 1;
            int value       = map->data[left_index];
            if (value == 1)
            {
                /*printf("\tFound visible asteroid at (%d, %d)\n", top_left.x, i);*/
                rect_count++;
                /*Block line of sight*/
                Point blocking = {.x = (top_left.x - origin->x), .y = (i - origin->y)};
                block_from(origin, &blocking, map->height, map->width, blocked);
                counted[left_index] = 1;
            }
        }
        int right_index = (i * map->width) + top_right.x;
        if (!blocked[right_index] && !counted[right_index])
        {
            not_all_blocked = 1;
            int value       = map->data[right_index];
            if (value == 1)
            {
                /*printf("\tFound visible asteroid at (%d, %d)\n", top_right.x, i);*/
                /*printf("\tright_index= %d, i= %d, Top Right (%d, %d)\n", right_index, i,
                 * top_right.x, top_right.y);*/
                rect_count++;
                /*Block line of sight*/
                Point blocking = {.x = (top_right.x - origin->x), .y = (i - origin->y)};
                block_from(origin, &blocking, map->height, map->width, blocked);
                counted[right_index] = 1;
            }
        }
    }

    *count += rect_count;

    /*Should we continue with next expansion?*/
    return not_all_blocked && !fully_expanded;
}

static void rotate_clockwise(Point* const p, int amount)
{
    assert(p != NULL);
    amount = amount % 4;
    for (int i = 0; i < amount; i++)
    {
        int tmp = p->x;
        p->y *= -1;
        p->x = p->y;
        p->y = tmp;
    }
}
static void rotate_counter_clockwise(Point* const p, int amount)
{
    assert(p != NULL);
    amount = amount % 4;
    for (int i = 0; i < amount; i++)
    {
        int tmp = p->y;
        p->x *= -1;
        p->y = p->x;
        p->x = tmp;
    }
}

static double point_slope(const Point* const p)
{
    assert(p != NULL);
    if (p->x == 0)
    {
        return -__DBL_MAX__;
    }
    else
    {
        return p->y / p->x;
    }
}
static int compare_points(const void* const a, const void* const b)
{
    Point* pa   = (Point*)a;
    Point* pb   = (Point*)b;
    double diff = point_slope(pa) - point_slope(pb);
    if (diff > 0.0)
    {
        return 1;
    }
    if (diff < 0.0)
    {
        return -1;
    }
    return 0;
}
