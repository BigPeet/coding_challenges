/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-09
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdlib.h"
#include "challenge/intcode.h"

int** create_permutations(const size_t k, size_t* const num_perms);
int** create_permutations_with_offset(const size_t k, const size_t offset, size_t* const num_perms);
void destroy_permutations(int** const perms, const size_t num_perms);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
