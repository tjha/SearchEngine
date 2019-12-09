// Implementation of basic mercator architecture

// 2019-12-08: blacklist getstencil.com, fix crawled logic: combsc
// 2019-12-07: Load crawled links
// 2019-12-06: Implement hashing to prevent overlap between distributed crawlers: combsc
//             Split up performance and saving. logging file now refreshing after 100 mb filled: jhirsh
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

//dex::string savePath = "/home/ec2-user/socket-html/";
dex::string savePath = "../socket-html/";
dex::string dataPath = "data/";
dex::string tmpPath = "data/tmp/";
dex::string toShipPath = "data/toShip/";

// Sizes of our data structures
size_t frontierSize = 50000;
size_t crawledLinksSize = 50000;
size_t robotsMapSize = 5000;

// All urls in the frontier must be known to be in our domain
// and lead to a legitimate endpoint, or must be unknown. This
// means we do not put broken links into our frontier and we do
// not put links that aren't our responsibility into our frontier

dex::frontier urlFrontier( frontierSize, nullptr );
pthread_mutex_t frontierLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t frontierCV = PTHREAD_COND_INITIALIZER;

char state = 0;
size_t numCrawledLinks = 0;
pthread_mutex_t crawledLinksLock = PTHREAD_MUTEX_INITIALIZER;

#define numWorkers 100
pthread_t workers [ numWorkers ];
int ids[ numWorkers ];

dex::robotsMap robotsCache( robotsMapSize );
pthread_mutex_t robotsLock = PTHREAD_MUTEX_INITIALIZER;

dex::unorderedSet < dex::string > crawledLinks;
dex::sharedReaderLock crawledLock;

// This is used to hash URLs
size_t numInstances;
size_t instanceId;

pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;

void print( const char *toPrint )
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

// Checkpoint is time elapsed to save log, performance, and data structure
long checkpointLog = 10;
long checkpointPerformance = 30;
long checkpointDataStructure = 2 * 60; // checkpoints every x seconds
long testTime = 40;
time_t lastLogCheckpoint = time( NULL );
time_t lastPerformanceCheckpoint = time( NULL );
time_t lastDataCheckpoint = time( NULL );
time_t startTime = time( NULL );
bool testing = false;

// Keep log file and performance file descriptors open.
// Delete old log files and performance files when you create a new one.
int logFileDescriptor;
int performanceFileDescriptor;
dex::string logFile;
dex::string performanceFile;
size_t logFileMaxSize = 100000000;
size_t performanceFileMaxSize = 100000000;
pthread_mutex_t loggingLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t performanceLock = PTHREAD_MUTEX_INITIALIZER;

int log( dex::string toWrite )
	{
	pthread_mutex_lock( &loggingLock );
	if ( dex::fileSize( logFileDescriptor ) > logFileMaxSize )
		{
		std::cout << "log too big, switching" << std::endl;
		close( logFileDescriptor );
		logFileDescriptor = createNewLog( tmpPath + "logs/", logFile );
		}

	int error = write( logFileDescriptor, toWrite.cStr( ), toWrite.size( ) );
	if ( error == -1 )
		{
		std::cerr << "Couldn't write to log";
		throw dex::fileWriteException( );
		}

	pthread_mutex_unlock( &loggingLock );
	return error;
	}

