// frontier.hpp
// 2019-11-15: Init Commit, not meant to be efficient: combsc
#ifndef DEX_FRONTIER_HPP
#define DEX_FRONTIER_HPP
#include "../utils/vector.hpp"
#include "../utils/basicString.hpp"
#include "../spinarak/url.hpp"

namespace dex
	{
	class frontier
		{
		private:
			vector < Url > toVisit;

		public:
			Url getUrl( )
				{
				Url toReturn = toVisit.front( );
				toVisit.erase( 0 );
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
		};
	}

#endif