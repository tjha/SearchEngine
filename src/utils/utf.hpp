// utf.hpp
// Utilities for dealing with UTF-8 encodings
// Usage:
//    T data;
//    dex::utf::encoder < T > tEncoder;
//    dex::utf::decoder < T > tDecoder;
//    unsigned char *pt; // In this example, we just assume that this has been allocated and assigned
//
//    // Populate data from an unsigned char *
//    data = tDecoder( pt );
//
//    // store to mmapped file (or just to pt)
//    dex::vector < unsigned char > encodedData = encoder( data );
//    dex::copy( encodedData.cbegin( ), encodedData.cend( ), pt );
//
//   // Additional arugments can be passed into the decoder's and encoder's operator( ) to keep track of where the
//   // encodings end
//
// 2019-11-14: Overload operator( ) to take in an iterator: jasina, lougheem
// 2019-11-10: Reformat file into encoder and decoder classes that take in many data types: jasina, lougheem
// 2019-11-03: File created, functions for converting between 32 bit numbers and UTF encodings: jasina, lougheem

#ifndef DEX_UTF
#define DEX_UTF

#include <cstddef>
#include "algorithm.hpp"
#include "basicString.hpp"
#include "exception.hpp"
#include "utility.hpp"
#include "unorderedMap.hpp"
#include "unorderedSet.hpp"
#include "vector.hpp"

