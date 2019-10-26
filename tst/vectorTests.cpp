// vector_tests.cpp
//
// Perform tests on vector implementation
//
// 2019-10-26 medhak, tjha - Added test to pushBack and popBack to vector <string>
// 2019-10-24 jasina, jhirshey, lougheem, medhak - finish up remaining test cases
// 2019-10-22 combsc - Improved insert test cases, added erase test cases
// 2019-10-21 combsc - Implemented test cases for pushBack, size manipulation, element access
//                   - insert, improved constructor test cases
// 2019-09-17 Tejas Jha - Implemented basic constructor test cases

#include "catch.hpp"
#include "../utils/vector.hpp"
#include "../utils/basicString.hpp"

using dex::vector;
using dex::string;

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
		v4.popBack( );
		v4[ 0 ] = 1;

		REQUIRE( v5.size( ) == 5 );
		REQUIRE( v5.at( 0 ) == 2 );
		}

	SECTION( "iterator constructor" )
		{
		vector < int > v1( 5, 1 );
		vector < int > v2( v1.begin( ), v1.end( ) );
		REQUIRE( v2.size() == 5 );
		REQUIRE( v1 == v2 );
		}

	SECTION( "assignment operator" )
		{
		vector < int > v4( 5, 2 );
		vector < int > v5;
		v5 = v4;                     // assignment operator
		REQUIRE( v5.size( ) == 5 );
		REQUIRE( v5.at( 0 ) == 2 );
		v4.popBack( );
		v4[ 0 ] = 1;

		REQUIRE( v5.size( ) == 5 );
		REQUIRE( v5.at( 0 ) == 2 );
		}

	SECTION( "dex::swap" )
		{
		vector < int > v1( 5, 1 );
		v1[ 0 ] = -1;
		dex::swap( v1.front( ), v1.back( ) );
		REQUIRE( v1.front( ) == 1 );
		REQUIRE( v1.back( ) == -1 );
		dex::swap( v1.front( ), v1.front( ) );
		REQUIRE( v1.front( ) == 1 );
		}
	}

TEST_CASE( "assign" )
	{
	vector < char > v1;
	char str[ 4 ] = "duo";
	v1.assign( str, str + 3 );
	REQUIRE( v1.size( ) == 3 );
	REQUIRE( v1[ 0 ] == 'd' );
	REQUIRE( v1[ 1 ] == 'u' );
	REQUIRE( v1[ 2 ] == 'o' );
	v1.assign( 2, 'm' );
	REQUIRE( v1.size( ) == 2 );
	REQUIRE( v1[ 0 ] == 'm' );
	REQUIRE( v1[ 1 ] == 'm' );
	}

TEST_CASE( "test pushBack, popBack", "[vector]" )
	{
	vector < int > v1;
	for ( int i = 0;  i < 5;  ++i )
		v1.pushBack( i );

	REQUIRE( v1[ 4 ] == 4 );
	int a = 5;
	v1.pushBack( a );
	REQUIRE( v1[ 5 ] == 5 );
	a = 10;
	REQUIRE( v1[ 5 ] == 5 );
	REQUIRE( v1.size( ) == 6 );

	v1.popBack( );
	REQUIRE( v1[ 4 ] == 4 );
	REQUIRE( v1.size( ) == 5 );

	v1.pushFront( 9 );
	REQUIRE( v1.size( ) == 6 );
	REQUIRE( v1[ 0 ] == 9 );
	v1.popFront( );
	REQUIRE( v1.size( ) == 5 );
	REQUIRE( v1[ 0 ] == 0 );
	}

TEST_CASE( "test pushBack, popBack with string", "[vector]" )
   {
   vector < string > v;

   for (size_t i = 0; i < 5; ++i)
      {
      v.pushBack("example");
      REQUIRE( v[i] == "example" );
      }

   for (size_t i = 0; i < 5; ++i)
      {
      v.popBack();
      REQUIRE( v.size() == 5 - i -1 );
      }
   }

