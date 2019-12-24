/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-19
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef enum {
    UP    = 0,
    DOWN  = 1,
    RIGHT = 2,
    LEFT  = 3,
} Direction;
#define NUM_OF_DIRECTIONS 4

#define EMPTY '.'
#define WALL '#'
#define ENTRANCE '@'


typedef struct
{
    char* id;
    Key* next;
    int steps;
} Solution;

typedef struct
{
    Solution** solutions;
    int num_solutions;
    int capacity;
} SolutionStorage;

static Overview* create_overview(const int width,
                                 const int height,
                                 const int num_keys,
                                 const int num_doors);
void destroy_overview(Overview* const overview);
static SolutionStorage* create_storage();
static Map* create_map(const int width, const int height);
static void destroy_map(Map* const map);
static void pickup(Key* const key);
static int* distance_map(const Map* const map,
                         const Position pos,
                         Door** const doors,
                         const int num_doors);
static void build_distance_map_rec(const Map* const map,
                                   Door** const doors,
                                   const int num_doors,
                                   const Position pos,
                                   const int distance,
                                   const int incoming_from,
                                   int* const distance_map);
static Position move_pos(const Position pos, const Direction dir);
static char get_value(const Map* const map, const Position pos);
static int is_locked(Door** const doors,
                     const int num_doors,
                     const Position pos);
static Direction reverse_direction(const Direction dir);
static int is_reachable(const int* dist_map,
                        const int width,
                        const int height,
                        const Position pos);
static int minimal_steps_rec(const Map* const map,
                             const Position pos,
                             SolutionStorage* const storage,
                             Solution* const current_solution,
                             Key** const keys,
                             Door** const doors,
                             const int count,
                             const int num_keys,
                             const int num_doors);
static Door** copy_doors(Door** const doors, const int num_doors);
static Key** copy_keys(Key** const keys,
                       Door** const doors,
                       const int num_keys,
                       const int num_doors);
static SolutionStorage* create_storage();
static Solution* create_solution(const int num_keys, const int steps, Key* const next);
static void store_solution(SolutionStorage* const storage, Solution* solution);
static int stored_value(const SolutionStorage* const storage,
                        const Solution* const solution);
static void destroy_solution(Solution* const solution);
static void destroy_storage(SolutionStorage* const storage);
static void set_keys_of_solution(Solution* const solution,
                                 Key** const keys,
                                 const int num_keys);
static Solution* remove_key_from_solution(const Solution* const solution,
                                          const Key* const key,
                                          const int num_keys);

Overview* read_input(const char* const file_path,
                     const int width,
                     const int height,
                     const int num_keys,
                     const int num_doors)
{
    Overview* overview = NULL;
    if (file_path == NULL)
    {
        return NULL;
    }

    overview = create_overview(width, height, num_keys, num_doors);

    if (overview == NULL)
    {
        return NULL;
    }

    FILE* fp = fopen(file_path, "r");

    if (fp == NULL)
    {
        destroy_overview(overview);
        return NULL;
    }

    int y          = 0;
    int x          = 0;
    int key_index  = 0;
    int door_index = 0;
    while (!feof(fp))
    {
        char c = fgetc(fp);
        if (c == '\n')
        {
            y++;
            x = 0;
        }
        else
        {
            if ((c >= 65) && (c <= 90))
            {
                /*Door*/
                Door* d = (Door*) malloc(sizeof(Door));
                if (d != NULL)
                {
                    d->id  = tolower(c);
                    d->pos = (Position){.x = x, .y = y};
                    d->opened                     = 0;
                    overview->doors[door_index++] = d;
                }
            }
            else if ((c >= 97) && (c <= 122))
            {
                /*Key*/
                Key* k = (Key*) malloc(sizeof(Key));
                if (k != NULL)
                {
                    k->id  = c;
                    k->pos = (Position){.x = x, .y = y};
                    k->picked_up                = 0;
                    k->door                     = NULL;
                    overview->keys[key_index++] = k;
                }
            }
            else if (c == ENTRANCE)
            {
                overview->entrance = (Position){.x = x, .y = y};
            }
            /*Save value in data. (maybe transform it)*/

            if (y < height && x < width)
            {
                int index                  = (y * width) + x;
                overview->map->data[index] = c;
            }

            x++;
        }
    }
    fclose(fp);

    /*Link keys to doors*/
    for (int i = 0; i < num_keys; ++i)
    {
        Key* k = overview->keys[i];
        for (int j = 0; j < num_doors; j++)
        {
            Door* d = overview->doors[j];
            if (d->id == k->id)
            {
                k->door = d;
                break;
            }
        }
    }
    return overview;
}