int writePerformance( dex::string &toWrite )
	{
	if ( dex::fileSize( performanceFileDescriptor ) > performanceFileMaxSize )
		{
		close( performanceFileDescriptor );
		performanceFileDescriptor = dex::createNewPerformanceFile( ( tmpPath + "performance" ).cStr( ), performanceFile );
		}

	int error = write( performanceFileDescriptor, toWrite.cStr( ), toWrite.size( ) );
	if ( error == -1 )
		{
		std::cerr << "Couldn't write to performance";
		throw dex::fileWriteException( );
		}

	close( performanceFileDescriptor );
	performanceFileDescriptor = open( performanceFile.cStr( ), O_WRONLY | O_APPEND, S_IRWXU );
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
void saveDataStructures( )
	{	
	dex::saveFrontier( ( tmpPath + "savedFrontier.txt" ).cStr( ), urlFrontier );
	crawledLock.readLock( );
	dex::saveCrawledLinks( "data/crawledLinks.txt", crawledLinks );
	crawledLock.releaseReadLock( );
	lastDataCheckpoint = time(NULL);
	}

// checkpoint performance every 15 seconds
void savePerformance( )
	{
	crawledLock.readLock( );
	dex::string toWrite;
	toWrite.reserve( 1000 );
	toWrite += "Number of links crawled in " + dex::toString( checkpointPerformance ) + " seconds: " + dex::toString( numCrawledLinks) + "\n";
	toWrite += "Size of frontier: " + dex::toString( urlFrontier.size( ) ) + "\n";
	toWrite += "Capacity of frontier: " + dex::toString( urlFrontier.capacity( ) ) + "\n";
	toWrite += "Size of crawledLinks " + dex::toString( crawledLinks.size( ) ) + "\n";
	toWrite += "Capacity of crawledLinks " + dex::toString( crawledLinks.bucketCount( ) ) + "\n";
	crawledLock.releaseReadLock( );
	toWrite += "Size of robotsMap " + dex::toString( robotsCache.size( ) ) + "\n";
	toWrite += "Capacity of robotsMap " + dex::toString( robotsCache.capacity( ) ) + "\n";

	writePerformance( toWrite );
	print( toWrite );

	numCrawledLinks = 0;
	lastPerformanceCheckpoint = time( NULL );
	}

void *worker( void *args )
	{
	int a = * ( ( int * ) args );
	dex::string name = dex::toString( a + 1000 * instanceId );
	dex::string folderPath = savePath + "html/" + name + "/";
	int currentFileDescriptor = dex::getCurrentFileDescriptor( folderPath );
	
	log( "Start thread " + name + "\n");
	//print( "fileName" );
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
		while ( alreadyCrawled( toCrawl ) || !robotsCache.politeToVisit( toCrawl.getHost( ), toCrawl.getPath( ) ) )
			{
			if ( !alreadyCrawled( toCrawl ) )
				{
				urlFrontier.putUrl( toCrawl );
				}
			toCrawl = urlFrontier.getUrl( );
			}
		if ( time( NULL ) - lastDataCheckpoint > checkpointDataStructure || state == 's' )
			{
			saveDataStructures( );
			state = 0;
			}

		// one worker should write the performanc at every checkpoint
		pthread_mutex_lock( &performanceLock );
		if ( time( NULL ) - lastPerformanceCheckpoint > checkpointPerformance )
			{
			savePerformance( );
			}
		pthread_mutex_unlock( &performanceLock );

		if ( testing && time( NULL ) - startTime > testTime )
			{
			
			pthread_mutex_unlock( &frontierLock );
			exit( 0 );
			}
		pthread_mutex_unlock( &frontierLock );
		
		log( name + ": Connecting to " + toCrawl.completeUrl( ) + "\n" );
		dex::string result;
		result.reserve( 2000000 );
		if ( robotsCache.purge( ) == 1 )
			print( "Purged Robot Cache" );
		int errorCode = dex::crawler::crawlUrl( toCrawl, result, robotsCache );
		addToCrawled( toCrawl );
		log( name + ": crawled domain: " + toCrawl.completeUrl( ) + " error code: " + dex::toString( errorCode ) + "\n" );
		// If we get a response from the url, nice. We've hit an endpoint that gives us some HTML.
		if ( errorCode == 0 || errorCode == dex::NOT_HTML )
			{
			if ( errorCode == dex::NOT_HTML )
				{
				print( toCrawl.completeUrl( ) + " is not html " );
				}
				

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
				
				for ( auto it = links.begin( );  it != links.end( );  ++it )
					{
					dex::Url current = *it;
					current.setFragment( "" );
					if ( current.getHost( ) != "getstencil.com" )
						{
						// Check to see if the endpoint we have is a known broken link or if we've already crawled
						if ( !alreadyCrawled( current) )
							{
							size_t urlId = getUrlInstance( *it );
							if ( urlId == instanceId )
								{
								urlFrontier.putUrl( *it );
								}
							}
						}
					
					}
				pthread_cond_signal( &frontierCV );
				
				pthread_mutex_unlock( &frontierLock );
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
		if ( errorCode >= 300 && errorCode < 400 )
			{
			dex::Url location = dex::Url( result.cStr( ) );
			size_t urlId = getUrlInstance( location );
			if ( urlId == instanceId )
				{
				pthread_mutex_lock( &frontierLock );
				
				urlFrontier.putUrl( location );
				
				pthread_mutex_unlock( &frontierLock );
				}
			}
		// This link doesn't lead anywhere, we need to add it to our broken links
		if ( errorCode >= 400 || errorCode == dex::DISALLOWED_ERROR || errorCode == dex::RESPONSE_TOO_LARGE )
			{
			}
		}
	return nullptr;
	}



int main( )
	{
	while( state != 'q' ) 
		{
		try
			{
			// setup logging file for this run
			// goodbye error code 13
			dex::pair < size_t, size_t > instanceInfo = dex::getInstanceInfo( "data/instanceInfo.txt" );
			numInstances = instanceInfo.first;
			instanceId = instanceInfo.second;
			if ( numInstances == 0 && instanceId == 0 )
				{
				std::cerr << "Need to have file in data/instanceInfo.txt with the instance information. Example: \nnumInstances=6\ninstanceId=2\n";
				return 0;
				}
			signal(SIGPIPE, SIG_IGN);
			dex::makeDirectory( savePath.cStr( ) );
			dex::makeDirectory( ( savePath + "/html" ).cStr( ) );
			for ( size_t i = 0;  i < numWorkers;  ++i )
				{
				dex::makeDirectory( ( savePath + "html/" + dex::toString( i + 1000 * instanceId ) ).cStr( ) );
				}
			dex::makeDirectory( tmpPath.cStr( ) );
			dex::makeDirectory( ( tmpPath + "logs" ).cStr( ) );
			dex::makeDirectory( ( tmpPath + "performance" ).cStr( ) );
			dex::makeDirectory( toShipPath.cStr( ) );

			std::cout << " creating log and performance files" << std::endl;
			logFileDescriptor = dex::createNewLog( tmpPath + "logs/", logFile );
			performanceFileDescriptor = dex::createNewPerformanceFile( tmpPath + "performance/", performanceFile );
			std::cout << " created log and performance files" << std::endl;

			urlFrontier = dex::loadFrontier( ( tmpPath + "savedFrontier.txt" ).cStr( ), frontierSize, &robotsCache, true );
			if ( urlFrontier.size( ) == 0 )
				{
				urlFrontier = dex::loadFrontier( "data/seedlist.txt", frontierSize, &robotsCache );
				}
			crawledLinks = dex::loadCrawledLinks( ( "data/crawledLinks.txt" ) );
			/*for ( auto it = urlFrontier.begin( );  it != urlFrontier.end( );  ++it )
				print( it->completeUrl( ) );*/

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
		catch ( const std::bad_alloc& )
			{
			std::cerr << "Invalid memory access or allocation" << std::endl;
			robotsCache.purge( );
			/*crawledLock.writeLock( );
			crawledLinks.clear( );
			crawledLock.releaseWriteLock( );*/
			}
		}
	}
