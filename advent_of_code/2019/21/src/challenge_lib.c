/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-05-07
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"

#ifdef NCURSES
#include "ncurses.h"
#endif

#define RESIZE_AMOUNT 10

static void provide_input(const ASCII* const system, const int value);
static int64_t read_output(const ASCII* const system);
static void update(Overview* const overview, const int64_t value);
static void resize_overview(Overview* const overview, const Direction dir);
static void display_overview(const Overview* const overview);
static Position* find_scaff_intersections(const Overview* const overview,
                                          int* const num_intersections);
static int alignment_parameter(const Position pos);

static void provide_main_routine(const ASCII* const system);
static void provide_function_A(const ASCII* const system);
static void provide_function_B(const ASCII* const system);
static void provide_function_C(const ASCII* const system);
static void provide_line(const ASCII* const system, const char* const line);

void* system_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    ASCII* system = (ASCII*) args;
    int ret       = execute(system->brain);
    if (ret != INT_CODE_HALT)
    {
        printf("Programm did not halt as expected. Err code: %d\n", ret);
    }
    else
    {
        /*Only writing access*/
        usleep(1000);
        system->finished = 1;
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

#ifdef NCURSES
    initscr();
    curs_set(0);
#endif

    /*Give robot some time to setup.*/
    usleep(3000);

    /*Only reading access*/
    while (!waiting_for_input(overview->system->brain) &&
           !overview->system->finished)
    {
        /*Read Response*/
        int64_t resp = read_output(overview->system);
        update(overview, resp);
#ifdef NCURSES
        display_overview(overview);
#endif
        usleep(10);
    }

    /*The area should be initialized now.*/
    /*The current output position should now be at the bottom right.*/
    /*We can therefore "lock in" the area.*/
    overview->height = overview->output_pos.y - 1;
    overview->initialized  = 1;

    if (overview->system->finished)
    {
        /*This is basically part 01.*/

#ifdef NCURSES
        endwin();
#endif
        int num_intersections = 0;
        Position* scaff_intersections =
            find_scaff_intersections(overview, &num_intersections);

        print_overview(overview);

        int sum = 0;
        printf("Found %d of intersections.\n", num_intersections);
        for (int i = 0; i < num_intersections; ++i)
        {
            int alignment_p = alignment_parameter(scaff_intersections[i]);
            printf("%d. Intersection at (%d, %d). Alignment Parameter: %d\n",
                   i + 1,
                   scaff_intersections[i].x,
                   scaff_intersections[i].y,
                   alignment_p);
            sum += alignment_p;
        }
        printf("Sum of all alignment parameters: %d\n", sum);
    }
    else
    {
        /*Robot is expecting input. Indicating Part 02.*/
        while (!waiting_for_input(overview->system->brain))
        {
            int64_t resp = read_output(overview->system);
            usleep(10);
        }

        /*Provide functions (hard coded, solved in python)*/
        provide_main_routine(overview->system);
        while (!waiting_for_input(overview->system->brain))
        {
            int64_t resp = read_output(overview->system);
            usleep(10);
        }
        provide_function_A(overview->system);
        while (!waiting_for_input(overview->system->brain))
        {
            int64_t resp = read_output(overview->system);
            usleep(10);
        }
        provide_function_B(overview->system);
        while (!waiting_for_input(overview->system->brain))
        {
            int64_t resp = read_output(overview->system);
            usleep(10);
        }
        provide_function_C(overview->system);
        while (!waiting_for_input(overview->system->brain))
        {
            int64_t resp = read_output(overview->system);
            usleep(10);
        }

        provide_line(overview->system, "n\n");

        usleep(100);

        int64_t final_output = -1;

        /*Robot sends two lines before drawing map*/
        /*Main:\n\n*/
        /*Skip those.*/
        int recvd_headline = 0;

        while (final_output == -1 && !overview->system->finished)
        {
            /*Read Response*/
            int64_t resp = read_output(overview->system);
            if (recvd_headline != 2 && (char) resp == '\n')
            {
                recvd_headline++;
            }
            else if (labs(resp) > 255)
            {
                final_output = resp;
            }
            else
            {
                update(overview, resp);
#ifdef NCURSES
                display_overview(overview);
#endif
            }
            usleep(10);
        }
#ifdef NCURSES
        endwin();
#endif
        printf("Final output: %ld\n", final_output);
    }
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
                char tile = (char) overview->area[index];
                printf("%c", tile);
            }
            /*printf("\n");*/
        }
    }
}

