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

// Part 1

typedef uint32_t Card;

void cut(Card* deck, uint64_t deck_size, int64_t n);
void deal_into_new_stack(Card* deck, uint64_t deck_size);
void deal_with_increment(Card* deck, uint64_t deck_size, int64_t inc);

// Part 2

typedef __int128 int128_t;

typedef enum Technique
{
    DealIntoNew,
    Cut,
    DealWithIncrement
} Technique;

typedef struct Instruction
{
    Technique technique;
    int128_t param;
} Instruction;

typedef struct ReverseParams
{
    int128_t factor;
    int128_t offset;
} ReverseParams;

int128_t no_in_position_after(Instruction const* instructions,
                              int128_t instructions_size,
                              int128_t index,
                              int128_t deck_size,
                              int128_t loops);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
