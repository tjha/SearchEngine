// robotTxtTests.cpp
// Testing for our robots class
//
// 2019-10-21: File creation: combsc

#include "catch.hpp"
#include <unistd.h>
#include "../src/spinarak/robots.hpp"

using dex::RobotTxt;
using dex::string;

TEST_CASE( "timing for visiting sites", "[robotsTxt]" )
	{
	string url = "https://domain.com";
	RobotTxt hello = RobotTxt( url, 1 );
	hello.updateLastVisited( );
	REQUIRE( !hello.canVisitPath( "/" ) );
	sleep(2);
	REQUIRE( hello.canVisitPath( "/" ) );

	RobotTxt henlo = RobotTxt( url, 1 );
	henlo.updateLastVisited( );
	henlo.addPathsDisallowed( "/" );
	REQUIRE( !henlo.canVisitPath( "/" ) );
	sleep(2);
	REQUIRE( !henlo.canVisitPath( "/" ) );
	}

TEST_CASE( "whitelist and blacklist rules", "[robotsTxt]" )
	{
	string url = "https://domain.com";
	RobotTxt hello = RobotTxt( url, 0 );
	hello.updateLastVisited( );
	sleep(1);
	hello.addPathsDisallowed( "/" );
	hello.addPathsAllowed( "/secret/path/" );
	REQUIRE( !hello.canVisitPath( "/some/other/path/" ) );
	REQUIRE( !hello.canVisitPath( "/" ) );
	REQUIRE( hello.canVisitPath( "/secret/path/" ) );
	REQUIRE( hello.canVisitPath( "/secret/path/somefile/" ) );

	// testing passing "bad" paths

	hello.addPathsAllowed ( "secret/other/path" );
	REQUIRE( hello.canVisitPath( "secret/other/path" ) );
	REQUIRE( hello.canVisitPath( "secret/other/path/file" ) );
	}
