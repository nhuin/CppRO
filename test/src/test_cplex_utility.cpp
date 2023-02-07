#include "catch2/catch.hpp"

#include <CppRO/cplex_utility.hpp>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

TEST_CASE("Test epsilon_equal", "[]") {
    REQUIRE(epsilon_equal<double>()(0.0, 0.0));
    REQUIRE(epsilon_equal<double>()(1.0, 1.0));
    REQUIRE(epsilon_equal<double>()(1.0, 1.0 + 1e-9));
    REQUIRE(!epsilon_equal<double>()(1 + 1e-5, 1.0));
}

TEST_CASE("Test epsilon_less", "[]") {
    REQUIRE(epsilon_less<double>()(0.0, 1.0));
    REQUIRE(!epsilon_less<double>()(1.0, 0.0));

    REQUIRE(!epsilon_less<double>()(1.0, 1.0));
    REQUIRE(!epsilon_less<double>()(1.00000008, 1.0));

    REQUIRE(epsilon_less<double>()(1.0, 1.0 + 1e-5));
    REQUIRE(!epsilon_less<double>()(1.0 + 1e-5, 1.0));

    REQUIRE(!epsilon_less<double>()(1.0, 1.0 - 1e-5));
    REQUIRE(epsilon_less<double>()(1.0 - 1e-5, 1.0));

    REQUIRE(!epsilon_less<double>()(1000, 1.0));
    REQUIRE(epsilon_less<double>()(1.0, 10000));

    REQUIRE(epsilon_less<double>()(-1e-5, 0.0));
}

TEST_CASE("Test epsilon_less_equal", "[]") {
    REQUIRE(epsilon_less_equal<double>()(0.0, 1.0));
    REQUIRE(!epsilon_less_equal<double>()(1.0, 0.0));

    REQUIRE(epsilon_less_equal<double>()(1.0, 1.0));
    REQUIRE(epsilon_less_equal<double>()(1.00000008, 1.0));

    REQUIRE(epsilon_less_equal<double>()(1.0, 1.0 + 1e-5));
    REQUIRE(!epsilon_less_equal<double>()(1.0 + 1e-5, 1.0));

    REQUIRE(!epsilon_less_equal<double>()(1.0, 1.0 - 1e-5));
    REQUIRE(epsilon_less_equal<double>()(1.0 - 1e-5, 1.0));

    REQUIRE(!epsilon_less_equal<double>()(1000, 1.0));
    REQUIRE(epsilon_less_equal<double>()(1.0, 10000));

    REQUIRE(epsilon_less_equal<double>()(-1e-5, 0.0));
    REQUIRE(epsilon_less_equal<double>()(-1e-9, 0.0));
}

TEST_CASE("Test epsilon_greater", "[]") {
    REQUIRE(!epsilon_greater<double>()(0.0, 1.0));
    REQUIRE(epsilon_greater<double>()(1.0, 0.0));

    REQUIRE(!epsilon_greater<double>()(1.0, 1.0));
    REQUIRE(!epsilon_greater<double>()(1.00000008, 1.0));

    REQUIRE(!epsilon_greater<double>()(1.0, 1.0 + 1e-5));
    REQUIRE(epsilon_greater<double>()(1.0 + 1e-5, 1.0));

    REQUIRE(epsilon_greater<double>()(1.0, 1.0 - 1e-5));
    REQUIRE(!epsilon_greater<double>()(1.0 - 1e-5, 1.0));

    REQUIRE(epsilon_greater<double>()(1000, 1.0));
    REQUIRE(!epsilon_greater<double>()(1.0, 10000));

    REQUIRE(!epsilon_greater<double>()(-1e-5, 0.0));
}

TEST_CASE("Test epsilon_greater_equal", "[]") {
    REQUIRE(!epsilon_greater_equal<double>()(0.0, 1.0));
    REQUIRE(epsilon_greater_equal<double>()(1.0, 0.0));

    REQUIRE(epsilon_greater_equal<double>()(1.0, 1.0));
    REQUIRE(epsilon_greater_equal<double>()(1.00000008, 1.0));

    REQUIRE(!epsilon_greater_equal<double>()(1.0, 1.0 + 1e-5));
    REQUIRE(epsilon_greater_equal<double>()(1.0 + 1e-5, 1.0));

    REQUIRE(epsilon_greater_equal<double>()(1.0, 1.0 - 1e-5));
    REQUIRE(!epsilon_greater_equal<double>()(1.0 - 1e-5, 1.0));

    REQUIRE(epsilon_greater_equal<double>()(1000, 1.0));
    REQUIRE(!epsilon_greater_equal<double>()(1.0, 10000));

    REQUIRE(!epsilon_greater_equal<double>()(-1e-5, 0.0));
    REQUIRE(epsilon_greater_equal<double>()(1e-5, 0.0));
    REQUIRE(epsilon_greater_equal<double>()(-1e-9, 0.0));
}

TEST_CASE("isInteger", "[]") {
    REQUIRE(isInteger(23.0000));
    REQUIRE(!isInteger(23.0001));
}

TEST_CASE("isMostFractional", "[]") {
    REQUIRE(!isMostFractional(23.0000, 23.0000));
    REQUIRE(!isMostFractional(23.0000, 23.0001));
    REQUIRE(isMostFractional(23.0001, 23.0000));
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
