// index.test.cpp
// Tests for building the index
//
// 2019-12-08: IndexStreamReader tests: lougheem
// 2019-12-04: File created: jasina, lougheem

#include <cstddef>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "catch.hpp"
#include "indexer/indexer.hpp"
#include "utils/file.hpp"
#include "utils/utf.hpp"

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
	const char filePath[ ] = "_in.dex";
	int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

	if ( fd == -1 )
		exit( 1 );
	indexChunk initializingIndexChunk = indexChunk( fd );

	string url = "hamiltoncshell.com";
	vector< string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector< string > body = { "some", "junk", "and", "more", "junk" };

	initializingIndexChunk.addDocument( url, title, titleString, body );

	close( fd );
	}

TEST_CASE( "create index chunk with bad HTML" )
	{
	SECTION( "empty title vector" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );
		indexChunk initializingIndexChunk = indexChunk( fd );

		string url = "hamiltoncshell.com";
		vector< string > title = { };
		string titleString = "Hamilton C Shell 2012";
		vector< string > body = { "some", "junk", "and", "more", "junk", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		string url2 = "hammyseeshell.biz";
		vector< string > title2 = { "good", "website" };
		string titleString2 = "This is a good website";
		vector< string > body2 = { "this", "is", "the", "body", "text" };

		REQUIRE( initializingIndexChunk.addDocument( url2, title2, titleString2, body2 ) );

		close( fd );
		}

	SECTION( "empty body vector" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );
		indexChunk initializingIndexChunk = indexChunk( fd );

		string url = "hamiltoncshell.com";
		vector< string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";
		vector< string > body = { };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		string url2 = "hammyseeshell.biz";
		vector< string > title2 = { "good", "website" };
		string titleString2 = "This is a good website";
		vector< string > body2 = { "this", "is", "the", "body", "text" };

		REQUIRE( initializingIndexChunk.addDocument( url2, title2, titleString2, body2 ) );

		close( fd );
		}
	}

TEST_CASE( "Basic ISR functions for body and title" )
	{
	const char filePath[ ] = "_in.dex";
	int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

	if ( fd == -1 )
		exit( 1 );

	SECTION( "ISR functions on single title and body words" )
		{
		indexChunk initializingIndexChunk = indexChunk( fd );

		REQUIRE( initializingIndexChunk.addDocument( "a.com", { "art" }, "art", { "body" } ) );

		indexChunk::indexStreamReader decoratedArtISR = indexChunk::indexStreamReader( &initializingIndexChunk, "#art" );
		indexChunk::indexStreamReader bodyISR = indexChunk::indexStreamReader( &initializingIndexChunk, "body" );

		// initializingIndexChunk.printDictionary( );
		REQUIRE( bodyISR.next( ) == 1 );
		REQUIRE( decoratedArtISR.next( ) == 3 );
		}

	SECTION( "ISR functions on nondecorated title" )
		{
		indexChunk initializingIndexChunk = indexChunk( fd );

		REQUIRE( initializingIndexChunk.addDocument( "a.com", { "art" }, "art", { "body" } ) );

		indexChunk::indexStreamReader nondecoratedArtISR = indexChunk::indexStreamReader( &initializingIndexChunk, "art" );

		REQUIRE( nondecoratedArtISR.next( ) == nondecoratedArtISR.npos );
		}

	SECTION( "ISR functions on empty body" )
		{
		indexChunk initializingIndexChunk = indexChunk( fd );

		// empty body vector
		REQUIRE( initializingIndexChunk.addDocument( "amazon.com", { "art" }, "art", { } ) );

		indexChunk::indexStreamReader artISR = indexChunk::indexStreamReader( &initializingIndexChunk, "art" );
		indexChunk::indexStreamReader decoratedArtISR = indexChunk::indexStreamReader( &initializingIndexChunk, "#art" );

		REQUIRE( decoratedArtISR.next( ) == 2 );
		REQUIRE( artISR.next( ) == artISR.npos );
		}

	SECTION( "Nonexistent word ISR" )
		{
		indexChunk chunk = indexChunk( fd );

		REQUIRE( chunk.addDocument( "a.com", { "title" }, "title", { "body" } ) );
		indexChunk::indexStreamReader nonexistentISR = indexChunk::indexStreamReader( &chunk, "nonexistent" );
		}

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
	vector< string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector< string > body = { "some", "junk", "and", "more", "junk", "and", "junk" };

	REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

	indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );
	indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk");

	SECTION( "next" )
		{
		REQUIRE( andISR.next( ) == 3 );
		REQUIRE( andISR.next( ) == 6 );
		REQUIRE( andISR.next( ) == andISR.npos );

		REQUIRE( junkISR.next( ) == 2 );
		REQUIRE( junkISR.next( ) == 5 );
		REQUIRE( junkISR.next( ) == 7 );
		REQUIRE( junkISR.next( ) == junkISR.npos );
		}

	SECTION( "seek" )
		{
		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );
		REQUIRE( andISR.seek( 0 ) == 3 );
		REQUIRE( andISR.seek( 3 ) == 3 );
		REQUIRE( andISR.seek( 4 ) == 6 );
		REQUIRE( andISR.seek( 7 ) == andISR.npos );

		REQUIRE( andISR.seek( 5 ) == 6 );
		REQUIRE( andISR.seek( 2 ) == 3 );
		REQUIRE( andISR.seek( 0 ) == 3 );
		}

	SECTION( "seek with next" )
		{
		junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
		REQUIRE( junkISR.next( ) == 2 );
		REQUIRE( junkISR.seek( 4 ) == 5 );
		REQUIRE( junkISR.next( ) == 7 );

		junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
		REQUIRE( junkISR.seek( 2 ) == 2 );
		REQUIRE( junkISR.next( ) == 5 );
		REQUIRE( junkISR.next( ) == 7 );

		junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
		REQUIRE( junkISR.seek( 0 ) == 2 );
		REQUIRE( junkISR.next( ) == 5 );
		REQUIRE( junkISR.next( ) == 7 );

		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );
		REQUIRE( andISR.seek( 0 ) == 3 );
		REQUIRE( andISR.next( ) == 6 );
		}

	close( fd );
	}

