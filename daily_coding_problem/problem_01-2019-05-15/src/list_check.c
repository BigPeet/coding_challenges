#include "problem/list_check.h"

static bool contains_summands_naive(int k, int amount, int* numbers)
{
    for (int i = 0; i < amount - 1; i++)
    {
        for (int j = i + 1; j < amount; j++)
        {
            int a = numbers[i];
            int b = numbers[j];
            if ((a + b) == k)
            {
                return true;
            }
        }
    }
    return false;
}

static bool contains_summands_single_pass(int k, int amount, int* numbers)
{
    bool flags[k];
    for (int i = 0; i < k; i++)
    {
        flags[i] = false;
    }
    for (int i = 0; i < amount; i++)
    {
        int num = numbers[i];
        if (num < k)
        {
            if (flags[num])
            {
                return true;
            }
            else
            {
                flags[k - num] = true;
            }
        }
    }
    return false;
}

static bool contains_summands_single_pass_negative_numbers(int k,
                                                           int amount,
                                                           int* numbers)
{
    /*TODO: use "dictionary" to map negative integers to boolean value*/
    return false;
}

bool contains_summands(int k, int amount, int* numbers)
{
    return contains_summands_single_pass(k, amount, numbers);
}
