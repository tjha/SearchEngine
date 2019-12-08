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

TEST_CASE( "create index chunk" )
	{
	const char filePath[ ] = "hello.txt";
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

TEST_CASE( "ISR shit" )
	{
	vector < size_t > deltas = { 5, 300, 2, 83, 900 };
	vector < unsigned char > encodedLengths;
	vector < unsigned char > utf8Delta;
	for ( int index = 0;  index < deltas.size( );  index++ )
		{
		vector < unsigned char > encodedDelta = dex::utf::encoder < size_t >( )( deltas[ index ] );
		encodedLengths.pushBack( encodedDelta.size( ) );
		for ( unsigned char b : encodedDelta )
			utf8Delta.pushBack( b );
		}

	size_t currentPostOffset = 0;
	unsigned char posts[ 50 ];

	for ( unsigned char b : utf8Delta )
		posts[ currentPostOffset++ ] = b;

	size_t currentEncodedOffset = 0;
	REQUIRE( dex::utf::decodeSafe( posts + currentEncodedOffset ) == deltas[ 0 ] );
	std::cout << dex::utf::decodeSafe( posts + currentEncodedOffset ) << "\n";
	currentEncodedOffset += encodedLengths[ 0 ];

	for ( int index = 1;  index < deltas.size( );  index++ )
		{
		REQUIRE( dex::utf::decodeSafe( posts + currentEncodedOffset ) == deltas[ index ] );
		std::cout << dex::utf::decodeSafe( posts + currentEncodedOffset ) << "\n";
		currentEncodedOffset += encodedLengths[ index ];
		}
	}

TEST_CASE( "ISR functions" )
	{
	const char filePath[ ] = "hello.txt";
	int fd = open( filePath, O_RDWR | O_CREAT, 0777 );

	if ( fd == -1 )
		exit( 1 );
	indexChunk initializingIndexChunk = indexChunk( fd );

	string url = "hamiltoncshell.com";
	vector < string > anchorText = { "best", "shell" };
	vector < string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector < string > body = { "some", "junk", "and", "more", "junk" };

	REQUIRE( initializingIndexChunk.addDocument( url, anchorText, title, titleString, body ) );

	std::cout << "Initialized indexChunk\n";

	string junkString = "junk";
	indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( junkString, &initializingIndexChunk);
	string someString = "some";
	indexChunk::indexStreamReader someISR = indexChunk::indexStreamReader( someString, &initializingIndexChunk);
	
	// std::cout << dex::utf::decodeSafe( *junkISR.next( ) ) << "\n";

	close( fd );

	}
