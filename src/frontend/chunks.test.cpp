// file for testing chunks
// Not going to use catch2 format yet.
#include "indexer/index.hpp"
#include "ranker/ranker.hpp"
#include "utils/basicString.hpp"
#include "utils/vector.hpp"
#include "utils/file.hpp"
#include <assert.h>


// Global variables for ranker
dex::vector < dex::index::indexChunk * > indexChunkPointers;

int findIndexChunks( dex::string chunkDirectory, dex::string chunkEndPattern )
	{
	int ret = 0;
	dex::vector< dex::string > indexChunkFilenames = dex::matchingFilenames( chunkDirectory, chunkEndPattern );
	indexChunkPointers.reserve( indexChunkFilenames.size( ) );
	for ( dex::vector< dex::string >::constIterator filenameIterator = indexChunkFilenames.cbegin( );
			filenameIterator != indexChunkFilenames.cend( );  filenameIterator++ )
		{
		int fd = open( filenameIterator->cStr( ), O_RDWR );
		if ( fd == -1 )
			{
			std::cerr << "fd is -1 for " << *filenameIterator << " something's gone wrong" << std::endl;
			ret = -1;
			}
		indexChunkPointers.pushBack( new dex::index::indexChunk( fd, false ) );	
		}
	return ret;
	}

dex::pair < dex::vector < dex::ranker::searchResult >, int > processChunks( dex::string query )
	{
	dex::ranker::ranker rankerObject;
	dex::pair < dex::vector < dex::ranker::searchResult >, int > searchResults = dex::ranker::getTopN( 10, query, 
			&rankerObject, indexChunkPointers );
	return searchResults;
	}

int main( int argc, char **argv )
	{
	if ( argc < 2 )
		{
		std::cerr << "Usage: build/frontend/chunks.test.exe pathToChunks" << std::endl;
		return 1;
		}

	// Find index chunks in the folder given
	if ( findIndexChunks( argv[ 1 ], "_in.dex" ) == -1 )
		return 1;


	// Test Case 1: Query that we expect to find documents for
	// TODO make a destructor for matchedDocuments objects
	// TODO make the string that the parser takes in a const reference.
	dex::string query = "apple";
	dex::queryCompiler::parser parser;
	dex::queryCompiler::matchedDocumentsGenerator mdg = parser.parse( query );
	dex::queryCompiler::matchedDocuments *testdocs = mdg( indexChunkPointers.front( ) );
	std::cout << "Asserting that the matchingDocumentsISR for 'apple' is not empty" << std::endl;
	size_t isrLength = 0;
	while( testdocs->matchingDocumentISR->next( ) != dex::constraintSolver::ISR::npos )
		++isrLength;
	delete testdocs->matchingDocumentISR;
	delete testdocs;
	assert( isrLength > 0 );

	query = "qubfiqebfoqbgoqbe";
	mdg = parser.parse( query );
	testdocs = mdg( indexChunkPointers.front( ) );
	std::cout << "Asserting that the matchingDocumentsISR for 'qubfiqebfoqbgoqbe' is empty" << std::endl;
	isrLength = 0;
	isrLength = 0;
	while( testdocs->matchingDocumentISR->next( ) != dex::constraintSolver::ISR::npos )
		++isrLength;
	delete testdocs->matchingDocumentISR;
	delete testdocs;
	assert( isrLength == 0 );

	for ( dex::vector < dex::index::indexChunk * >::constIterator indexChunkObjectIterator = indexChunkPointers.cbegin( );
			indexChunkObjectIterator != indexChunkPointers.cend( );  indexChunkObjectIterator++ )
		delete *indexChunkObjectIterator;
	
	}
