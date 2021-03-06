/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-08-10
 *
 */

#include "challenge/challenge_lib.h"

void cut(Card* deck, const uint64_t deck_size, int64_t n)
{
    /*Assuming that abs(n) is within deck_size.*/

    /*A cut by -n is equivalent to a cut by (deck_size - n).
     *E.g. with deck_size := 10 a cut with n=-4 is the same as a cut with n=(10-4)=6.*/
    if (n < 0)
    {
        n = deck_size + n;
    }

    Card tmp[n];
    for (uint64_t i = 0; i < n; i++)
    {
        tmp[i] = deck[i];
    }
    for (uint64_t i = n; i < deck_size; i++)
    {
        deck[i - n] = deck[i];
    }
    uint64_t j = 0;
    for (uint64_t i = deck_size - n; i < deck_size; i++)
    {
        deck[i] = tmp[j];
        j++;
    }
}

void cut_index(uint64_t* index, uint64_t deck_size, int64_t n)
{
    /*A cut by -n is equivalent to a cut by (deck_size - n).
     *E.g. with deck_size := 10 a cut with n=-4 is the same as a cut with n=(10-4)=6.*/
    if (n < 0)
    {
        n = deck_size + n;
    }

    if (*index < n)
    {
        /*the index will be cut to the bottom*/
        *index = deck_size - (n - *index);
    }
    else
    {
        *index = *index - n;
    }
}

void deal_into_new_stack(Card* deck, const uint64_t deck_size)
{
    for (uint64_t low = 0, high = deck_size - 1; low < high; low++, high--)
    {
        Card tmp   = deck[low];
        deck[low]  = deck[high];
        deck[high] = tmp;
    }
}

void deal_into_new_stack_index(uint64_t* index, uint64_t deck_size)
{
    *index = deck_size - 1 - *index;
}

void deal_with_increment(Card* deck, const uint64_t deck_size, int64_t inc)
{
    Card tmp[deck_size];
    for (uint64_t i = 0; i < deck_size; i++)
    {
        tmp[(i * inc) % deck_size] = deck[i];
    }
    for (uint64_t i = 0; i < deck_size; i++)
    {
        deck[i] = tmp[i];
    }
}

void deal_with_increment_index(uint64_t* index, uint64_t deck_size, int64_t inc)
{
    *index = (*index * inc) % deck_size;
}
