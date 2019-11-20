// crawlerTests.cpp
// Testing for our crawler class
//
// 2019-11-18: fixed broken tests (fb.com doens't let robots crawl): combsc
// 2019-11-04: updated tests to match the new crawlUrl format: combsc
// 2019-11-03: Added tests for politeness: combsc
// 2019-10-31: added an unordered map of robots, passed to crawlUrl: Jonas
// 2019-10-31: Added basic query + fragment testing: combsc
// 2019-10-30: File creation: combsc

#include <cassert>
#include "crawler.hpp"

using dex::string;
using dex::RobotTxt;
using dex::unorderedMap;
using std::cout;
using std::endl;


// TODO put this in tst/ and make it catch2
void testHttps( )
	{
	unorderedMap < string, RobotTxt > robots{ 10 };
	string res;
	
	int errorCode = dex::crawler::crawlUrl( "https://www.runescape.com", res, robots, "savedTestOutput.txt" );
	assert( errorCode == 302 );
	assert( res == "https://www.runescape.com/splash" );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/jaggywaggy", res, robots, "savedTestOutput.txt" );
	assert( errorCode == -5 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash", res, robots, "savedTestOutput.txt", false );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash?woodcutting=100", res, robots, "savedTestOutput.txt", false );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash?woodcutting=100#jagex", res, robots, "savedTestOutput.txt", false );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash#jagex", res, robots, "savedTestOutput.txt", false );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.fb.com/", res, robots, "savedTestOutput.txt" );
	assert( errorCode == -5 );
	}

void testHttp( )
	{
	unorderedMap < string, RobotTxt > robots{ 10 };
	string res;

	
	int errorCode = dex::crawler::crawlUrl( "http://www.runescape.com/splash", res, robots, "savedTestOutput.txt" );
	assert( errorCode == 302 );
	
	assert( res == "https://www.runescape.com/splash" );
	
	errorCode = dex::crawler::crawlUrl( "http://man7.org/", res, robots, "savedTestOutput.txt" );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "http://man7.org/?man=page", res, robots, "savedTestOutput.txt", false );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "http://man7.org/?man=page#foobar", res, robots, "savedTestOutput.txt" );
	assert( errorCode == -5 );
	}

int main( )
	{
   testHttp( );
	testHttps( );
	}
