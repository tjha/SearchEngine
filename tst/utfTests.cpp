// utfTests.cpp
// Tests for utf
//
// 2019-11-03: File created: jasina, lougheem

#include "catch.hpp"
#include "../src/utils/exception.hpp"
#include "../src/utils/utf.hpp"
#include "../src/utils/vector.hpp"

using namespace dex;
using namespace dex::utf;

TEST_CASE( "decode", "[utf]" )
	{
	unsigned char array[ 6 ];

	SECTION( "one byte" )
		{
		array[ 0 ] = 0b00000000;
		REQUIRE( utfToLong( array ) == 0b00000000 );

		array[ 0 ] = 0b00111010;
		REQUIRE( utfToLong( array ) == 0b00111010 );

		array[ 0 ] = 0b01111111;
		REQUIRE( utfToLong( array ) == 0b01111111 );
		}

	SECTION( "two bytes" )
		{
		array[ 0 ] = 0b11000010;
		array[ 1 ] = 0b10000000;
		REQUIRE( utfToLong( array ) == 0b00010000000 );

		array[ 0 ] = 0b11001001;
		array[ 1 ] = 0b10100111;
		REQUIRE( utfToLong( array ) == 0b01001100111 );

		array[ 0 ] = 0b11011111;
		array[ 1 ] = 0b10111111;
		REQUIRE( utfToLong( array ) == 0b11111111111 );
		}

	SECTION( "longer sequences" )
		{
		array[ 0 ] = 0b11110101;
		array[ 1 ] = 0b10110110;
		array[ 2 ] = 0b10001101;
		array[ 3 ] = 0b10110111;
		REQUIRE( utfToLong( array ) == 0b0101110110001101110111 );

		array[ 0 ] = 0b11111100;
		array[ 1 ] = 0b10110110;
		array[ 2 ] = 0b10001101;
		array[ 3 ] = 0b10110111;
		array[ 4 ] = 0b10001011;
		array[ 5 ] = 0b10111100;
		REQUIRE( utfToLong( array ) == 0b0110110001101110111001011111100 );

		array[ 0 ] = 0b11111101;
		array[ 1 ] = 0b10111111;
		array[ 2 ] = 0b10111111;
		array[ 3 ] = 0b10111111;
		array[ 4 ] = 0b10111111;
		array[ 5 ] = 0b10111111;
		REQUIRE( utfToLong( array ) == 0b1111111111111111111111111111111 );
		}
	}

TEST_CASE( "decode safe", "[utf]" )
	{
	unsigned char array[ 6 ];

	SECTION( "one byte" )
		{
		array[ 0 ] = 0b00000000;
		REQUIRE( utfToLongSafe( array ) == 0b00000000 );

		array[ 0 ] = 0b00111010;
		REQUIRE( utfToLongSafe( array ) == 0b00111010 );

		array[ 0 ] = 0b01111111;
		REQUIRE( utfToLongSafe( array ) == 0b01111111 );
		}

	SECTION( "two bytes" )
		{
		array[ 0 ] = 0b11000010;
		array[ 1 ] = 0b10000000;
		REQUIRE( utfToLongSafe( array ) == 0b00010000000 );

		array[ 0 ] = 0b11001001;
		array[ 1 ] = 0b10100111;
		REQUIRE( utfToLongSafe( array ) == 0b01001100111 );

		array[ 0 ] = 0b11011111;
		array[ 1 ] = 0b10111111;
		REQUIRE( utfToLongSafe( array ) == 0b11111111111 );
		}

	SECTION( "longer sequences" )
		{
		array[ 0 ] = 0b11110101;
		array[ 1 ] = 0b10110110;
		array[ 2 ] = 0b10001101;
		array[ 3 ] = 0b10110111;
		REQUIRE( utfToLongSafe( array ) == 0b0101110110001101110111 );

		array[ 0 ] = 0b11111100;
		array[ 1 ] = 0b10110110;
		array[ 2 ] = 0b10001101;
		array[ 3 ] = 0b10110111;
		array[ 4 ] = 0b10001011;
		array[ 5 ] = 0b10111100;
		REQUIRE( utfToLongSafe( array ) == 0b0110110001101110111001011111100 );

		array[ 0 ] = 0b11111101;
		array[ 1 ] = 0b10111111;
		array[ 2 ] = 0b10111111;
		array[ 3 ] = 0b10111111;
		array[ 4 ] = 0b10111111;
		array[ 5 ] = 0b10111111;
		REQUIRE( utfToLongSafe( array ) == 0b1111111111111111111111111111111 );
		}

	SECTION( "exceptions" )
		{
		SECTION( "malformed first byte" )
			{
			array[ 0 ] = 0b10000000;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b10100110;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b10111111;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11111110;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11111111;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );
			}

		SECTION( "overlong sequences" )
			{
			array[ 0 ] = 0b11000000;
			array[ 1 ] = 0b10000000;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11000001;
			array[ 1 ] = 0b10111111;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11000010;
			array[ 1 ] = 0b10000000;
			REQUIRE_NOTHROW( utfToLongSafe( array ) );

			array[ 0 ] = 0b11100000;
			array[ 1 ] = 0b10000000;
			array[ 2 ] = 0b10000000;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11100000;
			array[ 1 ] = 0b10011111;
			array[ 2 ] = 0b10111111;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11100000;
			array[ 1 ] = 0b10100000;
			array[ 2 ] = 0b10000000;
			REQUIRE_NOTHROW( utfToLongSafe( array ) );

			array[ 0 ] = 0b11111100;
			array[ 1 ] = 0b10000011;
			array[ 2 ] = 0b10111111;
			array[ 3 ] = 0b10111111;
			array[ 4 ] = 0b10111111;
			array[ 5 ] = 0b10111111;
			REQUIRE_THROWS_AS( utfToLongSafe( array ), invalidArgumentException );

			array[ 0 ] = 0b11111100;
			array[ 1 ] = 0b10000100;
			array[ 2 ] = 0b10000000;
			array[ 3 ] = 0b10000000;
			array[ 4 ] = 0b10000000;
			array[ 5 ] = 0b10000000;
			REQUIRE_NOTHROW( utfToLongSafe( array ) );
			}
		}
	}

TEST_CASE( "encode and decode", "[utf]" )
	{
	unsigned long number;

	for ( number = 1;  number != 0x80000000;  number <<= 1 )
		{
		REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
		REQUIRE( utfToLong( longToUTF( number - 1 ).data( ) ) == number - 1 );
		}

	// Just a bunch of random numbers
	number = 0b11110001101011101;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b1011011100101;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b1100010111000101010010101010001;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b1101;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b110101010100011111001;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b111000010111001111001100110;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b111111111111111111111111111110;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b111000;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b11111111111111000111000;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b1010101010101010101010;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	number = 0b101;
	REQUIRE( utfToLong( longToUTF( number ).data( ) ) == number );
	}
