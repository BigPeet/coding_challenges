/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#include "challenge/challenge_lib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_24 FILE_PATH.\n");
        return 0;
    }
    char text[AREA_WIDTH * AREA_HEIGHT];
    if (read_scan_input(argv[1], text) == 0)
    {
        ScanList* list = list_create();
        Scan s;
        parse_scan(text, &s);
        list_add(list, &s);
        while (1)
        {
            Scan next;
            apply_step(&s, &next);
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

    return 0;
}
