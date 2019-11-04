// crawlerTests.cpp
// Testing for our crawler class
//
// 2019-11-03: Added tests for politeness: combsc
// 2019-10-31: added an unordered map of robots, passed to crawlUrl: Jonas
// 2019-10-31: Added basic query + fragment testing: combsc
// 2019-10-30: File creation: combsc

#include <unistd.h>
#include <cassert>
#include "crawler.hpp"
#include <iostream>
#include "robots.hpp"
#include "../utils/unorderedMap.hpp"
#include "../utils/basicString.hpp"
#include "../utils/exception.hpp"
#include <unordered_map>
#include "../utils/file.hpp"
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
   if ( argc != 3 )
      {
      std::cerr << "Usage: ./crawl.exe [ frontier_file ] [ robots_save_file ]" << endl;
      exit( 1 );
      }

   std::list< string > frontier = loadFrontier( argv[ 1 ] );

   string res;
   int fileToWrite = 3;
   unorderedMap < string, RobotTxt > robots{ 20 };

   for ( auto it = frontier.begin( );  it != frontier.end( ); )
      {
      try
         {
         int errorCode = dex::crawler::crawlUrl( it->cStr( ), fileToWrite, res, robots );
         if ( errorCode == 0 )
            {
            cout << "crawled " << it->cStr( ) << endl;
            it = frontier.erase( it );
            }
         else
            {
            ++it;
            }
         }
      catch ( exception &e )
         {
         cout << it->cStr( ) << " " << e.what() << endl;
         ++it;
         }
      }

   // TODO make this output to a file such that the parser can pick it up
   // easily
   outputNewFrontier( "frontierNew.txt", frontier );
   outputRobots( argv[ 2 ], robots );
   }
