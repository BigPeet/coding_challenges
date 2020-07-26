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
#define ORBIT_DELIM ")"

static void object_combine(space_object_t* const object, const space_object_t* const to_add);

map_t* map_create()
{
    map_t* map = (map_t*) malloc(sizeof(map_t));
    if (map != NULL)
    {
        map->num_objects = 0;
        map->objects     = (space_object_t**) malloc(0);
    }
    return map;
}

int map_add_object(map_t* const map, space_object_t* const object)
{
    int index = -1;
    if ((map != NULL) && (object != NULL))
    {
        index = map_contains(map, object);
        if (index >= 0)
        {
            object_combine(map->objects[index], object);
            object_destroy(object);
        }
        else
        {
            map->num_objects++;
            map->objects = (space_object_t**) realloc(map->objects,
                                                      sizeof(space_object_t*) * map->num_objects);
            if (map->objects != NULL)
            {
                index               = map->num_objects - 1;
                map->objects[index] = object;
            }
        }
    }
    return index;
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
                if (map->objects[i] != NULL)
                {
                    object_destroy(map->objects[i]);
                }
            }
            free(map->objects);
        }
        free(map);
    }
}

size_t map_total_orbits(const map_t* const map)
{
    size_t total_orbits = 0;
    if (map != NULL)
    {
        for (size_t i = 0; i < map->num_objects; i++)
        {
            total_orbits += object_total_orbits(map->objects[i]);
        }
    }
    return total_orbits;
}

void object_init(space_object_t* const object, char* const name)
{
    if (object != NULL)
    {
        object->name              = name;
        object->num_direct_orbits = 0;
        object->orbits            = (space_object_t**) malloc(sizeof(space_object_t*) * 0);
    }
}

void object_add_orbit(space_object_t* const object, space_object_t* const orbit)
{
    if ((object != NULL) && (orbit != NULL))
    {
        object->num_direct_orbits++;
        object->orbits =
            (space_object_t**) realloc(object->orbits,
                                       sizeof(space_object_t*) * object->num_direct_orbits);
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
        if (object->name != NULL)
        {
            free(object->name);
        }
        free(object);
    }
}

size_t object_total_orbits(const space_object_t* const object)
{
    size_t total_orbits = 0;
    if (object != NULL)
    {
        total_orbits += object->num_direct_orbits;
        for (size_t i = 0; i < object->num_direct_orbits; i++)
        {
            total_orbits += object_total_orbits(object->orbits[i]);
        }
    }
    return total_orbits;
}

space_object_t* object_follow_path(space_object_t* const object, const size_t steps)
{
    space_object_t* goal = NULL;
    if (object != NULL)
    {
        if (steps == 0)
        {
            goal = object;
        }
        else if (object->num_direct_orbits > 0)
        {
            goal = object_follow_path(object->orbits[0], steps - 1);
        }
    }
    return goal;
}

size_t get_minimal_transfers(const space_object_t* const obj_a, const space_object_t* const obj_b)
{
    if ((obj_a != NULL) && (obj_b != NULL))
    {
        space_object_t* center_a = obj_a->orbits[0];
        space_object_t* center_b = obj_b->orbits[0];
        size_t length_a          = object_total_orbits(center_a);
        size_t length_b          = object_total_orbits(center_b);
        for (size_t i = 0; i < length_a; i++)
        {
            space_object_t* path_a = object_follow_path(center_a, i);
            for (size_t j = 0; j < length_b; j++)
            {
                space_object_t* path_b = object_follow_path(center_b, j);
                if (path_a == path_b)
                {
                    return (i + j);
                }
            }
        }
    }
    return 0;
}

int parse_map(const char* const file_path, map_t* const map)
{
    int success = 0;
    if ((file_path != NULL) && (map != NULL))
    {
        FILE* fp = fopen(file_path, "r");
        if (fp != NULL)
        {
            char* line = (char*) malloc(sizeof(char) * BUFFER_LENGTH);
            if (line != NULL)
            {
                size_t num_attempts = 0;
                size_t offset       = 0;
                while ((line != NULL) && fgets(line + offset, BUFFER_LENGTH, fp) != NULL)
                {
                    size_t length = strlen(line);
                    if (line[length - 1] == '\n')
                    {
                        num_attempts = 0;
                        offset       = 0;

                        space_object_t* center  = (space_object_t*) malloc(sizeof(space_object_t));
                        space_object_t* orbiter = (space_object_t*) malloc(sizeof(space_object_t));
                        if ((center != NULL) && (orbiter != NULL))
                        {
                            if (parse_objects(line, center, orbiter))
                            {
                                int index = map_add_object(map, center);
                                if (index >= 0)
                                {
                                    object_add_orbit(orbiter, map->objects[index]);
                                }
                                map_add_object(map, orbiter);
                            }
                        }

                        free(line);
                        line = (char*) malloc(sizeof(char) * BUFFER_LENGTH);
                    }
                    else
                    {
                        num_attempts++;
                        offset = strlen(line);
                        line   = (char*) realloc(line,
                                               sizeof(char) * BUFFER_LENGTH * (num_attempts + 1));
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

int parse_objects(char* const line, space_object_t* const center, space_object_t* const orbiter)
{
    int success = 0;
    if ((line != NULL) && (center != NULL) && (orbiter != NULL))
    {
        /*line should have this form: A)B
         * A is the "center" of the orbit, B is the "orbiter".*/

        /*Split line into two names, init center and orbiter accordingly*/
        char* token = strtok(line, ORBIT_DELIM);
        if (token != NULL)
        {
            char* name = (char*) malloc(sizeof(char) * strlen(token) + 1);
            if (name != NULL)
            {
                object_init(center, strcpy(name, token));
            }
            token = strtok(NULL, ORBIT_DELIM);
        }

        if (token != NULL)
        {
            /*Cut off the \n character*/
            char* name               = (char*) malloc(sizeof(char) * strlen(token));
            token[strlen(token) - 1] = '\0';
            object_init(orbiter, strcpy(name, token));
        }

        success = 1;
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
