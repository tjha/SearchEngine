// storageTests.cpp
// Tests the encoding and decoding of the defined types for crawler
//
// 2019-11-20: File created and testing integer encode: jhirsh

#include "catch.hpp"
#include "exception.hpp"
#include "unorderedMap.hpp"
#include "vector.hpp"
#include "encode.hpp"
#include "basicString.hpp"
#include <iostream>

using namespace dex;
using namespace dex::crawler;

TEST_CASE( "encode", "[types]" )
	{
	SECTION( "int" )
		{
		int i = 4;
		encoder < int > tEncoder;
		dex::vector < dex::byte > encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 4 );
		REQUIRE( encoded[ 3 ] == 4 );
		REQUIRE( encoded[ 2 ] == 0 );
		REQUIRE( encoded[ 1 ] == 0 );
		REQUIRE( encoded[ 0 ] == 0 );

		i = 0xFF;
		encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 4 );
		REQUIRE( encoded[ 0 ] == 0x00 );
		REQUIRE( encoded[ 3 ] == 0xFF );

		i = 0xFF00;
		encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 4 );
		REQUIRE( encoded[ 0 ] == 0x00 );
		REQUIRE( encoded[ 2 ] == 0xFF );

		decoder < int > IntegerDecoder;
		i = 5;
		REQUIRE( i == IntegerDecoder( tEncoder( i ).data( ) ) );
		}

	// string encoding = ( int size ) + ( string )
	SECTION( "basicString" )
		{
		string magikarp = "magikarp";
		encoder < string > tEncoder;
		vector< byte > encoded = tEncoder( magikarp );

		REQUIRE( encoded.size( ) == sizeof( int ) + magikarp.size( ) );
		for ( size_t i = sizeof( int );  i < encoded.size( );  ++i )
			{
			REQUIRE( encoded[ i ] == magikarp[ i - 4 ] );
			}

		magikarp = "";
		encoded = tEncoder( magikarp );
		REQUIRE( encoded.size( ) == sizeof( int ) + magikarp.size( ) );
		REQUIRE( encoded[ 0 ] == 0 );
		}

	SECTION( "empty types" )
		{
		int i = 0;
		encoder < int > tEncoder;
		dex::vector < dex::byte > encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 4 );
		REQUIRE( encoded[ 0 ] == 0 );
		}
	}

