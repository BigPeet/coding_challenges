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

static uint32_t check_neighbour(Scan const* const s,
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

static int will_live(Scan const* const s, int row, int col)
{
    uint32_t alive_neighbours =
        check_neighbour(s, row, col, UP) + check_neighbour(s, row, col, DOWN) +
        check_neighbour(s, row, col, RIGHT) + check_neighbour(s, row, col, LEFT);
    uint32_t currently_alive = s->data[row][col];

    if (currently_alive)
    {
        return alive_neighbours == 1u;
    }
    return alive_neighbours == 1u || alive_neighbours == 2u;
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

void apply_step(Scan const* old, Scan* out)
{
    if (old && out)
    {
        for (int i = 0; i < AREA_HEIGHT; ++i)
        {
            for (int j = 0; j < AREA_WIDTH; ++j)
            {
                out->data[i][j] = will_live(old, i, j);
            }
        }
    }
}

int cmp_scans(Scan const* a, Scan const* b)
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
