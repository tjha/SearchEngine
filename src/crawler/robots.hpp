// robots.hpp
// class for respecting robots protocol

// 2019-12-09: Add timeToWait: combsc
// 2019-12-08: Default delay 10 sceonds: combsc
// 2019-12-03: default delay 1 second:combsc
// 2019-11-27: Finished wildcard checking: combsc
// 2019-11-26: Wildcard checking: combsc
// 2019-11-23: Distinguish between politeness requests and disallowed paths, fixed parsing bug: combsc
// 2019-11-18: Added expiration date: combsc
// 2019-11-13: Added parsing for robots.txt files: combsc
// 2019-11-04: fixed fixPath function, should NOT have / at the end: combsc
// 2019-11-03: Added default values for all member variables for all
//             constructors, fixed the copy constructor: combsc
// 2019-11-01: Added iterators for add/remove paths functions, refactored
//             code to reflect our standards: combsc
// 2019-10-31: Path sets defined size now, hash func, Copy constructors,
//             operator=: jhirsh
// 2019-10-21: Improved definition of path: Chris
// 2019-10-17: File created: Jonas, Chris

#ifndef DEX_ROBOTS_HPP
#define DEX_ROBOTS_HPP

#include <time.h>
#include <stdio.h>
#include "utils/algorithm.hpp"
#include "utils/basicString.hpp"
#include "utils/functional.hpp"
#include "utils/unorderedSet.hpp"
#include "utils/vector.hpp"

