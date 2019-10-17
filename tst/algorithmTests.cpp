// algorithmTests.cpp
// Testing for the algorithm library
//
// 2019-10-17: Get tests to all compile and pass: jasina
// 2019-10-13: Write copy, fill, find, search, and findEnd tests: jasina
// 2019-9-19: Write min and max tests: combsc

#include "catch.hpp"
#include "../utils/algorithm.hpp"
#include <vector>

TEST_CASE( "test find" )
	{
	std::vector < int > a;
	for ( int i = 0;  i != 10;  ++i )
		a.push_back( i );
	REQUIRE( dex::find( a.begin( ), a.end( ), 0) == a.begin( ) );
	REQUIRE( dex::find( a.begin( ), a.end( ), 4) == a.begin( ) + 4 );
	REQUIRE( dex::find( a.begin( ), a.end( ), 9) == a.begin( ) + 9 );
	REQUIRE( dex::find( a.begin( ), a.end( ), 20) == a.end( ) );
	}

TEST_CASE( "test search" )
	{
	std::vector < int > a, b;
	for ( int i = 0;  i != 10;  ++i )
		a.push_back( i % 5 );

	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) );

	b.push_back(0);
	b.push_back(1);
	b.push_back(2);
	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) );

	b.push_back(4);
	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );

	b.clear( );
	b.push_back(1);
	b.push_back(2);
	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) + 1 );

	b.clear( );
	for ( int i = 0;  i != 11;  ++i )
		b.push_back( i );
	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );
	}

TEST_CASE( "test findEnd" )
	{
	std::vector < int > a, b;
	for ( int i = 0;  i != 10;  ++i )
		a.push_back( i % 5 );

	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );

	b.push_back(0);
	b.push_back(1);
	b.push_back(2);
	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) + 5 );

	b.push_back(4);
	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );

	b.clear( );
	b.push_back(1);
	b.push_back(2);
	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) + 6 );

	b.clear( );
	for ( int i = 0;  i != 11;  ++i )
		b.push_back( i );
	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );
	}

TEST_CASE( "test copy" )
	{
	std::vector < int > a, b;
	b.resize( 20 );
	for ( int i = 0;  i != 10;  ++i )
		a.push_back( i );
	auto copyPoint = dex::copy( a.cbegin(), a.cend(), b.begin() );
	for ( int i = 0;  i != 10;  ++i )
		REQUIRE( b[i] == a[i] );
	for ( int i = 10;  i != 20;  ++i )
		REQUIRE( b[i] == 0 );
	REQUIRE( copyPoint == b.begin() + 10 );

	copyPoint = dex::copyBackward( b.cbegin( ), b.cbegin( ) + 10, b.end( ) );
	for ( int i = 0;  i != 10;  ++i )
		{
		REQUIRE( b[i] == b[i] );
		REQUIRE( b[i + 10] == b[i] );
		}
	REQUIRE( copyPoint == b.begin() + 10 );
	}

TEST_CASE( "test fill" )
	{
	std::vector < int > a;
	for ( int i = 0;  i != 10;  ++i )
		a.push_back( i );
	dex::fill( a.begin( ), a.end( ), 489 );
	for ( int i = 0;  i != 10;  ++i )
		REQUIRE( a[i] == 489 );
	}

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