namespace dex
	{
	namespace utf
		{
		template < class T >
		class encoder
			{
			public:
				template < class InputIt >
				InputIt operator( )( T number, InputIt it ) const
					{
					if ( number > 0x7FFFFFFF )
						throw invalidArgumentException( );

					if ( number < 0x80 )
						{
						*it = number;
						return ++it;
						}

					// Find how many bytes we need to represent number in UTF (technically, find 1 less than that)
					unsigned numberAdditionalBytes = 0;
					while ( number >> ( 5 * ++numberAdditionalBytes + 6 ) );

					// Add the first byte
					*it = ( 0xFF << ( 7 - numberAdditionalBytes ) ) + ( number >> ( 6 * numberAdditionalBytes ) );
					++it;

					// Add the remaining bytes
					while ( numberAdditionalBytes != 0 )
						{
						*it = 0x80 + ( ( number >> ( 6 * ( --numberAdditionalBytes ) ) ) & 0x3F );
						++it;
						}

					return it;
					}

				dex::vector < unsigned char > operator( )( T number ) const
					{
					if ( number > 0x7FFFFFFF )
						throw invalidArgumentException( );

					dex::vector < unsigned char > returnVector;

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
					returnVector.pushBack( ( 0xFF << ( 7 - numberAdditionalBytes ) ) +
							( number >> ( 6 * numberAdditionalBytes ) ) );

					// Add the remaining bytes
					while ( numberAdditionalBytes != 0 )
						returnVector.pushBack( 0x80 + ( ( number >> ( 6 * ( --numberAdditionalBytes ) ) ) & 0x3F ) );

					return returnVector;
					}
			};

		template < class T1, class T2 >
		class encoder < dex::pair < T1, T2 > >
			{
			public:
				template < class InputIt >
				InputIt operator( )( const dex::pair < T1, T2 > &data, InputIt it ) const
					{
					dex::vector < unsigned char > encodedFirst = encoder < T1 >( )( data.first );
					it = dex::copy( encodedFirst.cbegin( ), encodedFirst.cend( ), it );

					dex::vector < unsigned char > encodedSecond = encoder < T2 >( )( data.second );
					it = dex::copy( encodedSecond.cbegin( ), encodedSecond.cend( ), it );

					return it;
					}

				dex::vector < unsigned char > operator( )( const dex::pair < T1, T2 > &data ) const
					{
					dex::vector < unsigned char > encodedData = encoder < T1 >( )( data.first );
					dex::vector < unsigned char > encodedSecond = encoder < T2 >( )( data.second );
					encodedData.insert( encodedData.cend( ), encodedSecond.cbegin( ), encodedSecond.cend( ) );
					return encodedData;
					}
			};

		template < class T >
		class encoder < dex::vector < T > >
			{
			public:
				template < class InputIt >
				InputIt operator( )( const dex::vector < T > &data, InputIt it ) const
					{
					encoder < T > TEncoder;
					it = encoder< size_t >( )( data.size( ), it );
					for ( typename dex::vector < T >::constIterator dataIt = data.cbegin( );  dataIt != data.cend( );
							++dataIt )
						it = TEncoder( *dataIt, it );
					return it;
					}

				dex::vector < unsigned char > operator( )( const dex::vector < T > &data ) const
					{
					encoder < T > TEncoder;
					dex::vector < unsigned char > encodedData = encoder< size_t >( )( data.size( ) );
					for ( typename dex::vector < T >::constIterator it = data.cbegin( );  it != data.cend( );  ++it )
						{
						dex::vector < unsigned char > encodedDatum = TEncoder( *it );
						encodedData.insert( encodedData.cend( ), encodedDatum.cbegin( ), encodedDatum.cend( ) );
						}
					return encodedData;
					}
			};

		template < class T >
		class encoder < dex::basicString < T > >
			{
			public:
				template < class InputIt >
				InputIt operator( )( const dex::basicString < T > &data, InputIt it ) const
					{
					encoder < T > TEncoder;
					it = encoder< size_t >( )( data.size( ), it );
					for ( typename dex::basicString < T >::constIterator dataIt = data.cbegin( );  dataIt != data.cend( );
							++dataIt )
						it = TEncoder( *dataIt, it );
					return it;
					}

				dex::vector < unsigned char > operator( )( const dex::basicString < T > &data ) const
					{
					encoder < T > TEncoder;
					dex::vector < unsigned char > encodedData = encoder< size_t >( )( data.size( ) );
					for ( typename dex::basicString < T >::constIterator it = data.cbegin( );  it != data.cend( );  ++it )
						{
						dex::vector < unsigned char > encodedDatum = TEncoder( *it );
						encodedData.insert( encodedData.cend( ), encodedDatum.cbegin( ), encodedDatum.cend( ) );
						}
					return encodedData;
					}
			};

		template < class Key, class Value >
		class encoder < dex::unorderedMap < Key, Value > >
			{
			public:
				template < class InputIt >
				InputIt operator( )( const dex::unorderedMap < Key, Value > &data, InputIt it ) const
					{
					encoder < dex::pair < Key, Value > > KeyValueEncoder;
					it = encoder< size_t >( )( data.size( ), it );
					for ( typename dex::unorderedMap < Key, Value >::constIterator dataIt = data.cbegin( );
							dataIt != data.cend( );  ++dataIt )
						it = KeyValueEncoder( *dataIt, it );
					return it;
					}

				dex::vector < unsigned char > operator( )( const dex::unorderedMap < Key, Value > &data ) const
					{
					encoder < dex::pair < Key, Value > > KeyValueEncoder;
					dex::vector < unsigned char > encodedData = encoder< size_t >( )( data.size( ) );
					for ( typename dex::unorderedMap < Key, Value >::constIterator it = data.cbegin( );
							it != data.cend( );  ++it )
						{
						dex::vector < unsigned char > encodedDatum = KeyValueEncoder( *it );
						encodedData.insert( encodedData.cend( ), encodedDatum.cbegin( ), encodedDatum.cend( ) );
						}
					return encodedData;
					}
			};

		template < class Key >
		class encoder < dex::unorderedSet < Key > >
			{
			public:
				template < class InputIt >
				InputIt operator( )( const dex::unorderedSet < Key > &data, InputIt it ) const
					{
					encoder < Key > KeyEncoder;
					it = encoder< size_t >( )( data.size( ), it );
					for ( typename dex::unorderedSet < Key >::constIterator dataIt = data.cbegin( );  dataIt != data.cend( );
							++dataIt )
						it = KeyEncoder( *dataIt, it );
					return it;
					}

				dex::vector < unsigned char > operator( )( const dex::unorderedSet < Key > &data ) const
					{
					encoder < Key > KeyEncoder;
					dex::vector < unsigned char > encodedData = encoder< size_t >( )( data.size( ) );
					for ( typename dex::unorderedSet < Key >::constIterator it = data.cbegin( ); it != data.cend( );  ++it )
						{
						dex::vector < unsigned char > encodedDatum = KeyEncoder( *it );
						encodedData.insert( encodedData.cend( ), encodedDatum.cbegin( ), encodedDatum.cend( ) );
						}
					return encodedData;
					}
			};

		template < class InputIt >
		unsigned long decodeSafe( InputIt encoding )
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
			if ( ( count == 5 && decodedValue < 0x80 )
					|| decodedValue < static_cast < unsigned long >( 1 << ( 31 - 5 * count ) ) )
				throw invalidArgumentException( );

			return decodedValue;
			}

		template < class T, class InputIt = unsigned char * >
		class decoder
			{
			public:
				unsigned long operator( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					// How many bits of the first byte we need to care about
					unsigned count = 8;
					// Value to return
					T decodedValue;

					// Handle the ASCII case
					if ( *encoding < 0x80 )
						{
						if ( advancedEncoding )
							{
							*advancedEncoding = encoding;
							++( *advancedEncoding );
							}
						return *encoding;
						}

					// Find how many bits of the first byte we need to care about
					while ( ( *encoding >> ( --count ) ) % 2 );

					// Determine the encoded value
					decodedValue = *( encoding++ ) & ( 0xFF >> ( 8 - count ) );
					for ( unsigned bytesToRead = 6 - count;  bytesToRead != 0;  ++encoding, --bytesToRead )
						decodedValue = ( decodedValue << 6 ) + ( *encoding & 0x3F );

					if ( advancedEncoding )
						*advancedEncoding = encoding;

					return decodedValue;
					}
			};

		template < class T1, class T2, class InputIt >
		class decoder < dex::pair < T1, T2 >, InputIt >
			{
			public:
				dex::pair < T1, T2 > operator( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt *localAdvancedEncoding = &encoding;
					T1 first = decoder < T1, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );
					T2 second = decoder < T2, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return dex::pair < T1, T2 >( first, second );
					}
			};

		template < class T, class InputIt >
		class decoder < dex::vector < T >, InputIt >
			{
			public:
				dex::vector < T > operator( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt *localAdvancedEncoding = &encoding;
					decoder < T, InputIt > TDecoder;
					dex::vector < T > decodedData;
					size_t size = decoder < size_t, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );

					for ( size_t encodingIndex = 0;  encodingIndex != size;  ++encodingIndex )
						decodedData.pushBack( TDecoder( *localAdvancedEncoding, localAdvancedEncoding ) );

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return decodedData;
					}
			};

		template < class T, class InputIt >
		class decoder < dex::basicString < T >, InputIt >
			{
			public:
				dex::basicString < T > operator( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt *localAdvancedEncoding = &encoding;
					decoder < T, InputIt > TDecoder;
					dex::basicString < T > decodedData;
					size_t size = decoder < size_t, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );

					for ( size_t encodingIndex = 0;  encodingIndex != size;  ++encodingIndex )
						decodedData.pushBack( TDecoder( *localAdvancedEncoding, localAdvancedEncoding ) );

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return decodedData;
					}
			};

		template < class Key, class Value, class InputIt >
		class decoder < dex::unorderedMap < Key, Value >, InputIt >
			{
			public:
				dex::unorderedMap < Key, Value > operator( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt *localAdvancedEncoding = &encoding;
					decoder < dex::pair < Key, Value >, InputIt > KeyValueDecoder;
					dex::unorderedMap < Key, Value > decodedData;
					size_t size = decoder < size_t, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );

					for ( size_t encodingIndex = 0;  encodingIndex != size;  ++encodingIndex )
						decodedData.insert( KeyValueDecoder( *localAdvancedEncoding, localAdvancedEncoding ) );

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return decodedData;
					}
			};

		template < class Key, class InputIt >
		class decoder < dex::unorderedSet < Key >, InputIt >
			{
			public:
				dex::unorderedSet < Key > operator( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt *localAdvancedEncoding = &encoding;
					decoder < Key, InputIt > KeyDecoder;
					dex::unorderedSet < Key > decodedData;
					size_t size = decoder < size_t, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );

					for ( size_t encodingIndex = 0;  encodingIndex != size;  ++encodingIndex )
						decodedData.insert( KeyDecoder( *localAdvancedEncoding, localAdvancedEncoding ) );

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return decodedData;
					}
			};

		// We'll say a byte is a sentinel if it looks like 11111111
		template < class InputIt >
		bool isUTFSentinel( InputIt encoding )
			{
			return *encoding == 0xFF;
			}
		}
	}

#endif
