// Implementation of basic mercator architecture

// 2019-12-02: Set maximum size for data structures: combsc
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

dex::string savePath = "../socket-html/";
/* dex::string savePath = \"../socket-html/\"; */ // local
dex::string tmpPath = "data/tmp/";

dex::string loggingFileName;
pthread_mutex_t loggingLock = PTHREAD_MUTEX_INITIALIZER;


// All urls in the frontier must be known to be in our domain
// and lead to a legitimate endpoint, or must be unknown. This
// means we do not put broken links into our frontier and we do
// not put links that aren't our responsibility into our frontier
size_t frontierSize = 50000;
dex::frontier urlFrontier( frontierSize );
pthread_mutex_t frontierLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t frontierCV = PTHREAD_COND_INITIALIZER;


long checkpoint = 5 * 60; // checkpoints every x seconds
time_t lastCheckpoint = time( NULL );

char state = 0;

#define numWorkers 100
pthread_t workers [ numWorkers ];
int ids[ numWorkers ];

dex::robotsMap robotsCache;
pthread_mutex_t robotsLock = PTHREAD_MUTEX_INITIALIZER;

// This set contains all known links in our domain that error out
// when visited.
dex::unorderedSet < dex::Url > brokenLinks{ 2000 };
pthread_mutex_t brokenLinksLock = PTHREAD_MUTEX_INITIALIZER;

// This vector contains all known links that are NOT in our domain
// and need to be given to other instances. Think of this as a frontier
// but for the other workers.
dex::vector < dex::Url > linksToShip;
pthread_mutex_t linksToShipLock = PTHREAD_MUTEX_INITIALIZER;

// This is the redirect cache. Used for handling known redirects on our
// side without having to actually visit the sites.
dex::redirectCache redirects;
pthread_mutex_t redirectsLock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t saveHtmlLock = PTHREAD_MUTEX_INITIALIZER;
dex::vector < dex::string > retrievedLinks;

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

// We should have a function for checking URLs to see if they're in our
// domain or not. If it's in our domain, put it back into our frontier.
// if it's not in our domain, we need to send it to the other crawler
// workers.
bool isUrlInDomain( const dex::Url &url )
	{
	dex::string work = url.completeUrl( );
	return true;
	}

// Call checkpointing functions after time alotted or user input
void saveWork( )
	{
	print( "saving" );
	dex::saveFrontier( ( tmpPath + "savedFrontier.txt" ).cStr( ), urlFrontier );
	//dex::saveBrokenLinks( ( tmpPath + "savedBrokenLinks.txt" ).cStr( ), brokenLinks );
	//dex::saveVisitedLinks( "data/crawledLinks.txt", retrievedLinks );
	lastCheckpoint = time(NULL);
	}

