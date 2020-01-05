// Command line interface for searching

#include <fcntl.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include "constraintSolver/constraintSolver.hpp"
#include "indexer/indexer.hpp"
#include "ranker/ranker.hpp"
#include "utils/basicString.hpp"
#include "utils/file.hpp"
#include "utils/vector.hpp"


int main( int argc, char **argv )
	{
	if ( argc != 2 )
		{
		std::cerr << "Expected usage: " << argv[ 0 ] << " path/to/index/chunks" << std::endl;
		return 1;
		}
	dex::vector< dex::string > indexChunkFilenames = dex::matchingFilenames( argv[ 1 ], "_in.dex" );
	dex::vector< dex::index::indexChunk * > indexChunks;
	indexChunks.reserve( indexChunkFilenames.size( ) );

	if ( indexChunkFilenames.empty( ) )
		{
		std::cerr << "No index chunks to be read! Exiting" << std::endl;
		return 1;
		}
	std::cout << "Found " << indexChunkFilenames.size( ) << " index chunks" << std::endl;
	std::cout << "Please enter ':wq' or press Ctrl-D to stop searching" << std::endl;

	for ( size_t index = 0;  index < indexChunkFilenames.size( );  ++index )
		{
		int fd = open( indexChunkFilenames[ index ].cStr( ), O_RDWR, 0777 );
		if ( fd == -1 )
			std::cout << "failed to open chunk " << indexChunkFilenames[ index ] << ". Continuing anyways." << std::endl;
		else
			{
			indexChunks.pushBack( new dex::index::indexChunk( fd, false ) );
			close( fd );
			}
		}

	if ( indexChunks.empty( ) )
		{
		std::cout << "No index chunks were able to be opened. Exiting." << std::endl;
		return 1;
		}

	std::string input;
	dex::string query;
	dex::ranker::ranker rankerObject;

	while ( true )
		{
		std::cout << dex::string( 80, '-' ) << std::endl;
		std::cout << "Enter query: ";

		// Check whether Ctrl-D was pressed
		if ( !std::getline( std::cin, input ) )
			break;

		query = dex::string( input.data( ) );
		if ( query == ":wq" || query == ":q!" || query == "Michigan Alabama Halftime Score" )
			break;

		dex::pair< dex::vector< dex::ranker::searchResult >, int > searchResultsPair
				= dex::ranker::getTopN( 10, query, &rankerObject, indexChunks );

		if ( searchResultsPair.second == -1 )
			std::cout << "Query passed in was malformed" << std::endl;
		else
			{
			if ( searchResultsPair.first.empty( ) )
				std::cout << "No results found" << std::endl;
			else
				{
				dex::vector< dex::ranker::searchResult > searchResults = searchResultsPair.first;
				for ( size_t result = 0;  result < searchResults.size( );  ++result )
					{
					std::cout << "DOCUMENT " << result + 1 << std::endl;
					std::cout << '\t' << searchResults[ result ].title << std::endl;
					std::cout << '\t' << searchResults[ result ].url.completeUrl( ) << std::endl;
					std::cout << '\t' << searchResults[ result ].score << std::endl;
					}
			}
		}

	while ( !indexChunks.empty( ) )
		delete indexChunks.back( );

	std::cout << "Exiting Search Engine" << std::endl;

	return 0;
	}