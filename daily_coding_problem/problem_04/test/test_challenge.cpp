#include "stdlib.h"
#include "gtest/gtest.h"

extern "C" {
#include "challenge/challenge_lib.h"
}

class challenge_test : public ::testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

int* example_01(size_t* const amount, int* solution)
{
  int* numbers = NULL;
  if ((amount != NULL) && (solution != NULL))
  {
    *amount   = 4;
    *solution = 2;
    numbers   = (int*)calloc(*amount, sizeof(int));
    if (numbers != NULL)
    {
      numbers[0] = 3;
      numbers[1] = 4;
      numbers[2] = -1;
      numbers[3] = 1;
    }
  }
  return numbers;
}

int* example_02(size_t* const amount, int* solution)
{
  int* numbers = NULL;
  if ((amount != NULL) && (solution != NULL))
  {
    *amount   = 3;
    *solution = 3;
    numbers   = (int*)calloc(*amount, sizeof(int));
    if (numbers != NULL)
    {
      numbers[0] = 1;
      numbers[1] = 2;
      numbers[2] = 0;
    }
  }
  return numbers;
}

int* example_03(size_t* const amount, int* solution)
{
  int* numbers = NULL;
  if ((amount != NULL) && (solution != NULL))
  {
    *amount   = 8;
    *solution = 3;
    numbers   = (int*)calloc(*amount, sizeof(int));
    if (numbers != NULL)
    {
      numbers[0] = 1;
      numbers[1] = 2;
      numbers[2] = 0;
      numbers[3] = 5;
      numbers[4] = 7;
      numbers[5] = 8;
      numbers[6] = 4;
      numbers[7] = 7;
    }
  }
  return numbers;
}

int* example_04(size_t* const amount, int* solution)
{
  int* numbers = NULL;
  if ((amount != NULL) && (solution != NULL))
  {
    *amount   = 8;
    *solution = 6;
    numbers   = (int*)calloc(*amount, sizeof(int));
    if (numbers != NULL)
    {
      numbers[0] = 1;
      numbers[1] = 2;
      numbers[2] = 3;
      numbers[3] = 5;
      numbers[4] = 7;
      numbers[5] = 8;
      numbers[6] = 4;
      numbers[7] = 7;
    }
  }
  return numbers;
}

int* example_05(size_t* const amount, int* solution)
{
  int* numbers = NULL;
  if ((amount != NULL) && (solution != NULL))
  {
    *amount   = 9;
    *solution = 9;
    numbers   = (int*)calloc(*amount, sizeof(int));
    if (numbers != NULL)
    {
      numbers[0] = 1;
      numbers[1] = 2;
      numbers[2] = -3;
      numbers[3] = 5;
      numbers[4] = 7;
      numbers[5] = 8;
      numbers[6] = 4;
      numbers[7] = 6;
      numbers[8] = 3;
    }
  }
  return numbers;
}

int* example_06(size_t* const amount, int* solution)
{
  int* numbers = NULL;
  if ((amount != NULL) && (solution != NULL))
  {
    *amount   = 10000;
    *solution = 10001;
    numbers   = (int*)calloc(*amount, sizeof(int));
    if (numbers != NULL)
    {
      for (size_t i = 0; i < *amount; ++i)
      {
        numbers[i] = i + 1;
      }
    }
  }
  return numbers;
}

TEST_F(challenge_test, nonlinear_solution_test_01)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_01(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonlinear_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonlinear_solution_test_02)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_02(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonlinear_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonlinear_solution_test_03)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_03(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonlinear_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonlinear_solution_test_04)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_04(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonlinear_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonlinear_solution_test_05)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_05(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonlinear_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonlinear_solution_test_06)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_06(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonlinear_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonconstant_solution_test_01)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_01(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonconstant_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonconstant_solution_test_02)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_02(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonconstant_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonconstant_solution_test_03)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_03(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonconstant_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonconstant_solution_test_04)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_04(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonconstant_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonconstant_solution_test_05)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_05(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonconstant_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}

TEST_F(challenge_test, nonconstant_solution_test_06)
{
  size_t amount   = 0;
  int groundtruth = -1;
  int* numbers    = example_06(&amount, &groundtruth);
  if (numbers != NULL)
  {
    int solution = nonconstant_solution(numbers, amount);
    ASSERT_EQ(solution, groundtruth);
    free(numbers);
  }
}
