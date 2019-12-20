// algorithmTests.cpp
// Testing for the algorithm library
//
// 2019-11-18: Add lowerBound tests: lougheem
// 2019-10-24: Add lexicographicalCompare tests: jasina
// 2019-10-17: Get tests to all compile and pass: jasina
// 2019-10-13: Write copy, fill, find, search, and findEnd tests: jasina
// 2019-9-19: Write min and max tests: combsc

#include "catch.hpp"
#include "utils/algorithm.hpp"
#include "utils/vector.hpp"

TEST_CASE( "test find" )
	{
	dex::vector < int > a;
	for ( int i = 0;  i != 10;  ++i )
		a.pushBack( i );
	REQUIRE( dex::find( a.begin( ), a.end( ), 0) == a.begin( ) );
	REQUIRE( dex::find( a.begin( ), a.end( ), 4) == a.begin( ) + 4 );
	REQUIRE( dex::find( a.begin( ), a.end( ), 9) == a.begin( ) + 9 );
	REQUIRE( dex::find( a.begin( ), a.end( ), 20) == a.end( ) );
	}

TEST_CASE( "test search" )
	{
	dex::vector < int > a, b;
	for ( int i = 0;  i != 10;  ++i )
		a.pushBack( i % 5 );

	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) );

	b.pushBack(0);
	b.pushBack(1);
	b.pushBack(2);
	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) );

	b.pushBack(4);
	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );

	b.clear( );
	b.pushBack(1);
	b.pushBack(2);
	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) + 1 );

	b.clear( );
	for ( int i = 0;  i != 11;  ++i )
		b.pushBack( i );
	REQUIRE( dex::search( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );
	}

TEST_CASE( "test findEnd" )
	{
	dex::vector < int > a, b;
	for ( int i = 0;  i != 10;  ++i )
		a.pushBack( i % 5 );

	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );

	b.pushBack(0);
	b.pushBack(1);
	b.pushBack(2);
	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) + 5 );

	b.pushBack(4);
	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );

	b.clear( );
	b.pushBack(1);
	b.pushBack(2);
	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.begin( ) + 6 );

	b.clear( );
	for ( int i = 0;  i != 11;  ++i )
		b.pushBack( i );
	REQUIRE( dex::findEnd( a.begin( ), a.end( ), b.begin( ), b.end( ) ) == a.end( ) );
	}

TEST_CASE( "test copy" )
	{
	dex::vector < int > a, b;
	b.resize( 20 );
	for ( int i = 0;  i != 10;  ++i )
		a.pushBack( i );
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
	dex::vector < int > a;
	for ( int i = 0;  i != 10;  ++i )
		a.pushBack( i );
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

TEST_CASE( "test lexicographical compare" )
	{
	dex::vector < int > a, b;
	for ( int i = 0;  i != 10;  ++i )
		a.pushBack( i % 5 );
	for ( int i = 0;  i != 10;  ++i )
		b.pushBack( i );

	REQUIRE( dex::lexicographicalCompare( a.begin( ), a.end( ), b.begin( ), b.end( ) ) < 0 );
	REQUIRE( dex::lexicographicalCompare( b.begin( ), b.end( ), a.begin( ), a.end( ) ) > 0 );
	REQUIRE( dex::lexicographicalCompare( a.begin( ), a.begin( ) + 5, b.begin( ), b.begin( ) + 5 ) == 0 );
	REQUIRE( dex::lexicographicalCompare( a.begin( ), a.begin( ) + 4, b.begin( ), b.begin( ) + 5 ) < 0 );
	REQUIRE( dex::lexicographicalCompare( b.begin( ), b.begin( ) + 5, a.begin( ), a.begin( ) + 4 ) > 0 );
	REQUIRE( dex::lexicographicalCompare( a.begin( ), a.begin( ) + 6, b.begin( ), b.begin( ) + 5 ) > 0 );
	REQUIRE( dex::lexicographicalCompare( b.begin( ), b.begin( ) + 5, a.begin( ), a.begin( ) + 6 ) < 0 );
	REQUIRE( dex::lexicographicalCompare( a.begin( ), a.begin( ), b.begin( ), b.begin( ) + 5 ) < 0 );
	REQUIRE( dex::lexicographicalCompare( b.begin( ), b.begin( ) + 5, a.begin( ), a.begin( ) ) > 0 );
	REQUIRE( dex::lexicographicalCompare( a.begin( ), a.begin( ), b.begin( ), b.begin( ) + 1 ) < 0 );
	REQUIRE( dex::lexicographicalCompare( b.begin( ), b.begin( ) + 1, a.begin( ), a.begin( ) ) > 0 );
	REQUIRE( dex::lexicographicalCompare( a.begin( ), a.begin( ), b.begin( ), b.begin( ) ) == 0 );
	REQUIRE( dex::lexicographicalCompare( a.begin( ), a.end( ), a.begin( ), a.end( ) ) == 0 );
	REQUIRE( dex::lexicographicalCompare( a.begin( ), a.begin( ), a.end( ), a.end( ) ) == 0 );
	}

TEST_CASE( "test lowerBound" )
	{
	dex::vector < int > a, b, c;
	for ( int number = 0;  number < 30;  ++number )
		{
		a.pushBack( number );
		b.pushBack( 100 + number );
		c.pushBack( number );
		c.pushBack( number );
		}

	REQUIRE( *dex::lowerBound( a.begin( ), a.end( ), 5 ) == 5 );
	REQUIRE( *dex::lowerBound( a.begin( ), a.end( ), 3 ) == 3 );
	for ( int number = 0;  number < 10;  ++number )
		{
		REQUIRE( *dex::lowerBound( a.begin( ), a.end( ), number ) == number );
		REQUIRE( *dex::lowerBound( b.begin( ), b.end( ), 100 + number ) == 100 + number );
		REQUIRE( *dex::lowerBound( c.begin( ), c.end( ), number) == number );
		REQUIRE( *( dex::lowerBound( c.begin( ), c.end( ), number ) + 1 ) == number );
		}
	}
