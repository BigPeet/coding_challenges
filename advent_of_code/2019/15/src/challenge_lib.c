/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-17
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "stdio.h"
#include "time.h"

#ifdef NCURSES
#include "ncurses.h"
#include "unistd.h"
#endif

#define DISTANCE_OFFSET 16
#define TILE_MASK 3
#define RESIZE_AMOUNT 10

/*Forward declare*/
struct Planner;
typedef Direction (*plan_func)(struct Planner* const planner);

typedef enum {
    HIT_WALL = 0,
    MOVED    = 1,
    FOUND    = 2,
} Response;

typedef enum {
    UNKNOWN = 0,
    EMPTY   = 1,
    WALL    = 2,
    ROBOT   = 3,
} Tile;

typedef struct Planner
{
    Response last_resp;
    Direction last_dir;
    Overview* overview;
    plan_func plan;
    int has_been_seeded;
} Planner;


static void provide_input(const Robot* const robot, const int value);
static int read_output(const Robot* const robot);
static void update(Overview* const overview,
                   const Direction dir,
                   const int value);
static Response step(Overview* const overview, const Direction dir);
static Position move_pos(Overview* const overview,
                         const Position pos,
                         const Direction dir);
static void resize_overview(Overview* const overview, const Direction dir);
static char tile_to_char(const Tile tile);
static void display_overview(const Overview* const overview);

static Direction plan_next_dir(Planner* const planner);
static Direction random_direction(Planner* const planner);
static Direction seek_unknown(Planner* const planner);
static Direction reverse_direction(const Direction dir);

static int distance_from_to(Overview* const overview,
                            const Position from,
                            const Position to);
static void build_distance_map(Overview* const overview,
                               const int distance,
                               const int incoming_from);

static void set_distance(Overview* const overview,
                         const Position pos,
                         int16_t distance);
static int16_t get_distance(Overview* const overview, const Position pos);
static void set_tile(Overview* const overview,
                     const Position pos,
                     const Tile tile);
static Tile get_tile(Overview* const overview, const Position pos);

void* robot_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    printf("MASK: %d\n", DISTANCE_MASK);
    Robot* robot = (Robot*) args;
    int ret      = execute(robot->brain);
    if (ret != INT_CODE_HALT)
    {
        printf("Programm did not halt as expected. Err code: %d\n", ret);
    }
    else
    {
        /*Only writing access*/
        robot->finished = 1;
    }
    return NULL;
}

void* control_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    Overview* overview    = (Overview*) args;
    Direction current_dir = UP;

    Planner planner = {.has_been_seeded = 0,
                       .plan            = seek_unknown,
                       .last_resp       = MOVED,
                       .overview        = overview};

#ifdef NCURSES
    initscr();
    curs_set(0);
#endif

    /*Only reading access*/
    while (!overview->oxygen_found)
    {
        /*First Exploration*/

        planner.last_dir = current_dir;
        current_dir      = plan_next_dir(&planner);

        Response resp     = step(overview, current_dir);
        planner.last_resp = resp;
#ifdef NCURSES
        display_overview(overview);
#endif
    }

    int distance_to_oxygen =
        distance_from_to(overview, overview->oxygen_pos, overview->robot_start);


#ifdef NCURSES
    endwin();
#endif

    if (!overview->robot->finished)
    {
        /*Send invalid input to terminate robot program.*/
        provide_input(overview->robot, 0);
    }

    printf("Distance from Oxygen to Starting point: %d\n", distance_to_oxygen);
    print_overview(overview);
    return NULL;
}

void print_overview(const Overview* const overview)
{
    if ((overview != NULL) && (overview->area != NULL))
    {
        printf("Overview:\n");
        for (int i = 0; i < overview->height; ++i)
        {
            for (int j = 0; j < overview->width; ++j)
            {
                int index = (i * overview->width) + j;
                int tile  = overview->area[index] & TILE_MASK;
                char val  = tile_to_char(tile);
                printf("%c", val);
            }
            printf("\n");
        }
    }
}

static Position move_pos(Overview* const overview,
                         const Position pos,
                         const Direction dir)
{
    assert(overview != NULL);

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

    if ((new_pos.x < 0) || (new_pos.x >= overview->width) || (new_pos.y < 0) ||
        (new_pos.y >= overview->height))
    {
        /* New position outside of current area.
         * Increase area in the correct direction.*/
        resize_overview(overview, dir);

        /*Recalculate position after resizing*/
        int offset_x = (dir == LEFT) ? RESIZE_AMOUNT : 0;
        int offset_y = (dir == UP) ? RESIZE_AMOUNT : 0;
        new_pos.x += offset_x;
        new_pos.y += offset_y;
    }
    return new_pos;
}

static Response step(Overview* const overview, const Direction dir)
{
    /*Provide direction as input*/
    provide_input(overview->robot, dir + 1);

    /*Read Response*/
    Response resp = read_output(overview->robot);
    update(overview, dir, resp);

    return resp;
}