TEST_CASE( "ISR functions on multiple documents" )
	{
	SECTION( "ISR functions on two documents" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );

		indexChunk initializingIndexChunk = indexChunk( fd );

		string url = "hamiltoncshell.com";
		vector< string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";
		vector< string > body = { "some", "junk", "and", "more", "junk", "and" };

		string url2 = "google.com";
		vector< string > title2 = { "evil", "company", "sell", "your", "data" };
		string titleString2 = "Evil company sell your data";
		vector< string > body2 = { "click", "this", "ad", "and", "you", "can", "live" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url2, title2, titleString2, body2 ) );

		indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
		indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

		REQUIRE( andISR.next( ) == 3 );
		REQUIRE( andISR.next( ) == 6 );
		REQUIRE( andISR.next( ) == 15 );
		REQUIRE( andISR.next( ) == andISR.npos );
		REQUIRE( junkISR.next( ) == 2 );
		REQUIRE( junkISR.next( ) == 5 );
		REQUIRE( junkISR.next( ) == junkISR.npos );

		indexChunk::indexStreamReader endOfDocumentISR( &initializingIndexChunk, "" );
		REQUIRE( endOfDocumentISR.next( ) == 0 );
		REQUIRE( endOfDocumentISR.next( ) == 11 );
		REQUIRE( endOfDocumentISR.next( ) == 25 );
		REQUIRE( endOfDocumentISR.next( ) == endOfDocumentISR.npos );

		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

		REQUIRE( andISR.next( ) == 3 );
		REQUIRE( andISR.nextDocument( ) == 15 );

		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

		REQUIRE( andISR.nextDocument( ) == 3 );

		REQUIRE( andISR.seek( 7 ) == 15 );

		close( fd );
		}

	SECTION( "ISR functions with empty title and body vectors" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );
		indexChunk initializingIndexChunk = indexChunk( fd );

		string url0 = "hamiltoncshell.com";
		string url1 = "hammyseeshell.biz";
		string url2 = "jammyhammy.edu";
		string url3 = "fannywanny.io";
		string url4 = "marypoppins.io";
		string url5 = "soundofmusic.io";
		vector< string > title = { "hamilton", "c", "shell" };
		vector< string > emptyTitle = { };
		string titleString0 = "Hamilton C Shell 2012";
		string titleString1 = "Best Website";
		string titleString2 = "Second Best Website";
		string titleString3 = "Third Best Website";
		string titleString4 = "Movie";
		string titleString5 = "Other Movie";
		vector< string > body = { "this", "is", "the", "body", "text" };
		vector< string > emptyBody = { };

		REQUIRE( initializingIndexChunk.addDocument( url0, emptyTitle, titleString0, emptyBody ) );
		REQUIRE( initializingIndexChunk.addDocument( url1, title, titleString1, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url2, emptyTitle, titleString2, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url3, title, titleString3, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url4, title, titleString4, emptyBody ) );
		REQUIRE( initializingIndexChunk.addDocument( url5, title, titleString5, body ) );

		indexChunk::indexStreamReader bodyISR = indexChunk::indexStreamReader( &initializingIndexChunk, "body" );
		indexChunk::indexStreamReader hamiltonISR = indexChunk::indexStreamReader( &initializingIndexChunk, "hamilton" );
		REQUIRE( bodyISR.next( ) == 6 );
		REQUIRE( bodyISR.next( ) == 16 );
		REQUIRE( bodyISR.next( ) == 23 );
		REQUIRE( bodyISR.next( ) == 38 );
		REQUIRE( bodyISR.next( ) == bodyISR.npos );

		REQUIRE( hamiltonISR.next( ) == hamiltonISR.npos );

		hamiltonISR = indexChunk::indexStreamReader( &initializingIndexChunk, "#hamilton" );
		REQUIRE( hamiltonISR.next( ) == 9 );
		REQUIRE( hamiltonISR.next( ) == 26 );
		REQUIRE( hamiltonISR.next( ) == 31);
		REQUIRE( hamiltonISR.next( ) == 41 );

		close( fd );
		}
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
		vector< string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";

		// Each postsChunk has a byte posts[ ] of size 1 << 10, so we just choose something larger than that
		vector< string > body( 1 << 12, word );

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		indexChunk::indexStreamReader wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, word );
		for ( size_t i = 0;  i < body.size( );  ++i )
			REQUIRE( wordISR.next( ) == i + 1 );
		REQUIRE( wordISR.next( ) == wordISR.npos );

		wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, word );
		for ( size_t i = 0;  i < body.size( ) - 1;  ++i )
			{
			REQUIRE( wordISR.seek( 0 ) == 1 );
			REQUIRE( wordISR.seek( i + 1 ) == i + 1 );
			REQUIRE( wordISR.next( ) == i + 2 );
			}
		REQUIRE( wordISR.seek( body.size( ) + 1 ) == wordISR.npos );
		REQUIRE( wordISR.next( ) == wordISR.npos );

		wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, word );
		for ( size_t i = 0;  i < ( 1 << 6 ) - 1;  ++i )
			{
			REQUIRE( wordISR.seek( ( i << 6 ) + 1 ) == ( i << 6 ) + 1 );
			REQUIRE( wordISR.next( ) == ( i << 6 ) + 2 );
			}

		close( fd );
		}

	SECTION( "Interweaving of linked posting lists" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );
		indexChunk initializingIndexChunk = indexChunk( fd );

		string uno = "uno";
		string dos = "dos";
		string url = "hamiltoncshell.com";
		vector< string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";

		// Each postsChunk has a byte posts[ ] of size 1 << 10
		vector< string > body1( 1 << 11, "uno" );
		vector< string > body2( 1 << 11, "dos" );

		// This is a helper variable for the for loops below
		size_t offset;

		REQUIRE( initializingIndexChunk.addDocument( url + uno, title, titleString, body1 ) );
		REQUIRE( initializingIndexChunk.addDocument( url + dos, title, titleString, body2 ) );
		REQUIRE( initializingIndexChunk.addDocument( url + uno + uno, title, titleString, body1 ) );
		REQUIRE( initializingIndexChunk.addDocument( url + dos + dos, title, titleString, body2 ) );
		indexChunk::indexStreamReader unoISR = indexChunk::indexStreamReader( &initializingIndexChunk, uno );
		indexChunk::indexStreamReader dosISR = indexChunk::indexStreamReader( &initializingIndexChunk, dos );

		for ( size_t location = 0;  location < body1.size( );  ++location )
			REQUIRE( unoISR.next( ) == location + 1 );
		offset = body1.size( ) + 1 + title.size( ) + 1 + body2.size( ) + 1 + title.size( ) + 1;
		for ( size_t location = offset; location < offset + body1.size( );  ++location )
			REQUIRE( unoISR.next( ) == location + 1 );
		REQUIRE( unoISR.next( ) == unoISR.npos );

		offset = body1.size( ) + 1 + title.size( ) + 1;
		for ( size_t location = offset;  location < offset + body2.size( );  ++location )
			REQUIRE( dosISR.next( ) == location + 1 );
		offset = body1.size( ) + 1 + title.size( ) + 1 + body2.size( ) + 1 + title.size( ) + 1
				+ body1.size( ) + 1 + title.size( ) + 1;
		for ( size_t location = offset;  location < offset + body2.size( );  ++location )
			REQUIRE( dosISR.next( ) == location + 1 );
		REQUIRE( dosISR.next( ) == dosISR.npos );
		}
	}

