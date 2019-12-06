/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-04
 *
 */

#include "challenge/challenge_lib.h"

int fits_first_set_criteria(const int passcode, const int lower, const int upper)
{
    return (is_in_range(passcode, lower, upper) && has_six_digits(passcode) &&
            no_decreasing_digits(passcode) && has_repeating_digits(passcode));
}

int fits_second_set_criteria(const int passcode, const int lower, const int upper)
{
    return (fits_first_set_criteria(passcode, lower, upper) && has_pair_repeating_digits(passcode));
}

int num_of_solutions(const int lower_bound, const int upper_bound, const criteria_f critfunc)
{
    int sum = 0;
    for (int i = lower_bound; i < upper_bound; ++i)
    {
        if (critfunc(i, lower_bound, upper_bound))
        {
            sum++;
        }
    }
    return sum;
}

int is_in_range(const int passcode, const int lower, const int upper)
{
    return (passcode >= lower) && (passcode <= upper);
}

int has_six_digits(const int passcode)
{
    return is_in_range(passcode, 100000, 999999);
}

int no_decreasing_digits(const int passcode)
{
    int no_decrease = 1;
    int tmp         = passcode;
    int mod         = 10;
    int x           = tmp % mod;
    int y           = 0;
    while (tmp != 0)
    {
        tmp /= mod;
        if (tmp != 0)
        {
            y = tmp % mod;
            if (x < y)
            {
                no_decrease = 0;
                break;
            }
            x = y;
        }
    }

    return no_decrease;
}

int has_repeating_digits(const int passcode)
{
    int repeat = 0;
    int tmp    = passcode;
    int mod    = 10;
    int x      = tmp % mod;
    int y      = 0;
    while (tmp != 0)
    {
        tmp /= mod;
        if (tmp != 0)
        {
            y = tmp % mod;
            if (x == y)
            {
                repeat = 1;
                break;
            }
            x = y;
        }
    }
    return repeat;
}

int has_pair_repeating_digits(const int passcode)
{
    int repeat        = 0;
    int streak_length = 0;
    int tmp           = passcode;
    int mod           = 10;
    int x             = tmp % mod;
    int y             = 0;
    while (tmp != 0)
    {
        tmp /= mod;
        if (tmp != 0)
        {
            y = tmp % mod;
            if ((x == y) && (!repeat))
            {
                repeat        = 1;
                streak_length = 2;
            }
            else if ((x != y) && (repeat))
            {
                repeat = 0;
                if (streak_length == 2)
                {
                    return 1;
                }
                else
                {
                    streak_length = 0;
                }
            }
            else if ((x == y) && (repeat))
            {
                streak_length++;
            }
            x = y;
        }
    }
    return (streak_length == 2);
}
