#include "gtest/gtest.h"

extern "C" {
#include "challenge/challenge_lib.h"
}

class challenge_test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    void check_output(const int* ground_truth,
                      const int* processed_solution,
                      const int amount)
    {
        for (int i = 0; i < amount; i++)
        {
            ASSERT_EQ(ground_truth[i], processed_solution[i]);
        }
    }
};

TEST_F(challenge_test, test_01)
{
    int input[]        = {1, 2, 3, 4, 5};
    int ground_truth[] = {120, 60, 40, 30, 24};
    int amount         = 5;
    int output[amount];
    process(input, amount, output);

    check_output(ground_truth, output, amount);
}

TEST_F(challenge_test, test_02)
{
    int input[]        = {1, 2, 3};
    int ground_truth[] = {6, 3, 2};
    int amount         = 3;
    int output[amount];
    process(input, amount, output);

    check_output(ground_truth, output, amount);
}

TEST_F(challenge_test, test_03)
{
    int input[]        = {10, 12, 4, 7, 9, 13, 3};
    int ground_truth[] = {117936, 98280, 294840, 168480, 131040, 90720, 393120};
    int amount         = 7;
    int output[amount];
    process(input, amount, output);

    check_output(ground_truth, output, amount);
}

TEST_F(challenge_test, test_04)
{
    int input[]        = {1, -2, 3};
    int ground_truth[] = {-6, 3, -2};
    int amount         = 3;
    int output[amount];
    process(input, amount, output);

    check_output(ground_truth, output, amount);
}

TEST_F(challenge_test, test_05)
{
    int input[]        = {1, 2, -3, -4, 5};
    int ground_truth[] = {120, 60, -40, -30, 24};
    int amount         = 5;
    int output[amount];
    process(input, amount, output);

    check_output(ground_truth, output, amount);
}
