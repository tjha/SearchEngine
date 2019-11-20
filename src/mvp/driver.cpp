// Implementation of basic mercator architecture

// 2019-11-20: Add logging, add file structure for saving html: combsc
// 2019-11-16: Init Commit: combsc
#include "../spinarak/crawler.hpp"
#include "../utils/basicString.hpp"
#include "../utils/vector.hpp"
#include "frontier.hpp"

struct workerStruct
	{
	dex::string name;
	};


dex::string loggingFileName;
pthread_mutex_t loggingLock = PTHREAD_MUTEX_INITIALIZER;

dex::frontier urlFrontier;
pthread_mutex_t frontierLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t frontierCV;
pthread_t workers [ 1 ];

dex::unorderedMap < dex::string, dex::RobotTxt > robotsCache{ 1000 };
pthread_mutex_t robotsLock = PTHREAD_MUTEX_INITIALIZER;

dex::vector < dex::Url > brokenLinks;
pthread_mutex_t brokenLinksLock = PTHREAD_MUTEX_INITIALIZER;

dex::vector < dex::Url > linksToShip;
pthread_mutex_t linksToShipLock = PTHREAD_MUTEX_INITIALIZER;

void log( dex::string toWrite )
	{
	pthread_mutex_lock( &loggingLock );
	dex::appendToFile( loggingFileName.cStr( ), toWrite.cStr( ), toWrite.size( ) );
	pthread_mutex_unlock( &loggingLock );
	}


// Folder structure
// Hash the URL
// 2 layers of folders
// bytes 1 determines the first folder
// bytes 2 determines the second folder
// bytes 3-4 determine the name of the files
// This gives us 4,294,967,296 possible locations for html
int saveHtml( dex::Url url )
	{
	dex::hash < dex::string > hasher;
	unsigned long h = hasher( url.completeUrl( ) );
	unsigned long first = h & 0x000000FF;
	unsigned long second = ( h & 0x0000FF00 ) >> 8;
	unsigned long name = ( h & 0xFFFF0000 ) >> 16 ;
	std::cout << "html/" + dex::toString( first ) + "/" + dex::toString( second ) + "/" + dex::toString( name ) + ".html" << std::endl;
	int err = dex::makeDirectory( "html" );
	if ( err == -1 )
		return err;
	dex::string dirName = "html/" + dex::toString( first );
	err = dex::makeDirectory( dirName.cStr( ) );
	if ( err == -1 )
		return err;
	dirName = "html/" + dex::toString( first ) + "/" + dex::toString( second );
	err = dex::makeDirectory( dirName.cStr( ) );
	if ( err == -1 )
		return err;
	dex::string filename = "html/" + dex::toString( first ) + "/" + dex::toString( second ) + "/" + dex::toString( name ) + ".html";
	err = dex::writeToFile( filename.cStr( ), url.completeUrl( ).cStr( ), url.completeUrl( ).size( ) );
	return err;
	}

dex::vector < dex::Url > fakeParseForLinks( dex::string html )
	{
	dex::vector < dex::Url > toReturn;
	toReturn.pushBack( dex::Url( "https://www.runescape.com" ) );
	html += 'b';
	return toReturn;
	}

dex::Url fakeRedirectLink( dex::Url in)
	{
	log( "Fixed link " + in.completeUrl( ) + "\n" );
	in.setPath( "fixed" );
	return in;
	}

// We should have a function for checking URLs to see if they're in our
// domain or not. If it's in our domain, put it back into our frontier.
// if it's not in our domain, we need to send it to the other crawler
// workers.
bool fakeUrlInDomain( const dex::Url &url )
	{
	return true;
	}

int fakeCrawl( dex::Url url, dex::string &result, dex::unorderedMap < dex::string, dex::RobotTxt > &robots )
	{
	return 0;
	}


