// storage.hpp
// Encode and Decode objects used in crawler
//a
// 2019-12-01: supports vectors: jhirsh
// 2019-11-sometime: encodes and decodes strings and ints: jhirsh
// 2019-11-20: File created and encoding integers: jhirsh

#ifndef DEX_ENCODE
#define DEX_ENCODE

#include "crawler/robots.hpp"
#include "crawler/url.hpp"
#include "utils/exception.hpp"
#include "utils/vector.hpp"
#include "utils/basicString.hpp"

namespace dex
	{
	// global definition of byte should go in a definitions
	typedef unsigned char byte;

	namespace encode
		{

		// Number types are stored as there maximum amount of bytes used
		template < class T >
		class encoder
			{
			public:
				dex::vector < unsigned char > operator ( )( T number )
					{
					dex::vector < unsigned char > encodedData;

					size_t bytes = sizeof( T );
					//encodedData.reserve( bytes );
					// push back bytes in LITTLE ENDIAN order
					for ( size_t i = 0;  i < bytes;  ++i )
						{
						unsigned char datum = static_cast < unsigned char > (( number >> ( 8 * ( bytes - i - 1 ) ) ) & 0x000000FF );
						encodedData.pushBack( datum );
						}

					return encodedData;
					}

				template < class InputIt >
				InputIt operator( )( T number, InputIt it = nullptr ) const
					{
					dex::vector < unsigned char > encodedData;
					bool advance = true;
					if ( it == nullptr )
						{
						advance = false;
						}

					size_t bytes = sizeof( T );
					// push back bytes in LITTLE ENDIAN order
					for ( size_t i = 0;  i < bytes;  ++i )
						{
						encodedData.pushBack( number & 0xFF );
						if ( it != nullptr )
							{
							++it;
							}
						number = number >> 8;
						}

					if ( advance )
						{
						return it;
						}

					return encodedData.cbegin( );
					}
			};


		// basicString Encoding
		// ( size of string as int ) + ( string )
		template < class T >
		class encoder < dex::basicString < T > >
			{
			public:
				dex::vector< unsigned char > operator( )( const dex::basicString < T > & data ) const
					{
					encoder < char > TEncoder;
					encoder < int > IntegerEncoder;
					int size = static_cast < int > ( data.size( ) );
					dex::vector < unsigned char > encodedVector = IntegerEncoder( size );
					for ( typename dex::basicString < T >::constIterator it = data.cbegin( );  it != data.cend( );  ++it )
						{
						dex::vector < unsigned char > encodedDatum = TEncoder( *it );
						encodedVector.insert( encodedVector.cend( ), encodedDatum.cbegin( ), encodedDatum.cend( ) );
						}
					return encodedVector;
					}

				template < class InputIt >
				InputIt operator( )( const dex::basicString < T > &data, InputIt it = nullptr ) const
					{
					encoder < char > TEncoder;
					dex::vector < unsigned char > encodedString = encoder < int >( )( data.size( ) );

					bool advance = true;
					if ( it == nullptr )
						{
						advance = false;
						}
					it = encodedString.cbegin( );

					for ( typename dex::basicString < T >::constIterator dataIt = data.cbegin( );  dataIt != data.cend( );  ++dataIt )
						{
						it = TEncoder( *dataIt, it );
						}

					if ( advance )
						{
						return it;
						}

					return encodedString.cbegin( );
					}
			};

		// encode a url object to just encoded the complete URL
		template < >
		class encoder < dex::Url >
			{
			public:
				dex::vector< unsigned char > operator( )( const dex::Url & data ) const
					{
					return encoder < dex::basicString < char > >( )( data.completeUrl( ) );
					}

				template < class InputIt >
				InputIt operator( )( const Url &data, InputIt it = nullptr ) const
					{
					encoder < dex::basicString < char > > TEncoder;
					dex::vector < unsigned char > encodedUrl;

					// if iterator is specificied, advance by return last iterator
					bool advance = ( it ) ? true : false;

					it = encodedUrl.begin( );

					it = TEncoder( data.completeUrl( ) );

					if ( advance )
						{
						return it;
						}

					return encodedUrl.cbegin( );
					}
			};

		// Encode a vector of type T
		template < class T >
		class encoder < dex::vector < T > >
			{
			public:
				dex::vector< unsigned char > operator( )( const dex::vector < T > & data ) const
					{
					encoder < T > TEncoder;
					encoder < int > IntegerEncoder;

					int size = static_cast < int > ( data.size( ) );
					dex::vector < unsigned char > encodedVector = IntegerEncoder( size );
					for ( auto it = data.cbegin( );  it != data.cend( );  ++it )
						{
						dex::vector < unsigned char > encodedDatum = TEncoder( *it );
						encodedVector.insert( encodedVector.cend( ), encodedDatum.cbegin( ), encodedDatum.cend( ) );
						}
					return encodedVector;
					}

				template < class InputIt >
				InputIt operator( )( const dex::vector< T > &data, InputIt it = nullptr ) const
					{
					encoder < T > TEncoder;
					dex::vector < unsigned char > encodedVector = encoder < int >( )( data.size( ) );

					bool advance = ( it ) ? true : false;
					it = encodedVector.cbegin( );

					for ( auto dataIt = data.cbegin( );  dataIt != data.cend( );  ++dataIt )
						{
						it = StringEncoder( *dataIt, it );
						}

					if ( advance )
						{
						return it;
						}

					return encodedVector.cbegin( );
					}
			};

		// Encode a vector of type T
		/*
		template < class T >
		class encoder < dex::unorderedSet < T > >
			{
			public:
				dex::vector< unsigned char > operator( )( const dex::unorderedSet < T > & data ) const
					{
					encoder < T > TEncoder;
					encoder < int > IntegerEncoder;

					dex::vector < unsigned char > encodedVector = IntegerEncoder( data.bucketCount( ) );
					for ( auto it = data.cbegin( );  it != data.cend( );  ++it )
						{
						dex::vector < unsigned char > encodedDatum = TEncoder( *it );
						encodedVector.insert( encodedVector.cend( ), encodedDatum.cbegin( ), encodedDatum.cend( ) );
						}
					return encodedVector;
					}

				template < class InputIt >
				InputIt operator( )( const dex::unorderedSet< T > &data, InputIt it = nullptr ) const
					{
					encoder < T > TEncoder;
					dex::vector < unsigned char > encodedVector = encoder < int >( )( data.bucketCount( ) );

					bool advance = ( it ) ? true : false;
					it = encodedVector.cbegin( );

					for ( auto dataIt = data.cbegin( );  dataIt != data.cend( );  ++dataIt )
						{
						it = StringEncoder( *dataIt, it );
						}

					if ( advance )
						{
						return it;
						}

					return encodedVector.cbegin( );
					}
			};*/

		template < >
		class encoder < dex::RobotTxt >
			{
			public:
				/*template < class InputIt >
				InputIt operator( )( const dex::RobotTxt &robot, InputIt it ) const
					{
					encoder < dex::string > StringEncoder;
					encoder < time_t > TimeEncoder;
					encoder < int > IntegerEncoder;
					encoder < dex::unorderedSet < dex::string > > PathEncoder;
					encoder < Key > KeyEncoder;
					it = encoder< size_t >( )( data.size( ), it );
					return it;
					}*/

				dex::vector < unsigned char > operator( )( const dex::RobotTxt &robot ) const
					{
					// All encoders RobotTxt needs
					encoder < dex::basicString < char > > StringEncoder;
					encoder < int > IntegerEncoder;
					//encoder < dex::unorderedSet < dex::string > > PathEncoder;

					// Data Vector for encoding
					dex::vector < unsigned char > encodedData;

					// Domain
					vector < unsigned char > domainEncoding = StringEncoder( robot.getDomain( ) );
					encodedData.insert( encodedData.cend( ), domainEncoding.cbegin( ), domainEncoding.cend( ) );

					// Crawl-Delay
					vector < unsigned char > crawlEncoding = IntegerEncoder( robot.getDelay( ) );
					encodedData.insert( encodedData.cend( ), crawlEncoding.cbegin( ), crawlEncoding.cend( ) );

					// Last-Visit-Time
					vector < unsigned char > lastVisitEncoding = IntegerEncoder(
							( long int ) robot.getLastVisit( ) );
					encodedData.insert( encodedData.cend( ), lastVisitEncoding.cbegin( ), lastVisitEncoding.cend( ) );

					// Allowed-Visit-Time
					vector < unsigned char > allowedVisitEncoding = IntegerEncoder(
							( long int ) robot.getAllowedVisitTime( ) );
					encodedData.insert( encodedData.cend( ), allowedVisitEncoding.cbegin( ), allowedVisitEncoding.cend( ) );

					// Expire-Time
					vector < unsigned char > expireTimeEncoding = IntegerEncoder(
							( long int ) robot.getExpireTime( ) );
					encodedData.insert( encodedData.cend( ), expireTimeEncoding.cbegin( ), expireTimeEncoding.cend( ) );

					/*// Allowed-Paths
					vector < unsigned char > allowedPathsEncoding = PathEncoder( robot.getAllowedPaths( ) );
					encodedData.insert( encodedData.cend( ), allowedPathsEncoding.cbegin( ), allowedPathsEncoding.cend( ) );

					// Disallowed-Paths
					vector < unsigned char > disallowedPathsEncoding = PathEncoder( robot.getDisallowedPaths( ) );
					encodedData.insert( encodedData.cend( ), disallowedPathsEncoding.cbegin( ), disallowedPathsEncoding.cend( ) );*/

					return encodedData;
					}
			};

		template < class T, class InputIt = unsigned char * >
		class decoder
			{
			public:
				T operator( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					T decodedValue = 0;
					size_t bytes = sizeof( T );
					// push back bytes in LITTLE ENDIAN order
					decodedValue += *encoding;
					++encoding;
					for ( size_t i = 1;  i < bytes;  ++i )
						{
						decodedValue = decodedValue << 8;
						decodedValue += *encoding;
						++encoding;
						}
					if ( advancedEncoding )
						{
						*advancedEncoding = encoding;
						}
					return +decodedValue;
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

					size_t size = decoder < int, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );
					for ( size_t encodingIndex = 0;  encodingIndex != size;  ++encodingIndex )
						{
						decodedData.pushBack( TDecoder( * localAdvancedEncoding, localAdvancedEncoding ) );
						}

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return decodedData;
					}
			};

		template < class InputIt >
		class decoder < dex::Url, InputIt >
			{
			public:
				dex::Url operator( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt *localAdvancedEncoding = &encoding;
					dex::Url decodedData( decoder < dex::basicString < char >, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding ) );
					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return decodedData;
					}
			};

		template < class T, class InputIt >
		class decoder < vector < T >, InputIt >
			{
			public:
				dex::vector < T > operator ( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt * localAdvancedEncoding = &encoding;
					decoder < T, InputIt > TDecoder;
					dex::vector < T > decodedData;

					size_t size = decoder < int, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );
					for ( size_t encodingIndex = 0;  encodingIndex != size;  ++encodingIndex )
						{
						decodedData.pushBack( TDecoder( *localAdvancedEncoding, localAdvancedEncoding ) );
						}

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return decodedData;
					}
			};

		/*
		template < class T, class InputIt >
		class decoder < unorderedSet < T >, InputIt >
			{
			public:
				dex::unorderedSet < T > operator ( )( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt * localAdvancedEncoding = &encoding;
					decoder < T, InputIt > TDecoder;
					size_t size = decoder < int, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );

					dex::unorderedSet < T > decodedData( size );
					for ( size_t encodingIndex = 0;  encodingIndex != size;  ++encodingIndex )
						{
						decodedData.insert( TDecoder( *localAdvancedEncoding, localAdvancedEncoding ) );
						}

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return decodedData;
					}
			};*/
		}
	}

#endif