void destroy_overview(Overview* const overview)
{
    if (overview != NULL)
    {
        destroy_map(overview->map);
        if (overview->doors != NULL)
        {
            for (int i = 0; i < overview->num_doors; ++i)
            {
                if (overview->doors[i] != NULL)
                {
                    free(overview->doors[i]);
                }
            }
            free(overview->doors);
        }
        if (overview->keys != NULL)
        {
            for (int i = 0; i < overview->num_keys; ++i)
            {
                if (overview->keys[i] != NULL)
                {
                    free(overview->keys[i]);
                }
            }
            free(overview->keys);
        }
        free(overview);
    }
}

int minimal_steps(const Overview* const overview)
{
    int step_count = 0;
    if ((overview != NULL) && (overview->map != NULL) &&
        (overview->doors != NULL) && (overview->keys != NULL))
    {
        SolutionStorage* storage = create_storage();
        Solution* solution       = create_solution(overview->num_keys, -1, NULL);
        set_keys_of_solution(solution, overview->keys, overview->num_keys);
        step_count               = minimal_steps_rec(overview->map,
                                       overview->entrance,
                                       storage,
                                       solution,
                                       overview->keys,
                                       overview->doors,
                                       0,
                                       overview->num_keys,
                                       overview->num_doors);
        destroy_storage(storage);
    }
    return step_count;
}

static int minimal_steps_rec(const Map* const map,
                             const Position pos,
                             SolutionStorage* const storage,
                             Solution* const current_solution,
                             Key** const keys,
                             Door** const doors,
                             const int count,
                             const int num_keys,
                             const int num_doors)
{
    int step_count = 0;
    int distances[num_keys];

    /*Apparently, optimization is needed here for larger key-door numbers.*/
    /*A solution contains the set of remaining keys and info about the next key.*/
    int steps = stored_value(storage, current_solution);
    if (steps != -1)
    {
        printf(
            "Found prior solution for %s: %d\n", current_solution->id, steps);
        /*destroy_solution(current_solution);*/
        return steps + count;
    }

    int* dist_map       = distance_map(map, pos, doors, num_doors);
    int reachable_count = 0;
    for (int i = 0; i < num_keys; ++i)
    {
        Key* k       = keys[i];
        distances[i] = dist_map[(k->pos.y * map->width) + k->pos.x];
        if (distances[i] != -1 && !k->picked_up)
        {
            reachable_count++;
        }
    }
    free(dist_map);

    if (reachable_count > 0)
    {
        Key* next = NULL;
        step_count = __INT_MAX__;
        for (int i = 0; i < num_keys; ++i)
        {
            if ((distances[i] != -1) && (!keys[i]->picked_up))
            {

                /*TODO fix the number of saved steps.
                 * Instead of storing the total amount, the solutions need to save the remaining amoung.
                 * E.g. if there is only 1 key left, the the stored amount should be 0. Additionally the position of the "next" key needs to be stored.
                 * The result is then stored amount + the distance from the last key to the next key*/

                Door** door_cpys = copy_doors(doors, num_doors);
                Key** key_cpys =
                    copy_keys(keys, door_cpys, num_keys, num_doors);
                pickup(key_cpys[i]);
                Solution* new_solution = remove_key_from_solution(
                    current_solution, key_cpys[i], num_keys);
                int key_step_count = minimal_steps_rec(map,
                                                       keys[i]->pos,
                                                       storage,
                                                       new_solution,
                                                       key_cpys,
                                                       door_cpys,
                                                       count + distances[i],
                                                       num_keys,
                                                       num_doors);
                key_step_count += distances[i];
                if (key_step_count < step_count)
                {
                    step_count = key_step_count;
                    next = key_cpys[i];
                }

                /*Clean up*/
                for (int j = 0; j < num_doors; ++j)
                {
                    free(door_cpys[j]);
                }
                for (int j = 0; j < num_keys; ++j)
                {
                    /*free(key_cpys[j]);*/
                }
                free(door_cpys);
                /*free(key_cpys);*/
            }
        }
    current_solution->steps = step_count;
    current_solution->next = next;
    printf("Store solution for %c -> %s: %d\n",
        current_solution->next->id,
           current_solution->id,
           current_solution->steps);
    store_solution(storage, current_solution);
    }


    return step_count;
}

