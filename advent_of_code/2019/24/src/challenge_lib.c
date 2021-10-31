/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "challenge/challenge_lib.h"

typedef enum
{
    UP,
    DOWN,
    RIGHT,
    LEFT
} Direction;

static uint32_t check_neighbour_01(Scan const* const s,
                                   int const row,
                                   int const col,
                                   Direction const dir)
{
    uint32_t alive = 0;
    switch (dir)
    {
        case UP:
            if (row > 0)
            {
                alive = s->data[row - 1][col];
            }
            break;
        case DOWN:
            if (row < AREA_HEIGHT - 1)
            {
                alive = s->data[row + 1][col];
            }
            break;
        case RIGHT:
            if (col < AREA_WIDTH - 1)
            {
                alive = s->data[row][col + 1];
            }
            break;
        case LEFT:
            if (col > 0)
            {
                alive = s->data[row][col - 1];
            }
            break;
        default:
            /*unknown direction*/
            break;
    }
    return alive;
}

static uint32_t check_neighbour_02(Scan const* const s,
                                   int const row,
                                   int const col,
                                   Direction const dir)
{
    uint32_t alive = 0;
    switch (dir)
    {
        case UP:
            if (row == 0 && s->upper)
            {
                alive = s->upper->data[1][2]; // 8th field
            }
            else if (row == 3 && col == 2 && s->lower) // 18th field
            {
                for (int i = 0; i < AREA_WIDTH; ++i)
                {
                    alive += s->lower->data[4][i];
                }
            }
            else if (row > 0)
            {
                alive = s->data[row - 1][col];
            }
            break;
        case DOWN:
            if (row == AREA_HEIGHT - 1 && s->upper)
            {
                alive = s->upper->data[3][2]; // 18th field
            }
            else if (row == 1 && col == 2 && s->lower) // 8th field
            {
                for (int i = 0; i < AREA_WIDTH; ++i)
                {
                    alive += s->lower->data[0][i];
                }
            }
            else if (row < AREA_HEIGHT - 1)
            {
                alive = s->data[row + 1][col];
            }
            break;
        case RIGHT:
            if (col == AREA_WIDTH - 1 && s->upper)
            {
                alive = s->upper->data[2][3]; // 14th field
            }
            else if (col == 1 && row == 2 && s->lower) // 12th field
            {
                for (int i = 0; i < AREA_HEIGHT; ++i)
                {
                    alive += s->lower->data[i][0];
                }
            }
            else if (col < AREA_WIDTH - 1)
            {
                alive = s->data[row][col + 1];
            }
            break;
        case LEFT:
            if (col == 0 && s->upper)
            {
                alive = s->upper->data[2][1]; // 12th field
            }
            else if (row == 2 && col == 3 && s->lower)
            {
                for (int i = 0; i < AREA_HEIGHT; ++i)
                {
                    alive += s->lower->data[i][AREA_WIDTH - 1];
                }
            }
            else if (col > 0)
            {
                alive = s->data[row][col - 1];
            }
            break;
        default:
            /*unknown direction*/
            break;
    }
    return alive;
}

typedef uint32_t (*check_fn)(Scan const*, int, int, Direction);

static int will_live(Scan const* const s, int row, int col, check_fn check)
{
    uint32_t alive_neighbours = check(s, row, col, UP) + check(s, row, col, DOWN) +
                                check(s, row, col, RIGHT) + check(s, row, col, LEFT);
    uint32_t currently_alive = s->data[row][col];

    if (currently_alive)
    {
        return alive_neighbours == 1u;
    }
    return alive_neighbours == 1u || alive_neighbours == 2u;
}

static uint32_t row_sum(Scan const* s, int row)
{
    uint32_t sum = 0u;
    for (int i = 0; i < AREA_WIDTH; ++i)
    {
        sum += s->data[row][i];
    }
    return sum;
}

static uint32_t col_sum(Scan const* s, int col)
{
    uint32_t sum = 0u;
    for (int i = 0; i < AREA_HEIGHT; ++i)
    {
        sum += s->data[i][col];
    }
    return sum;
}

static int scan_requires_upper(Scan const* s)
{
    uint32_t sum_top    = row_sum(s, 0);
    uint32_t sum_bottom = row_sum(s, AREA_HEIGHT - 1);
    uint32_t sum_left   = col_sum(s, 0);
    uint32_t sum_right  = col_sum(s, AREA_WIDTH - 1);

    return (sum_top == 1u || sum_top == 2u) || (sum_bottom == 1u || sum_bottom == 2u) ||
           (sum_left == 1u || sum_left == 2u) || (sum_right == 1u || sum_right == 2u);
}

