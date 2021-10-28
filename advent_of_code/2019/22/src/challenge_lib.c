/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-08-10
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"

// Part 1

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

void deal_into_new_stack(Card* deck, const uint64_t deck_size)
{
    for (uint64_t low = 0, high = deck_size - 1; low < high; low++, high--)
    {
        Card tmp   = deck[low];
        deck[low]  = deck[high];
        deck[high] = tmp;
    }
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

// Part 2

static int128_t mod(int128_t a, int128_t b)
{
    return (a >= 0) ? (a % b) : (b + a % b);
}

static int128_t gcd_extended(int128_t a, int128_t b, int128_t* x, int128_t* y)
{
    if (a == 0)
    {
        *x = 0;
        *y = 1;
        return b;
    }

    int128_t x1  = 0;
    int128_t y1  = 0;
    int128_t gcd = gcd_extended(b % a, a, &x1, &y1);
    *x           = y1 - (b / a) * x1;
    *y           = x1;
    return gcd;
}


static int128_t modular_power(int128_t base, int128_t exponent, int128_t n)
{
    assert(exponent >= 0);
    if (exponent == 0)
    {
        return (base == 0) ? 0 : 1;
    }

    int128_t bit   = 1;
    int128_t power = mod(base, n);
    int128_t out   = 1;
    while (bit <= exponent)
    {
        if (exponent & bit)
        {
            out = mod(out * power, n);
        }
        power = mod(power * power, n);
        bit <<= 1;
    }

    return out;
}

static int128_t modular_inverse(int128_t b, int128_t n)
{
    int128_t x = 0;
    int128_t y = 0;
    int128_t g = gcd_extended(b, n, &x, &y);
    return (g != 1) ? -1 : mod(x, n);
}

static int128_t modular_divide(int128_t a, int128_t b, int128_t n)
{
    a            = mod(a, n);
    int128_t inv = modular_inverse(b, n);
    return (inv == -1) ? -1 : (a * inv) % n;
}

static ReverseParams get_params_to_reverse_shuffle(Instruction const* instructions,
                                                   int128_t instructions_size,
                                                   int128_t deck_size)
{
    ReverseParams ret;
    ret.factor = 1;
    ret.offset = 0;


    for (int128_t i = 0; i < instructions_size; i++)
    {
        int128_t a      = 0;
        int128_t b      = 0;
        Instruction cur = instructions[i];
        int128_t arg    = cur.param;
        switch (cur.technique)
        {
            case DealIntoNew:
                a = -1;
                b = deck_size - 1;
                break;
            case Cut:
                if (arg < 0)
                {
                    arg += deck_size;
                }
                a = 1;
                b = deck_size - arg;
                break;
            case DealWithIncrement:
                a = arg;
                b = 0;
                break;
        }
        ret.factor = mod(a * ret.factor, deck_size);
        ret.offset = mod(a * ret.offset + b, deck_size);
    }
    return ret;
}

int128_t no_in_position_after(Instruction const* instructions,
                              int128_t instructions_size,
                              int128_t index,
                              int128_t deck_size,
                              int128_t loops)
{
    ReverseParams rev_params =
        get_params_to_reverse_shuffle(instructions, instructions_size, deck_size);

    int128_t fullFactor = modular_power(rev_params.factor, loops, deck_size);
    int128_t fullOffset =
        mod(rev_params.offset * modular_divide(fullFactor - 1, rev_params.factor - 1, deck_size),
            deck_size);

    return mod(modular_divide(mod(index - fullOffset, deck_size), fullFactor, deck_size),
               deck_size);
}
