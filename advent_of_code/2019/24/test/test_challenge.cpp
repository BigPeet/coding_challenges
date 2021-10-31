/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "gtest/gtest.h"

extern "C" {
#include "challenge/challenge_lib.h"
}

class challenge_test : public ::testing::Test
{
  protected:
    void SetUp() override {}

    void TearDown() override {}

    std::string test_input_01 = "....##..#.#..##..#..#....";
    std::string test_input_02 = "...............#.....#...";
};

TEST_F(challenge_test, parse_scan_01)
{
    Scan expected{};
    expected.data[0][4] = 1;
    expected.data[1][0] = 1;
    expected.data[1][3] = 1;
    expected.data[2][0] = 1;
    expected.data[2][3] = 1;
    expected.data[2][4] = 1;
    expected.data[3][2] = 1;
    expected.data[4][0] = 1;

    Scan s;
    parse_scan(test_input_01.data(), &s);
    ASSERT_EQ(cmp_scans(&expected, &s), 1);
}


TEST_F(challenge_test, apply_step_01)
{
    Scan s;
    Scan next_s;
    parse_scan(test_input_01.data(), &s);
    {
        Scan expected_01{};
        expected_01.data[0][0] = 1;
        expected_01.data[0][3] = 1;
        expected_01.data[1][0] = 1;
        expected_01.data[1][1] = 1;
        expected_01.data[1][2] = 1;
        expected_01.data[1][3] = 1;
        expected_01.data[2][0] = 1;
        expected_01.data[2][1] = 1;
        expected_01.data[2][2] = 1;
        expected_01.data[2][4] = 1;
        expected_01.data[3][0] = 1;
        expected_01.data[3][1] = 1;
        expected_01.data[3][3] = 1;
        expected_01.data[3][4] = 1;
        expected_01.data[4][1] = 1;
        expected_01.data[4][2] = 1;
        apply_step_01(&s, &next_s);
        ASSERT_EQ(cmp_scans(&expected_01, &next_s), 1);
        s = next_s;
    }
    {
        Scan expected_02{};
        expected_02.data[0][0] = 1;
        expected_02.data[0][1] = 1;
        expected_02.data[0][2] = 1;
        expected_02.data[0][3] = 1;
        expected_02.data[0][4] = 1;
        expected_02.data[1][4] = 1;
        expected_02.data[2][4] = 1;
        expected_02.data[3][3] = 1;
        expected_02.data[4][0] = 1;
        expected_02.data[4][2] = 1;
        expected_02.data[4][3] = 1;
        expected_02.data[4][4] = 1;
        apply_step_01(&s, &next_s);
        ASSERT_EQ(cmp_scans(&expected_02, &next_s), 1);
        s = next_s;
    }
    {
        Scan expected_03{};
        expected_03.data[0][0] = 1;
        expected_03.data[1][0] = 1;
        expected_03.data[1][1] = 1;
        expected_03.data[1][2] = 1;
        expected_03.data[1][3] = 1;
        expected_03.data[2][3] = 1;
        expected_03.data[2][4] = 1;
        expected_03.data[3][0] = 1;
        expected_03.data[3][2] = 1;
        expected_03.data[3][3] = 1;
        expected_03.data[4][1] = 1;
        expected_03.data[4][2] = 1;
        expected_03.data[4][4] = 1;
        apply_step_01(&s, &next_s);
        ASSERT_EQ(cmp_scans(&expected_03, &next_s), 1);
        s = next_s;
    }
    {
        Scan expected_04{};
        expected_04.data[0][0] = 1;
        expected_04.data[0][1] = 1;
        expected_04.data[0][2] = 1;
        expected_04.data[0][3] = 1;
        expected_04.data[1][4] = 1;
        expected_04.data[2][0] = 1;
        expected_04.data[2][1] = 1;
        expected_04.data[2][4] = 1;
        expected_04.data[4][0] = 1;
        expected_04.data[4][1] = 1;
        apply_step_01(&s, &next_s);
        ASSERT_EQ(cmp_scans(&expected_04, &next_s), 1);
    }
}


TEST_F(challenge_test, diversity_test_01)
{
    Scan s;
    parse_scan(test_input_02.data(), &s);
    ASSERT_EQ(scan_biodiversity(&s), 2129920);
}