namespace dex
	{
	inline bool match( dex::string str, dex::string pattern )
		{
		size_t lastFound = 0;
		size_t lastLocation = 0;
		size_t asteriskLocation = pattern.find( "*", lastLocation );
		for ( ;  asteriskLocation != dex::string::npos;  asteriskLocation = pattern.find( "*", lastLocation ) )
			{
			int size = asteriskLocation - lastLocation;
			if ( size > 0 )
				{
				string toFind = pattern.substr( lastLocation, size );
				lastFound = str.find( toFind, lastFound );
				if ( lastFound == dex::string::npos )
					return false;
				lastFound += toFind.size( );
				}
			lastLocation = asteriskLocation + 1;
			}
		int size = pattern.size( ) - lastLocation;
		if ( size > 0 )
			{
			string toFind = pattern.substr( lastLocation, pattern.size( ) - lastLocation );
			lastFound = str.find( toFind, lastFound );
			if ( lastFound == dex::string::npos )
				return false;
			}
		return true;
		}
	// using this link to understand the protocol:
	// https://www.promptcloud.com/blog/how-to-read-and-respect-robots-file/
	struct RobotTxt
		{
		private:
			dex::string domain;
			// Time we have to wait to hit domain again in seconds
			double crawlDelay;

			// Last time this domain was visited, time that we're allowed to visit again
			time_t lastTimeVisited;
			time_t allowedVisitTime;
			// When the cache entry needs to expire
			time_t expireTime;

			// Paths that are disallowed. All extensions on the paths within this set are
			// also disallowed, unless explicitly allowed in allowedPaths.
			dex::unorderedSet< dex::string > disallowedPaths { 10 };
			// Paths that are exceptions to disallowed paths above. All extensions on the paths
			// within this set are also allowed.
			dex::unorderedSet< dex::string > allowedPaths { 10 };
			// This datastructure is for checking wildcard paths
			dex::vector< dex::string > allowedWildcards;
			dex::vector< dex::string > disallowedWildcards;



			bool pathIsAllowed( dex::string path )
				{
				path = fixPath( path ); bool pathIsAllowed = true;
				// check to see if the path is in any wildcard paths
				for ( auto it = allowedWildcards.cbegin( );  it != allowedWildcards.cend( );  ++it )
					{
					if ( match( path, *it ) )
						{
						return true;
						}
					}
				for ( auto it = disallowedWildcards.cbegin( );  it != disallowedWildcards.cend( );  ++it )
					{
					if ( match( path, *it ) )
						{
						pathIsAllowed = false;
						}
					}

				// if the path passed in is explicitly in disallowed paths, return false
				if ( disallowedPaths.count( path ) > 0 )
					return false;

				// if the path passed in is explicitly in allowed paths, return true
				if ( allowedPaths.count( path ) > 0 )
					return true;


				// Parse the path to see if it is part of a disallowed path or allowed path
				for ( size_t nextSlashLocation = path.find( "/" );  nextSlashLocation != dex::string::npos;
						nextSlashLocation = path.find( "/", nextSlashLocation + 1 ) )
					{
					dex::string toCheck = path.substr(0, nextSlashLocation + 1 );

					// If at any point our path is in disallowed paths, we know that if it's not explicitly
					// allowed we need to return false
					if ( disallowedPaths.count( toCheck ) > 0 )
						pathIsAllowed = false;

					// If at any point our path is in allowed paths, we know that it's allowed and we can return true
					if ( allowedPaths.count( toCheck ) > 0 )
						return true;
					}


				return pathIsAllowed;
				}

			// A valid path has a '/' at the beginning and end.
			dex::string fixPath( const dex::string & path )
				{
				dex::string fixedPath = path.stripWhitespace( );
				if ( fixedPath.front( ) != '/' )
					fixedPath.insert( 0, "/" );
				if ( fixedPath.back( ) != '/' )
					fixedPath.append( "/" );
				return dex::toLower( fixedPath );
				}


		public:
			static const unsigned defaultDelay = 1;
			static const unsigned defaultExpiration = 60 * 60 * 24;
			static const dex::string userAgent;

			RobotTxt( )
				{
				domain = "no-domain";
				crawlDelay = defaultDelay;
				allowedVisitTime = time( nullptr );
				lastTimeVisited = allowedVisitTime - crawlDelay;
				expireTime = allowedVisitTime + defaultExpiration;
				}
			RobotTxt( const RobotTxt &other ) : domain( other.domain ), crawlDelay( other.crawlDelay ),
					lastTimeVisited( other.lastTimeVisited ), allowedVisitTime( other.allowedVisitTime ),
					expireTime( other.expireTime ), disallowedPaths( other.disallowedPaths ),
					allowedPaths( other.allowedPaths )
				{
				}
			RobotTxt( const dex::string &domain, unsigned crawlDelay = defaultDelay) : domain( domain ), crawlDelay( crawlDelay )
				{
				allowedVisitTime = time( nullptr );
				lastTimeVisited = allowedVisitTime - crawlDelay;
				expireTime = allowedVisitTime + defaultExpiration;
				}
			RobotTxt( const dex::string &otherDomain, const dex::string &robotTxtFile )
				{
				domain = otherDomain;
				crawlDelay = defaultDelay;
				lastTimeVisited = allowedVisitTime - crawlDelay;
				allowedVisitTime = time( nullptr );
				expireTime = allowedVisitTime + defaultExpiration;
				string toSearch = dex::toLower( robotTxtFile );
				// see if user-agent matches our user-agent OR if it's *
				int start = toSearch.find( "user-agent: " + userAgent );
				if ( start == -1 )
					start = toSearch.find( "user-agent: *" );
				if ( start != -1 )
					{
					int end = toSearch.find( "user-agent:", start + 1 );
					if ( end == -1 )
						end = toSearch.size( );
					dex::string toParse = toSearch.substr( start, end - start );

					// find the crawling rate they'd prefer us to use.
					int crawlStart, crawlEnd;
					crawlStart = toParse.find( "crawl-rate: " );
					if ( crawlStart != -1 )
						{
						crawlEnd = toParse.find( "\n", crawlStart + 1 );
						crawlDelay = 1 / atof( toParse.substr( crawlStart + 12, crawlEnd - crawlStart - 12 ).cStr( ) );
						}
					crawlStart = toParse.find( "crawl-delay: " );
					if ( crawlStart != -1 )
						{
						crawlEnd = toParse.find( "\n", crawlStart + 1 );
						crawlDelay = atof( toParse.substr( crawlStart + 13, crawlEnd - crawlStart - 13 ).cStr( ) );
						}

					// find all allowed paths
					int allowStart, allowEnd;
					for ( allowStart = toParse.find( "allow: " );  allowStart != -1;  allowStart = toParse.find( "allow: ", allowStart + 1) )
						{
						string check = toParse.substr( allowStart - 3, 10 );
						if ( allowStart > 2 && check.compare( "disallow: ") != 0 )
							{
							allowEnd = toParse.find( "\n", allowStart + 1 );
							addPathsAllowed( toParse.substr( allowStart + 7, allowEnd - allowStart - 7 ) );
							}
						}

					// find all disallowed paths
					int disallowStart, disallowEnd;
					for ( disallowStart = toParse.find( "disallow: " );  disallowStart != -1;  disallowStart = toParse.find( "disallow: ", disallowStart + 1 ) )
						{
						disallowEnd = toParse.find( "\n", disallowStart + 1 );
						addPathsDisallowed( toParse.substr( disallowStart + 10, disallowEnd - disallowStart - 10 ) );
						}
					}
				}

			RobotTxt operator=( const RobotTxt &other )
				{
				RobotTxt otherCopy ( other );
				dex::swap( domain, otherCopy.domain );
				dex::swap( crawlDelay, otherCopy.crawlDelay );
				dex::swap( lastTimeVisited, otherCopy.lastTimeVisited );
				dex::swap( allowedVisitTime, otherCopy.allowedVisitTime );
				dex::swap( expireTime, otherCopy.expireTime );
				dex::swap( disallowedPaths, otherCopy.disallowedPaths );
				dex::swap( allowedPaths, otherCopy.allowedPaths );
				return *this;
				}
			RobotTxt operator=( RobotTxt &&other )
				{
				dex::swap( domain, other.domain );
				dex::swap( crawlDelay, other.crawlDelay );
				dex::swap( lastTimeVisited, other.lastTimeVisited );
				dex::swap( allowedVisitTime, other.allowedVisitTime );
				dex::swap( expireTime, other.expireTime );
				dex::swap( disallowedPaths, other.disallowedPaths );
				dex::swap( allowedPaths, other.allowedPaths );
				return *this;
				}

			// Call each time you HTTP request a website
			void updateLastVisited( )
				{
				lastTimeVisited = time( nullptr );
				allowedVisitTime = lastTimeVisited + crawlDelay;
				}

			// Set the disallowed paths for the domain
			template < class InputIt >
			void setPathsDisallowed( const InputIt &begin, const InputIt &end )
				{
				disallowedPaths.clear( );
				addPathsDisallowed( begin, end );
				}
			void setPathsDisallowed( const dex::unorderedSet< dex::string > &paths )
				{
				setPathsDisallowed( paths.cbegin( ), paths.cend( ) );
				}

			// Add paths to the disallowed paths for the domain
			template < class InputIt >
			void addPathsDisallowed( const InputIt &begin, const InputIt &end )
				{
				for ( InputIt it = begin;  it != end;  ++it )
					{
					if ( it->find( "*" ) == dex::string::npos )
						disallowedPaths.insert( fixPath( *it ) );
					else
						disallowedWildcards.pushBack( *it );
					}

				}
			void addPathsDisallowed( const dex::unorderedSet< dex::string > &paths )
				{
				addPathsDisallowed( paths.cbegin( ), paths.cend( ) );
				}
			void addPathsDisallowed( const dex::vector< dex::string > &paths )
				{
				addPathsDisallowed( paths.cbegin( ), paths.cend( ) );
				}
			void addPathsDisallowed( const dex::string &str )
				{
				if ( str.find( "*" ) == dex::string::npos )
					disallowedPaths.insert( fixPath( str ) );
				else
					{
					disallowedWildcards.pushBack( str );
					}

				}

			// Set the allowed paths for the domain
			template < class InputIt >
			void setPathsAllowed( const InputIt &begin, const InputIt &end )
				{
				allowedPaths.clear( );
				addPathsAllowed( begin, end );
				}
			void setPathsAllowed( const dex::unorderedSet< dex::string > &paths )
				{
				setPathsAllowed( paths.cbegin( ), paths.cend( ) );
				}

			// Add paths to the allowed paths for the domain
			template < class InputIt >
			void addPathsAllowed( const InputIt &begin, const InputIt &end )
				{
				for ( InputIt it = begin;  it != end;  ++it )
					{
					if ( it->find( "*" ) == dex::string::npos )
						allowedPaths.insert( fixPath( *it ) );
					else
						allowedWildcards.pushBack( *it );
					}

				}
			void addPathsAllowed( const dex::unorderedSet< dex::string > &paths )
				{
				addPathsAllowed( paths.cbegin( ), paths.cend( ) );
				}
			void addPathsAllowed( const dex::vector< dex::string > &paths )
				{
				addPathsAllowed( paths.cbegin( ), paths.cend( ) );
				}
			void addPathsAllowed( const dex::string &path )
				{
				if ( path.find( "*" ) == dex::string::npos )
					allowedPaths.insert( fixPath( path ) );
				else
					{
					allowedWildcards.pushBack( path );
					}

				}

			// Checks for if you can perform HTTP request
			// if you can visit return 0, if politeness error return 1, if disallowed return 2
			int visitPathResult( const dex::string &path )
				{
				if ( !pathIsAllowed( path ) )
					return 2;

				if ( time( nullptr ) < allowedVisitTime )
					return 1;
				return 0;
				}

			bool hasExpired( )
				{
				return time( nullptr ) >= expireTime;
				}

			// Need access to all member variables for encoding
			dex::string getDomain( ) const
				{
				return domain;
				}
			int getDelay( ) const
				{
				return crawlDelay;
				}

			time_t getLastVisit( ) const
				{
				return lastTimeVisited;
				}

			time_t getAllowedVisitTime( ) const
				{
				return allowedVisitTime;
				}

			time_t getExpireTime( ) const
				{
				return expireTime;
				}
			time_t timeToWait( ) const
				{
				return allowedVisitTime - time( nullptr );
				}
			const dex::unorderedSet< dex::string > getAllowedPaths( ) const
				{
				return allowedPaths;
				}

			const dex::unorderedSet< dex::string > getDisallowedPaths( ) const
				{
				return disallowedPaths;
				}
		};

	template < >
	struct hash< dex::RobotTxt >
		{
		unsigned long operator()( const dex::RobotTxt &robot ) const
			{
			return dex::hash< dex::string >{} ( robot.getDomain( ) );
			}
		};
	}

#endif