TEST_CASE( "Read indexChunk from a file" )
	{
	SECTION( "one big doc")
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );

		string word = "someWord";
		string url = "hamiltoncshell.com";
		vector< string > title = { "hammy" };
		string titleString = "Hammy";

		// Create a scope here so that the index chunk calls its destructor
			{
			indexChunk initializingIndexChunk = indexChunk( fd );

			// Each postsChunk has a byte posts[ ] of size 1 << 10
			vector< string > body( 1 << 10, word );

			REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
			initializingIndexChunk.skip( 1 << 28 );
			REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
			indexChunk::indexStreamReader wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, word );
			for ( size_t iters = 0;  iters < ( 1 << 10 );  ++iters )
				REQUIRE( wordISR.next( ) == iters + 1 );
			for ( size_t iters = 0;  iters < ( 1 << 10 );  ++iters )
				REQUIRE( wordISR.next( ) == ( ( 1 << 10 ) + 1 + 1 + 1 ) + ( 1 << 28 ) + iters + 1 );
			REQUIRE( wordISR.next( ) == wordISR.npos );
			}

		close( fd );

		fd = open( filePath, O_RDWR, 0777 );
		REQUIRE( fd != -1 );

		indexChunk fromFileIndexChunk = indexChunk( fd, false );

		indexChunk::indexStreamReader wordISR = indexChunk::indexStreamReader( &fromFileIndexChunk, word );
		for ( size_t iters = 0;  iters < ( 1 << 10 );  ++iters )
				REQUIRE( wordISR.next( ) == iters + 1 );
		for ( size_t iters = 0;  iters < ( 1 << 10 );  ++iters )
			REQUIRE( wordISR.next( ) == ( ( 1 << 10 ) + 1 + 1 + 1 ) + ( 1 << 28 ) + iters + 1 );
		REQUIRE( wordISR.next( ) == wordISR.npos );

		close( fd );
		}
	SECTION( "ISR funtions on one document" )
		{
		const char filePath[ ] = "_in.dex";
		int fd = open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );

		// Create a scope here so that the index chunk calls its destructor
			{
			indexChunk initializingIndexChunk = indexChunk( fd );

			string url = "hamiltoncshell.com";
			vector< string > title = { "hamilton", "c", "shell" };
			string titleString = "Hamilton C Shell 2012";
			vector< string > body = { "some", "junk", "and", "more", "junk", "and", "junk" };

			REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
			}

		close( fd );
		fd = open( filePath, O_RDWR, 0777 );
		REQUIRE( fd != -1 );

		indexChunk fromFileIndexChunk = indexChunk( fd, false );

		indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "and" );
		indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "junk");

		REQUIRE( andISR.next( ) == 3 );
		REQUIRE( andISR.next( ) == 6 );
		REQUIRE( andISR.next( ) == andISR.npos );

		REQUIRE( junkISR.next( ) == 2 );
		REQUIRE( junkISR.next( ) == 5 );
		REQUIRE( junkISR.next( ) == 7 );
		REQUIRE( junkISR.next( ) == junkISR.npos );

		andISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "and" );
		REQUIRE( andISR.seek( 0 ) == 3 );
		REQUIRE( andISR.seek( 3 ) == 3 );
		REQUIRE( andISR.seek( 4 ) == 6 );
		REQUIRE( andISR.seek( 7 ) == andISR.npos );

		REQUIRE( andISR.seek( 5 ) == 6 );
		REQUIRE( andISR.seek( 2 ) == 3 );
		REQUIRE( andISR.seek( 0 ) == 3 );

		junkISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "junk" );
		REQUIRE( junkISR.next( ) == 2 );
		REQUIRE( junkISR.seek( 4 ) == 5 );
		REQUIRE( junkISR.next( ) == 7 );

		junkISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "junk" );
		REQUIRE( junkISR.seek( 2 ) == 2 );
		REQUIRE( junkISR.next( ) == 5 );
		REQUIRE( junkISR.next( ) == 7 );

		junkISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "junk" );
		REQUIRE( junkISR.seek( 0 ) == 2 );
		REQUIRE( junkISR.next( ) == 5 );
		REQUIRE( junkISR.next( ) == 7 );

		andISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "and" );
		REQUIRE( andISR.seek( 0 ) == 3 );
		REQUIRE( andISR.next( ) == 6 );

		close( fd );
		}
	}
