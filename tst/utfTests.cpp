// utfTests.cpp
// Tests for utf
//
// 2019-11-10: Write and pass tests for encoding and decoding various data types: jasina, lougheem
// 2019-11-03: File created: jasina, lougheem

#include "catch.hpp"
#include "../src/utils/exception.hpp"
#include "../src/utils/unorderedMap.hpp"
#include "../src/utils/unorderedSet.hpp"
#include "../src/utils/utf.hpp"
#include "../src/utils/vector.hpp"

using namespace dex;
using namespace dex::utf;

TEST_CASE( "decode", "[utf]" )
	{
	unsigned char array[ 6 ];
	

	SECTION( "unsigned long" )
		{
		decoder < unsigned long > unsignedLongDecoder;
		SECTION( "one byte" )
			{
			array[ 0 ] = 0b00000000;
			REQUIRE( unsignedLongDecoder( array ) == 0b00000000 );

			array[ 0 ] = 0b00111010;
			REQUIRE( unsignedLongDecoder( array ) == 0b00111010 );

			array[ 0 ] = 0b01111111;
			REQUIRE( unsignedLongDecoder( array ) == 0b01111111 );
			}

		SECTION( "two bytes" )
			{
			array[ 0 ] = 0b11000010;
			array[ 1 ] = 0b10000000;
			REQUIRE( unsignedLongDecoder( array ) == 0b00010000000 );

			array[ 0 ] = 0b11001001;
			array[ 1 ] = 0b10100111;
			REQUIRE( unsignedLongDecoder( array ) == 0b01001100111 );

			array[ 0 ] = 0b11011111;
			array[ 1 ] = 0b10111111;
			REQUIRE( unsignedLongDecoder( array ) == 0b11111111111 );
			}

		SECTION( "longer sequences" )
			{
			array[ 0 ] = 0b11110101;
			array[ 1 ] = 0b10110110;
			array[ 2 ] = 0b10001101;
			array[ 3 ] = 0b10110111;
			REQUIRE( unsignedLongDecoder( array ) == 0b0101110110001101110111 );

			array[ 0 ] = 0b11111100;
			array[ 1 ] = 0b10110110;
			array[ 2 ] = 0b10001101;
			array[ 3 ] = 0b10110111;
			array[ 4 ] = 0b10001011;
			array[ 5 ] = 0b10111100;
			REQUIRE( unsignedLongDecoder( array ) == 0b0110110001101110111001011111100 );

			array[ 0 ] = 0b11111101;
			array[ 1 ] = 0b10111111;
			array[ 2 ] = 0b10111111;
			array[ 3 ] = 0b10111111;
			array[ 4 ] = 0b10111111;
			array[ 5 ] = 0b10111111;
			REQUIRE( unsignedLongDecoder( array ) == 0b1111111111111111111111111111111 );
			}
		}
	}

TEST_CASE( "decode safe", "[utf]" )
	{
	unsigned char array[ 6 ];

	SECTION( "one byte" )
		{
		array[ 0 ] = 0b00000000;
		REQUIRE( decodeSafe( array ) == 0b00000000 );

		array[ 0 ] = 0b00111010;
		REQUIRE( decodeSafe( array ) == 0b00111010 );

		array[ 0 ] = 0b01111111;
		REQUIRE( decodeSafe( array ) == 0b01111111 );
		}

	SECTION( "two bytes" )
		{
		array[ 0 ] = 0b11000010;
		array[ 1 ] = 0b10000000;
		REQUIRE( decodeSafe( array ) == 0b00010000000 );

		array[ 0 ] = 0b11001001;
		array[ 1 ] = 0b10100111;
		REQUIRE( decodeSafe( array ) == 0b01001100111 );

		array[ 0 ] = 0b11011111;
		array[ 1 ] = 0b10111111;
		REQUIRE( decodeSafe( array ) == 0b11111111111 );
		}

	SECTION( "longer sequences" )
		{
		array[ 0 ] = 0b11110101;
		array[ 1 ] = 0b10110110;
		array[ 2 ] = 0b10001101;
		array[ 3 ] = 0b10110111;
		REQUIRE( decodeSafe( array ) == 0b0101110110001101110111 );

		array[ 0 ] = 0b11111100;
		array[ 1 ] = 0b10110110;
		array[ 2 ] = 0b10001101;
		array[ 3 ] = 0b10110111;
		array[ 4 ] = 0b10001011;
		array[ 5 ] = 0b10111100;
		REQUIRE( decodeSafe( array ) == 0b0110110001101110111001011111100 );

		array[ 0 ] = 0b11111101;
		array[ 1 ] = 0b10111111;
		array[ 2 ] = 0b10111111;
		array[ 3 ] = 0b10111111;
		array[ 4 ] = 0b10111111;
		array[ 5 ] = 0b10111111;
		REQUIRE( decodeSafe( array ) == 0b1111111111111111111111111111111 );
		}

	SECTION( "exceptions" )
		{
		SECTION( "malformed first byte" )
			{
			array[ 0 ] = 0b10000000;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b10100110;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b10111111;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11111110;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11111111;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );
			}

		SECTION( "overlong sequences" )
			{
			array[ 0 ] = 0b11000000;
			array[ 1 ] = 0b10000000;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11000001;
			array[ 1 ] = 0b10111111;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11000010;
			array[ 1 ] = 0b10000000;
			REQUIRE_NOTHROW( decodeSafe( array ) );

			array[ 0 ] = 0b11100000;
			array[ 1 ] = 0b10000000;
			array[ 2 ] = 0b10000000;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11100000;
			array[ 1 ] = 0b10011111;
			array[ 2 ] = 0b10111111;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11100000;
			array[ 1 ] = 0b10100000;
			array[ 2 ] = 0b10000000;
			REQUIRE_NOTHROW( decodeSafe( array ) );

			array[ 0 ] = 0b11111100;
			array[ 1 ] = 0b10000011;
			array[ 2 ] = 0b10111111;
			array[ 3 ] = 0b10111111;
			array[ 4 ] = 0b10111111;
			array[ 5 ] = 0b10111111;
			REQUIRE_THROWS_AS( decodeSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11111100;
			array[ 1 ] = 0b10000100;
			array[ 2 ] = 0b10000000;
			array[ 3 ] = 0b10000000;
			array[ 4 ] = 0b10000000;
			array[ 5 ] = 0b10000000;
			REQUIRE_NOTHROW( decodeSafe( array ) );
			}
		}
	}

