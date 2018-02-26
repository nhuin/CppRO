#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "cplex_utility.hpp"

TEST_CASE("Test epsilon_equal", "[]") {
	REQUIRE(epsilon_equal<double>()(1.0, 1.0));
	REQUIRE(!epsilon_equal<double>()(1+1e5, 1.0));
}
TEST_CASE("Test begin and end functions", "[]") {
	IloEnv env;
	IloWrapper<IloNumArray> array(env, 10);
	std::fill(array.begin(), array.end(), 5);
	for(const auto& val : array) {
		REQUIRE(epsilon_equal<double>()(val, 5));
	} 
} 

TEST_CASE("Test operator[]", "[]") {
	IloEnv env;
	IloWrapper<IloNumArray> array(env, 10);
	REQUIRE(array.size() == 10);
	std::fill(array.begin(), array.end(), 5);
	for(int i = 0; i < array.size(); ++i) {
		REQUIRE(epsilon_equal<double>()(array[i], 5));
	}
}

TEST_CASE("Test copy constructor", "[]") {
	IloEnv env;
	IloWrapper<IloNumArray> array1(env, 10);
	std::fill(array1.begin(), array1.end(), 0);
	IloWrapper<IloNumArray> array2 = array1;
	std::fill(array1.begin(), array1.end(), 5);
	for(int i = 0; i < array1.size(); ++i) {
		REQUIRE(epsilon_equal<double>()(array1[i], 5));
		REQUIRE(epsilon_equal<double>()(array2[i], 0));
	}
}

// TEST_CASE("Test conversion operator", "[]") {
// 	IloEnv env;
// 	IloWrapper<IloNumArray> array(env, 10);
// 	std::fill(array.begin(), array.end(), 5);
// 	REQUIRE(epsilon_equal<double>()(IloSum(array), 5*10));
// }
