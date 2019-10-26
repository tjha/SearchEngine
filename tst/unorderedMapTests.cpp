// unorderedMapTests.cpp
//
// Perform tests on unordered map implementation
//
// 2019-10-26: file created: jasina

#include "catch.hpp"
#include "../utils/basicString.hpp"
#include "../utils/unorderedMap.hpp"

using dex::string;
using dex::unorderedMap;

TEST_CASE( "test stuff", "[unorderedMap]")
	{
	unorderedMap < string, int > mappy( 2 );
	mappy[ "alpha" ] = 1;
	REQUIRE( mappy.size( ) == 1 );
	mappy[ "beta" ] = 2;
	REQUIRE( mappy.size( ) == 2 );
	mappy[ "gamma" ] = 3;
	REQUIRE( mappy.size( ) == 3 );
	mappy[ "delta" ] = 4;
	REQUIRE( mappy.size( ) == 4 );
	REQUIRE( mappy[ "alpha" ] == 1 );
	REQUIRE( mappy[ "beta" ] == 2 );
	REQUIRE( mappy[ "gamma" ] == 3 );
	REQUIRE( mappy[ "delta" ] == 4 );
	REQUIRE( mappy.count( "alpha" ) == 1 );
	REQUIRE( mappy.count( "epsilon" ) == 0 );
	}