static void update(Overview* const overview,
                   const Direction dir,
                   const int value)
{
    assert(overview != NULL);
    assert(overview->robot != NULL);
    Robot* robot = overview->robot;

    Position new_pos = move_pos(overview, robot->pos, dir);
    if (value == HIT_WALL)
    {
        /*Mark field in the direction robot was moving as wall.*/
        set_tile(overview, new_pos, WALL);
    }

    if (value == MOVED || value == FOUND)
    {
        /*Mark old position as empty.*/
        set_tile(overview, robot->pos, EMPTY);

        /*Update robot's position.*/
        robot->pos = new_pos;
        set_tile(overview, robot->pos, ROBOT);

        if (value == FOUND)
        {
            overview->oxygen_found = 1;
            overview->oxygen_pos   = new_pos;
        }
    }
}

static void resize_overview(Overview* const overview, const Direction dir)
{
    int new_height = ((dir == UP) || (dir == DOWN))
                         ? overview->height + RESIZE_AMOUNT
                         : overview->height;
    int new_width = ((dir == RIGHT) || (dir == LEFT))
                        ? overview->width + RESIZE_AMOUNT
                        : overview->width;

    int* new_area = (int*) calloc(new_height, new_width * sizeof(int));
    if (new_area != NULL)
    {
        int start_y = (dir == UP) ? RESIZE_AMOUNT : 0;
        int start_x = (dir == LEFT) ? RESIZE_AMOUNT : 0;
        for (int i = 0; i < overview->height; ++i)
        {
            for (int j = 0; j < overview->width; ++j)
            {
                int old_index       = (i * overview->width) + j;
                int new_index       = ((i + start_y) * new_width) + j + start_x;
                new_area[new_index] = overview->area[old_index];
            }
        }
        overview->robot->pos.x += start_x;
        overview->robot->pos.y += start_y;
        overview->robot_start.x += start_x;
        overview->robot_start.y += start_y;
        free(overview->area);
        overview->area   = new_area;
        overview->height = new_height;
        overview->width  = new_width;
    }
}

static Direction plan_next_dir(Planner* const planner)
{
    assert(planner != NULL);

    /*TODO implement faster exploration strategy*/
    return planner->plan(planner);
}

static Direction random_direction(Planner* const planner)
{
    if (!planner->has_been_seeded)
    {
        srand(time(0));
        planner->has_been_seeded = 1;
    }

    /*Random choice*/
    return (Direction)(rand() % NUM_OF_DIRECTIONS);
}

static Direction seek_unknown(Planner* const planner)
{
    /*Is there an unknown tile?*/
    for (int d = 0; d < NUM_OF_DIRECTIONS; ++d)
    {
        Position p =
            move_pos(planner->overview, planner->overview->robot->pos, d);
        Tile t = get_tile(planner->overview, p);
        if (t == UNKNOWN)
        {
            return d;
        }
    }

    /*Move to random empty tile*/
    if (!planner->has_been_seeded)
    {
        srand(time(0));
        planner->has_been_seeded = 1;
    }

    Direction d = (Direction)(rand() % NUM_OF_DIRECTIONS);
    Position p  = move_pos(planner->overview, planner->overview->robot->pos, d);
    Tile t      = get_tile(planner->overview, p);
    while (t == WALL)
    {
        d = (Direction)(rand() % NUM_OF_DIRECTIONS);
        p = move_pos(planner->overview, planner->overview->robot->pos, d);
        t = get_tile(planner->overview, p);
    }

    return d;
}


static int distance_from_to(Overview* const overview,
                            const Position from,
                            const Position to)
{
    assert(overview != NULL);
    assert(overview->robot != NULL);
    assert(overview->area != NULL);

    /*Initialize distance map with -1.*/
    for (int i = 0; i < overview->height; ++i)
    {
        for (int j = 0; j < overview->width; ++j)
        {
            Position p = {.x = j, .y = i};
            set_distance(overview, p, -1);
        }
    }

    /*Move robot to from (currently not implemented)*/
    Position cur_pos = overview->robot->pos;
    if ((cur_pos.x == from.x) && (cur_pos.y == from.y))
    {
        build_distance_map(overview, 0, -1);
    }

    return get_distance(overview, to);
}

