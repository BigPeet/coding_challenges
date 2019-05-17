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

TEST_F(challenge_test, pos_test_01)
{
    bool ret = true;
    ASSERT_TRUE(ret);
}
