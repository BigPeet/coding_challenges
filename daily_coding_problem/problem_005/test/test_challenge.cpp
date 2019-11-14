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

TEST_F(challenge_test, pair_test_01)
{
    int a = 3;
    int b = 4;
    pair_t p = cons(3, 4);
    ASSERT_EQ(car(p), a);
    ASSERT_EQ(cdr(p), b);
    ASSERT_EQ(car(cons(a, b)), a);
    ASSERT_EQ(cdr(cons(a, b)), b);
}
