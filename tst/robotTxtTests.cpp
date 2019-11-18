// robotTxtTests.cpp
// Testing for our robots class
//
// 2019-11-13: Add robots.txt parse tests: combsc
// 2019-11-02: better testing: combsc, jonas
// 2019-10-21: File creation: combsc

#include "catch.hpp"
#include <unistd.h>
#include "../src/spinarak/robots.hpp"
#include "../src/utils/unorderedMap.hpp"
#include "../src/utils/basicString.hpp"
#include "../src/utils/vector.hpp"

using dex::RobotTxt;
using dex::string;
using dex::vector;


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
      REQUIRE ( !rob2.canVisitPath( "/disallowed/path/" ) );
      rob2.updateLastVisited( );
      REQUIRE ( !rob2.canVisitPath( "/" ) );
      REQUIRE ( !rob2.canVisitPath( "/secret/path/" ) );
      sleep(3);
      REQUIRE ( rob2.canVisitPath( "/" ) );
      REQUIRE ( rob2.canVisitPath( "/secret/path/" ) );
      }

   SECTION( "Different Robots" )
      {
      string url = "https://domain.com";
      RobotTxt rob1 = RobotTxt( url, 2);
      rob1.addPathsAllowed( "/" );
      rob1.addPathsAllowed( "/secret/path/" );
      rob1.addPathsDisallowed( "/disallowed/path/");
      RobotTxt rob2 = RobotTxt( url, 2);
      rob2.addPathsAllowed( "/" );
      rob2.addPathsDisallowed( "/disallowed/path/");
      RobotTxt rob3 = RobotTxt( url, 2);
      rob3.addPathsAllowed( "/" );
      RobotTxt rob4 = RobotTxt( url, 1);
      rob4.addPathsAllowed( "/" );
      RobotTxt rob5 = RobotTxt( url, 2);
      rob5.addPathsAllowed( "/" );
      rob5.addPathsAllowed( "/secret/path/" );
      rob5.addPathsDisallowed( "/disallowed/path/");
      vector< RobotTxt > robots;
      robots.pushBack( rob1 );
      robots.pushBack( rob2 );
      robots.pushBack( rob3 );
      robots.pushBack( rob4 );

      REQUIRE( rob1.compress( ) == rob5.compress( ) );
      for ( int i = 0;  i < 4;  ++i )
         {
         for ( int j = i + 1;  j < 4;  ++j )
            {
            REQUIRE( robots[ i ].compress( ) != robots[ j ].compress( ) );
            }
         }
      }
   }

TEST_CASE( "parsing a robots.txt file", "[robotTxt]" )
   {
   // basic case
   string dummy = "";
   string exampleString0 = dummy + "User-agent: notUs\n" + 
         "Disallow: /\n" +
         "Crawl-delay: 500\n" +
         "User-agent: *\n" +
         "Disallow: /secret/\n" +
         "Allow: /secret/okpath/\n" +
         "Allow: /secret/okfile\n" +
         "Crawl-delay: 0\n" +
         "User-agent: susBot\n" +
         "Disallow: /\n";
   RobotTxt rob0 = RobotTxt( "https://www.domain.com", exampleString0 );
   REQUIRE( rob0.getDelay( ) == 0 );
   REQUIRE( rob0.canVisitPath( "/secret/okpath/otherfile") );
   REQUIRE( rob0.canVisitPath( "/secret/okfile") );
   REQUIRE( rob0.canVisitPath( "/somepublicthing") );
   REQUIRE( !rob0.canVisitPath( "/secret/file") );

   // spaces
   string exampleString1 = dummy + "User-agent: notUs\n" + 
         "Disallow: / \n" +
         "Crawl-delay: 500 \n" +
         "User-agent: * \n" +
         "Disallow: /secret/ \n" +
         "Allow: /secret/okpath/ \n" +
         "Allow: /secret/okfile \n" +
         "Crawl-delay: 0 \n" +
         "User-agent: susBot \n" +
         "Disallow: / \n";
   RobotTxt rob1 = RobotTxt( "https://www.domain.com", exampleString1 );
   REQUIRE( rob1.getDelay( ) == 0 );
   REQUIRE( rob1.canVisitPath( "/secret/okpath/otherfile") );
   REQUIRE( rob1.canVisitPath( "/secret/okfile") );
   REQUIRE( rob1.canVisitPath( "/somepublicthing") );
   REQUIRE( !rob1.canVisitPath( "/secret/file") );
   }

