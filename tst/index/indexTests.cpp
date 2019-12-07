// indexTests.cpp
// Tests for building the index
//
// 2019-12-04: File created: jasina, lougheem

#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include "catch.hpp"
#include "index.hpp"

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
	vector < string > anchorText;
	anchorText.pushBack( "best" );
	anchorText.pushBack( "shell" );
	vector < string > title;
	title.pushBack( "hamilton" );
	title.pushBack( "c" );
	title.pushBack( "shell" );
	string titleString = "Hamilton C Shell 2012";
	vector < string > body;
	body.pushBack( "some" );
	body.pushBack( "junk" );
	body.pushBack( "and" );
	body.pushBack( "more" );
	body.pushBack( "junk" );

	initializingIndexChunk.addDocument( url, anchorText, title, titleString, body );

	close( fd );
	}
