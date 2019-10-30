// crawlerTests.cpp
// Testing for our crawler class
//
// 2019-10-30: File creation: combsc

#include <unistd.h>
#include <cassert>
#include "crawler.cpp"
#include <iostream>

using dex::crawler;
using dex::string;

void testHttps( )
	{
	crawler spider;
	int fileToWrite = 1;
	string res;
	
	int errorCode = spider.crawlUrl( "https://www.runescape.com", fileToWrite, res );
	assert( errorCode == 302 );
	assert( res == "https://www.runescape.com/splash" );

	errorCode = spider.crawlUrl( "https://www.runescape.com/jaggywaggy", fileToWrite, res );
	assert( errorCode == 404 );

	errorCode = spider.crawlUrl( "https://www.runescape.com/splash", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = spider.crawlUrl( "https://www.fb.com", fileToWrite, res );
	assert( errorCode == 301 );
	assert( res == "https://www.facebook.com/");

	errorCode = spider.crawlUrl( "https://www.fb.com/", fileToWrite, res );
	assert( errorCode == 301 );
	assert( res == "https://www.facebook.com/");
	}

void testHttp( )
	{
	crawler spider;
	int fileToWrite = 1;
	string res;

	int errorCode = spider.crawlUrl( "http://www.runescape.com/splash", fileToWrite, res );
	assert( errorCode == 302 );
	assert( res == "https://www.runescape.com/splash" );

	errorCode = spider.crawlUrl( "http://man7.org/", fileToWrite, res );
	assert( errorCode == 0 );

	errorCode = spider.crawlUrl( "man7.org/", fileToWrite, res );
	assert( errorCode == 0 );
	}

int main( int argc, char *argv[ ] )
	{
	testHttp( );
	testHttps( );
	}