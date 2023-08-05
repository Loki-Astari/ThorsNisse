#include "Route.h"
#include <gtest/gtest.h>

using ThorsAnvil::Nisse::Protocol::HTTP::Route;
using ThorsAnvil::Nisse::Protocol::HTTP::RouteTester;

TEST(RouteTestBug, Bug1)
{
    Route           r1("/pathSpecific");
    std::string     s2("/pathSpecific-Missing");

    RouteTester     test;
    ASSERT_FALSE(test(r1,s2));
    ASSERT_TRUE(test(s2, r1));
}

TEST(RouteTest, ConstructBuild)
{
    std::string route("/This/is/a/normal/path");
    Route       r1("/This/is/a/normal/path");
    std::stringstream s;
    s << r1;
    ASSERT_EQ(route, s.str());
}
TEST(RouteTest, ConstructMoveString)
{
    std::string route("/This/is/a/normal/path");
    std::string input(route);
    Route       r1(std::move(input));
    std::stringstream s;
    s << r1;
    ASSERT_EQ(route, s.str());
}
TEST(RouteTest, ConstructMove)
{
    std::string route("/This/is/a/normal/path");
    Route       r1("/This/is/a/normal/path");
    Route       r2(std::move(r1));
    std::stringstream s;
    s << r2;
    ASSERT_EQ(route, s.str());
}
TEST(RouteTest, SameRouteNotLess)
{
    Route       r1("/This/is/a/normal/path");
    Route       r2("/This/is/a/normal/path");
    ASSERT_FALSE(r1 < r2);
    ASSERT_FALSE(r2 < r1);
}
TEST(RouteTest, GreaterRouteIsNotLess)
{
    Route       r1("/This/is/a/normal/bath");
    Route       r2("/This/is/a/normal/path");
    ASSERT_TRUE(r1 < r2);
    ASSERT_FALSE(r2 < r1);
}
TEST(RouteTest, LessRouteIsLess)
{
    Route       r1("/This/is/a/normal/sath");
    Route       r2("/This/is/a/normal/path");
    ASSERT_FALSE(r1 < r2);
    ASSERT_TRUE(r2 < r1);
}
// ----------
TEST(RouteTest, SameRouteStringNotLess)
{
    Route       r1("/This/is/a/normal/path");
    std::string s2("/This/is/a/normal/path");
    ASSERT_FALSE(r1 < s2);
}
TEST(RouteTest, GreaterRouteStringIsLess)
{
    Route       r1("/This/is/a/normal/bath");
    std::string s2("/This/is/a/normal/path");
    ASSERT_TRUE(r1 < s2);
}
TEST(RouteTest, LessRouteStringIsNotLess)
{
    Route       r1("/This/is/a/normal/sath");
    std::string s2("/This/is/a/normal/path");
    ASSERT_FALSE(r1 < s2);
}
// -----------
TEST(RouteTest, SameRouteStringNotGreater)
{
    Route       r1("/This/is/a/normal/path");
    std::string s2("/This/is/a/normal/path");
    //ASSERT_FALSE(r1 > s2);
    //ASSERT_FALSE(s2 > r1);
}
TEST(RouteTest, GreaterRouteStringIsGreater)
{
    Route       r1("/This/is/a/normal/bath");
    std::string s2("/This/is/a/normal/path");
    //ASSERT_FALSE(r1 > s2);
    //ASSERT_TRUE(s2 > r1);
}
TEST(RouteTest, LessRouteStringIsNotGreater)
{
    Route       r1("/This/is/a/normal/sath");
    std::string s2("/This/is/a/normal/path");
    //ASSERT_TRUE(r1 > s2);
    //ASSERT_FALSE(s2 > r1);
}
// --------------
TEST(RouteTest, SameRouteStringLessOrEqual)
{
    Route       r1("/This/is/a/normal/path");
    std::string s2("/This/is/a/normal/path");
    ASSERT_TRUE(r1 <= s2);
    //ASSERT_TRUE(s2 <= r1);
}
TEST(RouteTest, GreaterRouteStringIsLessOrEqual)
{
    Route       r1("/This/is/a/normal/bath");
    std::string s2("/This/is/a/normal/path");
    ASSERT_TRUE(r1 <= s2);
    //ASSERT_FALSE(s2 <= r1);
}
TEST(RouteTest, LessRouteStringIsNotLessOrEqual)
{
    Route       r1("/This/is/a/normal/sath");
    std::string s2("/This/is/a/normal/path");
    ASSERT_FALSE(r1 <= s2);
    //ASSERT_TRUE(s2 <= r1);
}
// --------------
TEST(RouteTest, SameRouteStringEqual)
{
    Route       r1("/This/is/a/normal/path");
    std::string s2("/This/is/a/normal/path");
    ASSERT_TRUE(r1 == s2);
    //ASSERT_TRUE(s2 == r1);
}
TEST(RouteTest, GreaterRouteStringIsNotEqual)
{
    Route       r1("/This/is/a/normal/bath");
    std::string s2("/This/is/a/normal/path");
    ASSERT_FALSE(r1 == s2);
    //ASSERT_FALSE(s2 == r1);
}
TEST(RouteTest, LessRouteStringIsNotEqual)
{
    Route       r1("/This/is/a/normal/sath");
    std::string s2("/This/is/a/normal/path");
    ASSERT_FALSE(r1 == s2);
    //ASSERT_FALSE(s2 == r1);
}
// --------------
TEST(RouteTest, RouteTesterSameRouteStringFalse)
{
    Route       r1("/This/is/a/normal/path");
    std::string s1("/This/is/a/normal/path");
    Route       r2("/This/is/a/normal/path");
    std::string s2("/This/is/a/normal/path");

    RouteTester tester;
    ASSERT_FALSE(tester(r1,r2));
    ASSERT_FALSE(tester(r1,s2));
    ASSERT_FALSE(tester(s1,r2));
}
TEST(RouteTest, RouteTesterLessRouteStringIsLess)
{
    Route       r1("/This/is/a/normal/bath");
    std::string s1("/This/is/a/normal/bath");
    Route       r2("/This/is/a/normal/path");
    std::string s2("/This/is/a/normal/path");

    RouteTester tester;
    ASSERT_TRUE(tester(r1,r2));
    ASSERT_TRUE(tester(r1,s2));
    ASSERT_TRUE(tester(s1,r2));
}
TEST(RouteTest, RouteTesterGreaterRouteStringIsNotLess)
{
    Route       r1("/This/is/a/normal/sath");
    std::string s1("/This/is/a/normal/sath");
    Route       r2("/This/is/a/normal/path");
    std::string s2("/This/is/a/normal/path");

    RouteTester tester;
    ASSERT_FALSE(tester(r1,r2));
    ASSERT_FALSE(tester(r1,s2));
    ASSERT_FALSE(tester(s1,r2));
}

