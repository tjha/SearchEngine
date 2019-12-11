// fileTests.cpp
// Testing for our mapping for different objects
//
// 2019-12-11: Added tests for matchingFilenames
// 2019-11-20: add appendToFile tests: combsc
// 2019-11-2: Initial Commit: combsc

#include "catch.hpp"
#include "file.hpp"
#include "basicString.hpp"
#include "vector.hpp"
#include <iostream>

TEST_CASE( "write to file", "[robotsTxt]")
	{
	SECTION( "mmap test" )
		{
		dex::writeToFile( "test.txt", "potato\n", 7 );
		char *file = dex::readFromFile( "test.txt" );
		size_t size = dex::getFileSize( "test.txt" );
		REQUIRE( file[0] == 'p' );
		REQUIRE( file[1] == 'o' );
		REQUIRE( file[2] == 't' );
		REQUIRE( file[3] == 'a' );
		REQUIRE( file[4] == 't' );
		REQUIRE( file[5] == 'o' );
		REQUIRE( file[6] == '\n' );
		REQUIRE( size == 7 );
		REQUIRE_THROWS_AS( dex::readFromFile( "test.txt", 3 ), dex::invalidArgumentException );
		remove( "test.txt" );
		}

	SECTION( "append test" )
		{
		dex::writeToFile( "test.txt", "potato\n", 7 );
		dex::appendToFile( "test.txt", "tomato\n", 7 );
		char *file = dex::readFromFile( "test.txt" );
		size_t size = dex::getFileSize( "test.txt" );
		REQUIRE( file[0] == 'p' );
		REQUIRE( file[1] == 'o' );
		REQUIRE( file[2] == 't' );
		REQUIRE( file[3] == 'a' );
		REQUIRE( file[4] == 't' );
		REQUIRE( file[5] == 'o' );
		REQUIRE( file[6] == '\n' );
		REQUIRE( file[7] == 't' );
		REQUIRE( file[8] == 'o' );
		REQUIRE( file[9] == 'm' );
		REQUIRE( file[10] == 'a' );
		REQUIRE( file[11] == 't' );
		REQUIRE( file[12] == 'o' );
		REQUIRE( file[13] == '\n' );
		REQUIRE( size == 14 );
		dex::string a;
		for ( long i = 0;  i < dex::getPageSize( );  ++i )
			a += 'z';
		dex::appendToFile( "test.txt", a.cStr( ), a.size( ) );
		file = dex::readFromFile( "test.txt", dex::getPageSize( ) );
		REQUIRE( file[ 0 ] == 'z' );
		remove( "test.txt" );
		}
		
	/*
	SECTION( "write unorderedMap< RobotTxt > into file" )
		{
		dex::unorderedMap< string, RobotTxt > robots { 10 };
		RobotTxt rob1( "domain1" );
		RobotTxt rob2( "domain2" );
		robots["domain1"] = rob1;
		robots["domain2"] = rob2;
		
		for ( auto it = robots.begin();  it != robots.cend();  ++it ) 
			{
			if ( it->second.getDomain( ) != "no-domain" )
				{
				//write to file
				std::cout << it->second;
				}
			}
		}
	*/
	}

TEST_CASE( "gether matching Filenames", "[File Match]" )
   {
   SECTION ( "get Tests.cpp starting from SearchEngine/" )
      {
      dex::string directory = "./"; // Top Directory of SearchEngine
      dex::string pattern = "Tests.cpp";
      dex::vector< dex::string > filenames = dex::matchingFilenames( directory, pattern );

      dex::vector< dex::string > correct;
      correct.pushBack("./tst/utfTests.cpp");
      correct.pushBack("./tst/redirectCacheTests.cpp");
      correct.pushBack("./tst/crawlerTests.cpp");
      correct.pushBack("./tst/fileTests.cpp");
      correct.pushBack("./tst/robotTxtTests.cpp");
      correct.pushBack("./tst/rankerTests.cpp");
      correct.pushBack("./tst/utils/unorderedMapTests.cpp");
      correct.pushBack("./tst/utils/encodeTests.cpp");
      correct.pushBack("./tst/utils/vectorTests.cpp");
      correct.pushBack("./tst/utils/algorithmTests.cpp");
      correct.pushBack("./tst/utils/functionalTests.cpp");
      correct.pushBack("./tst/utils/basicStringTests.cpp");
      correct.pushBack("./tst/utils/unorderedSetTests.cpp");
      correct.pushBack("./tst/utils/fileTests.cpp");
      correct.pushBack("./tst/parser/parserTests.cpp");
      correct.pushBack("./src/spinarak/crawlerTests.cpp");

      REQUIRE( filenames.size( ) == correct.size( ) );
      for ( size_t i = 0; i < filenames.size( ); i++ )
         REQUIRE( filenames[ i ] == correct[ i ] );
      }

   SECTION ( "get all .txt files starting from src/" )
      {
      dex::string directory = "src/";
      dex::string pattern = ".txt";
      dex::vector< dex::string > filenames = dex::matchingFilenames( directory, pattern );

      dex::vector< dex::string > correct;
      correct.pushBack("src/spinarak/seedlist.txt");
      correct.pushBack("src/crawler/savedCrawl.txt");
      correct.pushBack("src/crawler/savedRobots.txt");
      correct.pushBack("src/crawler/savedFrontier.txt");
      correct.pushBack("src/crawler/savedBrokenLinks.txt");
      correct.pushBack("src/crawler/savedTestOutput.txt");

      REQUIRE( filenames.size( ) == correct.size( ) );
      for ( size_t i = 0; i < filenames.size( ); i++ )
         REQUIRE( filenames[ i ] == correct[ i ] );
      }

    }
