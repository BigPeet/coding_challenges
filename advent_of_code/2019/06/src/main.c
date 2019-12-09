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
        /*for (int i = 0; i < map->num_objects; ++i)*/
        /*{*/
            /*space_object_t* obj = map->objects[i];*/
            /*printf("%s has %zu total orbits.\n", obj->name, object_total_orbits(obj));*/
        /*}*/
        printf("Total orbits: %zu\n", map_total_orbits(map));
        map_destroy(map);
    }

    return 0;
}
