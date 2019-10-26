// unorderedMapTests.cpp
//
// Perform tests on unordered map implementation
//
// 2019-10-26: file created, constructors, assignment: jasina

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

		map2 = map1;
		REQUIRE( map2.empty( ) );
		REQUIRE( map2.count( "hello" ) == 0 );
		}
	}
