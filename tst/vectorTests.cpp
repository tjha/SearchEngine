// vector_tests.cpp
//
// Perform tests on vector implementation
//
// 2019-10-21 combsc - Implemented test cases for push_back, size manipulation, element access
//                   - insert, improved constructor test cases
// 2019-09-17 Tejas Jha - Implemented basic constructor test cases

#include <iostream> // TODO: remove this later
#include "catch.hpp"
#include "../utils/vector.hpp"

using dex::vector;

TEST_CASE( "test constructors", "[vector]")
	{
	SECTION( "basic constructors" )
		{
		vector < int > v1;           // default constructor
		REQUIRE( v1.size( ) == 0 );
		vector < int > v2( 5 );      // constructor with specified size
		REQUIRE( v2.size( ) == 5 );
		vector < int > v3( 0 );      // constructor with size specified to 0
		REQUIRE( v3.size( ) == 0 );
		}

	SECTION( "copy" )
		{
		vector < int > v4( 5, 2 );   // constructor of specified size and default val
		REQUIRE( v4.size( ) == 5 );
		REQUIRE( v4.at( 0 ) == 2 );

		vector < int > v5( v4 );     // constructor where you pass in other vector
		REQUIRE( v5.size( ) == 5 );
		REQUIRE( v5.at( 0 ) == 2 );
		v4.pop_back( );
		v4[ 0 ] = 1;

		REQUIRE( v5.size( ) == 5 );
		REQUIRE( v5.at( 0 ) == 2 );
		}
	SECTION( "assignment operator" )
		{
		vector < int > v4( 5, 2 );
		vector < int > v5;
		v5 = v4;                     // assignment operator
		REQUIRE( v5.size( ) == 5 );
		REQUIRE( v5.at( 0 ) == 2 );
		v4.pop_back( );
		v4[ 0 ] = 1;

		REQUIRE( v5.size( ) == 5 );
		REQUIRE( v5.at( 0 ) == 2 );
		}
	}

TEST_CASE( "test pushBack, popBack", "[vector]" )
	{
	vector < int > v1;
	for ( int i = 0;  i < 5;  ++i )
		v1.push_back( i );

	REQUIRE( v1[ 4 ] == 4 );
	int a = 5;
	v1.push_back( a );
	REQUIRE( v1[ 5 ] == 5 );
	a = 10;
	REQUIRE( v1[ 5 ] == 5 );
	REQUIRE( v1.size( ) == 6 );

	v1.pop_back( );
	REQUIRE( v1[ 4 ] == 4 );
	REQUIRE( v1.size( ) == 5 );
	}

TEST_CASE( "test size manipulation" , "[vector]" )
	{
	SECTION( "clear works" )
		{
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.push_back( i );

		v1.clear( );
		REQUIRE( v1.size( ) == 0 );

		vector < int > v2;
		v2.clear( );
		REQUIRE( v2.size( ) == 0 );
		}

	SECTION( "resize works" )
		{
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.push_back( i );
		
		v1.resize( 100 );
		REQUIRE( v1.size( ) == 100 );
		v1.resize( 0 );
		REQUIRE( v1.size( ) == 0 );
		REQUIRE_THROWS_AS( v1.at( 5 ), dex::outOfRangeException );
		
		vector < int > v2;
		for ( int i = 0;  i < 5;  ++i )
			v2.push_back( i );
		
		v2.resize( 100, 5 );
		REQUIRE( v2.size( ) == 100 );
		REQUIRE( v2[7] == 5 );
		v2.resize( 3 , 100 );
		REQUIRE( v2.size( ) == 3 );
		REQUIRE( v2[ 2 ] == 2 );
		REQUIRE_THROWS_AS( v1.at( 5 ), dex::outOfRangeException );
		}

	SECTION( "shrinkToFit works" )
		{
		// Test shrink to fit
		// Assumes shrink to fit is binding.
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.push_back( i );

		bool itWorks = true;
		while ( v1.capacity( ) == v1.size( ) )
			{
			if( v1.size( ) == 100 )
				{
				itWorks = false;
				break;
				}
			v1.push_back( 1 );
			}

		if ( itWorks )
			{
			v1.shrink_to_fit( );
			REQUIRE( v1.size( ) == v1.capacity( ) );
			}
		
		}
	}

TEST_CASE( "test element access", "[vector]" )
	{
		{
		// test []
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.push_back( i );
		for ( int i = 0;  i < 5;  ++i )
			REQUIRE( v1[ i ] == i );
		}
		{
		// test front, back
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.push_back( i );
		REQUIRE( v1.front( ) == 0 );
		REQUIRE( v1.back( ) == 4 );
		}
		{
		// test at
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.push_back( i );
		for ( int i = 0;  i < 5;  ++i )
			REQUIRE( v1.at( i ) == i );
		REQUIRE_THROWS_AS( v1.at( 10 ), dex::outOfRangeException );
		}
		{
		// test data
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.push_back( i );
		for ( int i = 0;  i < 5;  ++i )
			REQUIRE( v1.data( )[ i ] == i );
		}
	}

TEST_CASE( "test insert", "[vector]" )
	{
	vector < int > v1;
	for( int i = 0;  i < 5;  ++i )
		v1.push_back( i );

	v1.insert( v1.begin( ), 10 );
	REQUIRE( v1.size( ) == 6 );
	REQUIRE( v1.front( ) == 10 );
	REQUIRE( v1[ 1 ] == 0 );

	v1.insert( v1.end( ), 10 );
	REQUIRE( v1.size( ) == 7 );
	REQUIRE( v1[ 6 ] == 10 );
	REQUIRE( v1[ 5 ] == 4 );

	vector < int > v2;
	v2.push_back( 1 );
	v2.push_back( 1 );

	/*
	v2.insert( v2.begin( ) + 1, 5, 0 );
	REQUIRE( v2[ 0 ] == 1 );
	REQUIRE( v2[ 3 ] == 0 );
	REQUIRE( v2[ 6 ] == 1 );
	REQUIRE( v2.size( ) == 7 );

	vector < int > v3;
	for( int i = 0;  i < 5;  ++i )
		v3.push_back( i );
	
	vector < int > v4;
	v4.push_back( -1 );
	v4.push_back( 5 );
	v4.insert( v4.begin( ) + 1, v3.begin( ), v3.end( ) );
	REQUIRE( v4.size( ) == 7 );
	REQUIRE( v4.front( ) == -1 );
	REQUIRE( v4.back( ) == 5 );
	REQUIRE( v4[ 1 ] == 0 );
	*/
	}
