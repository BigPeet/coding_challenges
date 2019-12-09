/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-06
 *
 */

#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_06 FILE_PATH.\n");
        return 0;
    }

    map_t* map = map_create();
    if (map != NULL)
    {
        parse_map(argv[1], map);
        printf("Map Size: %zu\n", map->num_objects);
        printf("Total orbits: %zu\n", map_total_orbits(map));

        int you_idx   = map_get_object_idx(map, "YOU");
        int santa_idx = map_get_object_idx(map, "SAN");
        if ((you_idx > 0) && (santa_idx > 0))
        {
            space_object_t* you   = map->objects[you_idx];
            space_object_t* santa = map->objects[santa_idx];
            printf("Num of orbital transfers between %s and %s: %zu\n",
                   you->name,
                   santa->name,
                   get_minimal_transfers(you, santa));
        }
        map_destroy(map);
    }

    return 0;
}
