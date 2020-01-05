// unorderedSetTests.cpp
//
// Perform tests on unordered set implementation
//
// 2019-10-28: File created: jasina

#include "catch.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/unorderedSet.hpp"

using dex::string;
using dex::unorderedSet;

TEST_CASE( "constructors and assignment ", "[unorderedSet]" )
	{
	SECTION ( "default constructor" )
		{
		unorderedSet< string > set;
		REQUIRE( set.empty( ) );
		REQUIRE( set.cbegin( ) == set.cend( ) );
		}
	SECTION ( "iterator constructor" )
		{
		unorderedSet< string > set1( 1 );
		set1.insert( "hello" );
		set1.insert( "hola" );
		set1.insert( "bonjour" );

		unorderedSet< string > set2( set1.cbegin( ), set1.cend( ) );
		REQUIRE( set2.size( ) == 3 );
		REQUIRE( set2.count( "hello" ) == 1 );
		REQUIRE( set2.count( "hola" ) == 1 );
		REQUIRE( set2.count( "bonjour" ) == 1 );

		set1.clear( );
		REQUIRE( set2.size( ) == 3 );
		REQUIRE( set2.count( "hello" ) == 1 );
		REQUIRE( set2.count( "hola" ) == 1 );
		REQUIRE( set2.count( "bonjour" ) == 1 );

		unorderedSet< string > set3( set1.cbegin( ), set1.cend( ) );
		REQUIRE( set3.empty( ) );
		REQUIRE( set3.count( "hello" ) == 0 );
		}
	SECTION ( "copy constructor" )
		{
		unorderedSet< string > set1( 1 );
		set1.insert( "hello" );
		set1.insert( "hola" );
		set1.insert( "bonjour" );

		unorderedSet< string > set2( set1 );
		REQUIRE( set2.size( ) == 3 );
		REQUIRE( set2.count( "hello" ) == 1 );
		REQUIRE( set2.count( "hola" ) == 1 );
		REQUIRE( set2.count( "bonjour" ) == 1 );

		set1.clear( );
		REQUIRE( set2.size( ) == 3 );
		REQUIRE( set2.count( "hello" ) == 1 );
		REQUIRE( set2.count( "hola" ) == 1 );
		REQUIRE( set2.count( "bonjour" ) == 1 );

		unorderedSet< string > set3( set1 );
		REQUIRE( set3.empty( ) );
		REQUIRE( set3.count( "hello" ) == 0 );
		}
	SECTION ( "operator=" )
		{
		unorderedSet< string > set1( 1 );
		set1.insert( "hello" );
		set1.insert( "hola" );
		set1.insert( "bonjour" );

		unorderedSet< string > set2;
		set2 = set1;
		REQUIRE( set2.size( ) == 3 );
		REQUIRE( set2.count( "hello" ) == 1 );
		REQUIRE( set2.count( "hola" ) == 1 );
		REQUIRE( set2.count( "bonjour" ) == 1 );

		set1.clear( );
		REQUIRE( set2.size( ) == 3 );
		REQUIRE( set2.count( "hello" ) == 1 );
		REQUIRE( set2.count( "hola" ) == 1 );
		REQUIRE( set2.count( "bonjour" ) == 1 );

		set2 = set2;
		REQUIRE( set2.size( ) == 3 );
		REQUIRE( set2.count( "hello" ) == 1 );
		REQUIRE( set2.count( "hola" ) == 1 );
		REQUIRE( set2.count( "bonjour" ) == 1 );

		set2 = set1;
		REQUIRE( set2.empty( ) );
		REQUIRE( set2.count( "hello" ) == 0 );

		set2 = set2;
		REQUIRE( set2.empty( ) );
		REQUIRE( set2.count( "hello" ) == 0 );
		}
	}

