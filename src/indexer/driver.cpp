// Index Driver process html files into index chunks
//
// 2019-12-11:	edited to call correct functions to index and use lexicoComp: jhirsh, loghead
//             brought in matchingFilenames, write statistics: combsc
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

using namespace dex;
using std::cerr;
using std::cout;
using std::endl;

int openFile( int indexChunkCount, string outputFolder )
	{
	cout << "outputFolder = " << outputFolder << endl << "toString = " << toString( indexChunkCount ) << endl;
	outputFolder += toString( indexChunkCount );
	outputFolder += "_in.dex";
	const char * filePath = outputFolder.cStr( );
	cout << "filePath " << filePath << endl;
	return open( filePath, O_RDWR | O_CREAT | O_TRUNC, 0777 );
	}

const string indexChunkPattern = ".dex";
const string startPattern = "_forIndexer";
const string intermediatePattern = "_processing";
const string finishedPattern = "_processed";
// TODO SET THIS VALUE TO SOMETHING LEGIT
const size_t maxBytesToProcess = 10000000000;
// const size_t maxBytesToProcess = 100000000;

int renameFile( const string &fileName, const string &patternStart, const string &patternEnd )
	{
	string newFilename( fileName );
	newFilename.replace( newFilename.cend( ) - patternStart.size( ) + 1, newFilename.cend( ),
			patternEnd.cbegin( ), patternEnd.cend( ) );
	int renamed = rename( fileName.cStr( ) , newFilename.cStr( ) );
	if ( renamed == -1 )
		{
		cerr << "Failed to rename " + fileName << endl;
		return -1;
		}
	return 0;
	}

int renameAll( const string &dirPath, const string &patternStart, const string &patternEnd )
	{
	vector< string > matches = matchingFilenames( dirPath, patternStart );
	for ( size_t i = 0;  i < matches.size( );  ++i )
		if ( renameFile( string( matches[ i ] ), patternStart, patternEnd ) == -1 )
			return -1;
	return 0;
	}

