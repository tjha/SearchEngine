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
#include "indexer/index.hpp"
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

// TODO: Add test case for closing and reopening an indexChunk

/*
TEST_CASE( "lougheed's fun read indexChunk test" )
	{
	std::cout << "Running lougheed's fun read indexChunk test ;)\n";
	const char filePath[ ] = "0_in.dex";
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
		REQUIRE( decoratedArtISR.next( ) == 1 );
		}

	SECTION( "ISR functions on nondecorated title" )
		{
		indexChunk initializingIndexChunk = indexChunk( fd );	

		REQUIRE( initializingIndexChunk.addDocument( "a.com", { "title" }, "titleString", { "body" } ) );

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

		REQUIRE( decoratedArtISR.next( ) == 0 );
		REQUIRE( artISR.next( ) == static_cast< size_t >( - 1 ) );
		}
	
	SECTION( "Nonexistent word ISR" )
		{
		indexChunk chunk = indexChunk( fd );	
		
		REQUIRE( chunk.addDocument( "a.com", { "title" }, "titleString", { "body" } ) );
		indexChunk::indexStreamReader nonexistentISR = indexChunk::indexStreamReader( &chunk, "nonexistent" );
		}
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
		
		indexChunk::indexStreamReader endOfDocumentISR( &initializingIndexChunk, "" );
		REQUIRE( endOfDocumentISR.next( ) == 9 );
		REQUIRE( endOfDocumentISR.next( ) == 22 );
		REQUIRE( endOfDocumentISR.next( ) == static_cast < size_t >( -1 ) );

		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

		REQUIRE( andISR.next( ) == 2 );
		REQUIRE( andISR.nextDocument( ) == 13 );

		andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );

		REQUIRE( andISR.nextDocument( ) == 13 );

		REQUIRE( andISR.seek( 6 ) == 13 );

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
		vector < string > nonEmptyTitle = { "wurd" };
		string titleString0 = "Hamilton C Shell 2012";
		string titleString1 = "Best Website";
		string titleString2 = "Second Best Website";
		string titleString3 = "Third Best Website";
		string titleString4 = "Movie";
		string titleString5 = "Other Movie";
		vector < string > body = { "this", "is", "the", "body", "text" };
		vector < string > emptyBody = { };

		// REQUIRE( initializingIndexChunk.addDocument( url0, nonEmptyTitle, titleString0, emptyBody ) );
		REQUIRE( initializingIndexChunk.addDocument( url0, emptyTitle, titleString0, emptyBody ) );
		REQUIRE( initializingIndexChunk.addDocument( url1, title, titleString1, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url2, emptyTitle, titleString2, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url3, title, titleString3, body ) );
		REQUIRE( initializingIndexChunk.addDocument( url4, title, titleString4, emptyBody ) );
		REQUIRE( initializingIndexChunk.addDocument( url5, title, titleString5, body ) );

		// exit( 1 );

		indexChunk::indexStreamReader bodyISR = indexChunk::indexStreamReader( &initializingIndexChunk, "body" );
		indexChunk::indexStreamReader hamiltonISR = indexChunk::indexStreamReader( &initializingIndexChunk, "hamilton" );
		/*
		REQUIRE( bodyISR.next( ) == 4 );
		REQUIRE( bodyISR.next( ) == 13 );
		REQUIRE( bodyISR.next( ) == 19 );
		REQUIRE( bodyISR.next( ) == 32 );
		*/

		size_t ham = hamiltonISR.next( );
		while ( ham != static_cast< size_t >( -1 ) )
			{
			std::cout << "next: " << ham << std::endl;
			ham = hamiltonISR.next( );
			}

		hamiltonISR = indexChunk::indexStreamReader( &initializingIndexChunk, "#hamilton" );
		REQUIRE( hamiltonISR.next( ) == 6 );
		REQUIRE( hamiltonISR.next( ) == 21 );
		REQUIRE( hamiltonISR.next( ) == 25 );
		REQUIRE( hamiltonISR.next( ) == 34 );
		}
	/*
	SECTION( "ISR functions on multiple documents" )
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
		vector < string > altBody = { "some", "junk", "and", "other", "junk", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		indexChunk::indexStreamReader junkISR = indexChunk::indexStreamReader( &initializingIndexChunk, "junk" );
		indexChunk::indexStreamReader andISR = indexChunk::indexStreamReader( &initializingIndexChunk, "and" );
		indexChunk::indexStreamReader moreISR = indexChunk::indexStreamReader( &initializingIndexChunk, "more" );
		indexChunk::indexStreamReader otherISR = indexChunk::indexStreamReader( &initializingIndexChunk, "other" );

		size_t junkPos = junkISR.next( );
		REQUIRE( junkPos == 1 );
		junkPos = junkISR.next( );
		REQUIRE( junkPos == 4 );

		size_t andPos = andISR.next( );
		REQUIRE( andPos == 2 );
		andPos = andISR.next( );
		REQUIRE( andPos == 5 );

		size_t morePos = moreISR.next( );
		REQUIRE( morePos == 3 );

		string url2 = url + dex::toString( 0 );
		vector < string > title2( title );
		string titleString2( titleString + dex::toString( 0 ) );
		REQUIRE( initializingIndexChunk.addDocument( url2, title2, titleString2, body ) );

		junkPos = junkISR.next( );
		REQUIRE( junkPos == 10 );
		junkPos = junkISR.next( );
		REQUIRE( junkPos == 13 );

		andPos = andISR.next( );
		REQUIRE( andPos == 11 );
		andPos = andISR.next( );
		REQUIRE( andPos == 14 );

		size_t otherPos = otherISR.next( );
		REQUIRE( otherPos == 12 );

		for ( int iteration = 1;  iteration < 50;  iteration++ )
			{
			string url2 = url + dex::toString( iteration );
			vector < string > title2( title );
			string titleString2( titleString + dex::toString( iteration ) );
			if ( iteration % 2 )
				{
				vector < string > body2( body );
				REQUIRE( initializingIndexChunk.addDocument( url2, title2, titleString2, body2 ) );
				}
			else
				{
				vector < string > body2( altBody );
				REQUIRE( initializingIndexChunk.addDocument( url2, title2, titleString2, body2 ) );
				}

			junkPos = junkISR.next( );
			REQUIRE( junkPos == )


			}
		close( fd );
		}
		*/
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
		indexChunk::indexStreamReader wordISR = indexChunk::indexStreamReader( &initializingIndexChunk, "someWord" );
		for ( int iters = 0;  iters < (1<<12) - 2;  iters++ )
			REQUIRE( wordISR.next( ) == iters );
		REQUIRE( wordISR.next( ) == ( 1<<12 ) - 2 );
		REQUIRE( wordISR.next( ) == ( 1<<12 ) - 1 );
		REQUIRE( wordISR.next( ) == static_cast < size_t >( -1 ) );
		}

		close( fd );
		fd = open( filePath, O_RDWR, 0777 );
		REQUIRE( fd != -1 );

		indexChunk fromFileIndexChunk = indexChunk( fd, false );

		indexChunk::indexStreamReader wordISR0 = indexChunk::indexStreamReader( &fromFileIndexChunk, "someWord" );
		for ( int iters = 0;  iters < (1<<12) - 2;  iters++ )
			REQUIRE( wordISR0.next( ) == iters );
		REQUIRE( wordISR0.next( ) == ( 1<<12 ) - 2 );
		REQUIRE( wordISR0.next( ) == ( 1<<12 ) - 1 );
		REQUIRE( wordISR0.next( ) == static_cast < size_t >( -1 ) );

		indexChunk::indexStreamReader wordISR1a = indexChunk::indexStreamReader( &fromFileIndexChunk, "someWord" );
		REQUIRE( wordISR1a.seek( 1 ) == 1 );
		REQUIRE( wordISR1a.next( ) == 2 );

		indexChunk::indexStreamReader wordISR1b = indexChunk::indexStreamReader( &fromFileIndexChunk, "someWord" );
		REQUIRE( wordISR1b.seek( 0 ) == 0 );

		indexChunk::indexStreamReader wordISR1 = indexChunk::indexStreamReader( &fromFileIndexChunk, "someWord" );
		REQUIRE( wordISR1.seek( 0 ) == 0 );
		REQUIRE( wordISR1.next( ) == 1 );

		indexChunk::indexStreamReader wordISR2 = indexChunk::indexStreamReader( &fromFileIndexChunk, "someWord" );
		for ( int iters = 0;  iters < (1<<12) - 2;  iters++ )
			REQUIRE( wordISR2.next( ) == iters );
		REQUIRE( wordISR2.next( ) == ( 1<<12 ) - 2 );
		REQUIRE( wordISR2.next( ) == ( 1<<12 ) - 1 );
		REQUIRE( wordISR2.next( ) == static_cast < size_t >( -1 ) );

		indexChunk::indexStreamReader wordISR3 = indexChunk::indexStreamReader( &fromFileIndexChunk, "someWord" );
		for ( int iters = 0;  iters < ( 1<<4 );  iters++ )
			{
			size_t seekLoc = wordISR3.seek( iters<<2 );
			size_t nextLoc = wordISR3.next( );
			// Seek to 0, then next( ) does not return the next location after 0, but returns 0
			REQUIRE( seekLoc == iters<<2 );
			REQUIRE( nextLoc == ( iters<<2 ) + 1 );
			}

		indexChunk::indexStreamReader thirdWordISR = indexChunk::indexStreamReader( &fromFileIndexChunk, "someWord" );
		for ( int iters = 0;  iters < ( 1<<4 );  iters++ )
			{
			REQUIRE( thirdWordISR.seek( iters<<8 ) == iters<<8 );
			REQUIRE( thirdWordISR.next( ) == ( iters<<8 ) + 1 );
			}
		}
	SECTION( "Fill an indexChunk" )
		{
		string filename = "shakespeare.txt";
		}

	SECTION( "Many unique words" )
		{

		}
	}
