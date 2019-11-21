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
		REQUIRE( encoded.size( ) == 1 );
		REQUIRE( encoded[ 0 ] == 4 );

		i = 0xFF;
		encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 1 );
		REQUIRE( encoded[ 0 ] == 0xFF );

		i = 0xFF00;
		encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 2 );
		REQUIRE( encoded[ 0 ] == 0x00 );
		REQUIRE( encoded[ 1 ] == 0xFF );
		}

	SECTION( "basicString" )
		{
		string magikarp = "magikarp";
		encoder < string > tEncoder;
		vector< byte > encoded = tEncoder( magikarp );
		// make sure there are 9 characters in the encoding with null terminator
		REQUIRE( encoded.size( ) == magikarp.size( ) + 1 );
		for ( size_t i = 1;  i < encoded.size( );  ++i )
			{
			REQUIRE( encoded[ i ] == magikarp[ i - 1 ] );
			}

		magikarp = "";
		encoded = tEncoder( magikarp );
		REQUIRE( encoded.size( ) == 1 );
		REQUIRE( encoded[ 0 ] == '\0' );
		REQUIRE( encoded.back( ) == '\0' );
		}

	SECTION( "empty types" )
		{
		int i = 0;
		encoder < int > tEncoder;
		dex::vector < dex::byte > encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 1 );
		REQUIRE( encoded[ 0 ] == 0 );
		}
	}

