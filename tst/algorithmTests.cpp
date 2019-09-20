#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../utils/algorithm.hpp"

TEST_CASE( "test min and max" )
	{
	REQUIRE( dex::min(10, 15) == 10 );
	REQUIRE( dex::min(15, 10) == 10 );
	REQUIRE( dex::min(unsigned(-1), unsigned(10)) == unsigned(10) );
	
	REQUIRE( dex::max(10, 15) == 15 );
	REQUIRE( dex::max(15, 10) == 15 );
	REQUIRE( dex::max(unsigned(-1), unsigned(10)) == unsigned(-1) );
	}

TEST_CASE( "test swap" )
	{
	int a = 10;
	int b = 11;
	dex::swap(a, b);
	REQUIRE( a == 11 );
	REQUIRE( b == 10 );
	}