void *worker( void *args )
	{
	int a = * ( ( int * ) args );
	dex::string name = dex::toString( a );
	log( "Start thread " + name + "\n");
	for ( int i = 0;  true;  ++i )
		{
		if ( state == 'q' )
			return nullptr;

		pthread_mutex_lock( &frontierLock );
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
		pthread_mutex_unlock( &frontierLock );

		// Fix link using our redirects cache
		pthread_mutex_lock( &redirectsLock );
		toCrawl = redirects.getEndpoint( toCrawl );
		pthread_mutex_unlock( &redirectsLock );
		
		log( name + ": Connecting to " + toCrawl.completeUrl( ) + "\n" );
		dex::string result = "";
		// I know that it's not safe to use robotsCache here
		// We need to rewrite crawlURL to use the robotsCache efficiently, don't want to
		// lock the cache for the entire time we're crawling the URL
		int errorCode = dex::crawler::crawlUrl( toCrawl, result, robotsCache );
		log( name + ": crawled domain: " + toCrawl.completeUrl( ) + " error code: " + dex::toString( errorCode ) + "\n" );
		// If we get a response from the url, nice. We've hit an endpoint that gives us some HTML.
		if ( errorCode == 0 || errorCode == dex::NOT_HTML )
			{
			dex::string html = "NEW URL: " + toCrawl.completeUrl( ) + "\n" + result + "\n";

			if ( errorCode == dex::NOT_HTML )
				print( toCrawl.completeUrl( ) + " is not html " );

			if ( errorCode == 0 )
				{
				// if valid html -> save
				pthread_mutex_lock( &saveHtmlLock );
				dex::saveHtml( toCrawl, html, savePath );
				retrievedLinks.pushBack( toCrawl.completeUrl( ) );
				pthread_mutex_unlock( &saveHtmlLock );

				dex::HTMLparser parser( result );
			
				dex::vector < dex::Url > links = parser.ReturnLinks( );
				for ( auto it = links.begin( );  it != links.end( );  ++it )
					{
					// Fix link using our redirects cache
					pthread_mutex_lock( &redirectsLock );
					it->setFragment( "" );
					dex::Url endpoint = redirects.getEndpoint( *it );
					pthread_mutex_unlock( &redirectsLock );

					// Check to see if the endpoint we have is a known broken link
					pthread_mutex_lock( &brokenLinksLock );
					if ( brokenLinks.count( endpoint ) == 0 )
						{
						pthread_mutex_unlock( &brokenLinksLock );
						// If we're in charge of this link, put it into our frontier
						if ( isUrlInDomain( endpoint ) )
							{
							pthread_mutex_lock( &frontierLock );
							urlFrontier.putUrl( endpoint );
							pthread_cond_signal( &frontierCV );
							pthread_mutex_unlock( &frontierLock );
							}
						// If we're not in charge of this link, send it off to be shipped
						else
							{
							pthread_mutex_lock( &linksToShipLock );
							linksToShip.pushBack( endpoint );
							pthread_mutex_unlock( &linksToShipLock );
							}
						}
					else
						{
						pthread_mutex_unlock( &brokenLinksLock );
						}
					}
				}
			}
		// If we get a politness error for this URL, we put it back into the frontier
		if ( errorCode == dex::POLITENESS_ERROR || errorCode == dex::PUT_BACK_IN_FRONTIER )
			{
			pthread_mutex_lock( &frontierLock );
			urlFrontier.putUrl( toCrawl );
			pthread_mutex_unlock( &frontierLock );
			}
        //print( "past politeness" );
		// If we get a redirect, we need to update the redirects cache and put the link
		// into the frontier or should be shipped.
		if ( errorCode >= 300 && errorCode < 400 )
			{
			dex::Url location = dex::Url( result.cStr( ) );
			pthread_mutex_lock( &redirectsLock );
			redirects.updateUrl( toCrawl, location );
			pthread_mutex_unlock( &redirectsLock );

			if ( isUrlInDomain( location ) )
				{
				pthread_mutex_lock( &frontierLock );
				urlFrontier.putUrl( location );
				pthread_mutex_unlock( &frontierLock );
				}
			else
				{
				pthread_mutex_lock( &linksToShipLock );
				linksToShip.pushBack( dex::Url( result.cStr( ) ) );
				pthread_mutex_unlock( &linksToShipLock );
				}
			}
        //print( "past redirect" );
		// This link doesn't lead anywhere, we need to add it to our broken links
		if ( errorCode >= 400 || errorCode == dex::DISALLOWED_ERROR )
			{
			pthread_mutex_lock( &brokenLinksLock );
			brokenLinks.insert( toCrawl );
			pthread_mutex_unlock( &brokenLinksLock );
			// All links that redirect to this should also be categorized as broken.
			}
        //print( "past lead" );
		}
	return nullptr;
	}



int main( )
	{
	// setup logging file for this run
	// goodbye error code 13
	signal(SIGPIPE, SIG_IGN);
	int result = dex::makeDirectory( savePath.cStr( ) );
	result = dex::makeDirectory( tmpPath.cStr( ) );
	result = dex::makeDirectory( ( tmpPath + "logs" ).cStr( ) );

	loggingFileName = tmpPath + "logs/";
	time_t now = time( nullptr );
	loggingFileName += ctime( &now );
	loggingFileName.popBack( );
	loggingFileName += ".log";
	loggingFileName = loggingFileName.replaceWhitespace( "_" );

	urlFrontier = dex::loadFrontier( "data/seedlist.txt", frontierSize );
	brokenLinks = dex::loadBrokenLinks( ( tmpPath + "savedBrokenLinks.txt" ).cStr( ) );

	if ( dex::getCurrentFileDescriptor( savePath + "html/" ) == -1 )
		{
		std::cerr << "Could not get current file" << std::endl;
		return -1;
		}

	for ( int i = 0;  i < numWorkers;  ++i )
		{
		ids[ i ] = i;
		pthread_create( &workers[ i ], nullptr, worker, &( ids[ i ] ) );
		}

	while ( std::cin >> state )
		{
		if ( state == 'q' )
			break;
		}

	for ( size_t i = 0;  i < numWorkers; ++i )
		pthread_join( workers[ i ], nullptr );
	
	dex::closeHtmlFile( );
	std::cout << "exiting safely...\n";
	return 0;
	}
