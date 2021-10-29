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

/*Thread functions*/
void* nic_func(void* args);
void* control_part01(void* args);
void* control_part02(void* args);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
