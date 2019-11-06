// utf.hpp
// Utilities for dealing with UTF-8 encodings
//
// 2019-11-03: File created, functions for converting between 32 bit numbers and UTF encodings: jasina, lougheem

#ifndef DEX_UTF
#define DEX_UTF

#include "exception.hpp"
#include "typeTraits.hpp"
#include "vector.hpp"

namespace dex
	{
	namespace utf
		{
		unsigned long utfToLong( const unsigned char *encoding )
			{
			// How many bits of the first byte we need to care about
			unsigned count = 8;
			// Value to return
			unsigned long decodedValue;

			// Handle the ASCII case
			if ( *encoding < 0x80 )
				return *encoding;

			// Find how many bits of the first byte we need to care about
			while ( ( *encoding >> ( --count ) ) % 2 );

			// Determine the encoded value
			decodedValue = *( encoding++ ) & ( 0xFF >> ( 8 - count ) );
			for ( unsigned bytesToRead = 6 - count;  bytesToRead != 0;  ++encoding, --bytesToRead )
				decodedValue = ( decodedValue << 6 ) + ( *encoding & 0x3F );

			return decodedValue;
			}

		unsigned long utfToLongSafe( const unsigned char *encoding )
			{
			// How many bits of the first byte we need to care about
			unsigned count = 8;
			// Value to return
			unsigned long decodedValue;

			// Check if our encoding starts with 1111111
			if ( *encoding >= 0xFE )
				throw invalidArgumentException( );

			// Handle the ASCII case
			if ( *encoding < 0x80 )
				return *encoding;

			// Find how many bits of the first byte we need to care about
			while ( ( *encoding >> ( --count ) ) % 2 );

			// Check if our encoding starts with 10
			if ( count == 6 )
				throw invalidArgumentException( );

			// Determine the encoded value
			decodedValue = *( encoding++ ) & ( 0xFF >> ( 8 - count ) );
			for ( unsigned bytesToRead = 6 - count;  bytesToRead != 0;  ++encoding, --bytesToRead )
				{
				if ( *encoding >> 6 != 2 )
					throw invalidArgumentException( );
				decodedValue = ( decodedValue << 6 ) + ( *encoding & 0x3F );
				}

			// Check that the encoded sequence is not overly long
			if ( ( count == 5 && decodedValue < 0x80 ) || decodedValue < ( 1 << ( 31 - 5 * count ) ) )
				throw invalidArgumentException( );

			return decodedValue;
			}

		dex::vector < unsigned char > longToUTF( unsigned long number )
			{
			if ( number > 0x7FFFFFFF )
				throw invalidArgumentException( );

			dex::vector < unsigned char > returnVector;
			// Find smallest number of bits to encode this number

			if ( number < 0x80 )
				{
				returnVector.pushBack( number );
				return returnVector;
				}

			// Find how many bytes we need to represent number in UTF (technically, find 1 less than that)
			unsigned numberAdditionalBytes = 0;
			while ( number >> ( 5 * ++numberAdditionalBytes + 6 ) );

			returnVector.reserve( numberAdditionalBytes + 1 );

			// Add the first byte
			returnVector.pushBack( ( 0xFF << ( 7 - numberAdditionalBytes ) ) + ( number >> ( 6 * numberAdditionalBytes ) ) );

			// Add the remaining bytes
			while ( numberAdditionalBytes != 0 )
				returnVector.pushBack( 0x80 + ( ( number >> ( 6 * ( --numberAdditionalBytes ) ) ) & 0x3F ) );

			return returnVector;
			}

		// We'll say a byte is a sentinel if it looks like 11111111
		bool isUTFSentinel( const unsigned char *encoding )
			{
			return *encoding == 0xFF;
			}
		}
	}

#endif
