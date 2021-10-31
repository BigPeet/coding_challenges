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

typedef struct
{
    ASCII* nics;
    int num_of_nics;
} ControllerParams;

/*Thread functions*/
void* nic_func(void* args);
void* nat_control(void* args);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
