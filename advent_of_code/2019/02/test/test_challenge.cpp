/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-02
 *
 */

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

TEST_F(challenge_test, add_test_01)
{
    size_t nums = 4;
    int memory[nums];
    memory[0] = 1;
    memory[1] = 10;
    memory[2] = 20;
    memory[3] = 40;

    int first    = 1;
    int second   = 2;
    int result   = 3;
    int solution = 30;

    intcode_t prog;
    prog.memory      = memory;
    prog.head        = 0;
    prog.memory_size = nums;

    add_op(&prog, first, second, result);

    ASSERT_EQ(prog.memory[result], solution);
}

TEST_F(challenge_test, multiply_test_01)
{
    size_t nums = 4;
    int memory[nums];
    memory[0] = 1;
    memory[1] = 10;
    memory[2] = 20;
    memory[3] = 40;

    int first    = 1;
    int second   = 2;
    int result   = 3;
    int solution = 200;

    intcode_t prog;
    prog.memory      = memory;
    prog.head        = 0;
    prog.memory_size = nums;

    multiply_op(&prog, first, second, result);

    ASSERT_EQ(prog.memory[result], solution);
}

TEST_F(challenge_test, execute_test_01)
{
    size_t nums = 5;
    int memory[nums];
    memory[0] = 1;
    memory[1] = 0;
    memory[2] = 0;
    memory[3] = 0;
    memory[4] = 99;

    int solution[nums];
    solution[0] = 2;
    solution[1] = 0;
    solution[2] = 0;
    solution[3] = 0;
    solution[4] = 99;

    intcode_t prog;
    prog.memory      = memory;
    prog.head        = 0;
    prog.memory_size = nums;

    int ret = execute(&prog);

    ASSERT_EQ(ret, 1);
    for (int i = 0; i < nums; ++i)
    {
        ASSERT_EQ(prog.memory[i], solution[i]);
    }
}

TEST_F(challenge_test, execute_test_02)
{
    size_t nums = 5;
    int memory[nums];
    memory[0] = 2;
    memory[1] = 3;
    memory[2] = 0;
    memory[3] = 3;
    memory[4] = 99;

    int solution[nums];
    solution[0] = 2;
    solution[1] = 3;
    solution[2] = 0;
    solution[3] = 6;
    solution[4] = 99;

    intcode_t prog;
    prog.memory      = memory;
    prog.head        = 0;
    prog.memory_size = nums;

    int ret = execute(&prog);

    ASSERT_EQ(ret, 1);
    for (int i = 0; i < nums; ++i)
    {
        ASSERT_EQ(prog.memory[i], solution[i]);
    }
}

TEST_F(challenge_test, execute_test_03)
{
    size_t nums = 6;
    int memory[nums];
    memory[0] = 2;
    memory[1] = 4;
    memory[2] = 4;
    memory[3] = 5;
    memory[4] = 99;
    memory[5] = 0;

    int solution[nums];
    solution[0] = 2;
    solution[1] = 4;
    solution[2] = 4;
    solution[3] = 5;
    solution[4] = 99;
    solution[5] = 9801;

    intcode_t prog;
    prog.memory      = memory;
    prog.head        = 0;
    prog.memory_size = nums;

    int ret = execute(&prog);

    ASSERT_EQ(ret, 1);
    for (int i = 0; i < nums; ++i)
    {
        ASSERT_EQ(prog.memory[i], solution[i]);
    }
}

TEST_F(challenge_test, execute_test_04)
{
    size_t nums = 9;
    int memory[nums];
    memory[0] = 1;
    memory[1] = 1;
    memory[2] = 1;
    memory[3] = 4;
    memory[4] = 99;
    memory[5] = 5;
    memory[6] = 6;
    memory[7] = 0;
    memory[8] = 99;

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
    prog.memory      = memory;
    prog.head        = 0;
    prog.memory_size = nums;

    int ret = execute(&prog);

    ASSERT_EQ(ret, 1);
    for (int i = 0; i < nums; ++i)
    {
        ASSERT_EQ(prog.memory[i], solution[i]);
    }
}
