// frontier.hpp
// 2019-12-03: Change getUrl to reduce number of pageFault: combsc + jhirsh
// 2019-12-02: Implement maximumSize: combsc
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
			size_t maximumSize;

			double scoreUrl( dex::Url url )
				{
				double score = 0;
				// Promote good tlds ( .com, .org, .gov )
				if ( url.getHost( ).find( ".com" ) != dex::string::npos )
					{
					score += 10;
					}
				else
					{
					if ( url.getHost( ).find( ".org" ) != dex::string::npos )
						{
						score += 10;
						}
					else
						{
						if ( url.getHost( ).find( ".gov" ) != dex::string::npos )
							{
							score += 10;
							}
						else
							{
							if ( url.getHost( ).find( ".edu" ) != dex::string::npos )
								{
								score += 10;
								}
							}
						}
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
				if ( !url.getFragment( ).empty( ) || !url.getQuery( ).empty( ) )
					score -= 5;

				return score;
				}

		public:
			frontier( size_t maxSize )
				{
				maximumSize = maxSize;
				}
			size_t size( )
				{
				return toVisit.size( );
				}
			Url getUrl( )
				{
				int maxScore = -1;
				size_t maxIndex = 0;
				size_t poolSize = dex::min( size_t( 10 ), toVisit.size( ) );
				dex::Url best;
				// Get the highest scoring URL out of 10
				int location = rand( ) % toVisit.size( );
				for ( size_t j = 0;  j < poolSize;  ++j )
					{
					int offset = ( rand( ) % 200 ) - 100;
					size_t arrayLocation = size_t ( location + offset );
					if ( arrayLocation >= toVisit.size( ) )
						{
						arrayLocation = location;
						}
					int score = scoreUrl( toVisit[ arrayLocation ] );
					if ( score > maxScore )
						{
						maxScore = score;
						maxIndex = arrayLocation;
						best = toVisit[ arrayLocation ];
						}
					}
				toVisit.erase( maxIndex );
				return best;
				}

			dex::vector < dex::Url > getUrls( int num = 10 )
				{
				dex::vector < dex::Url > toReturn;
				for ( int i = 0;  i < num && !toVisit.empty( );  ++i )
					{
					int maxScore = -1;
					size_t maxIndex = 0;
					size_t poolSize = dex::min( size_t( 10 ), toVisit.size( ) );
					dex::Url best;
					// Get the highest scoring URL out of 10
					for ( size_t j = 0;  j < poolSize;  ++j )
						{
						int location = rand( ) % toVisit.size( );
						int score = scoreUrl( toVisit[ location ] );
						if ( score > maxScore )
							{
							maxScore = score;
							maxIndex = j;
							best = toVisit[ location ];
							}
						
						}
					toReturn.pushBack( best );
					}
				return toReturn;
				}

			void putUrl( const Url &url)
				{
				if ( toVisit.size( ) > maximumSize )
					{
					int minScore = 1000;
					size_t minIndex = 0;
					size_t poolSize = dex::min( size_t( 2 ), toVisit.size( ) );
					// Get the worst scoring Url out of 5
					for ( size_t j = 0;  j < poolSize;  ++j )
						{
						int location = rand( ) % toVisit.size( );
						int score = scoreUrl( toVisit[ location ] );
						if ( score < minScore )
							{
							minScore = score;
							minIndex = location;
							}
						
						}
					toVisit[ minIndex ] = url;
					}
				else 
					{
					toVisit.pushBack( url );
					}
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