static Door** copy_doors(Door** const doors, const int num_doors)
{
    assert(doors != NULL);
    Door** copy = (Door**) malloc(sizeof(Door*) * num_doors);

    if (copy != NULL)
    {
        for (int i = 0; i < num_doors; ++i)
        {
            Door* d = (Door*) malloc(sizeof(Door));
            if (d != NULL)
            {
                d->id     = doors[i]->id;
                d->opened = doors[i]->opened;
                d->pos    = doors[i]->pos;
            }
            copy[i] = d;
        }
    }
    return copy;
}

static Key** copy_keys(Key** const keys,
                       Door** const doors,
                       const int num_keys,
                       const int num_doors)
{
    assert(keys != NULL);
    Key** copy = (Key**) malloc(sizeof(Key*) * num_keys);

    if (copy != NULL)
    {
        for (int i = 0; i < num_keys; ++i)
        {
            Key* k = (Key*) malloc(sizeof(Key));
            if (k != NULL)
            {
                k->id        = keys[i]->id;
                k->picked_up = keys[i]->picked_up;
                k->pos       = keys[i]->pos;
                k->door      = NULL;
                for (int j = 0; j < num_doors; ++j)
                {
                    Door* d = doors[j];
                    if (d != NULL && d->id == k->id)
                    {
                        k->door = d;
                        break;
                    }
                }
            }
            copy[i] = k;
        }
    }
    return copy;
}

static int is_reachable(const int* dist_map,
                        const int width,
                        const int height,
                        const Position pos)
{
    int reachable = 0;
    if ((pos.y >= 0) && (pos.y < height) && (pos.x >= 0) && (pos.x < width))
    {
        reachable = (dist_map[(pos.y * width) + pos.x] > -1);
    }
    return reachable;
}

static Overview* create_overview(const int width,
                                 const int height,
                                 const int num_keys,
                                 const int num_doors)
{
    Map* map = create_map(width, height);
    if (map == NULL)
    {
        return NULL;
    }

    Overview* overview = (Overview*) malloc(sizeof(Overview));
    if (overview == NULL)
    {
        destroy_map(map);
        return NULL;
    }

    Door** doors = (Door**) malloc(sizeof(Door*) * num_doors);
    Key** keys   = (Key**) malloc(sizeof(Key*) * num_keys);

    overview->map       = map;
    overview->doors     = doors;
    overview->keys      = keys;
    overview->num_keys  = num_keys;
    overview->num_doors = num_doors;
    return overview;
}

static Map* create_map(const int width, const int height)
{
    Map* map = (Map*) malloc(sizeof(Map));

    if (map != NULL)
    {
        map->data   = (char*) malloc(sizeof(char) * width * height);
        map->width  = width;
        map->height = height;
    }
    return map;
}

static void destroy_map(Map* const map)
{
    if (map != NULL)
    {
        if (map->data != NULL)
        {
            free(map->data);
        }
        free(map);
    }
}

