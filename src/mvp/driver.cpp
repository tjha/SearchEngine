// Implementation of basic mercator architecture

// 2019-12-04: added wrapper functions for perf, added limits for all data structures: combsc
// 2019-12-03: Uses frontier to start if it exists, otherwise uses seedlist, no duplicates in frontier: combsc
// 2019-12-02: Set maximum size for frontier, add hashing for distribution of URLs: combsc
// 2019-12-01: Improve frontier: combsc
// 2019-11-30: Made crawlUrl threadsafe: combsc
// 2019-11-26: Added relative paths, added compatability for pages that are not HTML: combsc
// 2019-11-23: Improved checkpointing: combsc
// 2019-11-21: Add working redirect cache, fix overall logic of file, test multithreading: combsc
// 2019-11-20: Add logging, add file structure for saving html: combsc
// 2019-11-16: Init Commit: combsc
#include "crawler.hpp"
#include "basicString.hpp"
#include "vector.hpp"
#include "frontier.hpp"
#include "checkpointing.hpp"
#include "redirectCache.hpp"
#include "parser.hpp"
#include <time.h>
#include <signal.h>
#include <iostream>
#include <pthread.h>

dex::string savePath = "../socket-html/";
/* dex::string savePath = \"../socket-html/\"; */ // local
dex::string tmpPath = "data/tmp/";
dex::string toShipPath = "data/toShip/";

dex::string loggingFileName;
pthread_mutex_t loggingLock = PTHREAD_MUTEX_INITIALIZER;

dex::string performanceName;


// Sizes of our data structures
size_t frontierSize = 5000;
size_t crawledLinksSize = 10000;
size_t robotsMapSize = 500;
const size_t redirectsSize = 1000;

// All urls in the frontier must be known to be in our domain
// and lead to a legitimate endpoint, or must be unknown. This
// means we do not put broken links into our frontier and we do
// not put links that aren't our responsibility into our frontier

dex::frontier urlFrontier( frontierSize );
pthread_mutex_t frontierLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t frontierCV = PTHREAD_COND_INITIALIZER;

long checkpoint = 15; // checkpoints every x seconds
long testTime = 40;
time_t lastCheckpoint = time( NULL );
time_t startTime = time( NULL );
bool testing = false;

char state = 0;
size_t numCrawledLinks = 0;
pthread_mutex_t crawledLinksLock = PTHREAD_MUTEX_INITIALIZER;

#define numWorkers 10
pthread_t workers [ numWorkers ];
int ids[ numWorkers ];

dex::robotsMap robotsCache( robotsMapSize );
pthread_mutex_t robotsLock = PTHREAD_MUTEX_INITIALIZER;

dex::unorderedSet < dex::string > crawledLinks;
dex::sharedReaderLock crawledLock;

// This vector contains all known links that are NOT in our domain
// and need to be given to other instances. Think of this as a frontier
// but for the other workers.
const size_t numInstances = 1;
const size_t instanceId = 0;
dex::vector < dex::Url > linksToShip [ numInstances ];
pthread_mutex_t linksToShipLock = PTHREAD_MUTEX_INITIALIZER;

// This is the redirect cache. Used for handling known redirects on our
// side without having to actually visit the sites.
dex::redirectCache redirects( redirectsSize );
pthread_mutex_t redirectsLock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;


void print( char *toPrint )
	{
	pthread_mutex_lock( &printLock );
	std::cout << toPrint << std::endl;
	pthread_mutex_unlock( &printLock );
	}

void print( dex::string toPrint )
	{
	pthread_mutex_lock( &printLock );
	std::cout << toPrint << std::endl;
	pthread_mutex_unlock( &printLock );
	}

int log( dex::string toWrite )
	{
	pthread_mutex_lock( &loggingLock );
	int error = dex::appendToFile( loggingFileName.cStr( ), toWrite.cStr( ), toWrite.size( ) );
	pthread_mutex_unlock( &loggingLock );
	return error;
	}

int writePerformance( dex::string &toWrite )
	{
	int error = dex::appendToFile( performanceName.cStr( ), toWrite.cStr( ), toWrite.size( ) );
	return error;
	}

// We should have a function for checking URLs to see if they're in our
// instance or not. If it's in our instance put it back into our frontier.
// if it's not in our instance we need to send it to the other crawler
// workers.
size_t getUrlInstance( const dex::Url &url )
	{
	dex::hash < dex::string > hasher;
	unsigned long h = hasher( url.getHost( ) );
	return h % numInstances;
	}

void fixRedirect( dex::Url &toCrawl )
	{
	pthread_mutex_lock( &redirectsLock );
	toCrawl = redirects.getEndpoint( toCrawl );
	if ( redirects.size( ) > redirectsSize )
		redirects.reset( );
	pthread_mutex_unlock( &redirectsLock );
	}

void updateRedirect( const dex::Url &toCrawl, const dex::Url &location)
	{
	pthread_mutex_lock( &redirectsLock );
	redirects.updateUrl( toCrawl, location );
	pthread_mutex_unlock( &redirectsLock );
	}

