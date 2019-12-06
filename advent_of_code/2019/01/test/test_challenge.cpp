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
    void SetUp() override {}

    void TearDown() override {}
};

TEST_F(challenge_test, fuel_by_mass_01)
{
    size_t mass     = 12;
    size_t solution = 2;
    ASSERT_EQ(fuel_by_mass(mass), solution);
}

TEST_F(challenge_test, fuel_by_mass_02)
{
    size_t mass     = 14;
    size_t solution = 2;
    ASSERT_EQ(fuel_by_mass(mass), solution);
}

TEST_F(challenge_test, fuel_by_mass_03)
{
    size_t mass     = 1969;
    size_t solution = 654;
    ASSERT_EQ(fuel_by_mass(mass), solution);
}

TEST_F(challenge_test, fuel_by_mass_04)
{
    size_t mass     = 100756;
    size_t solution = 33583;
    ASSERT_EQ(fuel_by_mass(mass), solution);
}

TEST_F(challenge_test, fuel_by_mass_05)
{
    size_t mass     = 2;
    size_t solution = 0;
    ASSERT_EQ(fuel_by_mass(mass), solution);
}

TEST_F(challenge_test, total_fuel_01)
{
    size_t masses[4];
    masses[0]       = 12;
    masses[1]       = 14;
    masses[2]       = 1969;
    masses[3]       = 100756;
    size_t solution = 2 + 2 + 654 + 33583;
    ASSERT_EQ(total_fuel(masses, 4), solution);
}

TEST_F(challenge_test, fuel_by_module_01)
{
    size_t module_mass = 4;
    size_t solution    = 0;
    ASSERT_EQ(fuel_for_module(module_mass), solution);
}

TEST_F(challenge_test, fuel_by_module_02)
{
    size_t module_mass = 12;
    size_t solution    = 2;
    ASSERT_EQ(fuel_for_module(module_mass), solution);
}

TEST_F(challenge_test, fuel_by_module_03)
{
    size_t module_mass = 14;
    size_t solution    = 2;
    ASSERT_EQ(fuel_for_module(module_mass), solution);
}

TEST_F(challenge_test, fuel_by_module_04)
{
    size_t module_mass = 1969;
    size_t solution    = 966;
    ASSERT_EQ(fuel_for_module(module_mass), solution);
}

TEST_F(challenge_test, fuel_by_module_05)
{
    size_t module_mass = 100756;
    size_t solution    = 50346;
    ASSERT_EQ(fuel_for_module(module_mass), solution);
}

TEST_F(challenge_test, total_fuel_complete_01)
{
    size_t masses[4];
    masses[0]       = 12;
    masses[1]       = 14;
    masses[2]       = 1969;
    masses[3]       = 100756;
    size_t solution = 2 + 2 + 966 + 50346;
    ASSERT_EQ(total_fuel_complete(masses, 4), solution);
}
