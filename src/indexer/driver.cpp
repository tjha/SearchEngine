// Index Driver process html files into index chunks
//
// 2019-12-11:	edited to call correct functions to index and use lexicoComp: jhirsh, loghead
//             brought in dex::matchingFilenames, write statistics: combsc
// 2019-12-10:	edits to attach to indexer.hpp functions: combsc, loghead
// 2019-12-08:	created index driver to read through directory: jhirsh
//

#include <dirent.h>
#include <iostream>
#include <time.h>
#include "indexer/indexer.hpp"
#include "parser/parser.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/file.hpp"
#include "utils/url.hpp"
#include "utils/utf.hpp"
#include "utils/vector.hpp"

using dex::string;
using dex::vector;

int openFile( int indexChunkCount, dex::string outputFolder )
	{
	std::cout << "outputFolder = " << outputFolder << std::endl <<
			"toString = " << dex::toString( indexChunkCount ) << std::endl;
	outputFolder += dex::toString( indexChunkCount );
	outputFolder += "_in.dex";
	const char * filePath = outputFolder.cStr( );
	std::cout << "filePath " << filePath << std::endl;
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
	//dex::makeDirectory( outputFolder.cStr( ) );
	dex::vector < dex::string > toProcess;
	toProcess = dex::matchingFilenames( batch, "_forIndexer" );
	dex::vector < dex::string > toDelete;
	toDelete = dex::matchingFilenames( batch, "_processed" );
	for ( int index = 0;  index < toDelete.size( );  index++ )
		{
		if ( remove( toDelete[ index ].cStr( ) ) != 0 )
			std::cout << "error deleting " << toDelete[ index ] << "\n";
		}

	dex::utf::decoder < dex::string > stringDecoder;
	dex::vector < dex::string > existingIndexChunks = dex::matchingFilenames( outputFolder, "_in.dex");
	int indexChunkCount = existingIndexChunks.size( );
	int fileDescriptor = openFile( indexChunkCount++, outputFolder );
	dex::index::indexChunk *initializingIndexChunk = new dex::index::indexChunk( fileDescriptor );


	unsigned documentsProcessed = 0;
	unsigned checkpoint = 100;
	time_t start = time( nullptr );
	int statisticsFileDescriptor = open( ( outputFolder + "statistics.txt").cStr( ), O_RDWR | O_CREAT, 0777 );
	size_t totalDocumentsProcessed = 0;
	size_t totalBytesProcessed = 0;
	for ( unsigned index = 0;  index < toProcess.size( );  ++index )
		{
		dex::string fileName = toProcess[ index ];
		// Decode the current file

		std::cout << "About to read file: " << fileName << "\n";
		// Decode the current file
		unsigned char *savedHtml = reinterpret_cast< unsigned char * >( dex::readFromFile( fileName.cStr( ), 0 ) );
		unsigned char *ptr = savedHtml;
		size_t filesize = dex::getFileSize( fileName.cStr( ) );
		while ( static_cast < size_t >( ptr - savedHtml ) < filesize )
			{
			// retrieve the saved url + html pair
			dex::Url url = dex::Url( stringDecoder( ptr, &ptr ).cStr( ) );
			dex::string html = stringDecoder( ptr, &ptr );
			// std::cout << "HTML: \n " << html << "\n";
			try
				{
				// std::cout << "\tAbout to add url: " << url.completeUrl( ) << "\n";
				dex::HTMLparser parser( url, html, true );

				dex::string titleString;
				titleString.reserve( 25 );
				for ( auto &titleWord: parser.ReturnTitle( ) )
					{
					titleString += ( titleWord + " " );
					}

				// TODO this should go in parser but didn't want to break dependent functionality
				// TODO add default argument for anchorText in indexer.hpp
				if ( !initializingIndexChunk->addDocument( url.completeUrl( ), parser.ReturnTitle( ), titleString,
						parser.ReturnWords( ) ) )
					{
					toDelete = dex::matchingFilenames( batch, "_processed" );
					for ( int index = 0;  index < toDelete.size( );  index++ )
						{
						if ( remove( toDelete[ index ].cStr( ) ) != 0 )
							std::cout << "error deleting " << toDelete[ index ] << "\n";
						}
					close( fileDescriptor );
					fileDescriptor = openFile( indexChunkCount++, outputFolder );
					delete initializingIndexChunk;
					initializingIndexChunk = new dex::index::indexChunk( fileDescriptor );
					}
				if ( !initializingIndexChunk->addDocument( url.completeUrl( ), parser.ReturnTitle( ), titleString,
						parser.ReturnWords( ) ) )
					{
					// TODO: Throw an exception. Should not fail to add a document to a new index chunk
					throw dex::fileWriteException( );
					}
				documentsProcessed++;
				if ( documentsProcessed % checkpoint == 0 )
					{
					dex::string toWrite = dex::toString( documentsProcessed ) + " documents processed in " +
										dex::toString( time( nullptr ) - start ) + " seconds\n";
					int error = write( statisticsFileDescriptor, toWrite.cStr( ), toWrite.size( ) );
					if ( error == -1 )
						{
						std::cout << "Failed to write to statistics file " << std::endl;
						throw dex::fileWriteException( );
						}
					documentsProcessed = 0;
					start = time( nullptr );
					}
				totalDocumentsProcessed++;
				}
			catch ( ... )
				{
				// std::cout << "Skipping malformed html: " << url.completeUrl( ) << "\n";
				continue;
				}
			}
		totalBytesProcessed += filesize;
		std::cout << "processed " + fileName << std::endl;
		std::cout << "total documents processed = " << totalDocumentsProcessed << std::endl << "total bytes processed = "
				<< totalBytesProcessed << std::endl;
		string newFilename( fileName );
		newFilename.erase( newFilename.end( ) - sizeof( "forIndexer" ) + 1 );
		newFilename += "processed";
		int renamed = rename( fileName.cStr( ) , ( newFilename ).cStr( ) );
		if ( renamed == -1 )
			{
			std::cout << "Failed to rename " + fileName << std::endl;
			throw dex::fileWriteException( );
			}
		close( fileDescriptor );
		}

	}



