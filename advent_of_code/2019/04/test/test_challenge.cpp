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
};

TEST_F(challenge_test, is_in_range_01)
{
    int val = 1000;
    int lower = 1000;
    int upper = 2000;
    ASSERT_TRUE(is_in_range(val, lower, upper));
}

TEST_F(challenge_test, is_in_range_02)
{
    int val = 100;
    int lower = 1000;
    int upper = 2000;
    ASSERT_FALSE(is_in_range(val, lower, upper));
}

TEST_F(challenge_test, is_in_range_03)
{
    int val = 1500;
    int lower = 1000;
    int upper = 2000;
    ASSERT_TRUE(is_in_range(val, lower, upper));
}

TEST_F(challenge_test, is_in_range_04)
{
    int val = 1500;
    int lower = -1000;
    int upper = 2000;
    ASSERT_TRUE(is_in_range(val, lower, upper));
}

TEST_F(challenge_test, is_in_range_05)
{
    int val = 1500;
    int lower = -2000;
    int upper = -1000;
    ASSERT_FALSE(is_in_range(val, lower, upper));
}

TEST_F(challenge_test, has_six_digits_01)
{
    int val = 1500;
    ASSERT_FALSE(has_six_digits(val));
}

TEST_F(challenge_test, has_six_digits_02)
{
    int val = 123456;
    ASSERT_TRUE(has_six_digits(val));
}

TEST_F(challenge_test, no_decreasing_digits_01)
{
    int val = 123456;
    ASSERT_TRUE(no_decreasing_digits(val));
}

TEST_F(challenge_test, no_decreasing_digits_02)
{
    int val = 111111;
    ASSERT_TRUE(no_decreasing_digits(val));
}

TEST_F(challenge_test, no_decreasing_digits_03)
{
    int val = 12224;
    ASSERT_TRUE(no_decreasing_digits(val));
}

TEST_F(challenge_test, no_decreasing_digits_04)
{
    int val = 12220;
    ASSERT_FALSE(no_decreasing_digits(val));
}

TEST_F(challenge_test, no_decreasing_digits_05)
{
    int val = 223450;
    ASSERT_FALSE(no_decreasing_digits(val));
}

TEST_F(challenge_test, has_repeating_digits_01)
{
    int val = 223450;
    ASSERT_TRUE(has_repeating_digits(val));
}

TEST_F(challenge_test, has_repeating_digits_02)
{
    int val = 111111;
    ASSERT_TRUE(has_repeating_digits(val));
}

TEST_F(challenge_test, has_repeating_digits_03)
{
    int val = 123789;
    ASSERT_FALSE(has_repeating_digits(val));
}

TEST_F(challenge_test, has_pair_repeating_digits_01)
{
    int val = 123789;
    ASSERT_FALSE(has_pair_repeating_digits(val));
}

TEST_F(challenge_test, has_pair_repeating_digits_02)
{
    int val = 111111;
    ASSERT_FALSE(has_pair_repeating_digits(val));
}

TEST_F(challenge_test, has_pair_repeating_digits_03)
{
    int val = 223450;
    ASSERT_TRUE(has_pair_repeating_digits(val));
}

TEST_F(challenge_test, has_pair_repeating_digits_04)
{
    int val = 112233;
    ASSERT_TRUE(has_pair_repeating_digits(val));
}

TEST_F(challenge_test, has_pair_repeating_digits_05)
{
    int val = 123444;
    ASSERT_FALSE(has_pair_repeating_digits(val));
}

TEST_F(challenge_test, has_pair_repeating_digits_06)
{
    int val = 111122;
    ASSERT_TRUE(has_pair_repeating_digits(val));
}

TEST_F(challenge_test, has_pair_repeating_digits_07)
{
    int val = 333221;
    ASSERT_TRUE(has_pair_repeating_digits(val));
}

TEST_F(challenge_test, fits_first_set_criteria_01)
{
    int val = 111111;
    int lower = 100000;
    int upper = 999999;
    ASSERT_TRUE(fits_first_set_criteria(val, lower, upper));
}

TEST_F(challenge_test, fits_first_set_criteria_02)
{
    int val = 223450;
    int lower = 100000;
    int upper = 999999;
    ASSERT_FALSE(fits_first_set_criteria(val, lower, upper));
}

TEST_F(challenge_test, fits_first_set_criteria_03)
{
    int val = 123789;
    int lower = 100000;
    int upper = 999999;
    ASSERT_FALSE(fits_first_set_criteria(val, lower, upper));
}

TEST_F(challenge_test, fits_second_set_criteria_01)
{
    int val = 111111;
    int lower = 100000;
    int upper = 999999;
    ASSERT_FALSE(fits_second_set_criteria(val, lower, upper));
}

TEST_F(challenge_test, fits_second_set_criteria_02)
{
    int val = 223450;
    int lower = 100000;
    int upper = 999999;
    ASSERT_FALSE(fits_second_set_criteria(val, lower, upper));
}

TEST_F(challenge_test, fits_second_set_criteria_03)
{
    int val = 123789;
    int lower = 100000;
    int upper = 999999;
    ASSERT_FALSE(fits_second_set_criteria(val, lower, upper));
}

TEST_F(challenge_test, fits_second_set_criteria_04)
{
    int val = 112233;
    int lower = 100000;
    int upper = 999999;
    ASSERT_TRUE(fits_second_set_criteria(val, lower, upper));
}

TEST_F(challenge_test, fits_second_set_criteria_05)
{
    int val = 111122;
    int lower = 100000;
    int upper = 999999;
    ASSERT_TRUE(fits_second_set_criteria(val, lower, upper));
}
