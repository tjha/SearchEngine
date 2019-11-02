// robotTxtTests.cpp
// Testing for our robots class
//
// 2019-11-02: better testing: combsc, jonas
// 2019-10-21: File creation: combsc

#include "catch.hpp"
#include <unistd.h>
#include "../src/spinarak/robots.hpp"
#include "../src/utils/unorderedMap.hpp"
#include "../src/utils/basicString.hpp"
#include <iostream>
#include <stdlib.h>

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

TEST_CASE( "constructors and operator=", "[robotTxt]")
   {
   SECTION( "constructors")
      {
      RobotTxt rob1 = RobotTxt( );
      REQUIRE ( rob1.getDomain( ) == "no-domain" );

      string url = "https://domain.com";
      RobotTxt rob2 = RobotTxt( url, 2 );
      REQUIRE ( rob2.getDomain( ) == url );

      RobotTxt rob3 = RobotTxt( rob2 );
      REQUIRE ( rob3.getDomain( ) == rob2.getDomain( ) );
      }
   
   SECTION( "copy constructor functionality test" )
      {
      string url = "https://domain.com";
      RobotTxt rob1 = RobotTxt( url, 2);
      rob1.addPathsAllowed( "/" );
      rob1.addPathsAllowed( "/secret/path/" );
      rob1.addPathsDisallowed( "/disallowed/path/");

      RobotTxt rob2 = RobotTxt( rob1 );
      REQUIRE ( rob2.canVisitPath( "/" ) );
      REQUIRE ( rob2.canVisitPath( "/secret/path/" ) );
      rob2.addPathsDisallowed( "/disallowed/path/");
      REQUIRE ( !rob2.canVisitPath( "/disallowed/path/" ) );
      rob2.updateLastVisited( );
      REQUIRE ( !rob2.canVisitPath( "/" ) );
      REQUIRE ( !rob2.canVisitPath( "/secret/path/" ) );
      sleep(3);
      REQUIRE ( rob2.canVisitPath( "/" ) );
      REQUIRE ( rob2.canVisitPath( "/secret/path/" ) );
      }

   SECTION( "operator <<" )
      {
      string url = "https://domain.com";
      RobotTxt rob1 = RobotTxt( url, 2);
      rob1.addPathsAllowed( "/" );
      rob1.addPathsAllowed( "/secret/path/" );
      rob1.addPathsDisallowed( "/disallowed/path/");
      RobotTxt rob2 = RobotTxt( rob1 );

      std::cout << rob1;
      // std::ostream rob1Output;
      /*rob1Output << rob1;

      std::ostream rob2Output;
      rob2Output << rob2;*/
      }
   }
