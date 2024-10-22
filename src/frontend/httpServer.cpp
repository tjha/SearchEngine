#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <pthread.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <cassert>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>

#include "indexer/indexer.hpp"
#include "ranker/ranker.hpp"
#include "utils/basicString.hpp"
#include "utils/file.hpp"

dex::string pathToHtml( "src/frontend/" );

// Temporary searchResults

dex::vector< dex::ranker::searchResult > tmpSearchResults =
	{
	{ "http://www.apple.com/", "The new iphone" },
	{ "http://www.amazon.com/", "Tejas' work place" },
	{ "http://www.nicolehamilton.com/", "#1 teacher" }
	};

// Global variables for ranker
dex::vector< dex::index::indexChunk * > indexChunkObjects;

//  Multipurpose Internet Mail Extensions (MIME) types
struct MimetypeMap
	{
	const char *Extension,
		*Mimetype;
	};

const MimetypeMap MimeTable[ ] =
	{
	// List of some of the most common MIME types.
	// https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Complete_list_of_MIME_types
	".css",     "text/css",
	".htm",     "text/html",
	".html",    "text/html",
	".jpeg",    "image/jpeg",
	".jpg",     "image/jpeg",
	".png",     "image/png",
	".svg",     "image/svg+xml",
	};

const char *Mimetype( const dex::string filename )
	{
	// If a matching a extentsion is found return the corresponding MIME type.
	// Else, anything not matched is an "octet-stream", treated as an unknown binary, which can be downloaded.
	for ( size_t i = 0;  i < 66;  ++i )
		{
		const char *extension = MimeTable[ i ].Extension;
		if ( filename.rfind( extension ) == filename.size( ) - strlen( extension ) )
			return MimeTable[ i ].Mimetype;
		}
	return "application/octet-stream";
	}

// Use to determine Content Length
off_t FileSize( int f )
	{
	struct stat fileInfo;
	fstat( f, &fileInfo );
	return fileInfo.st_size;
	}

bool pathIsLegal( dex::string path )
	{
	return path.find( "/../" ) == dex::string::npos && ( path.size( ) < 3 || path.substr( path.size( ) - 3, 3) != "/.." );
	}

dex::string outputResult( dex::ranker::searchResult &result )
	{
	return "\t\t\t\t\t<li class=\"result\"><a href=" + result.url.completeUrl( ) +
			">" + result.url.completeUrl( ) + "</a> " + result.title + "</li>\n";
	}

void *Talk( void *p )
	{
	size_t bufferSize = 10240;
	char buffer[ bufferSize ];
	int bytes;
	int socket = *( int * ) p;
	delete ( int * )p;

	dex::string request;
	bytes = recv( socket, buffer, bufferSize, 0 );
	request.append( buffer, buffer + bytes );

	if ( request.substr( 0, 3 ) != "GET" )
		{
		std::cout << "Invalid Request, can only serve get requests." << std::endl;
		return nullptr;
		}

	size_t typeEnd = request.find( " /" ), pathEnd, protocolEnd = request.find( "\r\n" );
	pathEnd = request.find( ' ', typeEnd + 1 );
	if ( pathEnd >= protocolEnd )
		return nullptr;
	dex::string path( request.substr( typeEnd + 2, pathEnd - typeEnd - 2 ).cStr( ) );
	dex::string query;
	bool toggle = false;

	size_t r = path.find( "results.html" );
	size_t q = path.find( "?query=" );
	size_t t = path.find( "&toggle=" );
	if ( r != dex::string::npos && q != dex::string::npos &&
		  t != dex::string::npos && r + 12 == q )
		{
		query = path.substr( q + 7, t - q - 7 );

		if ( path.substr( t + 8, pathEnd - t - 8 ) == "true" )
			toggle = true;
		path = "results.html";
		}

	// Loop through and convert %20 to spaces
	size_t spacePos = query.find( "%20" );
	while ( spacePos != dex::string::npos )
		{
		query.replace( spacePos, 3, " " );
		spacePos = query.find( "%20" );
		}

	// Redirect to Easter Egg Page
	if ( query == "Team Socket" )
		{
		dex::string responseHeader = "HTTP/1.1 301 Moved Permanently\r\nLocation: /team.html\r\nConnection: close\r\n\r\n";
		send( socket, responseHeader.data( ), responseHeader.size( ), 0 );
		return nullptr;
		}

	// reroute no path to index.html
	if ( path.empty( ) )
		path = "index.html";

	// adjust path so it can be found in repo
	path.insert( 0, pathToHtml );

	int file;
	if ( !pathIsLegal( path ) || ( file = open( path.cStr( ), O_RDONLY ) ) == -1 )
		{
		dex::string responseHeader = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
		send( socket, responseHeader.data( ), responseHeader.size( ), 0 );
		return nullptr;
		}

	// ranker stuff
	dex::ranker::ranker rankerObject;
	dex::pair< dex::vector< dex::ranker::searchResult >, int > searchResultsPair = dex::ranker::getTopN
			( 10, query, &rankerObject, indexChunkObjects );
	dex::vector< dex::ranker::searchResult > searchResults;
	if ( !query.empty( ) && searchResultsPair.second != -1 )
		{
		// results returned
		searchResults = searchResultsPair.first;
		}

	struct stat fileInfo;
	fstat( file, &fileInfo );
	char *map = ( char * )mmap( nullptr, fileInfo.st_size, PROT_READ, MAP_PRIVATE, file, 0 );
	dex::string content;
	size_t resultsSize = 0;
	if ( path == pathToHtml + "results.html" )
		{
		std::cout << "?query=" << query << std::endl;
		content = dex::string( map );
		content.insert( content.find( "placeholder=" ) + 13, query );
		for ( size_t i = 0;  i < searchResults.size( );  ++i )
			{
			dex::string searchRes( outputResult( searchResults[ searchResults.size( ) - i - 1 ] ) );
			resultsSize += searchRes.size( );
			content.insert( content.find( "class=\"results\">" ) + 17, searchRes );
			std::cout << i << ": " << searchRes << std::endl;
			}
		}

	if ( map == MAP_FAILED )
		return nullptr;

	long int totalSize = fileInfo.st_size + query.size( ) + resultsSize;
	char buff[ sizeof(long int) + 1 ];
	snprintf( buff, sizeof( buff ), "%ld", totalSize );
	dex::string responseHeader = "HTTP/1.1 200 OK\r\nContent-Length: "
		+ dex::string( buff )
		+ "\r\nConnection: close\r\nContent-Type: " + Mimetype( path ) + "\r\n\r\n";
	send( socket, responseHeader.data( ), responseHeader.size( ), 0 );

	if ( path == pathToHtml + "results.html" )
		send( socket, content.data( ), content.size( ), 0 );
	else
		{
		int sent = 0;
		for ( int i = 0;  i != fileInfo.st_size / bufferSize;  ++i )
			send( socket, map + i * bufferSize, bufferSize, 0 );
			sent += bufferSize;

		send( socket, map + bufferSize * ( fileInfo.st_size / bufferSize ), fileInfo.st_size % bufferSize, 0 );
		}

	close( file );
	return nullptr;
	}


