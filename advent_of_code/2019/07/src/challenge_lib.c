/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-09
 *
 */

#include "challenge/challenge_lib.h"
#include "stdio.h"

static void fill_block(size_t* used,
                       size_t num_used,
                       size_t k,
                       size_t col,
                       size_t rows,
                       size_t offset,
                       int** perms);
static int get_smallest_unused_value(const size_t* const used, const size_t length, size_t* val);
static int find_max(const size_t* const numbers, const size_t amount);
static void init_with_zeroes(int* const numbers, const size_t amount);

int** create_permutations(const size_t k, size_t* const num_perms)
{
    int** perms = NULL;
    if (num_perms != NULL)
    {
        size_t num = 1;
        for (size_t i = k; i > 0; i--)
        {
            num *= i;
        }

        /*Allocate memory*/
        perms = (int**)malloc(sizeof(int*) * num);
        if (perms != NULL)
        {
            for (size_t i = 0; i < num; i++)
            {
                perms[i] = (int*)malloc(sizeof(int) * k);
                if (perms[i] == NULL)
                {
                    return NULL;
                }
            }
            *num_perms = num;

            size_t used[k];
            fill_block(used, 0, k, 0, num, 0, perms);
        }
    }

    return perms;
}

void destroy_permutations(int** const perms, const size_t num_perms)
{
    if (perms != NULL)
    {
        for (size_t i = 0; i < num_perms; i++)
        {
            if (perms[i] != NULL)
            {
                free(perms[i]);
            }
        }
        free(perms);
    }
}

static int get_smallest_unused_value(const size_t* const used, const size_t length, size_t* val)
{
    /*This is linear but uses non constant space*/
    int success = 0;
    if (used != NULL)
    {
        if (length == 0)
        {
            *val = 0;
            return 1;
        }
        size_t max      = find_max(used, length);
        size_t solution = 0;
        int found[max + 1];
        init_with_zeroes(found, max + 1);
        for (size_t i = 0; i < length; ++i)
        {
            size_t num = used[i];
            if (found[num] == 0)
            {
                found[num] = 1;
            }
        }
        while (found[solution] == 1)
        {
            solution++;
        }
        *val    = solution;
        success = 1;
    }
    return success;
}

static void fill_block(size_t* used,
                       size_t num_used,
                       size_t k,
                       size_t col,
                       size_t rows,
                       size_t offset,
                       int** perms)
{
    if (num_used < k)
    {
        /*Assuming used is allocated for k numbers*/
        size_t* used_in_rows = (size_t*)malloc(sizeof(size_t) * k);
        if (used_in_rows != NULL)
        {
            size_t num_unused_cpy = num_used;
            for (size_t i = 0; i < num_used; ++i)
            {
                used_in_rows[i] = used[i];
            }
            size_t num_blocks = k - num_used;
            size_t block_size = rows / num_blocks;

            for (size_t i = 0; i < num_blocks; i++)
            {
                size_t val;
                int ret = get_smallest_unused_value(used, num_used, &val);
                for (size_t j = 0; j < block_size; j++)
                {
                    perms[offset + (i * block_size) + j][col] = val;
                }
                used[num_used] = val;
                num_used++;
                used_in_rows[num_unused_cpy] = val;

                fill_block(used_in_rows,
                           num_unused_cpy + 1,
                           k,
                           col + 1,
                           block_size,
                           (offset + (i * block_size)),
                           perms);
            }
            if (used_in_rows != NULL)
            {
                free(used_in_rows);
            }
        }
    }
}

static int find_max(const size_t* const numbers, const size_t amount)
{
    int max = 0;
    for (size_t i = 0; i < amount; ++i)
    {
        if (numbers[i] > max)
        {
            max = numbers[i];
        }
    }
    return max;
}

static void init_with_zeroes(int* const numbers, const size_t amount)
{
    for (size_t i = 0; i < amount; ++i)
    {
        numbers[i] = 0;
    }
}
