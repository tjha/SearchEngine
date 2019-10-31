// crawlerTests.cpp
// Testing for our crawler class
//
// 2019-10-31: Added basic query + fragment testing: combsc
// 2019-10-30: File creation: combsc

#include <unistd.h>
#include <cassert>
#include "crawler.hpp"
#include <iostream>

using dex::string;

void testHttps( )
	{
	int fileToWrite = 1;
	string res;
	
	int errorCode = dex::crawler::crawlUrl( "https://www.runescape.com", fileToWrite, res );
	assert( errorCode == 302 );
	assert( res == "https://www.runescape.com/splash" );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/jaggywaggy", fileToWrite, res );
	assert( errorCode == 404 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash?woodcutting=100", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash?woodcutting=100#jagex", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.runescape.com/splash#jagex", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "https://www.fb.com", fileToWrite, res );
	assert( errorCode == 301 );
	assert( res == "https://www.facebook.com/");

	errorCode = dex::crawler::crawlUrl( "https://www.fb.com/", fileToWrite, res );
	assert( errorCode == 301 );
	assert( res == "https://www.facebook.com/");
	}

void testHttp( )
	{
	int fileToWrite = 1;
	string res;

	int errorCode = dex::crawler::crawlUrl( "http://www.runescape.com/splash", fileToWrite, res );
	assert( errorCode == 302 );
	assert( res == "https://www.runescape.com/splash" );
	
	errorCode = dex::crawler::crawlUrl( "http://man7.org/", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "http://man7.org/?man=page", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "http://man7.org/?man=page#foobar", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "http://man7.org/#foobar", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = dex::crawler::crawlUrl( "man7.org/", fileToWrite, res );
	assert( errorCode == 0 );
	}

int main( )
	{
	testHttp( );
	testHttps( );
	}