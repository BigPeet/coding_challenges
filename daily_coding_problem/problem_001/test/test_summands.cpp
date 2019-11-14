#include "gtest/gtest.h"

extern "C" {
#include "problem/list_check.h"
}

class list_check_test : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(list_check_test, pos_test_01)
{
    int k = 17;
    int amount = 4;
    int numbers[] = {10, 15, 3, 7};
    bool ret = contains_summands(k, amount, numbers);
    ASSERT_TRUE(ret);
}

TEST_F(list_check_test, pos_test_02)
{
    int k = 50;
    int amount = 10;
    int numbers[] = {13, 24, 5, 42, 8, 12, 16, 19, 31, 67};
    bool ret = contains_summands(k, amount, numbers);
    ASSERT_TRUE(ret);
}

TEST_F(list_check_test, pos_test_03)
{
    int k = 105;
    int amount = 10;
    int numbers[] = {21, 61, 28, 50, 84, 63, 47, 21, 32, 48};
    bool ret = contains_summands(k, amount, numbers);
    ASSERT_TRUE(ret);
}

TEST_F(list_check_test, neg_test_01)
{
    int k = 12;
    int amount = 4;
    int numbers[] = {10, 15, 3, 7};
    bool ret = contains_summands(k, amount, numbers);
    ASSERT_FALSE(ret);
}

TEST_F(list_check_test, neg_test_02)
{
    int k = 45;
    int amount = 10;
    int numbers[] = {13, 24, 5, 42, 8, 12, 16, 19, 31, 67};
    bool ret = contains_summands(k, amount, numbers);
    ASSERT_FALSE(ret);
}

TEST_F(list_check_test, neg_test_03)
{
    int k = 12;
    int amount = 10;
    int numbers[] = {21, 61, 28, 50, 84, 63, 47, 21, 32, 48};
    bool ret = contains_summands(k, amount, numbers);
    ASSERT_FALSE(ret);
}
