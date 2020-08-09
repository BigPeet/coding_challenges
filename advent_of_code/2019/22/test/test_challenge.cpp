/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-08-10
 *
 */

#include <algorithm>
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

TEST_F(challenge_test, cut_test_01)
{
    Card deck[]     = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Card cut_deck[] = {3, 4, 5, 6, 7, 8, 9, 0, 1, 2};

    cut(deck, 10, 3);

    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(deck[i], cut_deck[i]);
    }
}

TEST_F(challenge_test, cut_test_02)
{
    Card deck[]     = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Card cut_deck[] = {6, 7, 8, 9, 0, 1, 2, 3, 4, 5};

    cut(deck, 10, -4);

    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(deck[i], cut_deck[i]);
    }
}

TEST_F(challenge_test, deal_into_new_stack_01)
{
    Card deck[]     = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Card new_deck[10];
    std::reverse_copy(std::begin(deck), std::end(deck), std::begin(new_deck));

    deal_into_new_stack(deck, 10);

    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(deck[i], new_deck[i]);
    }
}

TEST_F(challenge_test, deal_into_new_stack_02)
{
    Card deck[]     = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    Card new_deck[11];
    std::reverse_copy(std::begin(deck), std::end(deck), std::begin(new_deck));

    deal_into_new_stack(deck, 11);

    for (int i = 0; i < 11; i++)
    {
        ASSERT_EQ(deck[i], new_deck[i]);
    }
}

TEST_F(challenge_test, deal_with_increment_01)
{
    Card deck[]     = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    Card new_deck[] = {0, 7, 4, 1, 8, 5, 2, 9, 6, 3};

    deal_with_increment(deck, 10, 3);

    for (int i = 0; i < 10; i++)
    {
        ASSERT_EQ(deck[i], new_deck[i]);
    }
}