TEST_CASE( "element insertion", "[unorderedSet]" )
	{
	SECTION( "insert (single element)" )
		{
		unorderedSet< string > set( 1 );
		REQUIRE( set.count( "beta" ) == 0 );
		set.insert( "beta" );
		REQUIRE( set.count( "beta" ) == 1 );
		REQUIRE( set.size( ) == 1 );

		REQUIRE( set.count( "gamma" ) == 0 );
		set.insert( "gamma" );
		REQUIRE( set.count( "gamma" ) == 1 );
		REQUIRE( set.size( ) == 2 );

		REQUIRE( set.count( "delta" ) == 0 );
		set.insert( "delta" );
		REQUIRE( set.count( "delta" ) == 1 );
		REQUIRE( set.size( ) == 3 );

		REQUIRE( set.count( "epsilon" ) == 0 );
		set.insert( "epsilon" );
		REQUIRE( set.count( "epsilon" ) == 1 );
		REQUIRE( set.size( ) == 4 );

		REQUIRE( set.count( "alpha" ) == 0 );
		REQUIRE( set.size( ) == 4 );
		}
	SECTION( "insert (range)" )
		{
		unorderedSet< string > set1( 1 ), set2( 1 );
		set1.insert( "beta" );
		set1.insert( "gamma" );
		set1.insert( "delta" );

		set2.insert( set1.cbegin( ), set1.cend( ) );

		set1.insert( "epsilon" );

		REQUIRE( set2.size( ) == 3 );
		REQUIRE( set2.count( "beta" ) == 1 );
		REQUIRE( set2.count( "gamma" ) == 1 );
		REQUIRE( set2.count( "delta" ) == 1 );
		REQUIRE( set2.count( "epsilon" ) == 0 );
		}

	SECTION( "find" )
		{
		unorderedSet< string > set( 1 );
		set.insert( "beta" );
		set.insert( "gamma" );
		set.insert( "delta" );
		set.insert( "epsilon" );

		REQUIRE( set.find( "bagel" ) == set.cend( ) );

		REQUIRE( *set.find( "beta" ) == "beta" );
		set.erase( set.find( "beta" ) );
		REQUIRE( set.find( "beta" ) == set.cend( ) );
		}
	}

TEST_CASE( "element removal", "[unorderedSet]" )
	{
	SECTION( "erase with one iterator" )
		{
		unorderedSet< string > set( 1 );
		set.insert( "a" );
		set.insert( "b" );
		set.insert( "c" );
		set.insert( "d" );

		unorderedSet< string >::constIterator nextElement = set.find( *( ++set.cbegin( ) ) );
		REQUIRE( set.erase( set.cbegin( ) ) == nextElement++ );
		REQUIRE( set.erase( set.cbegin( ) ) == nextElement++ );
		REQUIRE( set.erase( set.cbegin( ) ) == nextElement++ );
		REQUIRE( set.erase( set.cbegin( ) ) == nextElement );

		set.insert( "a" );
		set.insert( "b" );
		set.insert( "c" );
		set.insert( "d" );
		nextElement = set.find( *( ++( ++set.cbegin( ) ) ) );
		REQUIRE( set.erase( ++set.cbegin( ) ) == nextElement );
		}
	SECTION( "erase with two iterators" )
		{
		unorderedSet< string > set( 1 );
		set.insert( "a" );
		set.insert( "b" );
		set.insert( "c" );
		set.insert( "d" );

		REQUIRE( set.erase( set.cbegin( ), set.cend( ) ) == set.cend( ) );
		REQUIRE( set.empty( ) );
		REQUIRE( set.count( "a" ) == 0 );
		REQUIRE( set.count( "b" ) == 0 );
		REQUIRE( set.count( "c" ) == 0 );
		REQUIRE( set.count( "d" ) == 0 );

		set.insert( "a" );
		set.insert( "b" );
		set.insert( "c" );
		set.insert( "d" );
		REQUIRE( set.erase( ++set.cbegin( ), set.cend( ) ) == set.cend( ) );
		REQUIRE( set.size( ) == 1 );
		}
	SECTION( "erase with one key" )
		{
		unorderedSet< string > set( 1 );
		set.insert( "a" );
		set.insert( "b" );
		set.insert( "c" );
		set.insert( "d" );

		REQUIRE( set.erase( "a" ) == 1 );
		REQUIRE( set.size( ) == 3 );
		REQUIRE( set.erase( "a" ) == 0 );
		REQUIRE( set.size( ) == 3 );
		REQUIRE( set.erase( "e" ) == 0 );
		REQUIRE( set.size( ) == 3 );
		REQUIRE( set.erase( "d" ) == 1 );
		REQUIRE( set.size( ) == 2 );
		}
	SECTION( "clear" )
		{
		unorderedSet< string > set( 1 );

		set.clear( );
		REQUIRE( set.empty( ) );

		set.insert( "a" );
		set.insert( "b" );
		set.insert( "c" );
		set.insert( "d" );

		set.clear( );
		REQUIRE( set.empty( ) );
		REQUIRE( set.count( "a" ) == 0 );
		REQUIRE( set.count( "b" ) == 0 );
		REQUIRE( set.count( "c" ) == 0 );
		REQUIRE( set.count( "d" ) == 0 );
		}
	}
