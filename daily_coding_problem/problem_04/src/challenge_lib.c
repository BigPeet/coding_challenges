#include "challenge/challenge_lib.h"

static void remove_negative_numbers(int* const numbers, const size_t amount);
static void decrease_by_one(int* const numbers, const size_t amount);
static void init_with_zeroes(int* const numbers, const size_t amount);
static int find_max(const int* const numbers, const size_t amount);
static int find_positive_min(const int* const numbers, const size_t amount);

int nonlinear_solution(int* const numbers, const size_t amount)
{
  /* this could also be any sort algorithm in O(n logn)*/
  /*Constant space, but not linear (but O(n^2))*/
  int solution = -1;
  if (numbers != NULL)
  {
    solution   = 0;
    int target = 1;
    remove_negative_numbers(numbers, amount);
    while (find_positive_min(numbers, amount) == target)
    {
      decrease_by_one(numbers, amount);
      solution++;
    }
  }
  return solution + 1;
}

int nonconstant_solution(int* const numbers, const size_t amount)
{
  /*This is linear but uses non constant space*/
  int solution = -1;
  if (numbers != NULL)
  {
    solution = 0;
    remove_negative_numbers(numbers, amount);
    int max = find_max(numbers, amount);
    int found[max];
    init_with_zeroes(found, max);
    for (size_t i = 0; i < amount; ++i)
    {
      int num = numbers[i];
      if (num > 0 && found[num - 1] == 0)
      {
        found[num - 1] = 1;
      }
    }
    while (found[solution] == 1)
    {
      solution++;
    }
  }
  return solution + 1;
}

static void remove_negative_numbers(int* const numbers, const size_t amount)
{
  for (size_t i = 0; i < amount; ++i)
  {
    if (numbers[i] < 0)
    {
      numbers[i] = 0;
    }
  }
}

static void init_with_zeroes(int* const numbers, const size_t amount)
{
  for (size_t i = 0; i < amount; ++i)
  {
    numbers[i] = 0;
  }
}

static int find_max(const int* const numbers, const size_t amount)
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

static int find_positive_min(const int* const numbers, const size_t amount)
{
  int min = __INT_MAX__;
  for (size_t i = 0; i < amount; ++i)
  {
    if (numbers[i] > 0 && numbers[i] < min)
    {
      min = numbers[i];
    }
  }
  return min;
}

static void decrease_by_one(int* const numbers, const size_t amount)
{
  for (size_t i = 0; i < amount; ++i)
  {
    if (numbers[i] > 0)
    {
      numbers[i]--;
    }
  }
}
