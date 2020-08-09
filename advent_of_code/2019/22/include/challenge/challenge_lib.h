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

void cut(Card* deck, size_t deck_size, int64_t n);
void deal_into_new_stack(Card* deck, size_t deck_size);
void deal_with_increment(Card* deck, size_t deck_size, size_t inc);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
