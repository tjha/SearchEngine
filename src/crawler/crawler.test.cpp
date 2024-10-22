// crawlerTests.cpp
// Testing for our crawler class
//
// 2019-11-30: threadsafe version: combsc
// 2019-11-21: add test cases for malformed urls: combsc
// 2019-11-21: converted to catch: jhirsh
// 2019-11-18: fixed broken tests (fb.com doens't let robots crawl): combsc
// 2019-11-04: updated tests to match the new crawlUrl format: combsc
// 2019-11-03: Added tests for politeness: combsc
// 2019-10-31: added an unordered map of robots, passed to crawlUrl: jhirsh
// 2019-10-31: Added basic query + fragment testing: combsc
// 2019-10-30: File creation: combsc

#include "catch.hpp"
#include "crawler/crawler.hpp"
#include "crawler/robotsMap.hpp"
#include "utils/exception.hpp"
#include "utils/unorderedMap.hpp"
#include "utils/unorderedSet.hpp"

using dex::string;
using dex::RobotTxt;
using dex::robotsMap;
using dex::unorderedMap;
using std::cout;
using std::endl;

const dex::string dex::RobotTxt::userAgent = "jhirshey@umich.edu (Linux)";

TEST_CASE( "Crawl", "[crawler]" )
	{
	SECTION( "HTTPS" )
		{
		robotsMap robots( 100 );
		string res;

		int errorCode = dex::crawler::crawlUrl( "https://www.runescape.com", res, robots );
		REQUIRE( errorCode == 302 );
		REQUIRE( res == "https://www.runescape.com/splash" );

		errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/jaggywaggy", res, robots );
		REQUIRE( errorCode == -5 );

		errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash", res, robots, false );
		REQUIRE( errorCode == 0 );

		errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash?woodcutting=100", res, robots, false );
		REQUIRE( errorCode == 0 );

		errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash?woodcutting=100#jagex", res, robots, false );
		REQUIRE( errorCode == 0 );

		errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash#jagex", res, robots, false );
		REQUIRE( errorCode == 0 );

		errorCode = dex::crawler::crawlUrl( "https://www.fb.com/", res, robots );
		REQUIRE( errorCode == -12 );
		}
	SECTION( "HTTP" )
		{
		robotsMap robots( 100 );
		string res;

		int errorCode = dex::crawler::crawlUrl( "http://www.runescape.com/splash", res, robots );
		REQUIRE( errorCode == 302 );

		REQUIRE( res == "https://www.runescape.com/splash" );

		errorCode = dex::crawler::crawlUrl( "http://man7.org/", res, robots );
		REQUIRE( errorCode == 0 );

		errorCode = dex::crawler::crawlUrl( "http://man7.org/?man=page", res, robots, false );
		REQUIRE( errorCode == 0 );

		errorCode = dex::crawler::crawlUrl( "http://man7.org/?man=page#foobar", res, robots );
		REQUIRE( errorCode == -5 );

		errorCode = dex::crawler::crawlUrl( "http://tlpi/index.html", res, robots );
		REQUIRE( errorCode == -1 );
		}
	}
