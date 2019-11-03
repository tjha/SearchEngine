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
#include <unordered_map>

using dex::string;
using dex::RobotTxt;
using dex::unorderedMap;
using std::cout;
using std::endl;



void testHttps( )
	{
	unorderedMap < string, RobotTxt > robots{ 10 };
	int fileToWrite = 2;
	string res;
	
	int errorCode = dex::crawler::crawlUrl( "https://www.runescape.com", fileToWrite, res, robots );
	assert( errorCode == 302 );
	assert( res == "https://www.runescape.com/splash" );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/jaggywaggy", fileToWrite, res, robots );
	assert( errorCode == -5 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash", fileToWrite, res, robots, false );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash?woodcutting=100", fileToWrite, res, robots, false  );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash?woodcutting=100#jagex", fileToWrite, res, robots, false  );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash#jagex", fileToWrite, res, robots, false  );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.fb.com", fileToWrite, res, robots );
	assert( errorCode == 301 );
	assert( res == "https://www.facebook.com/");

	errorCode = dex::crawler::crawlUrl( "https://www.fb.com/", fileToWrite, res, robots );
	assert( errorCode == -5 );
	}

void testHttp( )
	{
	unorderedMap < string, RobotTxt > robots{ 10 };
	int fileToWrite = 2;
	string res;

	int errorCode = dex::crawler::crawlUrl( "http://www.runescape.com/splash", fileToWrite, res, robots );
	assert( errorCode == 302 );
	assert( res == "https://www.runescape.com/splash" );
	
	errorCode = dex::crawler::crawlUrl( "http://man7.org/", fileToWrite, res, robots );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "http://man7.org/?man=page", fileToWrite, res, robots, false );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "http://man7.org/?man=page#foobar", fileToWrite, res, robots );
	assert( errorCode == -5 );
	}

int main( )
	{
   testHttp( );
	testHttps( );
	}
