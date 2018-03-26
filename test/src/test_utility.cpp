#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch/catch.hpp>
#include "utility.hpp"
#include <utility>

TEST_CASE("tuple for each", "[]") {
	std::tuple<int, double, std::string> t1 {1, 4.5, "foo"};
	for_each(t1, [](auto&& x){
		std::cout << x << ", ";
	});
	std::cout << '\n';	
}