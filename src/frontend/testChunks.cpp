// file for testing chunks
#include "file.hpp"
#include "index.hpp"
#include "ranker.hpp"
#include "basicString.hpp"


// Global variables for ranker
dex::vector < dex::index::indexChunk * > indexChunkObjects;

dex::pair < dex::vector < dex::searchResult >, int > processChunks( dex::string query )
	{
	dex::ranker rankerObject( indexChunkObjects );
	dex::pair < dex::vector < dex::searchResult >, int > searchResults = rankerObject.getTopN( 10, query );
	return searchResults;
	}

int main( int argc, char **argv )
	{
	if ( argc < 2 )
		{
		std::cerr << "Usage: testChunks.exe pathToChunks query" << std::endl;
		return 1;
		}

	dex::string indexChunkDirector = argv[ 1 ]; // Top directory of search
	dex::string pattern = "_in.dex";
	dex::vector< dex::string > indexChunkFilenames = dex::matchingFilenames( indexChunkDirector, pattern );
	indexChunkObjects.reserve( indexChunkFilenames.size( ) );
	for ( dex::vector< dex::string >::constIterator filenameIterator = indexChunkFilenames.cbegin( );
			filenameIterator != indexChunkFilenames.cend( );  filenameIterator++ )
		{
		int fd = open( filenameIterator->cStr( ), O_RDWR );
		if ( fd == -1 )
			{
			std::cerr << "fd is -1 for " << *filenameIterator << " something's gone wrong" << std::endl;
			return 1;
			}
		indexChunkObjects.pushBack( new dex::index::indexChunk( fd, false ) );	
		}
	dex::string query = argv[ 2 ];
	processChunks( query );

	for ( dex::vector < dex::index::indexChunk * >::constIterator indexChunkObjectIterator = indexChunkObjects.cbegin( );
			indexChunkObjectIterator != indexChunkObjects.cend( );  indexChunkObjectIterator++ )
		delete *indexChunkObjectIterator;
	
	}
