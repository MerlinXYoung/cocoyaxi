#pragma once
#include <catch.hpp>

#define EXPECT(x) REQUIRE(x)

#define EXPECT_OP(x, y, op) REQUIRE(x op y)

#define EXPECT_EQ(x, y) EXPECT_OP(x, y, ==)
#define EXPECT_NE(x, y) EXPECT_OP(x, y, !=)
#define EXPECT_GE(x, y) EXPECT_OP(x, y, >=)
#define EXPECT_LE(x, y) EXPECT_OP(x, y, <=)
#define EXPECT_GT(x, y) EXPECT_OP(x, y, >)
#define EXPECT_LT(x, y) EXPECT_OP(x, y, <)