static void pickup(Key* const key)
{
    if (key != NULL)
    {
        key->picked_up = 1;
        if (key->door != NULL)
        {
            key->door->opened = 1;
        }
    }
}

static int* distance_map(const Map* const map,
                         const Position pos,
                         Door** const doors,
                         const int num_doors)
{
    assert(map != NULL);

    int* dist_map = (int*) malloc(sizeof(int) * map->height * map->width);

    if (dist_map != NULL)
    {
        /*Initialize distance map with -1.*/
        for (int i = 0; i < map->height; ++i)
        {
            for (int j = 0; j < map->width; ++j)
            {
                dist_map[(i * map->width) + j] = -1;
            }
        }
        build_distance_map_rec(map, doors, num_doors, pos, 0, -1, dist_map);
    }
    return dist_map;
}

static int is_locked(Door** const doors,
                     const int num_doors,
                     const Position pos)
{
    int is_locked = 0;
    for (int i = 0; i < num_doors; ++i)
    {
        Door* d = doors[i];
        if ((!d->opened) && (d->pos.x == pos.x) && (d->pos.y == pos.y))
        {
            is_locked = 1;
            break;
        }
    }
    return is_locked;
}

static void build_distance_map_rec(const Map* const map,
                                   Door** const doors,
                                   const int num_doors,
                                   const Position pos,
                                   const int distance,
                                   const int incoming_from,
                                   int* const distance_map)
{
    int dist = distance;

    /*Short discovery*/
    for (int d = 0; d < NUM_OF_DIRECTIONS; ++d)
    {
        if (d == incoming_from)
        {
            continue;
        }
        Position p = move_pos(pos, d);
        char c     = get_value(map, p);
        if ((c == '#') || (is_locked(doors, num_doors, p)))
        {
            continue;
        }
        else if (c != -1)
        {
            int p_dist = distance_map[(p.y * map->width) + p.x];
            if ((p_dist != -1) && ((p_dist + 1) < dist))
            {
                dist = p_dist + 1;
            }
        }
    }

    /*Update distance value for this field.*/
    distance_map[(pos.y * map->width) + pos.x] = dist;

    /*Propagate distance*/
    for (int d = 0; d < NUM_OF_DIRECTIONS; ++d)
    {
        Position p = move_pos(pos, d);
        if (d == incoming_from)
        {
            if (dist < distance)
            {
                /*distance has been updated, backpropagate.*/
                build_distance_map_rec(map,
                                       doors,
                                       num_doors,
                                       p,
                                       dist + 1,
                                       reverse_direction(d),
                                       distance_map);
            }
            continue;
        }
        char c = get_value(map, p);
        if (c != WALL)
        {
            if ((c == EMPTY) || (c == ENTRANCE) ||
                (!is_locked(doors, num_doors, p)))
            {
                int p_dist = distance_map[(p.y * map->width) + p.x];
                if ((p_dist == -1) || (p_dist > (dist + 1)))
                {
                    build_distance_map_rec(map,
                                           doors,
                                           num_doors,
                                           p,
                                           dist + 1,
                                           reverse_direction(d),
                                           distance_map);
                }
            }
        }
    }
}

static Position move_pos(const Position pos, const Direction dir)
{
    Position new_pos;
    new_pos.x = pos.x;
    new_pos.y = pos.y;
    switch (dir)
    {
        case UP:
            new_pos.y -= 1;
            break;
        case RIGHT:
            new_pos.x += 1;
            break;
        case DOWN:
            new_pos.y += 1;
            break;
        case LEFT:
            new_pos.x -= 1;
            break;
        default:
            break;
    }
    return new_pos;
}

static char get_value(const Map* const map, const Position pos)
{
    char val = -1;
    if (map != NULL)
    {
        if ((pos.x < map->width) && (pos.x >= 0) && (pos.y < map->height) &&
            (pos.y >= 0))
        {
            val = map->data[(pos.y * map->width) + pos.x];
        }
    }
    return val;
}