static void build_distance_map(Overview* const overview,
                               const int distance,
                               const int incoming_from)
{
    int16_t dist = distance;

    /*Short discovery*/
    for (int d = 0; d < NUM_OF_DIRECTIONS; ++d)
    {
        if (d == incoming_from)
        {
            continue;
        }
        Position p = move_pos(overview, overview->robot->pos, d);
        Tile t     = get_tile(overview, p);
        if (t == UNKNOWN)
        {
            Response resp = step(overview, d);
            if (resp != HIT_WALL)
            {
                step(overview, reverse_direction(d));
            }
            t = get_tile(overview, p);
        }
        if (t == WALL)
        {
            continue;
        }
        int16_t p_dist = get_distance(overview, p);
        if ((p_dist != -1) && ((p_dist + 1) < dist))
        {
            dist = p_dist + 1;
        }
    }

    /*Update distance value for this field.*/
    set_distance(overview, overview->robot->pos, dist);

    /*display_overview(overview);*/

    /*Propagate distance*/
    for (int d = 0; d < NUM_OF_DIRECTIONS; ++d)
    {
        if (d == incoming_from)
        {
            if (dist < distance)
            {
                /*distance has been updated, backpropagate.*/
                step(overview, d);
                build_distance_map(overview, dist + 1, reverse_direction(d));
                step(overview, reverse_direction(d));
            }
            continue;
        }
        Position p = move_pos(overview, overview->robot->pos, d);
        Tile t     = get_tile(overview, p);
        if (t == EMPTY)
        {
            int16_t p_dist = get_distance(overview, p);
            if ((p_dist == -1) || (p_dist > (dist + 1)))
            {
                step(overview, d);
                build_distance_map(overview, dist + 1, reverse_direction(d));
                step(overview, reverse_direction(d));
            }
        }
    }
}


static void display_overview(const Overview* const overview)
{
    if ((overview != NULL) && (overview->area != NULL))
    {
#ifdef NCURSES
        /*Assuming the curse screen has already been initialized.*/
        usleep(100);
        mvprintw(0, 0, "Overview:");
#else
        printf("Overview:");
#endif
        for (int i = 0; i < overview->height; ++i)
        {
            for (int j = 0; j < overview->width; ++j)
            {
                int index = (i * overview->width) + j;
                int tile  = overview->area[index] & TILE_MASK;
                char val  = tile_to_char(tile);
#ifdef NCURSES
                mvprintw(i + 1, j, "%c", val);
#else
                printf("%c", val);
#endif
            }
#ifndef NCURSES
            printf("\n");
#endif
        }
    }
#ifdef NCURSES
    refresh();
#endif
}

static void provide_input(const Robot* const robot, const int value)
{
    assert(robot != NULL);
    assert(robot->brain != NULL);
    assert(robot->brain->mem_io_in != NULL);

    intcode_io_mem_t* input = robot->brain->mem_io_in;
    pthread_mutex_lock(&input->mut);
    while (!input->consumed && !robot->finished)
    {
        pthread_cond_wait(&input->cond, &input->mut);
    }
    input->value    = value;
    input->consumed = 0;
    pthread_cond_signal(&input->cond);
    pthread_mutex_unlock(&input->mut);
}

static int read_output(const Robot* const robot)
{
    assert(robot != NULL);
    assert(robot->brain != NULL);
    assert(robot->brain->mem_io_out != NULL);

    int read_value = -1;

    intcode_io_mem_t* output = robot->brain->mem_io_out;
    pthread_mutex_lock(&output->mut);
    while (output->consumed && !robot->finished)
    {
        pthread_cond_wait(&output->cond, &output->mut);
    }

    if (!robot->finished)
    {
        read_value       = output->value;
        output->consumed = 1;
    }
    pthread_cond_signal(&output->cond);
    pthread_mutex_unlock(&output->mut);


    return read_value;
}

static char tile_to_char(const Tile tile)
{
    switch (tile)
    {
        case UNKNOWN:
            return '~';
        case WALL:
            return '#';
        case ROBOT:
            return 'o';
        case EMPTY:
            return ' ';
        default:
            return ' ';
    }
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

static void set_distance(Overview* const overview,
                         const Position pos,
                         int16_t distance)
{
    assert(overview != NULL);
    assert(overview->area != NULL);
    assert(pos.x >= 0);
    assert(pos.x < overview->width);
    assert(pos.y >= 0);
    assert(pos.x < overview->height);

    int index = (pos.y * overview->width) + pos.x;
    overview->area[index] =
        (overview->area[index] & TILE_MASK) | (distance << DISTANCE_OFFSET);
}

static int16_t get_distance(Overview* const overview, const Position pos)
{
    assert(overview != NULL);
    assert(overview->area != NULL);
    assert(pos.x >= 0);
    assert(pos.x < overview->width);
    assert(pos.y >= 0);
    assert(pos.x < overview->height);

    int index = (pos.y * overview->width) + pos.x;
    return (int16_t)(overview->area[index] >> DISTANCE_OFFSET);
}

static void set_tile(Overview* const overview,
                     const Position pos,
                     const Tile tile)
{
    assert(overview != NULL);
    assert(overview->area != NULL);
    assert(pos.x >= 0);
    assert(pos.x < overview->width);
    assert(pos.y >= 0);
    assert(pos.x < overview->height);
    int index = (pos.y * overview->width) + pos.x;

    overview->area[index] = (overview->area[index] & ~TILE_MASK) | tile;
}

static Tile get_tile(Overview* const overview, const Position pos)
{
    assert(overview != NULL);
    assert(overview->area != NULL);
    assert(pos.x >= 0);
    assert(pos.x < overview->width);
    assert(pos.y >= 0);
    assert(pos.x < overview->height);
    int index = (pos.y * overview->width) + pos.x;

    return overview->area[index] & TILE_MASK;
}