int createIndexChunk( vector< string > toProcess, size_t maxBytesToProcess, string outputFolder )
	{
	// Setup statistics file
	unsigned documentsProcessed = 0;
	const unsigned checkpoint = 100;
	time_t start = time( nullptr );
	int statisticsFileDescriptor = open( ( outputFolder + "statistics.txt").cStr( ), O_RDWR | O_CREAT, 0777 );

	if ( statisticsFileDescriptor == -1 )
		{
		std::cerr << "Could not open statistics file: " << outputFolder + "statistics.txt" << std::endl;
		throw fileOpenException( );
		}

	// declare variables needed
	size_t totalDocumentsProcessed = 0;
	size_t totalBytesProcessed = 0;
	utf::decoder< string > stringDecoder;

	// initialize index chunk
	vector< string > existingIndexChunks = matchingFilenames( outputFolder, indexChunkPattern );
	int indexChunkCount = existingIndexChunks.size( );
	int fileDescriptor = openFile( indexChunkCount++, outputFolder );
	index::indexChunk *initializingIndexChunk = new index::indexChunk( fileDescriptor );

	for ( unsigned index = 1;  index < toProcess.size( );  ++index )
		{
		string fileName = toProcess[ index ];
		// If adding this file would make our index chunk too large, break out of the for loop we're done here.
		size_t filesize = getFileSize( fileName.cStr( ) );
		if ( filesize + totalBytesProcessed >= maxBytesToProcess )
			{
			std::cout << "Too many bytes processed by indexerDriver\n";
			std::cout << "\tfilesize[" << filesize << "]\n";
			break;
			}

		cout << "About to read file: " << fileName << "\n";
		// Decode the current file
		unsigned char *savedHtml = reinterpret_cast< unsigned char * >( readFromFile( fileName.cStr( ), 0 ) );
		unsigned char *ptr = savedHtml;

		// Iterate through the documents in our html file
		while ( static_cast< size_t >( ptr - savedHtml ) < filesize )
			{
			// retrieve the saved url + html pair
			Url url = Url( stringDecoder( ptr, &ptr ).cStr( ) );
			string html = stringDecoder( ptr, &ptr );
			try
				{
				HTMLparser parser( url, html, true );

				string titleString;
				titleString.reserve( 25 );
				for ( auto &titleWord : parser.ReturnTitle( ) )
					titleString += ( titleWord + " " );

				// Attempt to add this section of our html file into the index chunk
				// The constants should be large enough such that we can fit maxBytesToProcess in
				// before the index chunk fills up
				if ( !initializingIndexChunk->addDocument( url.completeUrl( ), parser.ReturnTitle( ), titleString,
						parser.ReturnWords( ) ) )
					{
					cout << "somehow failed to add a document into the index chunk" << endl;
					throw fileWriteException( );
					}
				++documentsProcessed;

				if ( documentsProcessed % checkpoint == 0 )
					{
					string toWrite = toString( documentsProcessed ) + " documents processed in " +
										toString( time( nullptr ) - start ) + " seconds\n";
					int error = write( statisticsFileDescriptor, toWrite.cStr( ), toWrite.size( ) );
					if ( error == -1 )
						{
						cerr << "Failed to write to statistics file " << endl;
						throw fileWriteException( );
						}
					documentsProcessed = 0;
					start = time( nullptr );
					}
				++totalDocumentsProcessed;
				}
			// catch ( ... )
			catch ( dex::outOfRangeException e )
				{
				cout << "Parser threw out of range exception\n";
				continue;
				}
			catch ( fileWriteException )
				{
				// cerr << "Skipping malformed html: " << url.completeUrl( ) << "\n";
				cout << "\tSkipping malformed html\n";
				// cout << "\tSkipping malformed html\tpostsMetadataCount[" << initializingIndexChunk->dictionary.size( ) << "]\tpostsChunkCount[" << initializingIndexChunk->postsChunkCount << "]\n";
				continue;
				}
			// std::cout << "Adding valid html document\n";
			}
		if ( renameFile( fileName, startPattern, intermediatePattern ) == -1 )
			{
			cerr << "Failed to rename " << fileName << endl;
			throw fileWriteException( );
			}
		totalBytesProcessed += filesize;
		cout << "processed " + fileName << endl;
		cout << "total documents processed = " << totalDocumentsProcessed << endl << "total bytes processed = "
				<< totalBytesProcessed << endl;
		}

	close( fileDescriptor );
	delete initializingIndexChunk;
	return 0;
	}

int main( int argc, char ** argv )
	{
	if ( argc != 3 )
		{
		cerr << "Usage ./build/indexer.exe <batch-name> <chunk-output-folder>";
		return 1;
		}

	string batch = argv[ 1 ];
	string outputFolder = argv[ 2 ];
	if ( outputFolder.back( ) != '/' )
		outputFolder.pushBack( '/' );
	//makeDirectory( outputFolder.cStr( ) );
	vector< string > toProcess;
	// Check to see if there was a crash on the previous run
	// If there was a crash, the files we pass in to use are the previous files that were
	// successfully processed before the driver crashed
	toProcess = matchingFilenames( batch, intermediatePattern );
	if ( toProcess.size( ) == 0 )
		{
		// Otherwise we use new files for this index chunk
		toProcess = matchingFilenames( batch, startPattern );
		std::cout << "toProcess " << toProcess.size( ) << " documents _forIndexer\n";
		}
	else
		{
		std::cout << "toProcess " << toProcess.size( ) << " documents _processing\n";
		}
	/*
	try
		{
		createIndexChunk( toProcess, maxBytesToProcess, outputFolder );
		}
	catch ( ... )
		{
		cout << "Messed up in createIndexChunk\n";
		return 1;
		}
	*/
	createIndexChunk( toProcess, maxBytesToProcess, outputFolder );

	if ( renameAll( batch, intermediatePattern, finishedPattern ) == -1 )
			cerr << "Failed to rename processed html files" << endl;
	std::cout << "Finished processing all HTML\n";
	return 0;
	}
