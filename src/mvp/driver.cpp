// Implementation of basic mercator architecture

// 2019-11-16: Init Commit: combsc
#include "../spinarak/crawler.hpp"
#include "../utils/basicString.hpp"
#include "../utils/vector.hpp"
#include "frontier.hpp"

struct workerStruct
	{
	dex::string name;
	};

dex::frontier urlFrontier;
pthread_mutex_t frontierLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t frontierCV;
pthread_t workers [ 1 ];

dex::unorderedMap < string, RobotTxt > robotsCache{ 1000 };
pthread_mutex_t robotsLock = PTHREAD_MUTEX_INITIALIZER;

dex::vector < dex::Url > brokenLinks;
pthread_mutex_t brokenLinksLock = PTHREAD_MUTEX_INITIALIZER;

dex::vector < dex::Url > linksToShip;
pthread_mutex_t linksToShipLock = PTHREAD_MUTEX_INITIALIZER;


dex::vector < dex::Url > fakeParseForLinks( dex::string html )
	{
	dex::vector < dex::Url > toReturn;
	toReturn.pushBack( dex::Url( "https://www.runescape.com" ) );
	html += 'b';
	return toReturn;
	}

dex::Url fakeRedirectLink( const dex::Url &in)
	{
	std::cout << "Fixed link " << in.completeUrl( ) << std::endl;
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

int fakeCrawl( dex::Url url, dex::string &result, dex::unorderedMap < string, RobotTxt > &robots, dex::string contentFilename )
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
	std::cout << "Added " << key.completeUrl( ) << "->" << val.completeUrl( ) << " to our redirect map" << std::endl;
	}

void *worker( void *args )
	{
	workerStruct a = *static_cast <workerStruct*>(args);
	dex::string name = a.name;
	size_t iteration = 0;
	for ( int i = 0;  i < 3;  ++i )
		{
		pthread_mutex_lock( &frontierLock );
		while ( urlFrontier.empty( ) )
			{
			pthread_cond_wait( &frontierCV, &frontierLock );
			}
		dex::Url toCrawl = urlFrontier.getUrl( );
		pthread_mutex_unlock( &frontierLock );
		std::cout << "Connecting to " << toCrawl.completeUrl( ) << std::endl;
		dex::string result;
		dex::string fileName = name + "_" + iteration + ".dex";
		// I know that it's not safe to use robotsCache here
		// We need to rewrite crawlURL to use the robotsCache efficiently, don't want to
		// lock the cache for the entire time we're crawling the URL.
		int errorCode = fakeCrawl( toCrawl, result, robotsCache, fileName );
		if ( errorCode == 0 )
			{
			std::cout << "crawled domain: " << toCrawl.completeUrl( ) << std::endl;
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

		if ( errorCode == dex::politenessError )
			{
			std::cout << "Mr. Robot says to be polite: " << toCrawl.completeUrl( ) << std::endl;
			std::cout << result << std::endl;
			// What should we do with the url?
			pthread_mutex_lock( &frontierLock );
			urlFrontier.putUrl( fakeRedirectLink( toCrawl ) );
			pthread_mutex_unlock( &frontierLock );
			}
		if ( errorCode >= 300 && errorCode < 400 )
			{
			std::cout << "link asks to be redirected: " << toCrawl.completeUrl( ) << std::endl;
			std::cout << "redirect location: " << result << std::endl;
			std::cout << errorCode << std::endl;
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
			std::cout << "link broken: " << toCrawl.completeUrl( ) << std::endl;
			std::cout << errorCode << std::endl;
			pthread_mutex_lock( &brokenLinksLock );
			brokenLinks.pushBack( toCrawl );
			pthread_mutex_unlock( &brokenLinksLock );
			// All links that redirect to this should also be categorized as broken.
			}
		++iteration; 
		}

	return nullptr;
	}



int main( )
	{

	urlFrontier.putUrl( "https://www.bonescape.bomb" );
	workerStruct a = { "test" };
	pthread_create( &workers[ 0 ], nullptr, worker, static_cast < void * > ( &a ) );
	pthread_join( workers[ 0 ], nullptr );
	return 0;
	}