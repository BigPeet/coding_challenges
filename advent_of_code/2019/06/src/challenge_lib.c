/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-06
 *
 */

#include "challenge/challenge_lib.h"
#include "stdio.h"
#include "string.h"

#define BUFFER_LENGTH (size_t)(20)

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

int map_get_object_idx(const map_t* const map, const char* const name)
{
    int index = -1;
    if ((map != NULL) && (name != NULL))
    {
        for (size_t i = 0; i < map->num_objects; i++)
        {
            if (strcmp(map->objects[i]->name, name) == 0)
            {
                index = i;
                break;
            }
        }
    }
    return index;
}

void map_destroy(map_t* const map)
{
    if (map != NULL)
    {
        if (map->objects != NULL)
        {
            for (size_t i = 0; i < map->num_objects; i++)
            {
                if (map->objects[i] == NULL)
                {
                    object_destroy(map->objects[i]);
                }
            }
            free(map->objects);
        }
        free(map);
    }
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

int parse_map(const char* const file_path, map_t* const map)
{
    int success = 0;
    if ((file_path != NULL) && (map != NULL))
    {
        map_init(map);
        FILE* fp = fopen(file_path, "r");
        if (fp != NULL)
        {
            char* line = (char*)malloc(sizeof(char) * BUFFER_LENGTH);
            if (line != NULL)
            {
                size_t num_attempts = 0;
                while ((line != NULL) && fgets(line + strlen(line), BUFFER_LENGTH, fp) != NULL)
                {
                    size_t length = strlen(line);
                    if (line[length - 1] == '\n')
                    {
                        num_attempts = 0;

                        /*TODO line has been read, use it*/

                        free(line);
                        line = (char*)malloc(sizeof(char) * BUFFER_LENGTH);
                    }
                    else
                    {
                        num_attempts++;
                        line =
                            (char*)realloc(line, sizeof(char) * BUFFER_LENGTH * (num_attempts + 1));
                    }
                }
                if (line != NULL)
                {
                    free(line);
                }
            }
            fclose(fp);
        }
    }
    return success;
}

int parse_objects(const char* const line,
                  space_object_t* const center,
                  space_object_t* const orbiter)
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
