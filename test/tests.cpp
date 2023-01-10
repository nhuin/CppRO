#include <catch2/catch.hpp>

unsigned int factorial(unsigned int _number) // NOLINT(misc-no-recursion)
{
    return _number <= 1 ? _number : factorial(_number - 1) * _number;
}

TEST_CASE("Factorials are computed", "[factorial]") {
    REQUIRE(factorial(1) == 1);
    REQUIRE(factorial(2) == 2);
    REQUIRE(factorial(3) == 6);
    REQUIRE(factorial(10) == 3628800);
}
