/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-15
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "stdio.h"

#define COLOR_MASK 1
#define PAINTED_MASK 2

static void provide_input(const Robot* const robot, const int value);
static int read_output(const Robot* const robot);

void* robot_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
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
    Overview* overview = (Overview*) args;

    /*Only reading access*/
    while (!overview->robot->finished)
    {
        Position current_pos = overview->robot->pos;
        if ((current_pos.x < 0) || (current_pos.x >= overview->width) || (current_pos.y < 0) ||
            (current_pos.y >= overview->height))
        {
            /*Robot left the field, increase hull in the correct direction.*/
            Direction dir;
            if (current_pos.x < 0)
            {
                dir = LEFT;
            }
            else if (current_pos.x >= overview->width)
            {
                dir = RIGHT;
            }
            else if (current_pos.y < 0)
            {
                dir = UP;
            }
            else if (current_pos.y >= overview->height)
            {
                dir = DOWN;
            }
            resize_overview(overview, dir);
        }

        Direction current_dir = overview->robot->direction;
        int pos_index         = (current_pos.y * overview->width) + current_pos.x;
        int color             = overview->hull[pos_index] & COLOR_MASK;

        /*Provide color as input*/
        provide_input(overview->robot, color);

        /*Read first ouput (color value): 0: black, 1: white, -1: finished*/
        int new_color = read_output(overview->robot);
        /*Read second ouput (turn value): 0: turn left, 1: turn_right*/
        int turn_right = read_output(overview->robot);

        if (!overview->robot->finished)
        {
            /*Paint current field in the new color*/
            overview->hull[pos_index] = new_color | PAINTED_MASK;

            /*Turn robot into new direction and move one field.*/
            Direction new_dir          = turn(current_dir, turn_right);
            overview->robot->direction = new_dir;
            move(overview->robot);
        }
    }

    return NULL;
}

Direction turn(const Direction current, const int command)
{
    int inc = (command == 1) ? 1 : -1;
    return (current + inc) % NUM_OF_DIRECTIONS;
}

void move(Robot* const robot)
{
    if (robot != NULL)
    {
        switch (robot->direction)
        {
            case UP:
                robot->pos.y -= 1;
                break;
            case RIGHT:
                robot->pos.x += 1;
                break;
            case DOWN:
                robot->pos.y += 1;
                break;
            case LEFT:
                robot->pos.x -= 1;
                break;
            default:
                break;
        }
    }
}

void resize_overview(Overview* const overview, const Direction dir)
{
    int new_height = ((dir == UP) || (dir == DOWN)) ? overview->height + 10 : overview->height;
    int new_width  = ((dir == RIGHT) || (dir == LEFT)) ? overview->width + 10 : overview->width;

    int* new_hull = (int*) calloc(new_height, new_width * sizeof(int));
    if (new_hull != NULL)
    {
        int start_y = (dir == UP) ? 10 : 0;
        int start_x = (dir == LEFT) ? 10 : 0;
        for (int i = start_y; i < overview->height; ++i)
        {
            for (int j = start_x; j < overview->width; ++j)
            {
                int old_index       = (i * overview->width) + j;
                int new_index       = (i * new_width) + j;
                new_hull[new_index] = overview->hull[old_index];
            }
        }
        overview->robot->pos.x += start_x;
        overview->robot->pos.y += start_y;
        free(overview->hull);
        overview->hull   = new_hull;
        overview->height = new_height;
        overview->width  = new_width;
    }
}

int count_painted_fields(const Overview* const overview)
{
    int count = 0;
    if ((overview != NULL) && (overview->hull != NULL))
    {
        for (int i = 0; i < overview->height; ++i)
        {
            for (int j = 0; j < overview->width; ++j)
            {
                int index   = (i * overview->width) + j;
                int painted = (overview->hull[index] & PAINTED_MASK) >> 1;
                if (painted == 1)
                {
                    count++;
                }
            }
        }
    }
    return count;
}


void print_overview(const Overview* const overview)
{
    if ((overview != NULL) && (overview->hull != NULL))
    {
        for (int i = 0; i < overview->height; ++i)
        {
            for (int j = 0; j < overview->width; ++j)
            {
                int color = overview->hull[(i * overview->width) + j] & COLOR_MASK;
                if (color != 0)
                {
                    printf("%d ", color);
                }
                else
                {
                    printf("  ");
                }
            }
            printf("\n");
        }
    }
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