bool alreadyCrawled( const dex::Url &toCrawl )
	{
	bool ret;
	crawledLock.readLock( );
	ret = crawledLinks.count( toCrawl.completeUrl( ) ) > 0;
	crawledLock.releaseReadLock( );
	return ret;
	}

void addToCrawled( const dex::Url &toCrawl )
	{
	crawledLock.writeLock( );
	if ( crawledLinks.size( ) > crawledLinksSize )
		{
		crawledLinks.clear( );
		print( "Purged crawledLinks" );
		}
	crawledLinks.insert( toCrawl.completeUrl( ) );
	crawledLock.releaseWriteLock( );
	}

// Call checkpointing functions after time alotted or user input
void saveWork( )
	{	
	dex::saveFrontier( ( tmpPath + "savedFrontier.txt" ).cStr( ), urlFrontier );
	crawledLock.readLock( );
	dex::saveCrawledLinks( "data/crawledLinks.txt", crawledLinks );
	dex::string toWrite;
	toWrite.reserve( 1000 );
	toWrite += "Number of links crawled in " + dex::toString( checkpoint ) + " seconds: " + dex::toString( numCrawledLinks) + "\n";
	toWrite += "Size of frontier: " + dex::toString( urlFrontier.size( ) ) + "\n";
	toWrite += "Capacity of frontier: " + dex::toString( urlFrontier.capacity( ) ) + "\n";
	toWrite += "Size of crawledLinks " + dex::toString( crawledLinks.size( ) ) + "\n";
	toWrite += "Capacity of crawledLinks " + dex::toString( crawledLinks.bucketCount( ) ) + "\n";
	crawledLock.releaseReadLock( );
	toWrite += "Size of robotsMap " + dex::toString( robotsCache.size( ) ) + "\n";
	toWrite += "Capacity of robotsMap " + dex::toString( robotsCache.capacity( ) ) + "\n";

	pthread_mutex_lock( &redirectsLock );
	toWrite += "Size of redirects " + dex::toString( redirects.size( ) ) + "\n";
	toWrite += "Capacity of redirects " + dex::toString( redirects.capacity( ) ) + "\n";
	pthread_mutex_unlock( &redirectsLock );
	writePerformance( toWrite );
	print( toWrite );

	numCrawledLinks = 0;
	lastCheckpoint = time(NULL);
	}

