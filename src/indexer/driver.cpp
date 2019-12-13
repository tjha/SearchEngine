// Index Driver process html files into index chunks
//
// 2019-12-11:	edited to call correct functions to index and use lexicoComp: jhirsh, loghead
//             brought in dex::matchingFilenames, write statistics: combsc
// 2019-12-10:	edits to attach to index.hpp functions: combsc, loghead
// 2019-12-08:	created index driver to read through directory: jhirsh
//

#include <iostream>
#include <unordered_set>
#include "dirent.h"
#include <time.h>
#include "index.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "vector.hpp"
#include "basicString.hpp"
#include "parser.hpp"
#include "url.hpp"
#include "utf.hpp"

using dex::string;
using dex::vector;

int openFile( int indexChunkCount, dex::string outputFolder )
	{
	const char * filePath = ( outputFolder + dex::toString( indexChunkCount ) + "_in.dex" ).cStr( );
	return open( filePath, O_RDWR | O_CREAT, 0777 );
	}

int main ( int argc, char ** argv )
	{
	
	if ( argc != 3 )
		{
		std::cerr << "Usage ./build/indexer.exe <batch-name> <chunk-output-folder>";
		exit( 1 );
		}

	string batch = argv[ 1 ];
	string outputFolder = argv[ 2 ];
	if ( outputFolder.back( ) != '/' )
		{
		outputFolder.pushBack( '/' );
		}
	dex::makeDirectory( outputFolder.cStr( ) );
	dex::vector < dex::string > toProcess;
	toProcess = dex::matchingFilenames( batch, "_forIndexer" );

	dex::utf::decoder < dex::string > stringDecoder;
	
	int indexChunkCount = 0;
	// TODO: What scheme will we use to name the files for the index chunks?
	int fileDescriptor = openFile( indexChunkCount++, outputFolder );
	dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fileDescriptor );


	unsigned documentsProcessed = 0;
	unsigned checkpoint = 100;
	time_t start = time( nullptr );
	int statisticsFileDescriptor = open( ( outputFolder + "statistics.txt").cStr( ), O_RDWR | O_CREAT, 0777 );
	
	for ( unsigned index = 0;  index < toProcess.size( );  ++index )
		{
		dex::string fileName = toProcess[ index ];
		// Decode the current file

		std::cout << "About to read file: " << fileName << "\n";
		// Decode the current file
		unsigned char *savedHtml = reinterpret_cast< unsigned char * >( dex::readFromFile( fileName.cStr( ), 0 ) );
		unsigned char *ptr = savedHtml;

		while ( !dex::utf::isSentinel( ptr ) )
			{
			// retrieve the saved url + html pair
			dex::Url url = dex::Url( stringDecoder( ptr, &ptr ).cStr( ) );
			dex::string html = stringDecoder( ptr, &ptr );
			std::cout << "\tAbout to add url: " << url.completeUrl( ) << "\n";
			std::cout << "HTML: \n " << html << "\n";
			/*
			try
				{
				std::cout << "\tAbout to add url: " << url.completeUrl( ) << "\n";
				dex::HTMLparser parser( url, html, true );

				dex::string titleString;
				titleString.reserve( 25 );
				for ( auto &titleWord: parser.ReturnTitle( ) )
					{
					titleString += titleWord;
					}
				std::cout << "\t\twith title: " << titleString << "\n";
				
				// TODO this should go in parser but didn't want to break dependent functionality
				// TODO add default argument for anchorText in index.hpp
				if ( !initializingIndexChunk.addDocument( url.completeUrl( ), parser.ReturnTitle( ), titleString, 
						parser.ReturnWords( ) ) )
					{
					close( fileDescriptor );
					fileDescriptor = openFile( indexChunkCount++, outputFolder );
					initializingIndexChunk = dex::index::indexChunk( fileDescriptor );
					}
				if ( !initializingIndexChunk.addDocument( url.completeUrl( ), parser.ReturnTitle( ), titleString,
						parser.ReturnWords( ) ) )
					{
					// TODO: Throw an exception. Should not fail to add a document to a new index chunk
					throw dex::fileWriteException( );
					}

				}
			catch ( ... )
				{
				std::cout << "Skipping malformed html: " << url.completeUrl( ) << "\n";
				continue;
				}
			*/
			}
		std::cout << "processed " + fileName << std::endl;
		documentsProcessed++;
		int renamed = rename( fileName.cStr( ) , ( fileName ).cStr( ) );
		if ( renamed == -1 )
			{
			std::cout << "Failed to rename " + fileName << std::endl;
			throw dex::fileWriteException( );
			}
		close( fileDescriptor );

		if ( documentsProcessed % checkpoint == 0 || documentsProcessed == toProcess.size( ) )
			{
			dex::string toWrite = dex::toString( documentsProcessed ) + " documents processed in " + 
					dex::toString( time( nullptr ) - start ) + " seconds\n";
			int error = write( statisticsFileDescriptor, toWrite.cStr( ), toWrite.size( ) );
			if ( error == -1 )
				{
				std::cout << "Failed to write to statistics file " << std::endl;
				throw dex::fileWriteException( );
				}
			start = time( nullptr );
			}
		}
	

	}



