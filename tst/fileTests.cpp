// fileTests.cpp
// Testing for our mapping for different objects
//
// 2019-11-2: Initial Commit: combsc

#include "catch.hpp"
#include "../src/utils/file.hpp"

TEST_CASE( "write to file", "[robotsTxt]")
	{
	SECTION( "mmap test")
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

