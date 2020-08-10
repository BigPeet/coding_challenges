/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-08-10
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdint.h"
#include "stdlib.h"

typedef uint32_t Card;

void cut(Card* deck, uint64_t deck_size, int64_t n);
void cut_index(uint64_t* index, uint64_t deck_size, int64_t n);
void deal_into_new_stack(Card* deck, uint64_t deck_size);
void deal_into_new_stack_index(uint64_t* index, uint64_t deck_size);
void deal_with_increment(Card* deck, uint64_t deck_size, int64_t inc);
void deal_with_increment_index(uint64_t* index, uint64_t deck_size, int64_t inc);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
