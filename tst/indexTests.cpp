// indexTests.cpp
// Tests for building the index
//
// 2019-12-08: IndexStreamReader tests: lougheem
// 2019-12-04: File created: jasina, lougheem

#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include "catch.hpp"
#include "index.hpp"

#include <iostream>

using namespace dex::index;
using dex::vector;
using dex::string;

size_t getFileSize( int fileDescriptor )
	{
	struct stat fileInfo;
	fstat( fileDescriptor, &fileInfo );
	return fileInfo.st_size;
	}

// TODO: Add test case for closing and reopening an indexChunk

TEST_CASE( "create index chunk" )
	{
	const char filePath[ ] = "_in.dex";
	int fd = open( filePath, O_RDWR | O_CREAT, 0777 );

	if ( fd == -1 )
		exit( 1 );
	indexChunk initializingIndexChunk = indexChunk( fd );

	string url = "hamiltoncshell.com";
	vector < string > anchorText = { "best", "shell" };
	vector < string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector < string > body = { "some", "junk", "and", "more", "junk" };

	initializingIndexChunk.addDocument( url, anchorText, title, titleString, body );

	close( fd );
	}

TEST_CASE( "ISR functions on one document" )
	{
	const char filePath[ ] = "_in.dex";
	int fd = open( filePath, O_RDWR | O_CREAT, 0777 );

	if ( fd == -1 )
		exit( 1 );
	indexChunk initializingIndexChunk = indexChunk( fd );

	string url = "hamiltoncshell.com";
	vector < string > anchorText;
	vector < string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector < string > body = { "some", "junk", "and", "more", "junk", "and" };

	REQUIRE( initializingIndexChunk.addDocument( url, anchorText, title, titleString, body ) );

	// "junk" and "some" have their offsets set to 0 in the dictionary!!
	// 	some in the first word, so it should probably be have its offset be 0
	// 	removing the second occurence of junk makes its offset be 1 instead of 0
	indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk");
	indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.next( ) == 2 );
	REQUIRE( andISR.next( ) == 5 );
	REQUIRE( andISR.next( ) == -1 );
	REQUIRE( junkISR.next( ) == 1 );
	REQUIRE( junkISR.next( ) == 4 );
	REQUIRE( junkISR.next( ) == -1 );

	andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.seek( 0 ) == 2 );
	REQUIRE( andISR.seek( 2 ) == 2 );
	REQUIRE( andISR.seek( 3 ) == 5 );
	REQUIRE( andISR.seek( 6 ) == -1 );

	close( fd );
	}

TEST_CASE( "ISR functions on two documents" )
	{
	const char filePath[ ] = "_in.dex";
	int fd = open( filePath, O_RDWR | O_CREAT, 0777 );

	if ( fd == -1 )
		exit( 1 );
	indexChunk initializingIndexChunk = indexChunk( fd );

	string url = "hamiltoncshell.com";
	vector < string > anchorText;
	vector < string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector < string > body = { "some", "junk", "and", "more", "junk", "and" };

	string url2 = "google.com";
	vector < string > anchorText2;
	vector < string > title2 = { "evil", "company", "sell", "your", "data" };
	string titleString2 = "Evil company sell your data";
	vector < string > body2 = { "click", "this", "ad", "and", "you", "can", "live" };

	REQUIRE( initializingIndexChunk.addDocument( url, anchorText, title, titleString, body ) );
	REQUIRE( initializingIndexChunk.addDocument( url2, anchorText2, title2, titleString2, body2 ) );

	// "junk" and "some" have their offsets set to 0 in the dictionary!!
	// 	some in the first word, so it should probably be have its offset be 0
	// 	removing the second occurence of junk makes its offset be 1 instead of 0
	indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
	indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.next( ) == 2 );
	REQUIRE( andISR.next( ) == 5 );
	REQUIRE( andISR.next( ) == 13 );
	REQUIRE( andISR.next( ) == -1 );
	REQUIRE( junkISR.next( ) == 1 );
	REQUIRE( junkISR.next( ) == 4 );
	REQUIRE( junkISR.next( ) == -1 );

	andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.next( ) == 2 );
	REQUIRE( andISR.nextDocument( ) == 13 );

	andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.nextDocument( ) == 13 );

	close( fd );
	}

string quickBinaryToStringReversed( unsigned n )
	{
	string toReturn( );
	toReturn.reserve( 32 );
	while ( n != 0 )
		{
		if ( n % 2 )
			toReturn.append( '0' );
		else
			toReturn.append( '1' );
		n /= 2;
		}
	}

TEST_CASE( "ONE BIG DOC" )
	{
	SECTION( "Many unique words" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT, 0777 );

		if ( fd == -1 )
			exit( 1 );
		indexChunk initializingIndexChunk = indexChunk( fd );

		string url = "hamiltoncshell.com";
		vector < string > anchorText;
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";
		vector < string > body;

		}
	}