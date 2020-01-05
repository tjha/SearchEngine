// functional.hpp
//
// A custom implementation of part of some of the function objects defined in std::functional
//
// 2019-11-02: File created, implement hash: jasina

#ifndef DEX_FUNCTIONAL
#define DEX_FUNCTIONAL

#include <cstddef>

namespace dex
	{
	template < class Key >
	struct hash
		{
		private:
			static const unsigned long prime = 16777619;
			static const unsigned long offsetBasis = 2166136261;

			typedef const unsigned char byte;
		public:
			unsigned long operator( )( const Key &key ) const
				{
				// Convert key to be a byte array
				byte *convertedKey = reinterpret_cast< byte * >( &key );
				size_t size = sizeof( key );

				// Compute hash using FNV-1a algorithm
				unsigned long hash = offsetBasis;
				for ( size_t index = 0;  index != size;  ++index )
					hash = ( hash ^ convertedKey[ index ] ) * prime;

				// Constrain our hash to 32 bits
				hash &= 0xFFFFFFFF;

				return hash;
				}
		};
	}

#endif
