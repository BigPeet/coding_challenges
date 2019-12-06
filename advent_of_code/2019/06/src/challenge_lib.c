/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-06
 *
 */

#include "challenge/challenge_lib.h"
#include "stdio.h"
#include "string.h"

static void object_combine(space_object_t* const object, const space_object_t* const to_add);

void map_init(map_t* const map)
{
    if (map != NULL)
    {
        map->num_objects = 0;
        map->objects     = (space_object_t**)malloc(sizeof(space_object_t*) * 0);
    }
}

void map_add_object(map_t* const map, space_object_t* const object)
{
    if ((map != NULL) && (object != NULL))
    {
        int index = map_contains(map, object);
        if (index >= 0)
        {
            object_combine(map->objects[index], object);
            object_destroy(object);
        }
        else
        {
            map->num_objects++;
            map->objects =
                (space_object_t**)realloc(map->objects, sizeof(space_object_t*) * map->num_objects);
            if (map->objects != NULL)
            {
                map->objects[map->num_objects - 1] = object;
            }
        }
    }
}

int map_contains(const map_t* const map, const space_object_t* const object)
{
    int index = -1;
    if ((map != NULL) && (object != NULL))
    {
        for (size_t i = 0; i < map->num_objects; i++)
        {
            if (strcmp(map->objects[i]->name, object->name) == 0)
            {
                index = i;
                break;
            }
        }
    }
    return index;
}

void object_init(space_object_t* const object, char* const name)
{
    if (object != NULL)
    {
        object->name              = name;
        object->num_direct_orbits = 0;
        object->orbits            = (space_object_t**)malloc(sizeof(space_object_t*) * 0);
    }
}

void object_add_orbit(space_object_t* const object, space_object_t* const orbit)
{
    if ((object != NULL) && (orbit != NULL))
    {
        object->num_direct_orbits++;
        object->orbits = (space_object_t**)realloc(
            object->orbits, sizeof(space_object_t*) * object->num_direct_orbits);
        if (object->orbits != NULL)
        {
            object->orbits[object->num_direct_orbits - 1] = orbit;
        }
    }
}

int object_contains(const space_object_t* const object, const space_object_t* const orbit)
{
    int index = -1;
    if ((object != NULL) && (orbit != NULL))
    {
        for (size_t i = 0; i < object->num_direct_orbits; i++)
        {
            if (strcmp(object->orbits[i]->name, orbit->name) == 0)
            {
                index = i;
                break;
            }
        }
    }
    return index;
}

void object_destroy(space_object_t* const object)
{
    if (object != NULL)
    {
        if (object->orbits != NULL)
        {
            free(object->orbits);
        }
        free(object);
    }
}

int parse_objects(const char* const line, space_object_t* const center, space_object_t* const orbiter)
{
    int success = 0;
    if ((line != NULL) && (center != NULL) && (orbiter != NULL))
    {
      /*line should have this form: A)B
       * A is the "center" of the orbite, B is the "orbiter".*/

      /*Split line into two names, init center and orbiter accordingly*/
    }
    return success;
}

static void object_combine(space_object_t* const object, const space_object_t* const to_add)
{
    if ((object != NULL) && (to_add != NULL))
    {
        for (size_t i = 0; i < to_add->num_direct_orbits; i++)
        {
            int index = object_contains(object, to_add->orbits[i]);
            if (index < 0)
            {
                /*Orbit is new, add it to object*/
                object_add_orbit(object, to_add->orbits[i]);
            }
        }
    }
}
