#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdlib.h"

typedef struct
{
    int x;
    int y;
} point_t;

typedef struct
{
    point_t* a;
    point_t* b;
} line_t;

typedef struct
{
    line_t** lines;
    size_t num_lines;
} wire_t;


wire_t** read_wires(const char* const file_path, size_t* const num_wires);
void destroy_wire(wire_t* wire);

int intersect_wires_manhattan(const wire_t* const a, const wire_t* const b, point_t* location);
int intersect_wires_steps(const wire_t* const a, const wire_t* const b, point_t* location);
int intersect_lines(const line_t* const l1, const line_t* const l2, point_t* location);

int manhattan_distance(const point_t* const a, const point_t* const b);
int distance_to_point(const wire_t* const w, const point_t* const p);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
