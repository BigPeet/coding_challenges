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

TEST_F(challenge_test, add_test_01)
{
    size_t nums = 4;
    int content[nums];
    content[0] = 1;
    content[1] = 10;
    content[2] = 20;
    content[3] = 40;

    int first    = 1;
    int second   = 2;
    int result   = 3;
    int solution = 30;

    intcode_t prog;
    prog.content      = content;
    prog.head         = 0;
    prog.content_size = nums;

    add_op(&prog, first, second, result);

    ASSERT_EQ(prog.content[result], solution);
}

TEST_F(challenge_test, multiply_test_01)
{
    size_t nums = 4;
    int content[nums];
    content[0] = 1;
    content[1] = 10;
    content[2] = 20;
    content[3] = 40;

    int first    = 1;
    int second   = 2;
    int result   = 3;
    int solution = 200;

    intcode_t prog;
    prog.content      = content;
    prog.head         = 0;
    prog.content_size = nums;

    multiply_op(&prog, first, second, result);

    ASSERT_EQ(prog.content[result], solution);
}

TEST_F(challenge_test, execute_test_01)
{
    size_t nums = 5;
    int content[nums];
    content[0] = 1;
    content[1] = 0;
    content[2] = 0;
    content[3] = 0;
    content[4] = 99;

    int solution[nums];
    solution[0] = 2;
    solution[1] = 0;
    solution[2] = 0;
    solution[3] = 0;
    solution[4] = 99;

    intcode_t prog;
    prog.content      = content;
    prog.head         = 0;
    prog.content_size = nums;

    int ret = execute(&prog);

    ASSERT_EQ(ret, 1);
    for (int i = 0; i < nums; ++i)
    {
        ASSERT_EQ(prog.content[i], solution[i]);
    }
}

TEST_F(challenge_test, execute_test_02)
{
    size_t nums = 5;
    int content[nums];
    content[0] = 2;
    content[1] = 3;
    content[2] = 0;
    content[3] = 3;
    content[4] = 99;

    int solution[nums];
    solution[0] = 2;
    solution[1] = 3;
    solution[2] = 0;
    solution[3] = 6;
    solution[4] = 99;

    intcode_t prog;
    prog.content      = content;
    prog.head         = 0;
    prog.content_size = nums;

    int ret = execute(&prog);

    ASSERT_EQ(ret, 1);
    for (int i = 0; i < nums; ++i)
    {
        ASSERT_EQ(prog.content[i], solution[i]);
    }
}

TEST_F(challenge_test, execute_test_03)
{
    size_t nums = 6;
    int content[nums];
    content[0] = 2;
    content[1] = 4;
    content[2] = 4;
    content[3] = 5;
    content[4] = 99;
    content[5] = 0;

    int solution[nums];
    solution[0] = 2;
    solution[1] = 4;
    solution[2] = 4;
    solution[3] = 5;
    solution[4] = 99;
    solution[5] = 9801;

    intcode_t prog;
    prog.content      = content;
    prog.head         = 0;
    prog.content_size = nums;

    int ret = execute(&prog);

    ASSERT_EQ(ret, 1);
    for (int i = 0; i < nums; ++i)
    {
        ASSERT_EQ(prog.content[i], solution[i]);
    }
}

TEST_F(challenge_test, execute_test_04)
{
    size_t nums = 9;
    int content[nums];
    content[0] = 1;
    content[1] = 1;
    content[2] = 1;
    content[3] = 4;
    content[4] = 99;
    content[5] = 5;
    content[6] = 6;
    content[7] = 0;
    content[8] = 99;

    int solution[nums];
    solution[0] = 30;
    solution[1] = 1;
    solution[2] = 1;
    solution[3] = 4;
    solution[4] = 2;
    solution[5] = 5;
    solution[6] = 6;
    solution[7] = 0;
    solution[8] = 99;

    intcode_t prog;
    prog.content      = content;
    prog.head         = 0;
    prog.content_size = nums;

    int ret = execute(&prog);

    ASSERT_EQ(ret, 1);
    for (int i = 0; i < nums; ++i)
    {
        ASSERT_EQ(prog.content[i], solution[i]);
    }
}
