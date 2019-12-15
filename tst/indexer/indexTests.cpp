// indexTests.cpp
// Tests for building the index
//
// 2019-12-08: IndexStreamReader tests: lougheem
// 2019-12-04: File created: jasina, lougheem

#include <cstddef>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "catch.hpp"
#include "file.hpp"
#include "index.cpp"
#include "utf.hpp"

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

/*
TEST_CASE( "lougheed's fun read indexChunk test" )
	{
	std::cout << "Running lougheed's fun read indexChunk test ;)\n";
	const char filePath[ ] = "2_in.dex";
	int fd = open( filePath, O_RDWR | O_CREAT, 0777 );

	if ( fd == -1 )
		exit( 1 );
	indexChunk initializingIndexChunk = indexChunk( fd, false );

	indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );
	int location = 0;
	while ( location != -1 )
		{
		location = andISR.next( );
		std::cout << "\t-" << location << "\n";
		}
	close( fd );
	}
*/

TEST_CASE( "create index chunk" )
	{
	const char filePath[ ] = "_in.dex";
	int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

	if ( fd == -1 )
		exit( 1 );
	indexChunk initializingIndexChunk = indexChunk( fd );

	string url = "hamiltoncshell.com";
	vector < string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector < string > body = { "some", "junk", "and", "more", "junk" };

	initializingIndexChunk.addDocument( url, title, titleString, body );

	close( fd );
	}

TEST_CASE( "ISR functions on one document" )
	{
	const char filePath[ ] = "_in.dex";
	int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

	if ( fd == -1 )
		exit( 1 );
	indexChunk initializingIndexChunk = indexChunk( fd );

	string url = "hamiltoncshell.com";
	vector < string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector < string > body = { "some", "junk", "and", "more", "junk", "and" };

	REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

	// "junk" and "some" have their offsets set to 0 in the dictionary!!
	// 	some in the first word, so it should probably be have its offset be 0
	// 	removing the second occurence of junk makes its offset be 1 instead of 0
	indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk");
	indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.next( ) == 2 );
	REQUIRE( andISR.next( ) == 5 );
	REQUIRE( andISR.next( ) == static_cast < size_t >( -1 ) );
	REQUIRE( junkISR.next( ) == 1 );
	REQUIRE( junkISR.next( ) == 4 );
	REQUIRE( junkISR.next( ) == static_cast < size_t >( -1 ) );

	andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.seek( 0 ) == 2 );
	REQUIRE( andISR.seek( 2 ) == 2 );
	REQUIRE( andISR.seek( 3 ) == 5 );
	REQUIRE( andISR.seek( 6 ) == static_cast < size_t >( -1 ) );

	REQUIRE( andISR.seek( 4 ) == 5 );
	REQUIRE( andISR.seek( 1 ) == 2 );
	REQUIRE( andISR.seek( 0 ) == 2 );

	close( fd );
	}

TEST_CASE( "ISR functions on two documents" )
	{
	const char filePath[ ] = "_in.dex";
	int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

	if ( fd == -1 )
		exit( 1 );
	indexChunk initializingIndexChunk = indexChunk( fd );

	string url = "hamiltoncshell.com";
	vector < string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector < string > body = { "some", "junk", "and", "more", "junk", "and" };

	string url2 = "google.com";
	vector < string > title2 = { "evil", "company", "sell", "your", "data" };
	string titleString2 = "Evil company sell your data";
	vector < string > body2 = { "click", "this", "ad", "and", "you", "can", "live" };

	REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
	REQUIRE( initializingIndexChunk.addDocument( url2, title2, titleString2, body2 ) );

	// "junk" and "some" have their offsets set to 0 in the dictionary!!
	// 	some in the first word, so it should probably be have its offset be 0
	// 	removing the second occurence of junk makes its offset be 1 instead of 0
	indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
	indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.next( ) == 2 );
	REQUIRE( andISR.next( ) == 5 );
	REQUIRE( andISR.next( ) == 13 );
	REQUIRE( andISR.next( ) == static_cast < size_t >( -1 ) );
	REQUIRE( junkISR.next( ) == 1 );
	REQUIRE( junkISR.next( ) == 4 );
	REQUIRE( junkISR.next( ) == static_cast < size_t >( -1 ) );

	andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.next( ) == 2 );
	REQUIRE( andISR.nextDocument( ) == 13 );

	andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

	REQUIRE( andISR.nextDocument( ) == 13 );

	REQUIRE( andISR.seek( 6 ) == 13 );

	close( fd );
	}

