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
	vector < string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector < string > body = { "some", "junk", "and", "more", "junk" };

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
		vector < string > title = { };
		string titleString = "Hamilton C Shell 2012";
		vector < string > body = { "some", "junk", "and", "more", "junk", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		string url2 = "hammyseeshell.biz";
		vector < string > title2 = { "good", "website" };
		string titleString2 = "This is a good website";
		vector < string > body2 = { "this", "is", "the", "body", "text" };

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
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";
		vector < string > body = { };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		string url2 = "hammyseeshell.biz";
		vector < string > title2 = { "good", "website" };
		string titleString2 = "This is a good website";
		vector < string > body2 = { "this", "is", "the", "body", "text" };

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
		REQUIRE( bodyISR.next( ) == 0 );
		REQUIRE( decoratedArtISR.next( ) == 2 );
		}

	SECTION( "ISR functions on nondecorated title" )
		{
		indexChunk initializingIndexChunk = indexChunk( fd );

		REQUIRE( initializingIndexChunk.addDocument( "a.com", { "art" }, "art", { "body" } ) );

		indexChunk::indexStreamReader nondecoratedArtISR = indexChunk::indexStreamReader( &initializingIndexChunk, "art" );

		REQUIRE( nondecoratedArtISR.next( ) == static_cast< size_t >( -1 ) );
		}

	SECTION( "ISR functions on empty body" )
		{
		indexChunk initializingIndexChunk = indexChunk( fd );

		// empty body vector
		REQUIRE( initializingIndexChunk.addDocument( "amazon.com", { "art" }, "art", { } ) );

		indexChunk::indexStreamReader artISR = indexChunk::indexStreamReader( &initializingIndexChunk, "art" );
		indexChunk::indexStreamReader decoratedArtISR = indexChunk::indexStreamReader( &initializingIndexChunk, "#art" );

		REQUIRE( decoratedArtISR.next( ) == 1 );
		REQUIRE( artISR.next( ) == static_cast< size_t >( - 1 ) );
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
	vector < string > title = { "hamilton", "c", "shell" };
	string titleString = "Hamilton C Shell 2012";
	vector < string > body = { "some", "junk", "and", "more", "junk", "and", "junk" };

	REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

	indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );
	indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk");

	SECTION( "next" )
		{
		REQUIRE( andISR.next( ) == 2 );
		REQUIRE( andISR.next( ) == 5 );
		REQUIRE( andISR.next( ) == static_cast < size_t >( -1 ) );

		REQUIRE( junkISR.next( ) == 1 );
		REQUIRE( junkISR.next( ) == 4 );
		REQUIRE( junkISR.next( ) == 6 );
		REQUIRE( junkISR.next( ) == static_cast < size_t >( -1 ) );
		}

	SECTION( "seek" )
		{
		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );
		REQUIRE( andISR.seek( 0 ) == 2 );
		REQUIRE( andISR.seek( 2 ) == 2 );
		REQUIRE( andISR.seek( 3 ) == 5 );
		REQUIRE( andISR.seek( 6 ) == static_cast < size_t >( -1 ) );

		REQUIRE( andISR.seek( 4 ) == 5 );
		REQUIRE( andISR.seek( 1 ) == 2 );
		REQUIRE( andISR.seek( 0 ) == 2 );
		}

	SECTION( "seek with next" )
		{
		junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
		REQUIRE( junkISR.next( ) == 1 );
		REQUIRE( junkISR.seek( 3 ) == 4 );
		REQUIRE( junkISR.next( ) == 6 );

		junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
		REQUIRE( junkISR.seek( 1 ) == 1 );
		REQUIRE( junkISR.next( ) == 4 );
		REQUIRE( junkISR.next( ) == 6 );

		junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
		REQUIRE( junkISR.seek( 0 ) == 1 );
		REQUIRE( junkISR.next( ) == 4 );
		REQUIRE( junkISR.next( ) == 6 );

		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );
		REQUIRE( andISR.seek( 0 ) == 2 );
		REQUIRE( andISR.next( ) == 5 );
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
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";
		vector < string > body = { "some", "junk", "and", "more", "junk", "and" };

		string url2 = "google.com";
		vector < string > title2 = { "evil", "company", "sell", "your", "data" };
		string titleString2 = "Evil company sell your data";
		vector < string > body2 = { "click", "this", "ad", "and", "you", "can", "live" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url2, title2, titleString2, body2 ) );

		indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
		indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

		REQUIRE( andISR.next( ) == 2 );
		REQUIRE( andISR.next( ) == 5 );
		REQUIRE( andISR.next( ) == 14 );
		REQUIRE( andISR.next( ) == static_cast < size_t >( -1 ) );
		REQUIRE( junkISR.next( ) == 1 );
		REQUIRE( junkISR.next( ) == 4 );
		REQUIRE( junkISR.next( ) == static_cast < size_t >( -1 ) );

		indexChunk::indexStreamReader endOfDocumentISR( &initializingIndexChunk, "" );
		REQUIRE( endOfDocumentISR.next( ) == 10 );
		REQUIRE( endOfDocumentISR.next( ) == 24 );
		REQUIRE( endOfDocumentISR.next( ) == static_cast < size_t >( -1 ) );

		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

		REQUIRE( andISR.next( ) == 2 );
		REQUIRE( andISR.nextDocument( ) == 14 );

		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

		REQUIRE( andISR.nextDocument( ) == 14 );

		REQUIRE( andISR.seek( 6 ) == 14 );

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
		vector < string > title = { "hamilton", "c", "shell" };
		vector < string > emptyTitle = { };
		string titleString0 = "Hamilton C Shell 2012";
		string titleString1 = "Best Website";
		string titleString2 = "Second Best Website";
		string titleString3 = "Third Best Website";
		string titleString4 = "Movie";
		string titleString5 = "Other Movie";
		vector < string > body = { "this", "is", "the", "body", "text" };
		vector < string > emptyBody = { };

		REQUIRE( initializingIndexChunk.addDocument( url0, emptyTitle, titleString0, emptyBody ) );
		REQUIRE( initializingIndexChunk.addDocument( url1, title, titleString1, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url2, emptyTitle, titleString2, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url3, title, titleString3, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url4, title, titleString4, emptyBody ) );
		REQUIRE( initializingIndexChunk.addDocument( url5, title, titleString5, body ) );

		indexChunk::indexStreamReader bodyISR = indexChunk::indexStreamReader( &initializingIndexChunk, "body" );
		indexChunk::indexStreamReader hamiltonISR = indexChunk::indexStreamReader( &initializingIndexChunk, "hamilton" );
		REQUIRE( bodyISR.next( ) == 5 );
		REQUIRE( bodyISR.next( ) == 15 );
		REQUIRE( bodyISR.next( ) == 22 );
		REQUIRE( bodyISR.next( ) == 37 );
		REQUIRE( bodyISR.next( ) == bodyISR.npos );

		REQUIRE( hamiltonISR.next( ) == hamiltonISR.npos );

		hamiltonISR = indexChunk::indexStreamReader( &initializingIndexChunk, "#hamilton" );
		REQUIRE( hamiltonISR.next( ) == 8 );
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
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";

		// Each postsChunk has a byte posts[ ] of size 1 << 10, so we just choose something larger than that
		vector < string > body( 1 << 12, word );

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		indexChunk::indexStreamReader wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, word );
		for ( size_t i = 0;  i < body.size( );  ++i )
			REQUIRE( wordISR.next( ) == i );
		REQUIRE( wordISR.next( ) == wordISR.npos );

		wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, word );
		for ( size_t i = 0;  i < body.size( ) - 1;  ++i )
			{
			REQUIRE( wordISR.seek( 0 ) == 0 );
			REQUIRE( wordISR.seek( i ) == i );
			REQUIRE( wordISR.next( ) == i + 1 );
			}
		REQUIRE( wordISR.seek( body.size( ) ) == wordISR.npos );
		REQUIRE( wordISR.next( ) == wordISR.npos );

		wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, word );
		for ( size_t i = 0;  i < ( 1 << 6 ) - 1;  ++i )
			{
			REQUIRE( wordISR.seek( i << 6 ) == i << 6 );
			REQUIRE( wordISR.next( ) == ( i << 6 ) + 1 );
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
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";

		// Each postsChunk has a byte posts[ ] of size 1 << 10
		vector < string > body1( 1 << 11, "uno" );
		vector < string > body2( 1 << 11, "dos" );

		// This is a helper variable for the for loops below
		size_t offset;

		REQUIRE( initializingIndexChunk.addDocument( url + uno, title, titleString, body1 ) );
		REQUIRE( initializingIndexChunk.addDocument( url + dos, title, titleString, body2 ) );
		REQUIRE( initializingIndexChunk.addDocument( url + uno + uno, title, titleString, body1 ) );
		REQUIRE( initializingIndexChunk.addDocument( url + dos + dos, title, titleString, body2 ) );
		indexChunk::indexStreamReader unoISR = indexChunk::indexStreamReader( &initializingIndexChunk, uno );
		indexChunk::indexStreamReader dosISR = indexChunk::indexStreamReader( &initializingIndexChunk, dos );

		for ( size_t location = 0;  location < body1.size( );  ++location )
			REQUIRE( unoISR.next( ) == location );
		offset = body1.size( ) + 1 + title.size( ) + 1 + body2.size( ) + 1 + title.size( ) + 1;
		for ( size_t location = offset; location < offset + body1.size( );  ++location )
			REQUIRE( unoISR.next( ) == location );
		REQUIRE( unoISR.next( ) == unoISR.npos );

		offset = body1.size( ) + 1 + title.size( ) + 1;
		for ( size_t location = offset;  location < offset + body2.size( );  ++location )
			REQUIRE( dosISR.next( ) == location );
		offset = body1.size( ) + 1 + title.size( ) + 1 + body2.size( ) + 1 + title.size( ) + 1
				+ body1.size( ) + 1 + title.size( ) + 1;
		for ( size_t location = offset;  location < offset + body2.size( );  ++location )
			REQUIRE( dosISR.next( ) == location );
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
		vector < string > title = { "hamilton", "c", "shell" };
		string titleString = "Hamilton C Shell 2012";

		// Create a scope here so that the index chunk calls its destructor
			{
			indexChunk initializingIndexChunk = indexChunk( fd );

			// Each postsChunk has a byte posts[ ] of size 1 << 10
			vector < string > body( 1 << 12, word );

			REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
			indexChunk::indexStreamReader wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, word );
			for ( size_t iters = 0;  iters < ( 1 << 12 );  ++iters )
				REQUIRE( wordISR.next( ) == iters );
			REQUIRE( wordISR.next( ) == wordISR.npos );
			}

		close( fd );

		fd = open( filePath, O_RDWR, 0777 );
		REQUIRE( fd != -1 );

		indexChunk fromFileIndexChunk = indexChunk( fd, false );

		indexChunk::indexStreamReader wordISR0 = indexChunk::indexStreamReader( &fromFileIndexChunk, word );
		for ( size_t iters = 0;  iters < ( 1 << 12 );  ++iters )
			REQUIRE( wordISR0.next( ) == iters );
		REQUIRE( wordISR0.next( ) == wordISR0.npos );

		indexChunk::indexStreamReader wordISR1a = indexChunk::indexStreamReader( &fromFileIndexChunk, word );
		REQUIRE( wordISR1a.seek( 1 ) == 1 );
		REQUIRE( wordISR1a.next( ) == 2 );

		indexChunk::indexStreamReader wordISR1b = indexChunk::indexStreamReader( &fromFileIndexChunk, word );
		REQUIRE( wordISR1b.seek( 0 ) == 0 );

		indexChunk::indexStreamReader wordISR1 = indexChunk::indexStreamReader( &fromFileIndexChunk, word );
		REQUIRE( wordISR1.seek( 0 ) == 0 );
		REQUIRE( wordISR1.next( ) == 1 );

		indexChunk::indexStreamReader wordISR2 = indexChunk::indexStreamReader( &fromFileIndexChunk, word );
		for ( size_t iters = 0;  iters < ( 1 << 12 );  ++iters )
			REQUIRE( wordISR2.next( ) == iters );
		REQUIRE( wordISR2.next( ) == static_cast < size_t >( -1 ) );

		indexChunk::indexStreamReader wordISR3 = indexChunk::indexStreamReader( &fromFileIndexChunk, word );
		for ( size_t iters = 0;  iters < ( 1 << 4 );  ++iters )
			{
			size_t seekLoc = wordISR3.seek( iters << 2 );
			size_t nextLoc = wordISR3.next( );
			// Seek to 0, then next( ) does not return the next location after 0, but returns 0
			REQUIRE( seekLoc == iters << 2 );
			REQUIRE( nextLoc == ( iters << 2 ) + 1 );
			}

		indexChunk::indexStreamReader thirdWordISR = indexChunk::indexStreamReader( &fromFileIndexChunk, word );
		for ( size_t iters = 0;  iters < ( 1 << 4 );  ++iters )
			{
			REQUIRE( thirdWordISR.seek( iters << 8 ) == iters << 8 );
			REQUIRE( thirdWordISR.next( ) == ( iters << 8 ) + 1 );
			}

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
			vector < string > title = { "hamilton", "c", "shell" };
			string titleString = "Hamilton C Shell 2012";
			vector < string > body = { "some", "junk", "and", "more", "junk", "and", "junk" };

			REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
			}

		close( fd );
		fd = open( filePath, O_RDWR, 0777 );
		REQUIRE( fd != -1 );

		indexChunk fromFileIndexChunk = indexChunk( fd, false );

		indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "and" );
		indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "junk");

		REQUIRE( andISR.next( ) == 2 );
		REQUIRE( andISR.next( ) == 5 );
		REQUIRE( andISR.next( ) == andISR.npos );

		REQUIRE( junkISR.next( ) == 1 );
		REQUIRE( junkISR.next( ) == 4 );
		REQUIRE( junkISR.next( ) == 6 );
		REQUIRE( junkISR.next( ) == junkISR.npos );

		andISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "and" );
		REQUIRE( andISR.seek( 0 ) == 2 );
		REQUIRE( andISR.seek( 2 ) == 2 );
		REQUIRE( andISR.seek( 3 ) == 5 );
		REQUIRE( andISR.seek( 6 ) == andISR.npos );

		REQUIRE( andISR.seek( 4 ) == 5 );
		REQUIRE( andISR.seek( 1 ) == 2 );
		REQUIRE( andISR.seek( 0 ) == 2 );

		junkISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "junk" );
		REQUIRE( junkISR.next( ) == 1 );
		REQUIRE( junkISR.seek( 3 ) == 4 );
		REQUIRE( junkISR.next( ) == 6 );

		junkISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "junk" );
		REQUIRE( junkISR.seek( 1 ) == 1 );
		REQUIRE( junkISR.next( ) == 4 );
		REQUIRE( junkISR.next( ) == 6 );

		junkISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "junk" );
		REQUIRE( junkISR.seek( 0 ) == 1 );
		REQUIRE( junkISR.next( ) == 4 );
		REQUIRE( junkISR.next( ) == 6 );

		andISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "and" );
		REQUIRE( andISR.seek( 0 ) == 2 );
		REQUIRE( andISR.next( ) == 5 );

		close( fd );
		}
	}