void *worker( void *args )
	{
	int a = * ( ( int * ) args );
	dex::string name = dex::toString( a );
	dex::string folderPath = savePath + "/html/" + name + "/";
	int currentFileDescriptor = dex::getCurrentFileDescriptor( folderPath );
	
	log( "Start thread " + name + "\n");
	for ( int i = 0;  true;  ++i )
		{
		if ( state == 'q' )
			return nullptr;

		pthread_mutex_lock( &frontierLock );
		log( "frontier lock" );
		while ( urlFrontier.empty( ) )
			{
			pthread_cond_wait( &frontierCV, &frontierLock );
			}
		dex::Url toCrawl = urlFrontier.getUrl( );
		if ( time( NULL ) - lastCheckpoint > checkpoint || state == 's' )
			{
			saveWork( );
			state = 0;
			}
		if ( testing && time( NULL ) - startTime > testTime )
			{
			log( "leaving frontier lock" );
			pthread_mutex_unlock( &frontierLock );
			exit( 0 );
			}
		log( "leaving frontier lock" );
		pthread_mutex_unlock( &frontierLock );

		// Fix link using our redirects cache
		fixRedirect( toCrawl );
		
		log( name + ": Connecting to " + toCrawl.completeUrl( ) + "\n" );
		dex::string result;
		result.reserve( 2000000 );
		if ( robotsCache.purge( ) == 1 )
			print( "Purged Robot Cache" );
		int errorCode = dex::crawler::crawlUrl( toCrawl, result, robotsCache );
		log( name + ": crawled domain: " + toCrawl.completeUrl( ) + " error code: " + dex::toString( errorCode ) + "\n" );
		// If we get a response from the url, nice. We've hit an endpoint that gives us some HTML.
		if ( errorCode == 0 || errorCode == dex::NOT_HTML )
			{
			

			if ( errorCode == dex::NOT_HTML )
				print( toCrawl.completeUrl( ) + " is not html " );

			if ( errorCode == 0 )
				{
				if ( dex::fileSize( currentFileDescriptor ) > dex::HTMLChunkSize )
					{
					close( currentFileDescriptor );
					print( "worker " + name + " switched html files" );
					currentFileDescriptor = dex::getCurrentFileDescriptor( folderPath );
					}
				// print( "saving" );
				dex::saveHtml( toCrawl.completeUrl( ), result, currentFileDescriptor );
				// print( "done saving" );

				pthread_mutex_lock( &crawledLinksLock );
				numCrawledLinks++;
				pthread_mutex_unlock( &crawledLinksLock );

				addToCrawled( toCrawl );

				log( "leaving save lock" );
				dex::vector < dex::Url > links;
				try
					{
					dex::HTMLparser parser( toCrawl, result, false );
					links = parser.ReturnLinks( );
					}
				catch( dex::outOfRangeException e )
					{
					print( toCrawl.completeUrl( ) + " Threw out of range exception" );
					}

				pthread_mutex_lock( &frontierLock );
				log( "put lock" );
				for ( auto it = links.begin( );  it != links.end( );  ++it )
					{
					// Fix link using our redirects cache
					dex::Url current = *it;
					current.setFragment( "" );
					fixRedirect( current );

					// Check to see if the endpoint we have is a known broken link or if we've already crawled
					if ( !alreadyCrawled( current) )
						{
						size_t urlId = getUrlInstance( *it );
						if ( urlId == instanceId )
							{
							urlFrontier.putUrl( *it );
							}
						// If we're not in charge of this link, send it off to be shipped
						else
							{
							pthread_mutex_lock( &linksToShipLock );
							log( "put ship Lock" );
							linksToShip[ urlId ].pushBack( *it );
							log( "leaving put ship Lock" );
							pthread_mutex_unlock( &linksToShipLock );
							}
						}
					}
				pthread_cond_signal( &frontierCV );
				log( "leave put lock" );
				pthread_mutex_unlock( &frontierLock );
				}
			}
		// If we get a politness error for this URL, we put it back into the frontier
		if ( errorCode == dex::POLITENESS_ERROR || errorCode == dex::PUT_BACK_IN_FRONTIER )
			{
			pthread_mutex_lock( &frontierLock );
			log( "politeness put lock" );
			urlFrontier.putUrl( toCrawl );
			log( "leaving politeness put lock" );
			pthread_mutex_unlock( &frontierLock );
			}
        //print( "past politeness" );
		// If we get a redirect, we need to update the redirects cache and put the link
		// into the frontier or should be shipped.
		if ( errorCode >= 300 && errorCode < 400 )
			{
			dex::Url location = dex::Url( result.cStr( ) );
			updateRedirect( toCrawl, location );
			size_t urlId = getUrlInstance( location );
			if ( urlId == instanceId )
				{
				pthread_mutex_lock( &frontierLock );
				log( "redirect lock" );
				urlFrontier.putUrl( location );
				log( "leaving redirect lock" );
				pthread_mutex_unlock( &frontierLock );
				}
			else
				{
				pthread_mutex_lock( &linksToShipLock );
				log( "ship redirect lock" );
				linksToShip[ urlId ].pushBack( dex::Url( result.cStr( ) ) );
				log( "leaving ship redirect lock" );
				pthread_mutex_unlock( &linksToShipLock );
				}
			}
		// This link doesn't lead anywhere, we need to add it to our broken links
		if ( errorCode >= 400 || errorCode == dex::DISALLOWED_ERROR || errorCode == dex::RESPONSE_TOO_LARGE )
			{
			addToCrawled( toCrawl );
			}
		}
	return nullptr;
	}



int main( )
	{
	// setup logging file for this run
	// goodbye error code 13
	signal(SIGPIPE, SIG_IGN);
	int result = dex::makeDirectory( savePath.cStr( ) );
	result = dex::makeDirectory( ( savePath + "/html" ).cStr( ) );
	for ( size_t i = 0;  i < numWorkers;  ++i )
		{
		result = dex::makeDirectory( ( savePath + "/html/" + dex::toString( i ) ).cStr( ) );
		}
	result = dex::makeDirectory( tmpPath.cStr( ) );
	result = dex::makeDirectory( ( tmpPath + "logs" ).cStr( ) );
	result = dex::makeDirectory( ( tmpPath + "performance" ).cStr( ) );
	result = dex::makeDirectory( toShipPath.cStr( ) );

	loggingFileName = tmpPath + "logs/";
	time_t now = time( nullptr );
	loggingFileName += ctime( &now );
	loggingFileName.popBack( );
	loggingFileName += ".log";
	loggingFileName = loggingFileName.replaceWhitespace( "_" );

	performanceName = tmpPath + "performance/" + ctime( &now );
	performanceName.popBack( );
	performanceName += ".txt";
	performanceName = performanceName.replaceWhitespace( "_" );

	urlFrontier = dex::loadFrontier( ( tmpPath + "savedFrontier.txt" ).cStr( ), frontierSize, true );
	if ( urlFrontier.size( ) == 0 )
		{
		urlFrontier = dex::loadFrontier( "data/seedlist.txt", frontierSize );
		}
	for ( auto it = urlFrontier.begin( );  it != urlFrontier.end( );  ++it )
		print( it->completeUrl( ) );

	std::cout << "Starting crawl with frontier size " << urlFrontier.size( ) << std::endl;
	if ( testing )
	{
	print( "YOU ARE TESTING, IT WILL END AFTER " + dex::toString( testTime ) + " seconds" );
	}
	for ( int i = 0;  i < numWorkers;  ++i )
		{
		ids[ i ] = i;
		pthread_create( &workers[ i ], nullptr, worker, &( ids[ i ] ) );
		}

	for ( size_t i = 0;  i < numWorkers; ++i )
		pthread_join( workers[ i ], nullptr );
	
	std::cout << "exiting safely...\n";
	return 0;
	}
