// storageTests.cpp
// Tests the encoding and decoding of the defined types for crawler
//
// 2019-11-20: File created and testing integer encode: jhirsh

#include "catch.hpp"
#include "exception.hpp"
#include "unorderedMap.hpp"
#include "vector.hpp"
#include "storage.hpp"

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

	SECTION( "empty types" )
		{
		int i = 0;
		encoder < int > tEncoder;
		dex::vector < dex::byte > encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 1 );
		REQUIRE( encoded[ 0 ] == 0 );
		}
	}

