// frontier.hpp
// 2019-12-01: Implement scoreUrl, getUrls: combsc
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

			double scoreUrl( dex::Url url )
				{
				double score = 0;
				// Promote good tlds ( .com, .org, .gov )
				if ( url.getHost( ).find( ".com" ) != dex::string::npos )
					{
					score += 10;
					}
				if ( url.getHost( ).find( ".org" ) != dex::string::npos )
					{
					score += 10;
					}
				if ( url.getHost( ).find( ".gov" ) != dex::string::npos )
					{
					score += 10;
					}
				if ( url.getHost( ).find( ".edu" ) != dex::string::npos )
					{
					score += 10;
					}
				// Promote short URLs
				// max of this should not give more than a good TLD
				// completeUrls of size 25 or lower get maximum score ( of 9 ).
				// decay after that
				dex::string completeUrl = url.completeUrl( );
				int urlSize = dex::max( size_t( 26 ), completeUrl.size( ) + 1 );
				score += 9 * 26 / urlSize;

				// Promote not a ton of /'s
				// take off points for every / you have
				int numSlashes = 0;
				for ( size_t i = 0;  i < completeUrl.size( );  ++i )
					{
					if ( completeUrl[ i ] == '/' )
						++numSlashes;
					}
				score -= numSlashes * 2;
				// Promote no queries or fragments
				if ( url.getFragment( ) != "" || url.getQuery( ) != "" )
					score -= 5;

				return score;
				}

		public:
			Url getUrl( )
				{
				int maxScore = -1;
				size_t maxIndex = 0;
				size_t poolSize = dex::min( size_t( 10 ), toVisit.size( ) );
				dex::vector < dex::Url > pool( poolSize );
				// Get the highest scoring URL out of 10
				for ( size_t j = 0;  j < poolSize;  ++j )
					{
					int location = rand( ) % toVisit.size( );
					pool[ j ] = toVisit[ location ];
					toVisit.erase( location );
					int score = scoreUrl( pool[ j ] );
					if ( score > maxScore )
						{
						maxScore = score;
						maxIndex = j;
						}
					
					}
				// Put the others back
				for ( size_t j = 0;  j < poolSize;  ++j )
					{
					if ( j != maxIndex )
						{
						toVisit.pushBack( pool[ j ] );
						}
					}
				return pool[ maxIndex ];
				}

			dex::vector < dex::Url > getUrls( int num = 10 )
				{
				dex::vector < dex::Url > toReturn;
				for ( int i = 0;  i < num && !toVisit.empty( );  ++i )
					{
					int maxScore = -1;
					size_t maxIndex = 0;
					size_t poolSize = dex::min( size_t( 10 ), toVisit.size( ) );
					dex::vector < dex::Url > pool( poolSize );
					// Get the highest scoring URL out of 10
					for ( size_t j = 0;  j < poolSize;  ++j )
						{
						int location = rand( ) % toVisit.size( );
						pool[ j ] = toVisit[ location ];
						toVisit.erase( location );
						int score = scoreUrl( pool[ j ] );
						if ( score > maxScore )
							{
							maxScore = score;
							maxIndex = j;
							}
						
						}
					// Put the others back
					for ( size_t j = 0;  j < poolSize;  ++j )
						{
						if ( j != maxIndex )
							{
							toVisit.pushBack( pool[ j ] );
							}
						}
					toReturn.pushBack( pool[ maxIndex ] );
					}
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
