// frontier.hpp
// 2019-12-10: Improve URL Object: combsc
// 2019-12-08: Get rid of impolite URLs: combsc
// 2019-12-06: Promote www. and https: combsc
// 2019-12-04: Made scoreUrl less aggressive, improved efficiency: combsc
// 2019-12-03: Change getUrl to reduce number of pageFault: combsc + jhirsh
// 2019-12-02: Implement maximumSize: combsc
// 2019-12-01: Implement scoreUrl, getUrls: combsc
// 2019-11-15: Init Commit, not meant to be efficient: combsc

#ifndef DEX_FRONTIER_HPP
#define DEX_FRONTIER_HPP

#include <stdlib.h>
#include "crawler/robotsMap.hpp"
#include "utils/basicString.hpp"
#include "utils/unorderedSet.hpp"
#include "utils/url.hpp"
#include "utils/vector.hpp"

namespace dex
	{
	class frontier
		{
		private:
			robotsMap *robotsPointer = nullptr;
			vector < Url > toVisit;
			unorderedSet < Url > *toCheck;

			size_t maximumSize;

			double scoreUrl( dex::Url url )
				{
				double score = 0;
				dex::string domain = url.getDomain( );
				if ( robotsPointer && robotsPointer->robotExists( domain ) && !robotsPointer->politeToVisit( domain , url.getPath( ) ) )
					{
					return -100;
					}
				if ( url.getSubdomain( ) == "www." || "" )
						{
						score += 8;
						}
				dex::string host = url.getHost( );
				// Promote good tlds ( .com, .org, .gov )
				if ( host.size( ) > 4 )
					{
					host = host.substr( host.size( ) - 4, 4);
					if ( host == ".com" )
						{
						score += 10;
						}
					else
						{
						if ( host == ".org" )
							{
							score += 10;
							}
						else
							{
							if ( host == ".gov" )
								{
								score += 10;
								}
							else
								{
								if ( host == ".edu" )
									{
									score += 10;
									}
								}
							}
						}
					}


				// Promote short URLs
				// max of this should not give more than a good TLD
				// completeUrls of size 45 or lower get maximum score ( of 9 ).
				// decay after that
				dex::string completeUrl = url.completeUrl( );
				int urlSize = dex::max( size_t( 45 ), completeUrl.size( ) );
				score += 9 * 45 / urlSize;

				// Promote not a ton of /'s
				// take off points for every / you have over 6 ( take off 3 for the slashes in every url )
				int numSlashes = 0;
				for ( size_t i = 0;  i < completeUrl.size( );  ++i )
					{
					if ( completeUrl[ i ] == '/' )
						++numSlashes;
					}
				numSlashes -= 6;
				if ( numSlashes > 0 )
					{
					score -= numSlashes * 2;
					}
				// Promote no queries or fragments
				if ( !url.getFragment( ).empty( ) || !url.getQuery( ).empty( ) )
					score -= 5;
				return score;
				}

		public:
			frontier( size_t maxSize, robotsMap *robot )
				{
				robotsPointer = robot;
				maximumSize = maxSize;
				toCheck = new dex::unorderedSet < dex::Url >( maxSize );
				}
			size_t size( )
				{
				return toVisit.size( );
				}
			size_t capacity( )
				{
				return toVisit.capacity( );
				}
			size_t toCheckSize( )
				{
				return toCheck->size( );
				}
			size_t toCheckCapacity( )
				{
				return toCheck->bucketCount( );
				}
			Url getUrl( )
				{
				int maxScore = -101;
				size_t maxIndex = 0;
				size_t poolSize = dex::min( size_t( 10 ), toVisit.size( ) );
				dex::Url best;
				dex::Url *currentBest = nullptr;
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
						currentBest = &toVisit[ arrayLocation ];
						}
					}
				if ( !currentBest )
					{
					currentBest = &toVisit[ location ];
					}
				best = *currentBest;
				toCheck->erase( best );
				toVisit[ maxIndex ] = toVisit.back( );
				toVisit.popBack( );

				return best;
				}

			void putUrl( const Url &url)
				{
				if ( toCheck->count( url ) > 0 )
					{
					return;
					}
				if ( toVisit.size( ) > maximumSize )
					{
					int minScore = 1000;
					size_t minIndex = 0;
					size_t poolSize = dex::min( size_t( 5 ), toVisit.size( ) );
					dex::Url worst;
					dex::Url *currentWorst;
					// Get the worst scoring Url out of 5
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
						if ( score < minScore )
							{
							minScore = score;
							minIndex = arrayLocation;
							currentWorst = &toVisit[ arrayLocation ];
							}
						}
					if ( !currentWorst )
						{
						currentWorst = &toVisit[ location ];
						}
					worst = *currentWorst;
					toCheck->erase( worst );
					toVisit[ minIndex ] = url;
					toCheck->insert( url );
					}
				else
					{
					toVisit.pushBack( url );
					toCheck->insert( url );
					}
				// clear the whole toCheck when it gets to be large
				if ( toCheckSize( ) >= maximumSize )
					{
					delete toCheck;
					toCheck = new dex::unorderedSet< dex::Url >( maximumSize );
					//toCheck->rehash( maximumSize * 4 );
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
