// crawl.cpp
// Testing for our crawler class
//
// 2019-11-04: edited code logic slightly to match other changes made today: combsc
// 2019-11-04: File creation: jhirsh

#include <unistd.h>
#include <cassert>
#include "crawler.hpp"
#include <iostream>
#include "robots.hpp"
#include "../utils/unorderedMap.hpp"
#include "../utils/basicString.hpp"
#include "../utils/exception.hpp"
#include <unordered_map>
// Believe we need to implement std::list for frontier
#include <list>
#include <iterator>

using dex::string;
using dex::RobotTxt;
using dex::unorderedMap;
using dex::writeToFile;
using dex::readFromFile;
using dex::exception;
using std::cout;
using std::endl;


std::list< string > loadFrontier( const char * fileName )
	{
	// read in the frontier file
	string frontierList( readFromFile( fileName ) );

	std::list< string > frontier;
	size_t start = 0;
	string delimiter = "\n";

	// Parse the frontier_file and add it to list of urls
	while ( start < frontierList.size( ) )
		{
		size_t found = frontierList.find( delimiter, start );
		if ( found < frontierList.npos ) 
			{
			string url( frontierList.begin( ) + start, frontierList.begin( ) + found );
			frontier.push_back ( url );
			start = found + delimiter.size( );
			}
		else
			{
			start = frontierList.npos;
			}
		}

	return frontier;
	}

int outputRobots( const char * fileName, unorderedMap < string, RobotTxt > &robots )
	{
	string robotsData = "ROBOTS DATA\n" + robots.compress( );
	return writeToFile( fileName, robotsData.cStr( ), robotsData.size( ) );
	}

int outputNewFrontier( const char * fileName, std::list< string > &frontier )
	{
	string frontierData = "FRONTIER DATA\n";
	for ( auto &it: frontier )
		{
		frontierData += it + "\n";
		}
	return writeToFile( fileName, frontierData.cStr( ), frontierData.size( ) );
	}

int main( int argc, char ** argv )
	{
	if ( argc != 4 )
		{
		std::cerr << "Usage: ./crawl.exe [ frontier ] [ robots_save ] [ crawler_results ]" << endl;
		exit( 1 );
		}

	std::list< string > frontier = loadFrontier( argv[ 1 ] );
	std::list< string > brokenLinks;

	string res;
	int robotFile = open( argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
	int crawlFile = open( argv[3], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
	unorderedMap < string, RobotTxt > robots{ 20 };

	for ( auto it = frontier.begin( );  it != frontier.end( ); )
		{
		int errorCode = dex::crawler::crawlUrl( it->cStr( ), crawlFile, robotFile, res, robots );
		if ( errorCode == 0 )
			{
			cout << "crawled domain: " << it->cStr( ) << endl;
			it = frontier.erase( it );
			}

		if ( errorCode == dex::politenessError )
			{
			cout << "Mr. Robot says to be polite: " << it->cStr( ) << endl;
			cout << res << endl;
			++it;
			}

		if ( errorCode >= 300 && errorCode < 400 )
			{
			cout << "broken link: " << it->cStr( ) << endl;
			brokenLinks.push_back( *it );
			it = frontier.erase( it );
			}
		}

	// TODO make this output to a file such that the parser can pick it up
	// easily
	outputNewFrontier( "savedFrontier.txt", frontier );
	outputNewFrontier( "savedBrokenLinks.txt", brokenLinks );
	outputRobots( argv[ 2 ], robots );
	}
