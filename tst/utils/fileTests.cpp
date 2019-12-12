// fileTests.cpp
// Testing for our mapping for different objects
//
// 2019-11-20: add appendToFile tests: combsc
// 2019-11-2: Initial Commit: combsc

#include "catch.hpp"
#include "file.hpp"
#include "basicString.hpp"
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

