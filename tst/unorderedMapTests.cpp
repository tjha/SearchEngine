// unorderedMapTests.cpp
//
// Perform tests on unordered map implementation
//
// 2019-10-26: file created: jasina

#include "catch.hpp"
#include "../utils/basicString.hpp"
#include "../utils/exception.hpp"
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

	mappy.erase( "alpha" );
	REQUIRE( mappy.size( ) == 3 );
	REQUIRE( mappy.count( "alpha" ) == 0 );
	REQUIRE_THROWS_AS( mappy.at( "alpha" ), dex::outOfRangeException );
	mappy.erase( "beta" );
	REQUIRE( mappy.size( ) == 2 );
	REQUIRE( mappy.count( "alpha" ) == 0 );
	REQUIRE_THROWS_AS( mappy.at( "alpha" ), dex::outOfRangeException );
	REQUIRE( mappy.count( "beta" ) == 0 );
	REQUIRE_THROWS_AS( mappy.at( "beta" ), dex::outOfRangeException );
	mappy.erase( mappy.cbegin( ), mappy.cend( ) );
	REQUIRE( mappy.empty( ) );
	REQUIRE( mappy.count( "alpha" ) == 0 );
	REQUIRE( mappy.count( "beta" ) == 0 );
	REQUIRE( mappy.count( "gamma" ) == 0 );
	REQUIRE( mappy.count( "delta" ) == 0 );
	}