static void initial_infest_upper(Scan const* s)
{
    uint32_t sum_top    = row_sum(s, 0);
    uint32_t sum_bottom = row_sum(s, AREA_HEIGHT - 1);
    uint32_t sum_left   = col_sum(s, 0);
    uint32_t sum_right  = col_sum(s, AREA_WIDTH - 1);

    if (sum_top == 1u || sum_top == 2u)
    {
        s->upper->data[1][2] = 1u;
    }
    if (sum_bottom == 1u || sum_bottom == 2u)
    {
        s->upper->data[3][2] = 1u;
    }
    if (sum_left == 1u || sum_left == 2u)
    {
        s->upper->data[2][1] = 1u;
    }
    if (sum_right == 1u || sum_right == 2u)
    {
        s->upper->data[2][3] = 1u;
    }
}

static int scan_requires_lower(Scan const* s)
{
    return s->data[1][2] || s->data[2][1] || s->data[2][3] || s->data[3][2];
}

static void initial_infest_lower(Scan const* s)
{
    if (s->data[1][2]) // 8th field
    {
        for (int i = 0; i < AREA_WIDTH; ++i)
        {
            s->lower->data[0][i] = 1u;
        }
    }
    if (s->data[2][1]) // 12th field
    {
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            s->lower->data[i][0] = 1u;
        }
    }
    if (s->data[2][3]) // 14th field
    {
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            s->lower->data[i][AREA_WIDTH - 1] = 1u;
        }
    }
    if (s->data[3][2]) // 18th field
    {
        for (int i = 0; i < AREA_WIDTH; ++i)
        {
            s->lower->data[AREA_HEIGHT - 1][i] = 1u;
        }
    }
}

static void print_scan_data(Scan const* s)
{
    printf("---------------\n");
    if (s)
    {
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            for (int j = 0; j < AREA_WIDTH; ++j)
            {
                char c = s->data[i][j] == 0 ? EMPTY : BUG;
                printf("%c", c);
            }
            printf("\n");
        }
    }
    printf("---------------\n");
}

static void print_next_levels(Scan const* s, LevelDirection dir)
{
    if ((dir == LEVEL_UP) && s->upper)
    {
        print_scan_data(s->upper);
        print_next_levels(s->upper, LEVEL_UP);
    }
    else if ((dir == LEVEL_DOWN) && s->lower)
    {
        print_scan_data(s->lower);
        print_next_levels(s->lower, LEVEL_DOWN);
    }
}

/*Preconditions:
 * - File holds a valid 5x5 scan.
 * - out can hold 25 characters.
 */
int read_scan_input(char const* file_path, char* out)
{
    FILE* fp = fopen(file_path, "r");
    if (fp == NULL)
    {
        printf("Error when opening file %s\n", file_path);
        return 1;
    }
    int counter = 0;
    char c      = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == EMPTY || c == BUG)
        {
            out[counter++] = c;
        }
    }
    fclose(fp);
    return 0;
}

/**
 * Preconditions:
 * - text contains at least 25 characters.
 * - the first 25 characters are either . or #
 */
void parse_scan(char const* const text, Scan* out)
{
    if (text && out)
    {
        for (int i = 0; i < AREA_HEIGHT; i++)
        {
            for (int j = 0; j < AREA_WIDTH; j++)
            {
                out->data[i][j] = text[i * AREA_WIDTH + j] == EMPTY ? 0u : 1u;
            }
        }
    }
}

void print_scan(Scan const* s)
{
    if (s)
    {
        print_scan_data(s);
        if (s->upper)
        {
            printf("Upper Levels:\n");
            print_next_levels(s, LEVEL_UP);
        }
        if (s->lower)
        {
            printf("Lower Levels:\n");
            print_next_levels(s, LEVEL_DOWN);
        }
    }
}

void apply_step_01(Scan const* old, Scan* out)
{
    if (old && out)
    {
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            for (int j = 0; j < AREA_WIDTH; ++j)
            {
                out->data[i][j] = will_live(old, i, j, check_neighbour_01);
            }
        }
    }
}

void apply_step_02(Scan* scan, LevelDirection dir)
{
    if (scan)
    {
        uint32_t tmp_data[AREA_HEIGHT][AREA_WIDTH];
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            for (int j = 0; j < AREA_WIDTH; ++j)
            {
                if (i != 2 || j != 2)
                {
                    tmp_data[i][j] = will_live(scan, i, j, check_neighbour_02);
                }
                else
                {
                    tmp_data[i][j] = 0u;
                }
            }
        }

        /*check upper level(s)*/
        if (!scan->upper && scan_requires_upper(scan))
        {
            scan_add_level(scan, LEVEL_UP);
            initial_infest_upper(scan);
        }
        else if (dir == LEVEL_BOTH || dir == LEVEL_UP)
        {
            apply_step_02(scan->upper, LEVEL_UP);
        }

        /*check lower level(s)*/
        if (!scan->lower && scan_requires_lower(scan))
        {
            scan_add_level(scan, LEVEL_DOWN);
            initial_infest_lower(scan);
        }
        else if (dir == LEVEL_BOTH || dir == LEVEL_DOWN)
        {
            apply_step_02(scan->lower, LEVEL_DOWN);
        }

        /*Write data to scan.*/
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            for (int j = 0; j < AREA_WIDTH; ++j)
            {
                scan->data[i][j] = tmp_data[i][j];
            }
        }
    }
}

