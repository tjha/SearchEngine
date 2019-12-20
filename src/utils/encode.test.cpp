// storageTests.cpp
// Tests the encoding and decoding of the defined types for crawler
//
// 2019-12-01: Vector encode/decode tests: jhirsh
// 2019-11-20: File created and testing integer encode: jhirsh

#include <iostream>
#include "catch.hpp"
#include "crawler/url.hpp"
#include "utils/basicString.hpp"
#include "utils/encode.hpp"
#include "utils/exception.hpp"
#include "utils/unorderedMap.hpp"
#include "utils/vector.hpp"

using namespace dex;
using namespace dex::encode;

TEST_CASE( "encode", "[types]" )
	{
	SECTION( "int" )
		{
		int i = 4;
		encoder < int > tEncoder;
		dex::vector < dex::byte > encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 4 );
		REQUIRE( encoded[ 3 ] == 4 );
		REQUIRE( encoded[ 1 ] == 0 );
		REQUIRE( encoded[ 2 ] == 0 );
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
		encoded = tEncoder( i );
		REQUIRE( i == IntegerDecoder( tEncoder( i ).data( ) ) );
		i = 0xFF;
		for ( int j = 0;  j < 4; ++j )
			{
			REQUIRE( i == IntegerDecoder( tEncoder( i ).data( ) ) );
			i = i << 8;
			}
		}

	/*SECTION( "iterators" )
		{
		encoder < int > IntegerEncoder;
		decoder < int > IntegerDecoder;
		int i = 0xFF;
		auto result = IntegerEncoder( i );
		std::cout << *result;
		//REQUIRE ( result == i ); 
		}*/
	// string encoding = ( int size ) + ( string )
	SECTION( "basicString" )
		{
		string magikarp = "magikarp";
		encoder < string > StringEncoder;
		decoder < string > StringDecoder;
		vector< byte > encoded = StringEncoder( magikarp );
		vector< byte > size;

		REQUIRE( encoded.size( ) == sizeof( int ) + magikarp.size( ) );
		for ( size_t i = sizeof( int );  i < encoded.size( );  ++i )
			{
			REQUIRE( encoded[ i ] == magikarp[ i - 4 ] );
			}

		magikarp = "";
		encoded = StringEncoder( magikarp );
		REQUIRE( encoded.size( ) == sizeof( int ) + magikarp.size( ) );
		REQUIRE( encoded[ 0 ] == 0 );
		magikarp = "hello";
		REQUIRE( magikarp == StringDecoder ( ( StringEncoder ( magikarp ) ).data( ) ) );
		magikarp = "hello";
		REQUIRE( magikarp == StringDecoder ( ( StringEncoder ( magikarp ) ).data( ) ) );
		}
	
	SECTION( "url" )
		{
		Url amazon("https://www.amazon.com/");
		Url apple("https://www.apple.com/");
		encoder < Url > UrlEncoder;
		decoder < Url > UrlDecoder;
		REQUIRE( 4 + 23 == UrlEncoder( amazon ).size( ) );
		vector< unsigned char > encoded = UrlEncoder( amazon );
		string decoded = decoder< string >( )(encoded.data( ));
		REQUIRE( amazon.completeUrl( ) == Url( decoded.cStr( ) ).completeUrl( ) );
		Url a = UrlDecoder( UrlEncoder ( amazon ).data( ) );
		REQUIRE( amazon.completeUrl( ) == UrlDecoder( UrlEncoder ( amazon ).data( ) ).completeUrl( ) );
		REQUIRE( apple.completeUrl( ) == UrlDecoder( UrlEncoder ( apple ).data( ) ).completeUrl( ) );
		vector < Url > urls;
		urls.pushBack( amazon );
		urls.pushBack( apple );
		encoder < vector < Url > > VectorUrlEncoder;
		decoder < vector < Url > > VectorUrlDecoder;
		vector < Url > encodeDecodeVector = VectorUrlDecoder( VectorUrlEncoder ( urls ).data( ) );
		for ( size_t i = 0;  i < urls.size( );  ++i )
			{
			REQUIRE( urls[ i ].completeUrl( ) == encodeDecodeVector[ i ].completeUrl( ) );
			}
		}
	
	SECTION( "vector" )
		{
		string a = "abclkajsdf;lkjas;dlkfj";
		string b = "curiousgeorgeandtheyellowbanana";
		string c = "pokemonassemble";
		vector < string > vec;
		vec.pushBack( a );
		vec.pushBack( b );
		vec.pushBack( c );
		encoder < vector < string > > VectorStringEncoder;
		decoder < vector < string > > VectorStringDecoder;
		vector < byte > encoded = VectorStringEncoder( vec );
		REQUIRE( encoded.size( ) == sizeof( int ) * 4 + a.size( ) + b.size( ) + c.size( ) );
		REQUIRE( vec == VectorStringDecoder( VectorStringEncoder( vec ).data( ) ) );

		int one = 123;
		int two = 666;
		int three = 0xFFFFFF;
		vector < int > integers;
		integers.pushBack( one );
		integers.pushBack( two );
		integers.pushBack( three );
		encoder < vector < int > > VectorIntegerEncoder;
		decoder < vector < int > > VectorIntegerDecoder;
		vector < byte > intsEncoded = VectorIntegerEncoder( integers );
		REQUIRE( intsEncoded.size( ) == sizeof( int ) * ( 1 + 3 ) );
		REQUIRE( integers == VectorIntegerDecoder( VectorIntegerEncoder( integers ).data( ) ) );
		}

	/*
	SECTION( "unorderedSet" )
		{
		unorderedSet < dex::Url > links;
		links.insert( Url("https://www.amazon.com/") );
		links.insert( Url("https://www.apple.com/") );
		links.insert( Url("https://www.nytimes.com/") );
		encoder < unorderedSet < Url > > SetEncoder;
		decoder < unorderedSet < Url > > SetDecoder;
		unorderedSet< Url > decoded = SetDecoder( SetEncoder( links ).data( ) );
		auto itDec = decoded.cbegin( );
		for ( auto it = links.begin( );  it != links.cend( );  ++it )
			{
			REQUIRE( it->completeUrl( ).size( ) == itDec->completeUrl( ).size( ) );
			REQUIRE( it->completeUrl( ) == itDec->completeUrl( ) );
			++itDec;
			}
		}*/

	SECTION( "empty types" )
		{
		int i = 0;
		encoder < int > tEncoder;
		dex::vector < dex::byte > encoded = tEncoder ( i );
		REQUIRE( encoded.size( ) == 4 );
		REQUIRE( encoded[ 0 ] == 0 );
		}
	}

