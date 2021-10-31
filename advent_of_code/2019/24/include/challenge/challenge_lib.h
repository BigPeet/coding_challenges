/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include <stdint.h>

#define AREA_WIDTH (5u)
#define AREA_HEIGHT (5u)
#define EMPTY '.'
#define BUG '#'

typedef struct Scan
{
    uint32_t data[AREA_HEIGHT][AREA_WIDTH];
    struct Scan* upper;
    struct Scan* lower;
} Scan;

typedef struct ScanListNode
{
    Scan content;
    struct ScanListNode* next;
} ScanListNode;

typedef struct
{
    ScanListNode* head;
} ScanList;

typedef enum
{
    LEVEL_UP,
    LEVEL_DOWN,
    LEVEL_BOTH
} LevelDirection;

// IO
int read_scan_input(char const* file_path, char* out);
void parse_scan(char const* text, Scan* out);

// Scan stuff
void print_scan(Scan const* s);
void apply_step_01(Scan const* old, Scan* out);
void apply_step_02(Scan* scan, LevelDirection dir);
int cmp_scans(Scan const* a, Scan const* b);
uint32_t scan_biodiversity(Scan const* scan);
Scan* scan_create_empty();
void scan_destroy_levels(Scan* scan);
void scan_add_level(Scan* scan, LevelDirection dir);
uint32_t scan_count_bugs(Scan const* scan, LevelDirection dir);

// List stuff
ScanList* list_create();
void list_destroy(ScanList* list);
void list_add(ScanList* list, Scan const* scan);
int list_contains(ScanList const* list, Scan const* scan);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
