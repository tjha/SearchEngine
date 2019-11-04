// robots.hpp
// class for respecting robots protocol

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

#include <iostream>
#include <time.h>
#include <stdio.h>
#include "../utils/basicString.hpp"
#include "../utils/vector.hpp"
#include "../utils/unorderedSet.hpp"
#include "../utils/algorithm.hpp"

using std::cerr;
using std::ostream;
using dex::unorderedSet;
using dex::vector;
using dex::string;


namespace dex
	{

	// using this link to understand the protocol: 
	// https://www.promptcloud.com/blog/how-to-read-and-respect-robots-file/
	struct RobotTxt
		{
		private:
			string domain;
			// Time we have to wait to hit domain again in seconds
			int crawlDelay;
			// TODO: To be used if there is a specified time when crawling is disallowed
			//int visitTimeHourStart;
			//int visitTimeMinuteStart;
			//int visitTimeHourEnd;
			//int visitTimeMinuteEnd;

			// Last time this domain was visited, time that we're allowed to visit again
			time_t lastTimeVisited;
			time_t allowedVisitTime;

			// Paths that are disallowed. All extensions on the paths within this set are
			// also disallowed, unless explicitly allowed in allowedPaths.
			unorderedSet < string > disallowedPaths { 10 };
			// Paths that are exceptions to disallowed paths above. All extensions on the paths
			// within this set are also allowed.
			unorderedSet < string > allowedPaths { 10 };

			bool pathIsAllowed( string path )
				{
				path = fixPath( path );
				// if the path passed in is explicitly in disallowed paths, return false
				if ( disallowedPaths.count( path ) > 0 )
					return false;
					
				// if the path passed in is explicitly in allowed paths, return true
				if ( allowedPaths.count( path ) > 0 )
					return true;

				bool pathIsAllowed = true;
				// Parse the path to see if it is part of a disallowed path or allowed path
				for ( size_t nextSlashLocation = path.find( "/" );  nextSlashLocation != string::npos;
						nextSlashLocation = path.find( "/", nextSlashLocation + 1 ) )
					{
					string toCheck = path.substr(0, nextSlashLocation + 1 );

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
			string fixPath( const string & path )
				{
				string fixedPath = path;
				if ( fixedPath[ 0 ] != '/' )
					fixedPath.insert( 0, "/" );
				if ( fixedPath.back( ) != '/' )
					fixedPath.append( "/" );
				return fixedPath;
				}
			

		public:
			static const unsigned defaultDelay = 10;

			RobotTxt( )
				{
				domain = "no-domain";
				crawlDelay = defaultDelay;
				allowedVisitTime = time( nullptr );
				lastTimeVisited = allowedVisitTime - crawlDelay;
				}
			RobotTxt( const RobotTxt &other ) : domain( other.domain ), crawlDelay( other.crawlDelay ), 
					lastTimeVisited( other.lastTimeVisited ), allowedVisitTime( other.allowedVisitTime ),
					disallowedPaths( other.disallowedPaths ), allowedPaths( other.allowedPaths )
				{
				}
			RobotTxt( const string &domain, unsigned crawlDelay = defaultDelay) : domain( domain ), crawlDelay( crawlDelay )
				{
				allowedVisitTime = time( nullptr );
				lastTimeVisited = allowedVisitTime - crawlDelay;
				}
			RobotTxt( const string &otherDomain, const string &robotTxtFile )
				{
				// here is where the parsing of the actual file will take place
				// this is to silence -wall
				domain = robotTxtFile;
				domain = otherDomain;
				crawlDelay = defaultDelay;
				allowedVisitTime = time( nullptr );
				lastTimeVisited = allowedVisitTime - crawlDelay;
				}
			
			RobotTxt operator=( const RobotTxt &other )
				{
				RobotTxt otherCopy ( other );
				dex::swap( domain, otherCopy.domain );
				dex::swap( crawlDelay, otherCopy.crawlDelay );
				dex::swap( lastTimeVisited, otherCopy.lastTimeVisited );
				dex::swap( allowedVisitTime, otherCopy.allowedVisitTime );
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
				dex::swap( disallowedPaths, other.disallowedPaths );
				dex::swap( allowedPaths, other.allowedPaths );
				return *this;
				}

			// File stream input, output
			friend ostream &operator<<( ostream& out, RobotTxt &obj );
			//friend istream &operator>>( istream& in, RobotTxt &rhs );

			// Call each time you HTTP request a website
			void updateLastVisited( )
				{
				lastTimeVisited = time( nullptr );
				allowedVisitTime = lastTimeVisited + crawlDelay;
				}

			// Set the disallowed paths for the domain
			template < class InputIt,
					typename = typename std::enable_if < !std::is_integral< InputIt >::value >::type >
			void setPathsDisallowed( const InputIt &begin, const InputIt &end )
				{
				disallowedPaths.clear( );
				addPathsDisallowed( begin, end );
				}
			void setPathsDisallowed( const unorderedSet < string > &paths )
				{
				setPathsDisallowed( paths.cbegin( ), paths.cend( ) );
				}

			// Add paths to the disallowed paths for the domain
			template < class InputIt,
					typename = typename std::enable_if < !std::is_integral< InputIt >::value >::type >
			void addPathsDisallowed( const InputIt &begin, const InputIt &end )
				{
				for ( InputIt it = begin;  it != end;  ++it )
					disallowedPaths.insert( fixPath( *it ) );
				}
			void addPathsDisallowed( const unorderedSet < string > &paths )
				{
				addPathsDisallowed( paths.cbegin( ), paths.cend( ) );
				}
			void addPathsDisallowed( const vector < string > &paths )
				{
				addPathsDisallowed( paths.cbegin( ), paths.cend( ) );
				}
			void addPathsDisallowed( const string &str )
				{
				disallowedPaths.insert( fixPath( str ) );
				}

			// Set the allowed paths for the domain
			template < class InputIt,
					typename = typename std::enable_if < !std::is_integral< InputIt >::value >::type >
			void setPathsAllowed( const InputIt &begin, const InputIt &end )
				{
				allowedPaths.clear( );
				addPathsAllowed( begin, end );
				}
			void setPathsAllowed( const unorderedSet < string > &paths )
				{
				setPathsAllowed( paths.cbegin( ), paths.cend( ) );
				}

			// Add paths to the allowed paths for the domain
			template < class InputIt,
					typename = typename std::enable_if < !std::is_integral< InputIt >::value >::type >
			void addPathsAllowed( const InputIt &begin, const InputIt &end )
				{
				for ( InputIt it = begin;  it != end;  ++it )
					allowedPaths.insert( fixPath( *it ) );
				}
			void addPathsAllowed( const unorderedSet < string > &paths )
				{
				addPathsAllowed( paths.cbegin( ), paths.cend( ) );
				}
			void addPathsAllowed( const vector < string > &paths )
				{
				addPathsAllowed( paths.cbegin( ), paths.cend( ) );
				}
			void addPathsAllowed( const string &path )
				{
				allowedPaths.insert( fixPath( path) );
				}

			// Checks for if you can perform HTTP request
			bool canVisitPath( const string &path )
			{
			string fixedPath = fixPath( path );
			if ( !pathIsAllowed( fixedPath ) )
				return false;
			
			return time( nullptr ) >= allowedVisitTime; 
			}

			// All of the information of the robot
			string compress( )
				{
				string robot = "Domain:\t\t\t" + domain + "\n" +
									"Crawl-Delay:\t\t" + char( crawlDelay ) + "\n" +
									"Allowed-Visit-Time:\t" + ctime( &allowedVisitTime ) +
									"Last-Visit:\t\t" + ctime( &lastTimeVisited ) + "\n" +
									"Allowed-Paths\n" + allowedPaths.compress( ) +
									"Disallowed-Paths\n" + disallowedPaths.compress( );
				return robot;
				}

			// need domain for hash func
			const string getDomain( ) const
				{
				return domain;
				}
		};

	ostream & operator<<( ostream &out, RobotTxt &obj ) 
		{
		return out << "Domain:\t\t\t" << obj.domain << std::endl
					  << "Crawl-Delay:\t\t" << obj.crawlDelay << std::endl
					  << "Allowed-Visit-Time:\t" << ctime( &obj.allowedVisitTime )
					  << "Last-Visit:\t\t" << ctime( &obj.lastTimeVisited ) << std::endl
					  << "Allowed-Paths\n" << obj.allowedPaths.compress( )
					  << "Disallowed-Paths\n" << obj.disallowedPaths.compress( );
		}


	
	template < class Key >
	struct hash;

	template< >
	struct hash< dex::RobotTxt >
		{
		unsigned long operator()( const dex::RobotTxt &robot ) const
			{
			return dex::hash< dex::string >{} ( robot.getDomain( ) );
			}
		};
	}

#endif