int main( int argc, char **argv )
	{
	if ( argc != 3 )
		{
		std::cerr << "Usage: ./build/server.exe port path/to/index/chunks" << std::endl;
		return 1;
		}

	int port = atoi( argv[ 1 ] );

	std::cout << "Starting Server..." << std::endl;

	struct sockaddr_in listenAddress, talkAddress;
	socklen_t talkAddressLength;

	memset( &listenAddress, 0, sizeof( listenAddress ) );
	memset( &talkAddress, 0, sizeof( talkAddress ) );

	listenAddress.sin_family = AF_INET;
	listenAddress.sin_port = htons( port );
	listenAddress.sin_addr.s_addr = htonl( INADDR_ANY );

	int listenSockfd = socket( AF_INET, SOCK_STREAM, 0 );
	int talkSockfd;

	int yesval = 1;
	if ( setsockopt( listenSockfd, SOL_SOCKET, SO_REUSEADDR, &yesval, sizeof(int) ) == -1 )
		{
		std::cerr << "Error setting socket options" << std::endl;
		return 1;
		}

	if ( listenSockfd == -1 )
		{
		std::cerr << "Could not create socket" << std::endl;
		return 1;
		}

	if ( bind( listenSockfd, ( struct sockaddr * )&listenAddress, sizeof( listenAddress ) ) == -1 )
		{
		std::cerr << "Could not bind socket" << std::endl;
		return 1;
		}

	if ( listen( listenSockfd, SOMAXCONN ) == -1 )
		{
		std::cerr << "Could not start listening" << std::endl;
		return 1;
		}

	// Create indexChunkObjects
	dex::string indexChunkDirectory( argv[ 2 ] ); // Top directory of search
	dex::string pattern = ".dex";
	dex::vector< dex::string > indexChunkFilenames = dex::matchingFilenames( indexChunkDirectory, pattern );
	indexChunkObjects.reserve( indexChunkFilenames.size( ) );
	for ( dex::vector< dex::string >::constIterator filenameIterator = indexChunkFilenames.cbegin( );
			filenameIterator != indexChunkFilenames.cend( );  filenameIterator++ )
		{
		int fd = open( filenameIterator->cStr( ), O_RDWR );
		if ( fd == -1 )
			{
			std::cerr << "fd is -1 for " << *filenameIterator<< " something's gone wrong" << std::endl;
			return 1;
			}
		indexChunkObjects.pushBack( new dex::index::indexChunk( fd, false ) );
		}

	while ( ( talkAddressLength = sizeof( talkAddress ),
			talkSockfd = accept( listenSockfd, ( struct sockaddr * )&talkAddress, &talkAddressLength ) )
			&& talkSockfd != -1 )
		{
		pthread_t child;
		pthread_create( &child, nullptr, Talk, new int( talkSockfd ) );
		pthread_detach( child );
		}

	for ( dex::vector< dex::index::indexChunk * >::constIterator indexChunkObjectIterator = indexChunkObjects.cbegin( );
			indexChunkObjectIterator != indexChunkObjects.cend( );  indexChunkObjectIterator++ )
		delete *indexChunkObjectIterator;

	}