TEST_CASE( "encode and decode", "[utf]" )
	{
	SECTION( "unsigned long" )
		{
		unsigned long number;
		decoder < unsigned long > unsignedLongDecoder;
		encoder <unsigned long > unsignedLongEncoder;

		for ( number = 1;  number != 0x80000000;  number <<= 1 )
			{
			REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
			REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number - 1 ).data( ) ) == number - 1 );
			}

		// Just a bunch of random numbers
		number = 0b11110001101011101;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b1011011100101;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b1100010111000101010010101010001;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b1101;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b110101010100011111001;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b111000010111001111001100110;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b111111111111111111111111111110;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b111000;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b11111111111111000111000;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b1010101010101010101010;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		number = 0b101;
		REQUIRE( unsignedLongDecoder( unsignedLongEncoder( number ).data( ) ) == number );
		}

	SECTION( "vector" )
		{
		SECTION( "vector of unsigneds" )
			{
			vector < unsigned > v;
			v.pushBack( 123 );
			v.pushBack( 456 );
			v.pushBack( 789 );

			vector < unsigned char > encodedV = encoder < vector < unsigned > >( )( v );
			REQUIRE( v == decoder < vector < unsigned > >( )( encodedV.data( ) ) );
			}

		SECTION( "vector of vectors of unsigneds" )
			{
			vector < vector < unsigned > > v;
			v.resize( 3 );
			v[ 0 ].resize( 1 );
			v[ 1 ].resize( 3 );
			v[ 2 ].resize( 5 );
			v[ 0 ][ 0 ] = 13;
			v[ 1 ][ 0 ] = 289;
			v[ 1 ][ 1 ] = 298374;
			v[ 1 ][ 2 ] = 192;
			v[ 2 ][ 0 ] = 983;
			v[ 2 ][ 1 ] = 0;
			v[ 2 ][ 2 ] = 2398;
			v[ 2 ][ 3 ] = 2938792;
			v[ 2 ][ 4 ] = 234;

			vector < unsigned char > encodedV = encoder < vector < vector < unsigned > > >( )( v );
			REQUIRE( v == decoder < vector < vector< unsigned > > >( )( encodedV.data( ) ) );
			}
		}

	SECTION( "string" )
		{
		string s = "I am a string!";
		REQUIRE( s == decoder < string >( )( ( encoder < string >( )( s ) ).data( ) ) );

		s.clear( );
		REQUIRE( s == decoder < string >( )( ( encoder < string >( )( s ) ).data( ) ) );
		}

	SECTION( "unordered map" )
		{
		unorderedMap < string, unsigned > mappy;
		mappy[ "alpha" ] = 100;
		mappy[ "beta" ] = 200;
		mappy[ "gamma" ] = 300;
		mappy[ "delta" ] = 400;

		unorderedMap < string, unsigned > mappyCopy =
				decoder < unorderedMap < string, unsigned > >( )
				( encoder < unorderedMap < string, unsigned > >( )( mappy ).data( ) );

		REQUIRE( mappyCopy.size( ) == 4 );
		REQUIRE( mappyCopy[ "alpha" ] == 100 );
		REQUIRE( mappyCopy[ "beta" ] == 200 );
		REQUIRE( mappyCopy[ "gamma" ] == 300 );
		REQUIRE( mappyCopy[ "delta" ] == 400 );
		}

	SECTION( "unordered set" )
		{
		unorderedSet < string > set;
		set.insert( "alpha" );
		set.insert( "beta" );
		set.insert( "gamma" );
		set.insert( "delta" );

		unorderedSet < string > setCopy =
				decoder < unorderedSet < string > >( )
				( encoder < unorderedSet < string > >( )( set ).data( ) );

		REQUIRE( set.size( ) == 4 );
		REQUIRE( set.count( "alpha" ) );
		REQUIRE( set.count( "beta" ) );
		REQUIRE( set.count( "gamma" ) );
		REQUIRE( set.count( "delta" ) );
		}
	}
