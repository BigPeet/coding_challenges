/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-03
 *
 */

#include "challenge/challenge_lib.h"
#include "stdio.h"
#include "string.h"

#define LINE_DELIM ","

static size_t count_lines(const char* const file_path);
static size_t count_points(const char* const str);
static size_t* get_size_info(const char* const file_path, size_t* const amount_lines);
static wire_t* parse_wire(char* const str);
static point_t* parse_point(const char* const str, const point_t* const origin);
static int line_horizontal(const line_t* const line);
static int line_vertical(const line_t* const line);
static int between_values(const int range_a, const int range_b, const int val);
static int on_line(const line_t* const l, const point_t* const p);
static int line_length(const line_t* const l);

wire_t** read_wires(const char* const file_path, size_t* const num_wires)
{
    wire_t** wires = NULL;
    if ((file_path != NULL) && (num_wires != NULL))
    {
        size_t* amount_chars = get_size_info(file_path, num_wires);
        if (amount_chars != NULL)
        {
            if (*num_wires > 0)
            {
                wires = (wire_t**)malloc(sizeof(wire_t*) * *num_wires);
                if (wires != NULL)
                {
                    FILE* fp = fopen(file_path, "r");
                    if (fp != NULL)
                    {
                        for (size_t i = 0; i < *num_wires; i++)
                        {
                            size_t chars_for_line = amount_chars[i];
                            char buffer[chars_for_line];
                            if (fgets(buffer, chars_for_line, fp) != NULL)
                            {
                                if (buffer[strlen(buffer) - 1] != '\n')
                                {
                                    printf("Unexpected error, when reading line.\n");
                                    return NULL;
                                }
                                wires[i] = parse_wire(buffer);
                            }
                        }
                        fclose(fp);
                    }
                }
            }
            free(amount_chars);
        }
    }
    return wires;
}

void destroy_wire(wire_t* wire)
{
    if (wire != NULL)
    {
        if (wire->lines != NULL)
        {
            for (int i = 0; i < wire->num_lines; ++i)
            {
                line_t* l = wire->lines[i];
                if (l->a != NULL)
                {
                    free(l->a);
                }
                if ((i == wire->num_lines - 1) && (l->b != NULL))
                {
                    /*only remove the second point from last line*/
                    /*the others use the point from prior line*/
                    free(l->b);
                }
                free(l);
            }
            free(wire->lines);
        }
        free(wire);
    }
}

int intersect_wires_manhattan(const wire_t* const a, const wire_t* const b, point_t* location)
{
    int min_distance = __INT_MAX__;
    point_t origin;
    origin.x = 0;
    origin.y = 0;
    if ((a != NULL) && (b != NULL) && (location != NULL))
    {
        for (size_t i = 0; i < a->num_lines; ++i)
        {
            for (size_t j = 0; j < b->num_lines; ++j)
            {
                point_t d;
                if (intersect_lines(a->lines[i], b->lines[j], &d))
                {
                    int distance = manhattan_distance(&origin, &d);
                    if ((distance > 0) && (distance < min_distance))
                    {
                        min_distance = distance;
                        location->x  = d.x;
                        location->y  = d.y;
                    }
                }
            }
        }
    }
    return min_distance;
}

int intersect_wires_steps(const wire_t* const a, const wire_t* const b, point_t* location)
{
    int min_distance = __INT_MAX__;
    point_t origin;
    origin.x = 0;
    origin.y = 0;
    if ((a != NULL) && (b != NULL) && (location != NULL))
    {
        for (size_t i = 0; i < a->num_lines; ++i)
        {
            for (size_t j = 0; j < b->num_lines; ++j)
            {
                point_t d;
                if (intersect_lines(a->lines[i], b->lines[j], &d))
                {
                    int dist_a   = distance_to_point(a, &d);
                    int dist_b   = distance_to_point(b, &d);
                    int distance = dist_a + dist_b;
                    if ((distance > 0) && (distance < min_distance))
                    {
                        min_distance = distance;
                        location->x  = d.x;
                        location->y  = d.y;
                    }
                }
            }
        }
    }
    return min_distance;
}

int intersect_lines(const line_t* const l1, const line_t* const l2, point_t* location)
{
    int found_intersection = 0;
    if ((l1 != NULL) && (l2 != NULL) && (l1->a != NULL) && (l1->b != NULL) && (l2->a != NULL) &&
        (l2->b != NULL) && (location != NULL))
    {
        if (line_horizontal(l1) && line_vertical(l2))
        {
            if (between_values(l1->a->x, l1->b->x, l2->a->x) &&
                (between_values(l2->a->y, l2->b->y, l1->a->y)))
            {
                found_intersection = 1;
                location->x        = l2->a->x;
                location->y        = l1->a->y;
            }
        }
        else if (line_vertical(l1) && line_horizontal(l2))
        {
            if (between_values(l2->a->x, l2->b->x, l1->a->x) &&
                (between_values(l1->a->y, l1->b->y, l2->a->y)))
            {
                found_intersection = 1;
                location->x        = l1->a->x;
                location->y        = l2->a->y;
            }
        }
    }
    return found_intersection;
}

int manhattan_distance(const point_t* const a, const point_t* const b)
{
    int dist = 0;
    if ((a != NULL) && (b != NULL))
    {
        int lon_dist = abs(a->x - b->x);
        int lat_dist = abs(a->y - b->y);
        dist         = lon_dist + lat_dist;
    }
    return dist;
}

