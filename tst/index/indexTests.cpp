// indexTests.cpp
// Tests for building the index
//
// 2019-12-04: File created: jasina, lougheem

#include <fcntl.h>
#include "catch.hpp"
#include "indexer/index.hpp"

using dex::index;

TEST_CASE( "create index chunk" )
	{
	const char filePath[ ] = "hello.txt";
	int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
	index::indexChunk initializingIndexChunk = index::indexChunk( fd );

	dex::string url = "hamiltoncshell.com";
	dex::vector < dex::string > anchorText;
	anchorText.pushBack( "best" );
	anchorText.pushBack( "shell" );
	dex::vector < dex::string > title;
	title.pushBack( "hamilton" );
	title.pushBack( "c" );
	title.pushBack( "shell" );
	dex::string titleString = "Hamilton C Shell 2012";
	dex::vector < dex::string > body;
	body.pushBack( "some" );
	body.pushBack( "junk" );
	body.pushBack( "and" );
	body.pushBack( "more" );
	body.pushBack( "junk" );

	initializingIndexChunk.addDocument( url, anchorText, title, titleString, body );

	}