/* ------------------------------------------ */
TEST(RouteTest, SameRouteNotLess_SubEqual)
{
    Route       r1("/This/:is/a/normal/path");
    Route       r2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 < r2);
    ASSERT_FALSE(r2 < r1);
}
TEST(RouteTest, GreaterRouteIsNotLess_SubEqual)
{
    Route       r1("/This/:is/a/normal/bath");
    Route       r2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 < r2);
    ASSERT_FALSE(r2 < r1);
}
TEST(RouteTest, LessRouteIsLess_SubEqual)
{
    Route       r1("/This/:is/a/normal/sath");
    Route       r2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 < r2);
    ASSERT_TRUE(r2 < r1);
}
// ----------
TEST(RouteTest, SameRouteStringNotLess_SubEqual)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 < s2);
}
TEST(RouteTest, GreaterRouteStringIsLess_SubEqual)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 < s2);
}
TEST(RouteTest, LessRouteStringIsNotLess_SubEqual)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 < s2);
}
// -----------
TEST(RouteTest, SameRouteStringNotGreater_SubEqual)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");
    //ASSERT_FALSE(r1 > s2);
    //ASSERT_FALSE(s2 > r1);
}
TEST(RouteTest, GreaterRouteStringIsGreater_SubEqual)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s2("/This/:is/a/normal/path");
    //ASSERT_FALSE(r1 > s2);
    //ASSERT_TRUE(s2 > r1);
}
TEST(RouteTest, LessRouteStringIsNotGreater_SubEqual)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s2("/This/:is/a/normal/path");
    //ASSERT_TRUE(r1 > s2);
    //ASSERT_FALSE(s2 > r1);
}
// --------------
TEST(RouteTest, SameRouteStringLessOrEqual_SubEqual)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 <= s2);
    //ASSERT_TRUE(s2 <= r1);
}
TEST(RouteTest, GreaterRouteStringIsLessOrEqual_SubEqual)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 <= s2);
    //ASSERT_FALSE(s2 <= r1);
}
TEST(RouteTest, LessRouteStringIsNotLessOrEqual_SubEqual)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 <= s2);
    //ASSERT_TRUE(s2 <= r1);
}
// --------------
TEST(RouteTest, SameRouteStringEqual_SubEqual)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 == s2);
    //ASSERT_TRUE(s2 == r1);
}
TEST(RouteTest, GreaterRouteStringIsNotEqual_SubEqual)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 == s2);
    //ASSERT_FALSE(s2 == r1);
}
TEST(RouteTest, LessRouteStringIsNotEqual_SubEqual)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 == s2);
    //ASSERT_FALSE(s2 == r1);
}
// --------------
TEST(RouteTest, RouteTesterSameRouteStringFalse_SubEqual)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s1("/This/:is/a/normal/path");
    Route       r2("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");

    RouteTester tester;
    ASSERT_FALSE(tester(r1,r2));
    ASSERT_FALSE(tester(r1,s2));
    ASSERT_FALSE(tester(s1,r2));
}
TEST(RouteTest, RouteTesterLessRouteStringIsLess_SubEqual)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s1("/This/:is/a/normal/bath");
    Route       r2("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");

    RouteTester tester;
    ASSERT_TRUE(tester(r1,r2));
    ASSERT_TRUE(tester(r1,s2));
    ASSERT_TRUE(tester(s1,r2));
}
TEST(RouteTest, RouteTesterGreaterRouteStringIsNotLess_SubEqual)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s1("/This/:is/a/normal/sath");
    Route       r2("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");

    RouteTester tester;
    ASSERT_FALSE(tester(r1,r2));
    ASSERT_FALSE(tester(r1,s2));
    ASSERT_FALSE(tester(s1,r2));
}

