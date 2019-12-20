// fileTests.cpp
// Testing for our mapping for different objects
//
// 2019-12-11: Added tests for matchingFilenames: tejas
// 2019-11-20: add appendToFile tests: combsc
// 2019-11-2: Initial Commit: combsc

#include "catch.hpp"
#include "file.hpp"
#include "basicString.hpp"
#include "vector.hpp"
#include <iostream>

TEST_CASE( "write to file", "[file]")
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
	}

TEST_CASE( "gether matching Filenames", "[file]" )
	{
	SECTION ( "test with slash" )
		{
		dex::string directory = "./tst/utils/fileTestsFolder/"; // Top Directory of SearchEngine
		dex::string pattern = ".cpp";
		dex::vector< dex::string > filenames = dex::matchingFilenames( directory, pattern );

		dex::unorderedSet< dex::string > correct;
		correct.insert("./tst/utils/fileTestsFolder/1.cpp");
		correct.insert("./tst/utils/fileTestsFolder/2.cpp");
		correct.insert("./tst/utils/fileTestsFolder/3.cpp");
		correct.insert("./tst/utils/fileTestsFolder/alpha/4.cpp");
		correct.insert("./tst/utils/fileTestsFolder/alpha/5.cpp");
		correct.insert("./tst/utils/fileTestsFolder/beta/6.cpp");
		correct.insert("./tst/utils/fileTestsFolder/beta/delta/7.cpp");
		REQUIRE( filenames.size( ) == correct.size( ) );
		for ( size_t i = 0; i < filenames.size( ); i++ )
			REQUIRE( correct.count( filenames[ i ] ) == 1 );
		}

	SECTION ( "test with no slash" )
		{
		dex::string directory = "./tst/utils/fileTestsFolder"; // Top Directory of SearchEngine
		dex::string pattern = ".cpp";
		dex::vector< dex::string > filenames = dex::matchingFilenames( directory, pattern );

		dex::unorderedSet< dex::string > correct;
		correct.insert("./tst/utils/fileTestsFolder/1.cpp");
		correct.insert("./tst/utils/fileTestsFolder/2.cpp");
		correct.insert("./tst/utils/fileTestsFolder/3.cpp");
		correct.insert("./tst/utils/fileTestsFolder/alpha/4.cpp");
		correct.insert("./tst/utils/fileTestsFolder/alpha/5.cpp");
		correct.insert("./tst/utils/fileTestsFolder/beta/6.cpp");
		correct.insert("./tst/utils/fileTestsFolder/beta/delta/7.cpp");
		REQUIRE( filenames.size( ) == correct.size( ) );
		for ( size_t i = 0; i < filenames.size( ); i++ )
			REQUIRE( correct.count( filenames[ i ] ) == 1 );
		}
	 }
