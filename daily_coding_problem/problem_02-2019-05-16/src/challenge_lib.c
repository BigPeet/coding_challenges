#include "challenge/challenge_lib.h"

static void straight_forward(const int* input, const int amount, int* output)
{
    /*in O(n)*/
    /*assuming no overflow*/
    int acc = 1;
    for (int i = 0; i < amount; i++)
    {
        acc *= input[i];
    }
    for (int i = 0; i < amount; i++)
    {
        output[i] = acc / input[i];
    }
}

static void without_division_naive(const int* input,
                                   const int amount,
                                   int* output)
{
    /*in O(n^2)*/
    /*assuming no overflow*/
    for (int i = 0; i < amount; i++)
    {
        int acc = 1;
        for (int j = 0; j < amount; j++)
        {
            if (j != i)
            {
                acc *= input[j];
            }
        }
        output[i] = acc;
    }
}

void process(const int* input, const int amount, int* output)
{
    /*straight_forward(input, amount, output);*/
    without_division_naive(input, amount, output);
}