static void update(Overview* const overview, const int64_t value)
{
    assert(overview != NULL);
    assert(overview->system != NULL);

    Direction d;


    if (overview->initialized && (overview->output_pos.y >= overview->height))
    {
        overview->output_pos.x = 0;
        overview->output_pos.y = 0;
    }

    /*Save value to area overview at current output position*/
    int index =
        (overview->output_pos.y * overview->width) + overview->output_pos.x;
    overview->area[index] = value;


    /*Move output position*/
    /*Cast to character*/
    char c = (char) value;
    if (c == '\n')
    {
        d = DOWN;
        overview->output_pos.y++;
        overview->output_pos.x = 0;
    }
    else if ((c == '^') || (c == '<') || (c == '>') || (c == 'v') ||
             (c == '#') || (c == '.') || (c == 'X'))
    {
        if ((c == '^') || (c == '<') || (c == '>') || (c == 'v') || (c == 'X'))
        {
            overview->robot->pos = overview->output_pos;
            if (c == '^')
            {
                overview->robot->direction = UP;
            }
            else if (c == '<')
            {
                overview->robot->direction = LEFT;
            }
            else if (c == '>')
            {
                overview->robot->direction = RIGHT;
            }
            else if (c == 'v')
            {
                overview->robot->direction = DOWN;
            }
        }
        overview->output_pos.x++;
        d = RIGHT;
    }
    if ((overview->output_pos.x < 0) ||
        (overview->output_pos.x >= overview->width) ||
        (overview->output_pos.y < 0) ||
        (overview->output_pos.y >= overview->height))
    {
        if (!overview->initialized)
        {
            /*New position outside of current area.*/
            /*Increase area in the correct direction.*/
            resize_overview(overview, d);
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

        overview->output_pos.x += start_x;
        overview->output_pos.y += start_y;
        overview->robot->pos.x += start_x;
        overview->robot->pos.y += start_y;
        free(overview->area);
        overview->area   = new_area;
        overview->height = new_height;
        overview->width  = new_width;
    }
}

static void display_overview(const Overview* const overview)
{
    if ((overview != NULL) && (overview->area != NULL))
    {
#ifdef NCURSES
        /*Assuming the curse screen has already been initialized.*/
        /*usleep(10);*/
        mvprintw(0, 0, "Overview:");
#else
        printf("Overview:");
#endif
        for (int i = 0; i < overview->height; ++i)
        {
            for (int j = 0; j < overview->width; ++j)
            {
                int index = (i * overview->width) + j;
                char tile = (char) overview->area[index];
#ifdef NCURSES
                mvprintw(i + 1, j, "%c", tile);
#else
                printf("%c", tile);
#endif
            }
            /*#ifndef NCURSES*/
            /*printf("\n");*/
            /*#endif*/
        }
    }
#ifdef NCURSES
    refresh();
#endif
}

static void provide_input(const ASCII* const system, const int value)
{
    assert(system != NULL);
    assert(system->brain != NULL);
    assert(system->brain->mem_io_in != NULL);

    intcode_io_mem_t* input = system->brain->mem_io_in;
    pthread_mutex_lock(&input->mut);
    while (!input->consumed && !system->finished)
    {
        pthread_cond_wait(&input->cond, &input->mut);
    }
    input->value    = value;
    input->consumed = 0;
    pthread_cond_signal(&input->cond);
    pthread_mutex_unlock(&input->mut);
    usleep(10);
}

static int64_t read_output(const ASCII* const system)
{
    assert(robot != NULL);
    assert(robot->brain != NULL);
    assert(robot->brain->mem_io_out != NULL);

    int64_t read_value = -1;

    intcode_io_mem_t* output = system->brain->mem_io_out;
    pthread_mutex_lock(&output->mut);
    while (output->consumed && !system->finished)
    {
        pthread_cond_wait(&output->cond, &output->mut);
    }

    if (!system->finished)
    {
        read_value       = output->value;
        output->consumed = 1;
    }
    pthread_cond_signal(&output->cond);
    pthread_mutex_unlock(&output->mut);


    return read_value;
}

static Position* find_scaff_intersections(const Overview* const overview,
                                          int* const num_intersections)
{
    assert(overview != NULL);
    assert(overview->area != NULL);
    assert(num_intersections != NULL);

    int count = 0;

    /*Value in the second row all the way to the left.*/
    char last_value     = (char) overview->area[overview->width];
    Position* positions = NULL;

    for (int y = 1; y < overview->height - 1; ++y)
    {
        for (int x = 1; x < overview->width - 1; ++x)
        {
            int top_index    = ((y - 1) * overview->width) + x;
            int center_index = (y * overview->width) + x;
            int bottom_index = ((y + 1) * overview->width) + x;
            int right_index  = (y * overview->width) + x + 1;

            char top    = (char) overview->area[top_index];
            char center = (char) overview->area[center_index];
            char bottom = (char) overview->area[bottom_index];
            char right  = (char) overview->area[right_index];

            if ((last_value == '#') && (top == '#') && (center == '#') &&
                (bottom == '#') && (right == '#'))
            {
                /*Create position*/
                if (count == 0)
                {
                    positions = (Position*) malloc(sizeof(Position));
                }
                else
                {
                    positions = (Position*) realloc(
                        positions, sizeof(Position) * (count + 1));
                }
                if (positions == NULL)
                {
                    return NULL;
                }
                Position p = {.x = x, .y = y};
                positions[count++] = p;
            }
            last_value = center;
        }
    }
    *num_intersections = count;
    return positions;
}

static int alignment_parameter(const Position pos)
{
    return pos.x * pos.y;
}

static void provide_main_routine(const ASCII* const system)
{
    /*See the notes.md for explanation*/
    const char* main_routine = "A,B,A,C,B,A,C,A,C,B\n";
    provide_line(system, main_routine);
}

static void provide_function_A(const ASCII* const system)
{
    /*See the notes.md for explanation*/
    const char* func = "L,6,6,L,8,L,8\n";
    provide_line(system, func);
}

static void provide_function_B(const ASCII* const system)
{
    /*See the notes.md for explanation*/
    const char* func = "L,6,6,R,4,L,6,6,R,6\n";
    provide_line(system, func);
}

static void provide_function_C(const ASCII* const system)
{
    /*See the notes.md for explanation*/
    const char* func = "R,4,L,6,6,L,6,6,R,6\n";
    provide_line(system, func);
}

static void provide_line(const ASCII* const system, const char* const line)
{
    assert(system != NULL);
    assert(system->brain != NULL);
    assert(line != NULL);

    for (int i = 0; i < strlen(line); ++i)
    {
        provide_input(system, (int64_t) line[i]);
    }
}
