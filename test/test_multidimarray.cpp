#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <algorithm>
#include <boost/multi_array.hpp>

#include "MultiDimArray.hpp"

TEST_CASE("Test operator()", "[]") {
    const int x = 25, y = 56, z = 8;
    MultiDimArray<int, 3> m1({x, y, z}, 5);
    REQUIRE(m1.getTotalSize() == x * y * z);
    REQUIRE(m1.getDim<0>() == x);
    REQUIRE(m1.getDim<1>() == y);
    REQUIRE(m1.getDim<2>() == z);
    m1(0, 1, 2) = 1;
    for (int i = 0; i < m1.getDim<0>(); ++i) {
        for (int j = 0; j < m1.getDim<1>(); ++j) {
            for (int k = 0; k < m1.getDim<2>(); ++k) {

                if (std::make_tuple(i, j, k) != std::make_tuple(0, 1, 2)) {
                    REQUIRE(m1(i, j, k) == 5);
                    REQUIRE(m1({i, j, k}) == 5);
                } else {
                    REQUIRE(m1(0, 1, 2) == 1);
                    REQUIRE(m1({0, 1, 2}) == 1);
                }
            }
        }
    }
}

TEST_CASE("Test boost", "[]") {
    const int x = 25, y = 56, z = 8;
    boost::multi_array<int, 3> m1(boost::extents[x][y][z]);
    std::fill_n(m1.data(), m1.num_elements(), 5);

    REQUIRE(m1.num_elements() == x * y * z);
    REQUIRE(m1.shape()[0] == x);
    REQUIRE(m1.shape()[1] == y);
    REQUIRE(m1.shape()[2] == z);
    m1[0][1][2] = 1;
    for (int i = 0; i < m1.shape()[0]; ++i) {
        for (int j = 0; j < m1.shape()[1]; ++j) {
            for (int k = 0; k < m1.shape()[2]; ++k) {
                if (std::make_tuple(i, j, k) != std::make_tuple(0, 1, 2)) {
                    REQUIRE(m1[i][j][k] == 5);
                } else {
                    REQUIRE(m1[0][1][2] == 1);
                    ;
                }
            }
        }
    }
}

TEST_CASE("Test for each loop", "[]") {
    int x = 25, y = 56, z = 8;
    MultiDimArray<int, 3> m1({{x, y, z}}, 5);

    for (auto& v : MultiDimArray<int, 3>::DimensionView<2>(m1, {25, 56, 0})) {
        v = 1;
    }

    for (int i = 0; i < m1.getDim<2>(); ++i) {
        REQUIRE(m1({25, 56, i}) == 1);
    }

    for (const auto& v : MultiDimArray<int, 3>::DimensionView<2>(m1, {25, 56, 0})) {
        REQUIRE(v == 1);
    }
}