static Direction reverse_direction(const Direction dir)
{
    switch (dir)
    {
        case UP:
            return DOWN;
        case DOWN:
            return UP;
        case LEFT:
            return RIGHT;
        case RIGHT:
            return LEFT;
    }
}

void print_map(const Map* const map)
{
    if (map != NULL && map->data != NULL)
    {
        for (int i = 0; i < map->height; ++i)
        {
            for (int j = 0; j < map->width; ++j)
            {
                printf("%c", map->data[(i * map->width) + j]);
            }
            printf("\n");
        }
    }
}

static SolutionStorage* create_storage()
{
    SolutionStorage* storage =
        (SolutionStorage*) malloc(sizeof(SolutionStorage));
    if (storage != NULL)
    {
        storage->num_solutions = 0;
        storage->capacity      = 10;
        storage->solutions     = (Solution**) malloc(sizeof(Solution*) * 10);
    }
    return storage;
}

static Solution* create_solution(const int num_keys, const int steps, Key* const next)
{
    Solution* solution = (Solution*) malloc(sizeof(Solution));
    if (solution != NULL)
    {
        char* id = (char*) malloc(sizeof(char) * num_keys + 1);
        if (id != NULL)
        {
            id[0] = '\0';
        }
        solution->id    = id;
        solution->steps = steps;
        solution->next  = next;
    }
    return solution;
}

static void set_keys_of_solution(Solution* const solution,
                                 Key** const keys,
                                 const int num_keys)
{
    assert(solution != NULL);
    assert(keys != NULL);

    int length = 0;
    for (int i = 0; i < num_keys; ++i)
    {
        solution->id[i] = keys[i]->id;
        length++;
    }
    solution->id[length] = '\0';
}

static Solution* remove_key_from_solution(const Solution* const solution,
                                          const Key* const key,
                                          const int num_keys)
{
    Solution* removed = create_solution(num_keys, -1, NULL);
    if (solution != NULL && removed != NULL)
    {
        strcpy(removed->id, solution->id);
        int prev_length = strlen(removed->id);
        int found       = 0;
        for (int i = 0; i < prev_length; ++i)
        {
            if (removed->id[i] == key->id)
            {
                found = 1;
            }
            if (found)
            {
                removed->id[i] = removed->id[i + 1];
            }
        }
    }
    return removed;
}

static void store_solution(SolutionStorage* const storage, Solution* solution)
{
    assert(storage != NULL);
    assert(storage->solutions != NULL);
    assert(solution != NULL);

    storage->solutions[storage->num_solutions++] = solution;

    if (storage->num_solutions == storage->capacity)
    {
        storage->capacity *= 2;
        storage->solutions = (Solution**) realloc(
            storage->solutions, sizeof(Solution*) * storage->capacity);
    }
}

static int stored_value(const SolutionStorage* const storage,
                        const Solution* const solution)
{
    assert(storage != NULL);
    assert(storage->solutions != NULL);

    if (solution != NULL)
    {
        for (int i = 0; i < storage->num_solutions; ++i)
        {
            assert(storage->solutions[i] != NULL);
            if ((strcmp(storage->solutions[i]->id, solution->id) == 0) && (storage->solutions[i]->next == solution->next))
            {
                return storage->solutions[i]->steps;
            }
        }
    }
    return -1;
}

static void destroy_solution(Solution* const solution)
{
    if (solution != NULL)
    {
        if (solution->id != NULL)
        {
            free(solution->id);
        }
        free(solution);
    }
}

static void destroy_storage(SolutionStorage* const storage)
{
    if (storage != NULL)
    {
        if (storage->solutions != NULL)
        {
            for (int i = 0; i < storage->num_solutions; ++i)
            {
                destroy_solution(storage->solutions[i]);
            }
            free(storage->solutions);
        }
        free(storage);
    }
}
