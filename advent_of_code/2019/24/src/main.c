/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "challenge/challenge_lib.h"

#define DEFAULT_MINUTES (200)

static void part01(Scan s)
{
    printf("Part 1\n");
    ScanList* list = list_create();
    list_add(list, &s);
    while (1)
    {
        Scan next;
        next.upper = NULL;
        next.lower = NULL;
        apply_step_01(&s, &next);
        if (!list_contains(list, &next))
        {
            list_add(list, &next);
        }
        else
        {
            printf("First duplicate:\n");
            print_scan(&next);
            uint32_t biodiv = scan_biodiversity(&next);
            printf("This scan has a biodiversity of %d.\n", biodiv);
            break;
        }
        s = next;
    }
    list_destroy(list);
}

static void part02(Scan s, int minutes)
{
    printf("Part 2\n");
    for (int i = 0; i < minutes; ++i)
    {
        apply_step_02(&s, LEVEL_BOTH);
    }
    printf("Amoung of bugs after %d minutes: %d\n", minutes, scan_count_bugs(&s, LEVEL_BOTH));

    // Clean up
    scan_destroy_levels(&s);
}

int main(int argc, char* argv[])
{
    int minutes = DEFAULT_MINUTES;
    if (argc < 2)
    {
        printf("This executabel takes at least one argument.\n");
        printf("Usage: aoc2019_24 FILE_PATH [MINUTES].\n");
        return 0;
    }
    if (argc >= 3)
    {
        minutes = (int) strtoll(argv[2], NULL, 10);
    }
    char text[AREA_WIDTH * AREA_HEIGHT];
    if (read_scan_input(argv[1], text) == 0)
    {
        Scan s;
        s.upper = NULL;
        s.lower = NULL;
        parse_scan(text, &s);
        printf("Initial Input:\n");
        print_scan(&s);
        part01(s);
        part02(s, minutes);
    }

    return 0;
}
