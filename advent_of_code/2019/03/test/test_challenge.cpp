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

TEST_F(challenge_test, distance_test_01)
{
    point_t a = {.x = 100, .y = 100};
    point_t b = {.x = 0, .y = 0};
    int solution = 200;
    ASSERT_EQ(manhattan_distance(&a, &b), solution);
    ASSERT_EQ(manhattan_distance(&b, &a), solution);
}

TEST_F(challenge_test, distance_test_02)
{
    point_t a = {.x = 0, .y = 0};
    point_t b = {.x = 0, .y = 0};
    int solution = 0;
    ASSERT_EQ(manhattan_distance(&a, &b), solution);
    ASSERT_EQ(manhattan_distance(&b, &a), solution);
}

TEST_F(challenge_test, distance_test_03)
{
    point_t a = {.x = -100, .y = 100};
    point_t b = {.x = 0, .y = 0};
    int solution = 200;
    ASSERT_EQ(manhattan_distance(&a, &b), solution);
    ASSERT_EQ(manhattan_distance(&b, &a), solution);
}

TEST_F(challenge_test, distance_test_04)
{
    point_t a = {.x = -100, .y = 100};
    point_t b = {.x = 100, .y = -100};
    int solution = 400;
    ASSERT_EQ(manhattan_distance(&a, &b), solution);
    ASSERT_EQ(manhattan_distance(&b, &a), solution);
}

TEST_F(challenge_test, distance_test_05)
{
    point_t a = {.x = 3, .y = -8};
    point_t b = {.x = 72, .y = 50};
    int solution = 127;
    ASSERT_EQ(manhattan_distance(&a, &b), solution);
    ASSERT_EQ(manhattan_distance(&b, &a), solution);
}


TEST_F(challenge_test, intersect_lines_test_01)
{
    point_t a = {.x = 3, .y = 1};
    point_t b = {.x = 6, .y = 1};
    point_t c = {.x = 4, .y = 0};
    point_t d = {.x = 4, .y = 5};

    point_t p;

    int solution = 1;
    point_t pos = {.x = 4, .y = 1};


    line_t l1 = {.a = &a, .b = &b};
    line_t l2 = {.a = &c, .b = &d};

    ASSERT_EQ(intersect_lines(&l1, &l2, &p), solution);
    ASSERT_EQ(p.x, pos.x);
    ASSERT_EQ(p.y, pos.y);
}

TEST_F(challenge_test, intersect_lines_test_02)
{
    point_t a = {.x = 3, .y = 1};
    point_t b = {.x = 6, .y = 1};
    point_t c = {.x = 4, .y = 0};
    point_t d = {.x = 5, .y = 0};

    point_t p;

    int solution = 0;


    line_t l1 = {.a = &a, .b = &b};
    line_t l2 = {.a = &c, .b = &d};

    ASSERT_EQ(intersect_lines(&l1, &l2, &p), solution);
}

TEST_F(challenge_test, intersect_lines_test_03)
{
    point_t a = {.x = -3, .y = 1};
    point_t b = {.x = 6, .y = 1};
    point_t c = {.x = 4, .y = -5};
    point_t d = {.x = 4, .y = 5};

    point_t p;

    int solution = 1;
    point_t pos = {.x = 4, .y = 1};


    line_t l1 = {.a = &a, .b = &b};
    line_t l2 = {.a = &c, .b = &d};

    ASSERT_EQ(intersect_lines(&l1, &l2, &p), solution);
    ASSERT_EQ(p.x, pos.x);
    ASSERT_EQ(p.y, pos.y);
}

TEST_F(challenge_test, intersect_lines_test_04)
{
    point_t a = {.x = 3, .y = 1};
    point_t b = {.x = 6, .y = 1};
    point_t c = {.x = 2, .y = 0};
    point_t d = {.x = 2, .y = 5};

    point_t p;

    int solution = 0;


    line_t l1 = {.a = &a, .b = &b};
    line_t l2 = {.a = &c, .b = &d};

    ASSERT_EQ(intersect_lines(&l1, &l2, &p), solution);
}