TEST_CASE( "ONE BIG DOC" )
	{
	SECTION( "Single word more than a single postsChunk" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );
		indexChunk initializingIndexChunk = indexChunk( fd );

		string word = "someWord";
		string url = "hamiltoncshell.com";
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";

		// Each postsChunk has a byte posts[ ] of size 1<<12
		vector < string > body( 1<<12, "someWord" );

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		indexChunk::indexStreamReader wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, "someWord" );
		REQUIRE( body.size( ) == ( 1<<12 ) );
		for ( int iters = 0;  iters < (1<<12) - 2;  iters++ )
			REQUIRE( wordISR.next( ) == iters );
		REQUIRE( wordISR.next( ) == ( 1<<12 ) - 2 );
		REQUIRE( wordISR.next( ) == ( 1<<12 ) - 1 );
		REQUIRE( wordISR.next( ) == static_cast < size_t >( -1 ) );
		}

	SECTION( "Interweaving of linked posting lists" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );
		indexChunk initializingIndexChunk = indexChunk( fd );

		string word1 = "uno";
		string word2 = "dos";
		string url = "hamiltoncshell.com";
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";

		// Each postsChunk has a byte posts[ ] of size 1<<12
		vector < string > body1( 1<<11, "uno" );
		vector < string > body2( 1<<11, "dos" );

		REQUIRE( initializingIndexChunk.addDocument( url + word1, title, titleString, body1 ) );
		REQUIRE( initializingIndexChunk.addDocument( url + word2, title, titleString, body2 ) );
		REQUIRE( initializingIndexChunk.addDocument( url + word1 + word1, title, titleString, body1 ) );
		REQUIRE( initializingIndexChunk.addDocument( url + word2 + word2, title, titleString, body2 ) );
		indexChunk::indexStreamReader unoISR = indexChunk::indexStreamReader( &initializingIndexChunk, "uno" );
		indexChunk::indexStreamReader dosISR = indexChunk::indexStreamReader( &initializingIndexChunk, "dos" );

		int location;
		for ( location = 0;  location < (1<<11);  location++ )
			REQUIRE( unoISR.next( ) == location );

		for ( ;  location < (1<<11);  location++ )
			REQUIRE( dosISR.next( ) == location + title.size( ) + ( 1<<11 ) );

		}

	SECTION( "Read indexChunk from a file" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );

		{
		indexChunk initializingIndexChunk = indexChunk( fd );

		string url = "hamiltoncshell.com";
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";
		vector < string > body = { "some", "junk", "and", "more", "junk", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}

		close( fd );
		fd = open( filePath, O_RDWR, 0777 );
		REQUIRE( fd != -1 );

		indexChunk fromFileIndexChunk = indexChunk( fd, false );
		indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "junk");
		indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "and" );

		REQUIRE( andISR.next( ) == 2 );
		REQUIRE( andISR.next( ) == 5 );
		REQUIRE( andISR.next( ) == static_cast < size_t >( -1 ) );
		REQUIRE( junkISR.next( ) == 1 );
		REQUIRE( junkISR.next( ) == 4 );
		REQUIRE( junkISR.next( ) == static_cast < size_t >( -1 ) );

		andISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "and" );

		REQUIRE( andISR.seek( 0 ) == 2 );
		REQUIRE( andISR.seek( 2 ) == 2 );
		REQUIRE( andISR.seek( 3 ) == 5 );
		REQUIRE( andISR.seek( 6 ) == static_cast < size_t >( -1 ) );
		}

	SECTION( "Read indexChunk from a file" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );

		{
		indexChunk initializingIndexChunk = indexChunk( fd );

		string word = "someWord";
		string url = "hamiltoncshell.com";
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";

		// Each postsChunk has a byte posts[ ] of size 1<<12
		vector < string > body( 1<<12, "someWord" );

		REQUIRE( body.size( ) == ( 1<<12 ) );
		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}

		close( fd );
		fd = open( filePath, O_RDWR, 0777 );
		REQUIRE( fd != -1 );

		indexChunk fromFileIndexChunk = indexChunk( fd, false );
		indexChunk::indexStreamReader wordISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "someWord" );
		for ( int iters = 0;  iters < (1<<12) - 2;  iters++ )
			REQUIRE( wordISR.next( ) == iters );
		REQUIRE( wordISR.next( ) == ( 1<<12 ) - 2 );
		REQUIRE( wordISR.next( ) == ( 1<<12 ) - 1 );
		REQUIRE( wordISR.next( ) == static_cast < size_t >( -1 ) );
		}
	SECTION( "Fill an indexChunk" )
		{
		string filename = "shakespeare.txt";
		}

	SECTION( "Many unique words" )
		{

		}
	}

