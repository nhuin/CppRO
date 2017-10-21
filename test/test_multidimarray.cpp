#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "MultiDimArray.hpp"

TEST_CASE( "Factorials are computed", "[factorial]" ) {
	MultiDimArray<int, 3> m({{1, 2, 3}}, 5);
	REQUIRE(m.getTotalSize() == 1 * 2 * 3);
	REQUIRE(m.getDim<0>() == 1);
	REQUIRE(m.getDim<1>() == 2);
	REQUIRE(m.getDim<2>() == 3);
	m(0, 1, 2) = 1;
	for(std::size_t i = 0; i < 1; ++i) {
		for(std::size_t j = 0; j < 2; ++j) {
			for(std::size_t k = 0; k < 3; ++k) {
				if(std::make_tuple(i, j, k) != std::make_tuple(0, 1, 2)) {
					REQUIRE(m(i, j, k) == 5);
				} else {
					REQUIRE(m(0, 1, 2) == 1);
				}
			}
		}
	}
	
}
