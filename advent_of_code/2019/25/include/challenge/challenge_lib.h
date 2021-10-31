/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "challenge/intcode.h"

typedef struct
{
    intcode_t* brain;
    int finished;
} ASCII;

void* drone_func(void* args);
void* control_func(void* args);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
