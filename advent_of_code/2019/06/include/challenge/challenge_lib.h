/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-06
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdlib.h"

typedef struct space_object_t
{
    char* name;
    /*Per task description, this should always be 1 (or 0)*/
    size_t num_direct_orbits;
    struct space_object_t** orbits;
} space_object_t;

typedef struct
{
    size_t num_objects;
    space_object_t** objects;
} map_t;

int map_add_object(map_t* const map, space_object_t* const object);
int map_contains(const map_t* const map, const space_object_t* const object);
int map_get_object_idx(const map_t* const map, const char* const name);
map_t* map_create();
void map_destroy(map_t* const map);
size_t map_total_orbits(const map_t* const map);

void object_init(space_object_t* const object, char* const name);
void object_add_orbit(space_object_t* const object, space_object_t* const orbit);
int object_contains(const space_object_t* const object, const space_object_t* const orbit);
void object_destroy(space_object_t* const object);
size_t object_total_orbits(const space_object_t* const object);

int parse_map(const char* const file_path, map_t* const map);
int parse_objects(char* const line, space_object_t* const center, space_object_t* const orbiter);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
