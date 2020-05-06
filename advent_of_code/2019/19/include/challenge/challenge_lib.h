/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-05-06
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "challenge/intcode.h"

/*Thread functions*/
void* system_func(void* args);
void* control_func(void* args);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
