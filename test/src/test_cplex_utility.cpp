#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch2/catch.hpp"

#include "cplex_utility.hpp"

TEST_CASE("Test epsilon_equal", "[]") {
	REQUIRE(epsilon_equal<double>()(0.0, 0.0));
	REQUIRE(epsilon_equal<double>()(1.0, 1.0));
	REQUIRE(epsilon_equal<double>()(1.0, 1.0+1e-9));
	REQUIRE(!epsilon_equal<double>()(1+1e-5, 1.0));
} 

TEST_CASE("Test epsilon_less", "[]") {
	REQUIRE(epsilon_less<double>()(0.0, 1.0));
	REQUIRE(!epsilon_less<double>()(1.0, 0.0));

	REQUIRE(!epsilon_less<double>()(1.0, 1.0));
	REQUIRE(!epsilon_less<double>()(1.00000008, 1.0));
	
	REQUIRE(epsilon_less<double>()(1.0, 1.0+1e-5));
	REQUIRE(!epsilon_less<double>()(1.0+1e-5, 1.0));

	REQUIRE(!epsilon_less<double>()(1.0, 1.0-1e-5));
	REQUIRE(epsilon_less<double>()(1.0-1e-5, 1.0));

	REQUIRE(!epsilon_less<double>()(1000, 1.0));
	REQUIRE(epsilon_less<double>()(1.0, 10000));

	REQUIRE(epsilon_less<double>()(-1e-5, 0.0));
}

TEST_CASE("Test epsilon_less_equal", "[]") {
	REQUIRE(epsilon_less_equal<double>()(0.0, 1.0));
	REQUIRE(!epsilon_less_equal<double>()(1.0, 0.0));

	REQUIRE(epsilon_less_equal<double>()(1.0, 1.0));
	REQUIRE(epsilon_less_equal<double>()(1.00000008, 1.0));
	
	REQUIRE(epsilon_less_equal<double>()(1.0, 1.0+1e-5));
	REQUIRE(!epsilon_less_equal<double>()(1.0+1e-5, 1.0));

	REQUIRE(!epsilon_less_equal<double>()(1.0, 1.0-1e-5));
	REQUIRE(epsilon_less_equal<double>()(1.0-1e-5, 1.0));

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
	
	REQUIRE(!epsilon_greater<double>()(1.0, 1.0+1e-5));
	REQUIRE(epsilon_greater<double>()(1.0+1e-5, 1.0));

	REQUIRE(epsilon_greater<double>()(1.0, 1.0-1e-5));
	REQUIRE(!epsilon_greater<double>()(1.0-1e-5, 1.0));

	REQUIRE(epsilon_greater<double>()(1000, 1.0));
	REQUIRE(!epsilon_greater<double>()(1.0, 10000));

	REQUIRE(!epsilon_greater<double>()(-1e-5, 0.0));
}

TEST_CASE("Test epsilon_greater_equal", "[]") {
	REQUIRE(!epsilon_greater_equal<double>()(0.0, 1.0));
	REQUIRE(epsilon_greater_equal<double>()(1.0, 0.0));

	REQUIRE(epsilon_greater_equal<double>()(1.0, 1.0));
	REQUIRE(epsilon_greater_equal<double>()(1.00000008, 1.0));
	
	REQUIRE(!epsilon_greater_equal<double>()(1.0, 1.0+1e-5));
	REQUIRE(epsilon_greater_equal<double>()(1.0+1e-5, 1.0));

	REQUIRE(epsilon_greater_equal<double>()(1.0, 1.0-1e-5));
	REQUIRE(!epsilon_greater_equal<double>()(1.0-1e-5, 1.0));

	REQUIRE(epsilon_greater_equal<double>()(1000, 1.0));
	REQUIRE(!epsilon_greater_equal<double>()(1.0, 10000));

	REQUIRE(!epsilon_greater_equal<double>()(-1e-5, 0.0));
	REQUIRE(epsilon_greater_equal<double>()(1e-5, 0.0));
	REQUIRE(epsilon_greater_equal<double>()(-1e-9, 0.0));
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
	array[5] += 1;
	REQUIRE(epsilon_equal<double>()(array[5], 5+1));
	array[9]--;
	REQUIRE(epsilon_equal<double>()(array[9], 5-1));
	const auto& array2 = array;
	REQUIRE(epsilon_equal<double>()(array2[5], 5+1));
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
	{
		IloWrapper<IloNumArray> array3(env, 5);
		std::fill(array3.begin(), array3.end(), 6);
		array2 = array3;
	}
	REQUIRE(array2.size() == 5);
	for(const auto v : array2) {
		REQUIRE(epsilon_equal<double>()(v, 6));
	}

} 

TEST_CASE("Test conversion operator", "[]") {
	IloEnv env;
	IloWrapper<IloNumArray> array(env, 10);
	std::fill(array.begin(), array.end(), 5);
	REQUIRE(epsilon_equal<double>()(IloSum(array), 5*10));
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