// This function is not trivial. For example, suppose we know a -> c and
// b -> c. Let's say we find that c -> d. We need to update a -> d and
// b -> d. The values in our redirect map should either be known endpoints
// or unknown URLs. Also if we find a broken URL we should check to see if it's
// a value in our map and remove the entry.
void fakeAddToRedirectMap( const dex::Url &key, const dex::Url &val )
	{
	log( "Added " + key.completeUrl( ) + "->" + val.completeUrl( ) + " to our redirect map\n" );
	}

void *worker( void *args )
	{
	workerStruct a = *static_cast <workerStruct*>(args);
	dex::string name = a.name;
	for ( int i = 0;  i < 3;  ++i )
		{
		pthread_mutex_lock( &frontierLock );
		while ( urlFrontier.empty( ) )
			{
			pthread_cond_wait( &frontierCV, &frontierLock );
			}
		dex::Url toCrawl = urlFrontier.getUrl( );
		pthread_mutex_unlock( &frontierLock );
		log( name + ": Connecting to " + toCrawl.completeUrl( ) + "\n" );
		dex::string result;
		// I know that it's not safe to use robotsCache here
		// We need to rewrite crawlURL to use the robotsCache efficiently, don't want to
		// lock the cache for the entire time we're crawling the URL.
		int errorCode = fakeCrawl( toCrawl, result, robotsCache );
		log( name + ": crawled domain: " + toCrawl.completeUrl( ) + " error code: " + dex::toString( errorCode ) + "\n" );
		if ( errorCode == 0 )
			{
			dex::string html = result;
			dex::vector < dex::Url > links = fakeParseForLinks( html );
			
			for ( auto it = links.cbegin( );  it != links.cend( );  ++it )
				{
				if ( fakeUrlInDomain( *it ) )
					{
					pthread_mutex_lock( &frontierLock );
					urlFrontier.putUrl( fakeRedirectLink( *it ) );
					pthread_mutex_unlock( &frontierLock );
					}
				else
					{
					// No need to call fakeRedirectLink since it shouldn't be in our domain anyways
					pthread_mutex_lock( &linksToShipLock );
					linksToShip.pushBack( *it );
					pthread_mutex_unlock( &linksToShipLock );
					}
				}
			}

		if ( errorCode == dex::POLITENESS_ERROR )
			{
			// What should we do with the url?
			pthread_mutex_lock( &frontierLock );
			urlFrontier.putUrl( fakeRedirectLink( toCrawl ) );
			pthread_mutex_unlock( &frontierLock );
			}
		if ( errorCode >= 300 && errorCode < 400 )
			{
			if ( fakeUrlInDomain( dex::Url( result.cStr( ) ) ) )
				{
				pthread_mutex_lock( &frontierLock );
				urlFrontier.putUrl( fakeRedirectLink( dex::Url( result.cStr( ) ) ) );
				pthread_mutex_unlock( &frontierLock );
				fakeAddToRedirectMap( toCrawl, dex::Url( result.cStr( ) ) );
				}
			else
				{
				// No need to call fakeRedirectLink since it shouldn't be in our domain anyways
				pthread_mutex_lock( &linksToShipLock );
				linksToShip.pushBack( dex::Url( result.cStr( ) ) );
				pthread_mutex_unlock( &linksToShipLock );
				}
			}
		if ( errorCode >= 400 )
			{
			pthread_mutex_lock( &brokenLinksLock );
			brokenLinks.pushBack( toCrawl );
			pthread_mutex_unlock( &brokenLinksLock );
			// All links that redirect to this should also be categorized as broken.
			}
		}
	return nullptr;
	}



int main( )
	{
	loggingFileName = "logs/";
	time_t now = time( nullptr );
	loggingFileName += ctime( &now );
	loggingFileName.popBack( );
	loggingFileName += ".log";
	loggingFileName = loggingFileName.replaceWhitespace( "_" );
	urlFrontier.putUrl( "https://www.bonescape.bomb" );
	workerStruct a = { "test" };
	pthread_create( &workers[ 0 ], nullptr, worker, static_cast < void * > ( &a ) );
	pthread_join( workers[ 0 ], nullptr );
	saveHtml( "https://www.bonescape.bomb" );
	return 0;
	}