//redirectCache.hpp
// 2019-11-21: fix pair syntax: combsc
// 2019-11-18: Init Commit: combsc
#ifndef REDIRECT_CACHE_HPP
#define REDIRECT_CACHE_HPP

#include <time.h>
#include "../utils/unorderedMap.hpp"
#include "../utils/vector.hpp"
#include "../utils/utility.hpp"
#include "../spinarak/url.hpp"

namespace dex
	{
	class redirectCache
		{
		private:
			dex::unorderedMap < dex::Url, dex::pair < dex::Url, time_t > > endpoints;
			static const time_t defaultExpireTime = 60 * 60 * 24;
			size_t maxSize;


		public:
			int removeUrl( const dex::Url &url )
				{
				return endpoints.erase( url );
				}

			dex::Url getEndpoint( const dex::Url &url )
				{
				dex::pair < dex::Url, time_t > current = pair < dex::Url, time_t > ( url, time( nullptr ) );
				dex::pair < dex::Url, time_t > next = pair < dex::Url, time_t > ( url, time( nullptr ) );
				while ( endpoints.count( current.first ) > 0 )
					{
					next = endpoints[ current.first ];
					if ( time( nullptr ) >= next.second )
						{
						removeUrl( current.first );
						return current.first;
						}
					current = next;
					}
				return current.first;
				}

			// If updating the URL creates a loop, return -1. Otherwise, return 0
			int updateUrl( const dex::Url &key, const dex::Url &value, const time_t expireTime = defaultExpireTime )
				{
				if ( endpoints.bucketCount( ) > 8 * maxSize )
					{
					endpoints.rehash( maxSize * 4 );
					}
				dex::Url check = getEndpoint( value );
				if ( check.completeUrl( ) == key.completeUrl( ) )
					return -1;
				dex::pair < dex::Url, time_t > p = pair < dex::Url, time_t > ( value, time( nullptr ) + expireTime );
				endpoints[ key ] = p;
				return 0;
				}

			void reset( )
				{
				endpoints.clear( );
				}

			size_t size( ) const
				{
				return endpoints.size( );
				}
			size_t capacity( ) const
				{
				return endpoints.bucketCount( );
				}

			redirectCache( size_t max )
				{
				maxSize = max;
				endpoints.rehash( max * 4 );
				}
		};
	}

#endif
