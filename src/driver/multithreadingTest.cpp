// robotsMapTests.cpp
// Test crawler for multithreading
//
// 2019-11-30: Init Commit: combsc

#include "crawler/crawler.hpp"

#define numWorkers 10
pthread_t workers [ numWorkers ];
int ids[ numWorkers ];
dex::robotsMap robotsCache;
pthread_mutex_t robotsLock = PTHREAD_MUTEX_INITIALIZER;

dex::vector < dex::string > frontier;
pthread_mutex_t frontierLock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;

void print( dex::string toPrint )
	{
	pthread_mutex_lock( &printLock );
	std::cout << toPrint << std::endl;
	pthread_mutex_unlock( &printLock );
	}

void *crawl( void* arg )
	{
	int a = * ( ( int * ) arg );
	dex::string name = dex::toString( a );
	print( "Start thread " + name + "\n");
	while ( true )
		{
		pthread_mutex_lock( &frontierLock );
		if ( frontier.empty( ) )
			{
			print( "Finished " + name );
			pthread_mutex_unlock( &frontierLock );
			return nullptr;
			}

		dex::Url toCrawl = dex::Url( frontier.front( ).cStr( ) );
		frontier.erase( 0 );
		pthread_mutex_unlock( &frontierLock );
		dex::string result;
		print( name + " " + toCrawl.completeUrl( ) );
		int errorCode = dex::crawler::crawlUrl( toCrawl, result, robotsCache );
		print( name + " " + toCrawl.completeUrl( ) + " " + dex::toString( errorCode ) );
		}
	}


int main( )
	{
	frontier.pushBack( "https://www.latimes.com/" );
	frontier.pushBack( "https://www.latimes.com/entertainment-arts/books/los-angeles-times-book-club" );
	for ( int i = 0;  i < numWorkers;  ++i )
		{
		ids[ i ] = i;
		pthread_create( &workers[ i ], nullptr, crawl, &( ids[ i ] ) );
		}

	for ( size_t i = 0;  i < numWorkers; ++i )
		pthread_join( workers[ i ], nullptr );
	return 0;
	}