/* ------------------------------------------ */
TEST(RouteTest, SameRouteNotLess_SubLess)
{
    Route       r1("/This/:aa/a/normal/path");
    Route       r2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 < r2);
    ASSERT_FALSE(r2 < r1);
}
TEST(RouteTest, GreaterRouteIsNotLess_SubLess)
{
    Route       r1("/This/:aa/a/normal/bath");
    Route       r2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 < r2);
    ASSERT_FALSE(r2 < r1);
}
TEST(RouteTest, LessRouteIsLess_SubLess)
{
    Route       r1("/This/:aa/a/normal/sath");
    Route       r2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 < r2);
    ASSERT_TRUE(r2 < r1);
}
// ----------
TEST(RouteTest, SameRouteStringNotLess_SubLess)
{
    Route       r1("/This/:aa/a/normal/path");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 < s2);
}
TEST(RouteTest, GreaterRouteStringIsLess_SubLess)
{
    Route       r1("/This/:aa/a/normal/bath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 < s2);
}
TEST(RouteTest, LessRouteStringIsNotLess_SubLess)
{
    Route       r1("/This/:aa/a/normal/sath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 < s2);
}
// -----------
TEST(RouteTest, SameRouteStringNotGreater_SubLess)
{
    Route       r1("/This/:aa/a/normal/path");
    std::string s2("/This/:is/a/normal/path");
    //ASSERT_FALSE(r1 > s2);
    //ASSERT_FALSE(s2 > r1);
}
TEST(RouteTest, GreaterRouteStringIsGreater_SubLess)
{
    Route       r1("/This/:aa/a/normal/bath");
    std::string s2("/This/:is/a/normal/path");
    //ASSERT_FALSE(r1 > s2);
    //ASSERT_TRUE(s2 > r1);
}
TEST(RouteTest, LessRouteStringIsNotGreater_SubLess)
{
    Route       r1("/This/:aa/a/normal/sath");
    std::string s2("/This/:is/a/normal/path");
    //ASSERT_TRUE(r1 > s2);
    //ASSERT_FALSE(s2 > r1);
}
// --------------
TEST(RouteTest, SameRouteStringLessOrEqual_SubLess)
{
    Route       r1("/This/:aa/a/normal/path");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 <= s2);
    //ASSERT_TRUE(s2 <= r1);
}
TEST(RouteTest, GreaterRouteStringIsLessOrEqual_SubLess)
{
    Route       r1("/This/:aa/a/normal/bath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 <= s2);
    //ASSERT_FALSE(s2 <= r1);
}
TEST(RouteTest, LessRouteStringIsNotLessOrEqual_SubLess)
{
    Route       r1("/This/:aa/a/normal/sath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 <= s2);
    //ASSERT_TRUE(s2 <= r1);
}
// --------------
TEST(RouteTest, SameRouteStringEqual_SubLess)
{
    Route       r1("/This/:aa/a/normal/path");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_TRUE(r1 == s2);
    //ASSERT_TRUE(s2 == r1);
}
TEST(RouteTest, GreaterRouteStringIsNotEqual_SubLess)
{
    Route       r1("/This/:aa/a/normal/bath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 == s2);
    //ASSERT_FALSE(s2 == r1);
}
TEST(RouteTest, LessRouteStringIsNotEqual_SubLess)
{
    Route       r1("/This/:aa/a/normal/sath");
    std::string s2("/This/:is/a/normal/path");
    ASSERT_FALSE(r1 == s2);
    //ASSERT_FALSE(s2 == r1);
}
// --------------
TEST(RouteTest, RouteTesterSameRouteStringFalse_SubLess)
{
    Route       r1("/This/:aa/a/normal/path");
    std::string s1("/This/:aa/a/normal/path");
    Route       r2("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");

    RouteTester tester;
    ASSERT_FALSE(tester(r1,r2));
    ASSERT_FALSE(tester(r1,s2));
    ASSERT_FALSE(tester(s1,r2));
}
TEST(RouteTest, RouteTesterLessRouteStringIsLess_SubLess)
{
    Route       r1("/This/:aa/a/normal/bath");
    std::string s1("/This/:aa/a/normal/bath");
    Route       r2("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");

    RouteTester tester;
    ASSERT_TRUE(tester(r1,r2));
    ASSERT_TRUE(tester(r1,s2));
    ASSERT_TRUE(tester(s1,r2));
}
TEST(RouteTest, RouteTesterGreaterRouteStringIsNotLess_SubLess)
{
    Route       r1("/This/:aa/a/normal/sath");
    std::string s1("/This/:aa/a/normal/sath");
    Route       r2("/This/:is/a/normal/path");
    std::string s2("/This/:is/a/normal/path");

    RouteTester tester;
    ASSERT_FALSE(tester(r1,r2));
    ASSERT_FALSE(tester(r1,s2));
    ASSERT_FALSE(tester(s1,r2));
}
/* ------------------------------------------ */
TEST(RouteTest, SameRouteNotLess_SubGreater)
{
    Route       r1("/This/:is/a/normal/path");
    Route       r2("/This/:aa/a/normal/path");
    ASSERT_FALSE(r1 < r2);
    ASSERT_FALSE(r2 < r1);
}
TEST(RouteTest, GreaterRouteIsNotLess_SubGreater)
{
    Route       r1("/This/:is/a/normal/bath");
    Route       r2("/This/:aa/a/normal/path");
    ASSERT_TRUE(r1 < r2);
    ASSERT_FALSE(r2 < r1);
}
TEST(RouteTest, LessRouteIsLess_SubGreater)
{
    Route       r1("/This/:is/a/normal/sath");
    Route       r2("/This/:aa/a/normal/path");
    ASSERT_FALSE(r1 < r2);
    ASSERT_TRUE(r2 < r1);
}
// ----------
TEST(RouteTest, SameRouteStringNotLess_SubGreater)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s2("/This/:aa/a/normal/path");
    ASSERT_FALSE(r1 < s2);
}
TEST(RouteTest, GreaterRouteStringIsLess_SubGreater)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s2("/This/:aa/a/normal/path");
    ASSERT_TRUE(r1 < s2);
}
TEST(RouteTest, LessRouteStringIsNotLess_SubGreater)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s2("/This/:aa/a/normal/path");
    ASSERT_FALSE(r1 < s2);
}
// -----------
TEST(RouteTest, SameRouteStringNotGreater_SubGreater)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s2("/This/:aa/a/normal/path");
    //ASSERT_FALSE(r1 > s2);
    //ASSERT_FALSE(s2 > r1);
}
TEST(RouteTest, GreaterRouteStringIsGreater_SubGreater)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s2("/This/:aa/a/normal/path");
    //ASSERT_FALSE(r1 > s2);
    //ASSERT_TRUE(s2 > r1);
}
TEST(RouteTest, LessRouteStringIsNotGreater_SubGreater)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s2("/This/:aa/a/normal/path");
    //ASSERT_TRUE(r1 > s2);
    //ASSERT_FALSE(s2 > r1);
}
// --------------
TEST(RouteTest, SameRouteStringLessOrEqual_SubGreater)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s2("/This/:aa/a/normal/path");
    ASSERT_TRUE(r1 <= s2);
    //ASSERT_TRUE(s2 <= r1);
}
TEST(RouteTest, GreaterRouteStringIsLessOrEqual_SubGreater)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s2("/This/:aa/a/normal/path");
    ASSERT_TRUE(r1 <= s2);
    //ASSERT_FALSE(s2 <= r1);
}
TEST(RouteTest, LessRouteStringIsNotLessOrEqual_SubGreater)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s2("/This/:aa/a/normal/path");
    ASSERT_FALSE(r1 <= s2);
    //ASSERT_TRUE(s2 <= r1);
}
// --------------
TEST(RouteTest, SameRouteStringEqual_SubGreater)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s2("/This/:aa/a/normal/path");
    ASSERT_TRUE(r1 == s2);
    //ASSERT_TRUE(s2 == r1);
}
TEST(RouteTest, GreaterRouteStringIsNotEqual_SubGreater)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s2("/This/:aa/a/normal/path");
    ASSERT_FALSE(r1 == s2);
    //ASSERT_FALSE(s2 == r1);
}
TEST(RouteTest, LessRouteStringIsNotEqual_SubGreater)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s2("/This/:aa/a/normal/path");
    ASSERT_FALSE(r1 == s2);
    //ASSERT_FALSE(s2 == r1);
}
// --------------
TEST(RouteTest, RouteTesterSameRouteStringFalse_SubGreater)
{
    Route       r1("/This/:is/a/normal/path");
    std::string s1("/This/:is/a/normal/path");
    Route       r2("/This/:aa/a/normal/path");
    std::string s2("/This/:aa/a/normal/path");

    RouteTester tester;
    ASSERT_FALSE(tester(r1,r2));
    ASSERT_FALSE(tester(r1,s2));
    ASSERT_FALSE(tester(s1,r2));
}
TEST(RouteTest, RouteTesterLessRouteStringIsLess_SubGreater)
{
    Route       r1("/This/:is/a/normal/bath");
    std::string s1("/This/:is/a/normal/bath");
    Route       r2("/This/:aa/a/normal/path");
    std::string s2("/This/:aa/a/normal/path");

    RouteTester tester;
    ASSERT_TRUE(tester(r1,r2));
    ASSERT_TRUE(tester(r1,s2));
    ASSERT_TRUE(tester(s1,r2));
}
TEST(RouteTest, RouteTesterGreaterRouteStringIsNotLess_SubGreater)
{
    Route       r1("/This/:is/a/normal/sath");
    std::string s1("/This/:is/a/normal/sath");
    Route       r2("/This/:aa/a/normal/path");
    std::string s2("/This/:is/a/normal/path");

    RouteTester tester;
    ASSERT_FALSE(tester(r1,r2));
    ASSERT_FALSE(tester(r1,s2));
    ASSERT_FALSE(tester(s1,r2));
}
TEST(RouteTest, InvalidNameInRoute)
{
    ASSERT_THROW(
        Route       r1("/This/ThisIsNotAllowed:is/a/normal/sath"),
        std::runtime_error
    );
}
