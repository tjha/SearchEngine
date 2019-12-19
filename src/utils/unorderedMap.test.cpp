// unorderedMapTests.cpp
//
// Perform tests on unordered map implementation
//
// 2019-11-30: Added insertionWillRehash tests: combsc
// 2019-10-26: file created, constructors, assignment, element access, element removal: jasina

#include "catch.hpp"
#include "basicString.hpp"
#include "exception.hpp"
#include "unorderedMap.hpp"

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
		unorderedMap < string, string > map1( 1 );
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
		unorderedMap < string, string > map1( 1 );
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
		unorderedMap < string, string > map1( 1 );
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
	SECTION( "operator [ ]" )
		{
		unorderedMap < string, unsigned > map( 1 );
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
	SECTION( "at" )
		{
		unorderedMap < string, unsigned > map( 1 );
		map[ "beta" ] = 1;
		map[ "gamma" ] = 2;
		map[ "delta" ] = 3;
		map[ "epsilon" ] = 4;

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

	SECTION( "find" )
		{
		unorderedMap < string, unsigned > map( 1 );
		map[ "beta" ] = 1;
		map[ "gamma" ] = 2;
		map[ "delta" ] = 3;
		map[ "epsilon" ] = 4;

		REQUIRE( map.find( "bagel" ) == map.cend( ) );

		REQUIRE( map.find( "beta" )->first == "beta" );
		REQUIRE( map.find( "beta" )->second == 1 );
		map.erase( map.find( "beta" ) );
		REQUIRE( map.find( "beta" ) == map.cend( ) );
		}
	
	SECTION( "insertionWillRehash" )
		{
		unorderedMap < string, unsigned > map( 2 );
		map[ "beta" ] = 1;
		REQUIRE( map.insertionWillRehash( "gamma" ) );
		REQUIRE( !map.insertionWillRehash( "beta" ) );
		map[ "gamma" ] = 2;
		REQUIRE( !map.insertionWillRehash( "gamma" ) );
		REQUIRE( !map.insertionWillRehash( "beta" ) );
		REQUIRE( map.insertionWillRehash( "alpha" ) );
		REQUIRE( map.insertionWillRehash( "delta" ) );
		map[ "delta" ] = 3;
		REQUIRE( !map.insertionWillRehash( "alpha" ) );
		}
	}

TEST_CASE( "element removal", "[unorderedMap]" )
	{
	SECTION( "erase with one iterator" )
		{
		unorderedMap < string, char > map( 1 );
		map[ "a" ] = 'a';
		map[ "b" ] = 'b';
		map[ "c" ] = 'c';
		map[ "d" ] = 'd';

		unorderedMap < string, char >::constIterator nextElement = map.find( ( ++map.cbegin( ) )->first );
		REQUIRE( map.erase( map.cbegin( ) ) == nextElement++ );
		REQUIRE( map.erase( map.cbegin( ) ) == nextElement++ );
		REQUIRE( map.erase( map.cbegin( ) ) == nextElement++ );
		REQUIRE( map.erase( map.cbegin( ) ) == nextElement );

		map[ "a" ] = 'a';
		map[ "b" ] = 'b';
		map[ "c" ] = 'c';
		map[ "d" ] = 'd';
		nextElement = map.find( ( ++( ++map.cbegin( ) ) )->first );
		REQUIRE( map.erase( ++map.cbegin( ) ) == nextElement );
		}
	SECTION( "erase with two iterators" )
		{
		unorderedMap < string, char > map( 1 );
		map[ "a" ] = 'a';
		map[ "b" ] = 'b';
		map[ "c" ] = 'c';
		map[ "d" ] = 'd';

		REQUIRE( map.erase( map.cbegin( ), map.cend( ) ) == map.cend( ) );
		REQUIRE( map.empty( ) );
		REQUIRE( map.count( "a" ) == 0 );
		REQUIRE( map.count( "b" ) == 0 );
		REQUIRE( map.count( "c" ) == 0 );
		REQUIRE( map.count( "d" ) == 0 );

		map[ "a" ] = 'a';
		map[ "b" ] = 'b';
		map[ "c" ] = 'c';
		map[ "d" ] = 'd';
		REQUIRE( map.erase( ++map.cbegin( ), map.cend( ) ) == map.cend( ) );
		REQUIRE( map.size( ) == 1 );
		}
	SECTION( "erase with one key" )
		{
		unorderedMap < string, char > map( 1 );
		map[ "a" ] = 'a';
		map[ "b" ] = 'b';
		map[ "c" ] = 'c';
		map[ "d" ] = 'd';

		REQUIRE( map.erase( "a" ) == 1 );
		REQUIRE( map.size( ) == 3 );
		REQUIRE( map.erase( "a" ) == 0 );
		REQUIRE( map.size( ) == 3 );
		REQUIRE( map.erase( "e" ) == 0 );
		REQUIRE( map.size( ) == 3 );
		REQUIRE( map.erase( "d" ) == 1 );
		REQUIRE( map.size( ) == 2 );
		}
	SECTION( "clear" )
		{
		unorderedMap < string, char > map( 1 );

		map.clear( );
		REQUIRE( map.empty( ) );

		map[ "a" ] = 'a';
		map[ "b" ] = 'b';
		map[ "c" ] = 'c';
		map[ "d" ] = 'd';

		map.clear( );
		REQUIRE( map.empty( ) );
		REQUIRE( map.count( "a" ) == 0 );
		REQUIRE( map.count( "b" ) == 0 );
		REQUIRE( map.count( "c" ) == 0 );
		REQUIRE( map.count( "d" ) == 0 );
		}
	}
