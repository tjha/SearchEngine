// unorderedMapTests.cpp
//
// Perform tests on unordered map implementation
//
// 2019-10-26: file created, constructors, assignment, element access: jasina

#include "catch.hpp"
#include "../utils/basicString.hpp"
#include "../utils/exception.hpp"
#include "../utils/unorderedMap.hpp"

using dex::string;
using dex::unorderedMap;

TEST_CASE( "constructors and assignment ", "[unorderedMap]" )
	{
	SECTION ( "default constructor" )
		{
		unorderedMap < string, int > map;
		REQUIRE( map.empty( ) );
		REQUIRE( map.cbegin( ) == map.cend( ) );
		}
	SECTION ( "iterator constructor" )
		{
		unorderedMap < string, string > map1;
		map1[ "hello" ] = "world";
		map1[ "hola" ] = "mundo";
		map1[ "bonjour" ] = "monde";

		unorderedMap < string, string > map2( map1.cbegin( ), map1.cend( ) );
		REQUIRE( map2.size( ) == 3 );
		REQUIRE( map2[ "hello" ] == "world" );
		REQUIRE( map2[ "hola" ] == "mundo" );
		REQUIRE( map2[ "bonjour" ] == "monde" );

		map1.clear( );
		REQUIRE( map2.size( ) == 3 );
		REQUIRE( map2[ "hello" ] == "world" );
		REQUIRE( map2[ "hola" ] == "mundo" );
		REQUIRE( map2[ "bonjour" ] == "monde" );

		unorderedMap < string, string > map3( map1.cbegin( ), map1.cend( ) );
		REQUIRE( map3.empty( ) );
		REQUIRE( map3.count( "hello" ) == 0 );
		}
	SECTION ( "copy constructor" )
		{
		unorderedMap < string, string > map1;
		map1[ "hello" ] = "world";
		map1[ "hola" ] = "mundo";
		map1[ "bonjour" ] = "monde";

		unorderedMap < string, string > map2( map1 );
		REQUIRE( map2.size( ) == 3 );
		REQUIRE( map2[ "hello" ] == "world" );
		REQUIRE( map2[ "hola" ] == "mundo" );
		REQUIRE( map2[ "bonjour" ] == "monde" );

		map1.clear( );
		REQUIRE( map2.size( ) == 3 );
		REQUIRE( map2[ "hello" ] == "world" );
		REQUIRE( map2[ "hola" ] == "mundo" );
		REQUIRE( map2[ "bonjour" ] == "monde" );

		unorderedMap < string, string > map3( map1 );
		REQUIRE( map3.empty( ) );
		REQUIRE( map3.count( "hello" ) == 0 );
		}
	SECTION ( "operator=" )
		{
		unorderedMap < string, string > map1;
		map1[ "hello" ] = "world";
		map1[ "hola" ] = "mundo";
		map1[ "bonjour" ] = "monde";

		unorderedMap < string, string > map2;
		map2 = map1;
		REQUIRE( map2.size( ) == 3 );
		REQUIRE( map2[ "hello" ] == "world" );
		REQUIRE( map2[ "hola" ] == "mundo" );
		REQUIRE( map2[ "bonjour" ] == "monde" );

		map1.clear( );
		REQUIRE( map2.size( ) == 3 );
		REQUIRE( map2[ "hello" ] == "world" );
		REQUIRE( map2[ "hola" ] == "mundo" );
		REQUIRE( map2[ "bonjour" ] == "monde" );

		map2 = map2;
		REQUIRE( map2.size( ) == 3 );
		REQUIRE( map2[ "hello" ] == "world" );
		REQUIRE( map2[ "hola" ] == "mundo" );
		REQUIRE( map2[ "bonjour" ] == "monde" );

		map2 = map1;
		REQUIRE( map2.empty( ) );
		REQUIRE( map2.count( "hello" ) == 0 );

		map2 = map2;
		REQUIRE( map2.empty( ) );
		REQUIRE( map2.count( "hello" ) == 0 );
		}
	}

TEST_CASE( "element access", "[unorderedMap]" )
	{
	SECTION( "operator [ ]")
		{
		unorderedMap < string, unsigned > map;
		REQUIRE( map.count( "beta" ) == 0 );
		map[ "beta" ] = 1;
		REQUIRE( map.count( "beta" ) == 1 );
		REQUIRE( map.size( ) == 1 );

		REQUIRE( map.count( "gamma" ) == 0 );
		map[ "gamma" ] = 2;
		REQUIRE( map.count( "gamma" ) == 1 );
		REQUIRE( map.size( ) == 2 );

		REQUIRE( map.count( "delta" ) == 0 );
		map[ "delta" ] = 3;
		REQUIRE( map.count( "delta" ) == 1 );
		REQUIRE( map.size( ) == 3 );

		REQUIRE( map.count( "epsilon" ) == 0 );
		map[ "epsilon" ] = 4;
		REQUIRE( map.count( "epsilon" ) == 1 );
		REQUIRE( map.size( ) == 4 );

		REQUIRE( map[ "alpha" ] == 0 );
		REQUIRE( map.size( ) == 5 );
		REQUIRE( map[ "beta" ] == 1 );
		REQUIRE( map[ "gamma" ] == 2 );
		REQUIRE( map[ "delta" ] == 3 );
		REQUIRE( map[ "epsilon" ] == 4 );
		REQUIRE( map.size( ) == 5 );
		}
	SECTION( "at")
		{
		unorderedMap < string, unsigned > map;
		map[ "beta" ] = 1;
		REQUIRE( map.size( ) == 1 );
		map[ "gamma" ] = 2;
		REQUIRE( map.size( ) == 2 );
		map[ "delta" ] = 3;
		REQUIRE( map.size( ) == 3 );
		map[ "epsilon" ] = 4;
		REQUIRE( map.size( ) == 4 );

		REQUIRE_THROWS_AS( map.at( "alpha" ), dex::outOfRangeException );
		REQUIRE( map.size( ) == 4 );
		REQUIRE( map.at( "beta" ) == 1 );
		REQUIRE( map.at( "gamma" ) == 2 );
		REQUIRE( map.at( "delta" ) == 3 );
		REQUIRE( map.at( "epsilon" ) == 4 );
		REQUIRE( map.size( ) == 4 );

		map.at( "beta" ) = 37;
		REQUIRE( map.at( "beta" ) == 37 );
		}
	}
