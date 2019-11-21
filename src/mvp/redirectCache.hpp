//redirectCache.hpp
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
			dex::unorderedMap < dex::Url, dex::pair < dex::Url, time_t > > endpoints { 1000 };
			static const time_t defaultExpireTime = 60 * 60 * 24;


		public:
			int removeUrl( const dex::Url &url )
				{
				return endpoints.erase( url );
				}

			dex::Url getEndpoint( const dex::Url &url )
				{
				dex::pair < dex::Url, time_t > current = pair( url, time( nullptr ) );
				dex::pair < dex::Url, time_t > next = pair( url, time( nullptr ) );
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
				dex::Url check = getEndpoint( value );
				if ( check.completeUrl( ) == key.completeUrl( ) )
					return -1;
				dex::pair < dex::Url, time_t > p = pair( value, time( nullptr ) + expireTime );
				endpoints [ key ] = p;
				return 0;
				}
		};
	}

#endif