int distance_to_point(const wire_t* const w, const point_t* const p)
{
    int dist = -1;
    if ((w != NULL) && (p != NULL) && (w->lines != NULL))
    {
        dist = 0;
        for (size_t i = 0; i < w->num_lines; ++i)
        {
            line_t* l = w->lines[i];
            if (l != NULL)
            {
                int line_dist = on_line(l, p);
                if (line_dist)
                {
                    dist += line_dist;
                    break;
                }
                else
                {
                    dist += line_length(l);
                }
            }
        }
    }
    return dist;
}

static wire_t* parse_wire(char* const str)
{
    wire_t* wire   = NULL;
    line_t** lines = NULL;
    point_t* start = (point_t*)malloc(sizeof(point_t));
    if (start != NULL)
    {
        start->x = 0;
        start->y = 0;
    }
    size_t num_points = count_points(str);
    if (num_points > 0)
    {
        wire  = (wire_t*)malloc(sizeof(wire_t));
        lines = (line_t**)malloc(sizeof(line_t*) * num_points);
    }
    if ((lines != NULL) && (wire != NULL) && (start != NULL))
    {
        wire->num_lines = num_points;
        size_t index    = 0;
        char* token     = strtok(str, LINE_DELIM);
        while (token != NULL)
        {
            point_t* end = parse_point(token, start);
            if (end != NULL)
            {
                line_t* line = (line_t*)malloc(sizeof(line_t));
                if (line != NULL)
                {
                    line->a        = start;
                    line->b        = end;
                    lines[index++] = line;
                }
                start = end;
                token = strtok(NULL, LINE_DELIM);
            }
        }
        wire->lines = lines;
    }
    return wire;
}

static point_t* parse_point(const char* const str, const point_t* const origin)
{
    point_t* parsed = NULL;
    size_t length   = strlen(str);
    if (length > 0)
    {
        char direction = str[0];
        int value      = atoi(str + 1);
        parsed         = (point_t*)malloc(sizeof(point_t));
        parsed->x      = origin->x;
        parsed->y      = origin->y;
        switch (direction)
        {
            case 'U':
                parsed->y += value;
                break;
            case 'D':
                parsed->y -= value;
                break;
            case 'R':
                parsed->x += value;
                break;
            case 'L':
                parsed->x -= value;
                break;
            default:
                break;
        }
    }
    return parsed;
}

static size_t count_points(const char* const str)
{
    size_t num_points = 0;
    if (strlen(str) > 0)
    {
        num_points = 1;
    }
    for (size_t i = 0; i < strlen(str); ++i)
    {
        char ch = str[i];
        if (ch == ',')
        {
            num_points++;
        }
    }
    return num_points;
}

static size_t count_lines(const char* const file_path)
{
    /*Ugly and inefficient to read the same file twice, but I'm*/
    /*currently too lazy to dynamically adjust the memory needed.
     *And I don't want to hardcode the amount either.*/
    size_t num_lines = 0;
    FILE* fp;
    fp = fopen(file_path, "r");
    if (fp != NULL)
    {
        char ch = fgetc(fp);
        while (ch != EOF)
        {
            if (ch == '\n')
            {
                num_lines++;
            }
            ch = fgetc(fp);
        }
        fclose(fp);
    }
    return num_lines;
}

static size_t* get_size_info(const char* const file_path, size_t* const amount_lines)
{
    size_t* amount_chars = NULL;
    /*Again, inefficient but lazy*/
    *amount_lines = count_lines(file_path);
    amount_chars  = (size_t*)malloc(sizeof(size_t) * *amount_lines);
    if (amount_chars != NULL)
    {
        FILE* fp = fopen(file_path, "r");
        if (fp != NULL)
        {
            size_t line_index = 0;
            size_t char_count = 0;
            char ch           = fgetc(fp);
            while (ch != EOF)
            {
                char_count++;
                if (ch == '\n')
                {
                    amount_chars[line_index++] = char_count + 1; /* one extra for \0*/
                    char_count                 = 0;
                }
                ch = fgetc(fp);
            }
            fclose(fp);
        }
    }
    return amount_chars;
}

static int line_horizontal(const line_t* const line)
{
    int is_horiz = 0;
    if ((line->a->y == line->b->y) && (line->a->x != line->b->x))
    {
        is_horiz = 1;
    }
    return is_horiz;
}

static int line_vertical(const line_t* const line)
{
    return !line_horizontal(line);
}

static int on_line(const line_t* const l, const point_t* const p)
{
    int on_line = 0;
    if (line_horizontal(l))
    {
        on_line = between_values(l->a->x, l->b->x, p->x) && (l->a->y == p->y);
        if (on_line)
        {
            on_line = abs(l->a->x - p->x);
        }
    }
    else
    {
        on_line = between_values(l->a->y, l->b->y, p->y) && (l->a->x == p->x);
        if (on_line)
        {
            on_line = abs(l->a->y - p->y);
        }
    }
    return on_line;
}

static int line_length(const line_t* const l)
{
    int length = 0;
    if (line_horizontal(l))
    {
        length = abs(l->a->x - l->b->x);
    }
    else
    {
        length = abs(l->a->y - l->b->y);
    }
    return length;
}

static int between_values(const int range_a, const int range_b, const int val)
{
    int inbetween = 0;
    int high, low;
    if (range_a >= range_b)
    {
        high = range_a;
        low  = range_b;
    }
    else
    {
        low  = range_a;
        high = range_b;
    }

    if ((val >= low) && (val <= high))
    {
        inbetween = 1;
    }
    return inbetween;
}