TEST_CASE( "test size manipulation" , "[vector]" )
	{
	SECTION( "test grow" )
		{
		vector < unsigned > v;
		for ( unsigned i = 0;  i != 9001;  ++i)
			{
			v.pushBack( i );
			REQUIRE( v.size( ) == i + 1 );
			}
		}

	SECTION( "test reserve" )
		{
		vector < char > word;
		word.reserve( 10 );
		REQUIRE( word.empty( ) );
		REQUIRE( word.capacity( ) == 10 );
		for ( char c = 'a';  c != 'k';  ++c )
			word.pushBack( c );
		REQUIRE( word.capacity( ) == 10 );
		REQUIRE( word.size( ) == 10);
		word.reserve( 5 ); // This shouldn't do anything
		REQUIRE( word.capacity( ) == 10 );
		REQUIRE( word.size( ) == 10 );
		}

	SECTION( "empty" )
		{
		vector < bool > v1;
		REQUIRE( v1.empty( ) );

		v1.insert( v1.cbegin( ), false );
		REQUIRE( !v1.empty( ) );

		v1.erase( v1.cbegin( ) );
		REQUIRE( v1.empty( ) );

		for( int count = 0; count < 5; ++count )
			{
			v1.insert( v1.cbegin( ), true );
			REQUIRE( !v1.empty( ) );
			}

		v1.erase( v1.cbegin( ), v1.cend( ) - 1 );
		REQUIRE( !v1.empty( ) );
		
		v1.popBack( );
		REQUIRE( v1.empty( ) );
		}

	SECTION( "clear works" )
		{
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.pushBack( i );

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
			v1.pushBack( i );
		
		v1.resize( 100 );
		REQUIRE( v1.size( ) == 100 );
		v1.resize( 0 );
		REQUIRE( v1.size( ) == 0 );
		REQUIRE_THROWS_AS( v1.at( 5 ), dex::outOfRangeException );
		
		vector < int > v2;
		for ( int i = 0;  i < 5;  ++i )
			v2.pushBack( i );
		
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
			v1.pushBack( i );

		bool itWorks = true;
		while ( v1.capacity( ) == v1.size( ) )
			{
			if( v1.size( ) == 100 )
				{
				itWorks = false;
				break;
				}
			v1.pushBack( 1 );
			}

		if ( itWorks )
			{
			v1.shrinkToFit( );
			REQUIRE( v1.size( ) == v1.capacity( ) );
			}
		}
	}

TEST_CASE( "test element access", "[vector]" )
	{
	SECTION( "test operator [ ]" ) 
		{
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.pushBack( i );
		for ( int i = 0;  i < 5;  ++i )
			REQUIRE( v1[ i ] == i );
		}
	SECTION( "test front, back" ) 
		{
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.pushBack( i );
		REQUIRE( v1.front( ) == 0 );
		REQUIRE( v1.back( ) == 4 );
		}
	SECTION( "test at" ) 
		{
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.pushBack( i );
		for ( int i = 0;  i < 5;  ++i )
			REQUIRE( v1.at( i ) == i );
		REQUIRE_THROWS_AS( v1.at( 10 ), dex::outOfRangeException );
		}
	SECTION( "test data" ) 
		{
		vector < int > v1;
		for ( int i = 0;  i < 5;  ++i )
			v1.pushBack( i );
		for ( int i = 0;  i < 5;  ++i )
			REQUIRE( v1.data( )[ i ] == i );
		}
	}

TEST_CASE( "test insert", "[vector]" )
	{
	vector < int > v1;
	for( int i = 0;  i < 5;  ++i )
		v1.pushBack( i );

	v1.insert( v1.begin( ), 10 );
	REQUIRE( v1.size( ) == 6 );
	REQUIRE( v1.front( ) == 10 );
	REQUIRE( v1[ 1 ] == 0 );

	v1.insert( v1.end( ), 10 );
	REQUIRE( v1.size( ) == 7 );
	REQUIRE( v1[ 6 ] == 10 );
	REQUIRE( v1[ 5 ] == 4 );

	vector < int > v2;
	v2.pushBack( 1 );
	v2.pushBack( 1 );

	
	v2.insert( v2.begin( ) + 1, 5, 0 );
	REQUIRE( v2.size( ) == 7 );
	REQUIRE( v2[ 0 ] == 1 );
	REQUIRE( v2[ 3 ] == 0 );
	REQUIRE( v2[ 6 ] == 1 );

	
	vector < int > v3;
	for( int i = 0;  i < 5;  ++i )
		v3.pushBack( i );
	
	vector < int > v4;
	v4.pushBack( -1 );
	v4.pushBack( 5 );
	v4.insert( v4.begin( ) + 1, v3.begin( ), v3.end( ) );
	REQUIRE( v4.size( ) == 7 );
	REQUIRE( v4.front( ) == -1 );
	REQUIRE( v4.back( ) == 5 );
	REQUIRE( v4[ 1 ] == 0 );
	}