uint32_t scan_count_bugs(Scan const* scan, LevelDirection dir)
{
    uint32_t sum = 0;
    if (scan)
    {
        uint32_t upper = scan->upper && (dir == LEVEL_BOTH || dir == LEVEL_UP)
                             ? scan_count_bugs(scan->upper, LEVEL_UP)
                             : 0;
        uint32_t lower = scan->lower && (dir == LEVEL_BOTH || dir == LEVEL_DOWN)
                             ? scan_count_bugs(scan->lower, LEVEL_DOWN)
                             : 0;
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            for (int j = 0; j < AREA_WIDTH; ++j)
            {
                sum += scan->data[i][j];
            }
        }
        sum += upper + lower;
    }
    return sum;
}

int cmp_scans(Scan const* a, Scan const* b)
{
    if (a && b)
    {
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            for (int j = 0; j < AREA_WIDTH; ++j)
            {
                if (a->data[i][j] != b->data[i][j])
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}

uint32_t scan_biodiversity(Scan const* scan)
{
    uint32_t diversity = 0;
    for (int i = 0; i < AREA_HEIGHT; ++i)
    {
        for (int j = 0; j < AREA_WIDTH; ++j)
        {
            diversity |= scan->data[i][j] << (i * AREA_WIDTH + j);
        }
    }
    return diversity;
}

Scan* scan_create_empty()
{
    Scan* created = (Scan*) malloc(sizeof(Scan));
    if (created)
    {
        created->lower = NULL;
        created->upper = NULL;
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            for (int j = 0; j < AREA_WIDTH; ++j)
            {
                created->data[i][j] = 0u;
            }
        }
    }
    return created;
}

void scan_destroy_levels(Scan* scan)
{
    if (scan)
    {
        // delete upper levels
        if (scan->upper)
        {
            Scan* current = scan->upper;
            Scan* next    = current->upper;
            free(current);
            while (next)
            {
                current = next;
                next    = current->upper;
                free(current);
            }
        }
        // delete lower levels
        if (scan->lower)
        {
            Scan* current = scan->lower;
            Scan* next    = current->lower;
            free(current);
            while (next)
            {
                current = next;
                next    = current->lower;
                free(current);
            }
        }
    }
}
void scan_add_level(Scan* scan, LevelDirection dir)
{
    if (scan)
    {
        Scan* new_level = scan_create_empty();
        if (dir == LEVEL_UP)
        {
            scan->upper      = new_level;
            new_level->lower = scan;
        }
        else if (dir == LEVEL_DOWN)
        {
            scan->lower      = new_level;
            new_level->upper = scan;
        }
    }
}

static ScanListNode* list_node_create(Scan const* scan)
{
    ScanListNode* node = (ScanListNode*) malloc(sizeof(ScanListNode));
    if (node)
    {
        node->content = *scan;
        node->next    = NULL;
    }
    return node;
}

static void list_node_destroy(ScanListNode* node)
{
    if (node)
    {
        free(node);
    }
}

ScanList* list_create()
{
    ScanList* created = (ScanList*) malloc(sizeof(ScanList));
    if (created)
    {
        created->head = NULL;
    }
    return created;
}

void list_destroy(ScanList* const list)
{
    if (list)
    {
        if (list->head)
        {
            ScanListNode* current = list->head;
            ScanListNode* next    = current->next;
            list_node_destroy(current);
            while (next)
            {
                current = next;
                next    = current->next;
                list_node_destroy(current);
            }
        }
        free(list);
    }
}

void list_add(ScanList* list, Scan const* scan)
{
    if (list && scan)
    {
        if (!list->head)
        {
            // list is empty
            list->head = list_node_create(scan);
        }
        else
        {
            ScanListNode* last = list->head;
            while (last->next)
            {
                last = last->next;
            }
            last->next = list_node_create(scan);
        }
    }
}

int list_contains(ScanList const* list, Scan const* scan)
{
    if (list && scan)
    {
        ScanListNode* current = list->head;
        while (current)
        {
            if (cmp_scans(&current->content, scan))
            {
                return 1;
            }
            current = current->next;
        }
    }
    return 0;
}
