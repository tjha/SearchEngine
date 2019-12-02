// frontier.hpp
// 2019-11-15: Init Commit, not meant to be efficient: combsc
#ifndef DEX_FRONTIER_HPP
#define DEX_FRONTIER_HPP
#include "../utils/vector.hpp"
#include "../utils/basicString.hpp"
#include "../spinarak/url.hpp"
#include <stdlib.h>

namespace dex
	{
	class frontier
		{
		private:
			vector < Url > toVisit;

		public:
			Url getUrl( )
				{
				int location = rand( ) % toVisit.size( );
				Url toReturn = toVisit[ location ];
				toVisit.erase( location );
				return toReturn;
				}

			void putUrl( const Url &url)
				{
				toVisit.pushBack( url );
				}

			bool empty( )
				{
				return toVisit.empty( );
				}

			const dex::vector< Url > getFrontier( )
				{
				return toVisit;
				}

			dex::vector < Url > ::iterator begin( )
				{
				return toVisit.begin( );
				}

			dex::vector < Url > ::iterator end( )
				{
				return toVisit.end( );
				}
		};
	}

#endif
