// storage.hpp
// Encode and Decode objects used in crawler
//
// 2019-11-20: File created and encoding integers: jhirsh

#ifndef DEX_ENCODE
#define DEX_ENCODE

#include <cstddef>
#include "exception.hpp"
#include "vector.hpp"
#include <iostream>
using namespace std;

namespace dex
	{
	// global definition of byte should go in a definitions
	typedef unsigned char byte;

	namespace crawler
		{
		template < class T >
		class encoder
			{
			public:
				/*template < class InputIt >
				InputIt operator( )( T number, InputIt it ) const
					{
					size_t 
					}*/

				dex::vector < byte > operator ( )( T number ) const
					{
					dex::vector < byte > encodedVector;

					// Calculate how many bytes is needed to represent number
					unsigned numberAdditionalBytes = 0;
					while ( number >> ( 8 * ++numberAdditionalBytes ) );

					encodedVector.reserve( numberAdditionalBytes + 1 );

					// push back the the bytes in LITTLE ENDIAN order
					while ( numberAdditionalBytes != 0 )
						{
						encodedVector.pushBack( number & 0xFF );
						number = number >> 8;
						--numberAdditionalBytes;
						}

					return encodedVector;
					}
			};
		}
	}

#endif

