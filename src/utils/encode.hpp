// storage.hpp
// Encode and Decode objects used in crawler
//a
// 2019-11-20: File created and encoding integers: jhirsh

#ifndef DEX_ENCODE
#define DEX_ENCODE

#include "exception.hpp"
#include "vector.hpp"
#include "basicString.hpp"
#include "../src/spinarak/robots.hpp"

namespace dex
	{
	// global definition of byte should go in a definitions
	typedef unsigned char byte;

	namespace crawler
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

					if ( !advance )
						{
						return encodedData.cbegin( );
						}

					return it;
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

					if ( !advance )
						{
						return encodedString.cbegin( );
						}

					return it;
					}
			};

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
					//std::cout << "decoded : " << +decodedValue;
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
					std::cout << std::endl;

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return decodedData;
					}
			};
		}
	}

#endif

