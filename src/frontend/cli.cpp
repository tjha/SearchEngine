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
	dex::vector< dex::index::indexChunk * > indexChunkObjects;
	indexChunkObjects.reserve( indexChunkFilenames.size( ) );

	if ( indexChunkFilenames.empty( ) )
		{
		std::cerr << "No index chunks to be read! Exiting" << std::endl;
		return 1;
		}
	std::cout << "Found " << indexChunkFilenames.size( ) << " index chunks" << std::endl;
	std::cout << "Please enter ':wq' to stop searching" << std::endl;

	for ( size_t index = 0;  index < indexChunkFilenames.size( );  ++index )
		{
		int fd = open( indexChunkFilenames[ index ].cStr( ), O_RDWR, 0777 );
		indexChunkObjects.pushBack( new dex::index::indexChunk( fd, false ) );
		close( fd );
		}

	std::string input;
	dex::string query;
	dex::ranker::ranker rankerObject;

	while ( true )
		{
		std::cout << "Enter query: ";
		if ( !std::getline( std::cin, input ) )
			break;

		query = dex::string( input.data( ) );
		if ( query == ":wq" || query == ":q!" || query == "Michigan Alabama Halftime Score" )
			{
			std::cout << "Exiting Search Engine" << std::endl;
			return 0;
			}
		dex::pair< dex::vector< dex::ranker::searchResult >, int > searchResultsPair
				= dex::ranker::getTopN( 10, query, &rankerObject, indexChunkObjects );

		if ( searchResultsPair.second == -1 )
			std::cout << "\nQuery passed in was malformed" << std::endl;
		else
			{
			dex::vector< dex::ranker::searchResult > searchResults = searchResultsPair.first;
			for ( size_t result = 0;  result < searchResults.size( );  ++result )
				{
				std::cout << "DOCUMENT " << result + 1 << ":\n";
				std::cout << searchResults[ result ].title << '\n' << searchResults[ result ].url.completeUrl( )
						<< '\n' << searchResults[ result ].score << '\n' << std::endl;
				}
			}
		}

	std::cout << std::endl;

	while ( !indexChunkObjects.empty( ) )
		delete indexChunkObjects.back( );

	return 0;
	}