TEST_CASE( "test erase", "[vector]" )
	{
	vector < int > v1;
	for( int i = 0;  i < 5;  ++i )
		v1.pushBack( i );
	
	v1.erase( v1.cbegin( ) + 2 );
	REQUIRE( v1.size( ) == 4 );
	REQUIRE( v1[ 1 ] == 1 );
	REQUIRE( v1[ 2 ] == 3 );
	v1.erase( v1.cbegin( ) + 2 );
	REQUIRE( v1[ 2 ] == 4 );
	REQUIRE( v1.size( ) == 3 );

	vector < int > v2;
	for( int i = 0;  i < 5;  ++i )
		v2.pushBack( i );

	v2.erase( v2.cbegin( ), v2.cbegin( ) + 2 );
	REQUIRE( v2.size( ) == 3 );
	REQUIRE( v2[ 0 ] == 2 );
	REQUIRE( v2[ 1 ] == 3 );
	REQUIRE( v2[ 2 ] == 4 );
	v2.erase( v2.cbegin( ), v2.cend( ) );
	REQUIRE( v2.size( ) == 0 );

	v1.clear( );
	v1.pushBack( 1 );
	v1.erase( 0 );
	REQUIRE( v1.empty( ) );
	v1.pushBack( 1 );
	v1.pushBack( 2 );
	v1.erase( 0 );
	REQUIRE( v1.size( ) == 1 );
	v1.pushBack( 3 );
	v1.erase( 1 );
	REQUIRE( v1.size( ) == 1 );
	}

TEST_CASE( "test swap", "[vector]" )
	{
	vector < int > v1;
	vector < int > v2;
	for ( int i = 0;  i < 5;  ++i )
		{
		v1.pushBack( i );
		v2.pushBack( 10 + i );
		}
	vector < int > v1Copy = v1;
	vector < int > v2Copy = v2;
	
	v1Copy.swap( v2Copy );
	REQUIRE( v1Copy == v2 );
	REQUIRE( v2Copy == v1 );

	v1Copy.popBack( );
	REQUIRE( v1Copy != v2 );

	vector < int > v3;
	v3.swap( v1 );
	REQUIRE( v3 == v2Copy );
	REQUIRE( v1.empty( ) );
	}
TEST_CASE( "test comparators", "[vector]" )
	{
	vector < int > v1;
	vector < int > v2;
	for ( int i = 0;  i < 5;  ++i )
		{
		v1.pushBack( i );
		v2.pushBack( 2 * i );
		}

	REQUIRE ( v1 < v2 );
	REQUIRE ( v1 <= v2 );
	REQUIRE ( v1 != v2 );
	REQUIRE ( v2 > v1 );
	REQUIRE ( v2 >= v1 );
	REQUIRE ( v1 == v1 );
	REQUIRE ( v1 <= v1 );
	REQUIRE ( v1 >= v1 );

	vector < char > v3;
	v3.pushBack('a');
	v3.pushBack('b');
	v3.pushBack('c');
	v3.pushFront('d');
	
	REQUIRE ( v3[0] == 'd' );
	REQUIRE ( v3 == v3 );
	REQUIRE ( v3 <= v3 );
	REQUIRE ( v3 >= v3 );

	vector < char > v4;
	v4.pushBack('a');
	v4.pushBack('b');
	v4.pushBack('c');
	v4.pushFront('d');

	REQUIRE ( v3 == v4 );
	REQUIRE ( v3 <= v4 );
	REQUIRE ( v3 >= v4 );

	v4[3] = 'f';
	REQUIRE ( v3 != v4 );
	
	vector < char > v;
	REQUIRE ( v == v );
	REQUIRE ( v <= v );
	REQUIRE ( v >= v );
	REQUIRE ( v < v3 );
	REQUIRE ( v != v3 );

	v4.popBack();
	REQUIRE ( v4 < v3 );
	REQUIRE ( v4 <= v3 );
	REQUIRE ( v3 != v4 );
	v3.popFront();
	REQUIRE ( v3 < v4 );
	REQUIRE ( v3 <= v4 );
	REQUIRE ( v3 != v4 );

	}
