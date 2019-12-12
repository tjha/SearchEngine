// Index Driver process html files into index chunks
//
// 2019-12-11:	edited to call correct functions to index and use lexicoComp: jhirsh, loghead
// 2019-12-10:	edits to attach to index.hpp functions: combsc, loghead
// 2019-12-08:	created index driver to read through directory: jhirsh
//

#include <iostream>
#include <unordered_set>
#include "dirent.h"
#include "index.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "vector.hpp"
#include "basicString.hpp"
#include "parser.hpp"
#include "url.hpp"

using dex::string;
using dex::vector;

int openFile( int indexChunkCount )
	{
	const char * filePath = ( dex::toString( indexChunkCount ) + "_in.dex" ).cStr( );
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

	dex::unorderedSet < char * > toProcess;
	DIR * dir;
	dir = opendir( batch.cStr( ) );
	dirent * entry = readdir( dir );

	string currentDirectory( "." );
	string parentDirectory( ".." );

	while ( entry != NULL )
		{
		string direntryName( entry->d_name );
		if ( dex::lexicographicalCompare( direntryName.cbegin( ), direntryName.cend( ),
				currentDirectory.cbegin( ), currentDirectory.cend( ) )
				&& dex::lexicographicalCompare( direntryName.cbegin( ), direntryName.cend( ),
				parentDirectory.cbegin( ), parentDirectory.cend( ) ) 
				&& entry->d_type == DT_DIR )
			{
			// look at the directory
			toProcess.insert( entry->d_name );
			}
		}
	closedir( dir );

	dex::utf::decoder < dex::string > stringDecoder;

	int indexChunkCount = 0;
	// TODO: What scheme will we use to name the files for the index chunks?
	int fileDescriptor = openFile( indexChunkCount++ );
	dex::index::indexChunk *initializingIndexChunk = new dex::index::indexChunk( fileDescriptor );

	for ( auto &directory: toProcess )
		{
		dir = opendir( directory );
		entry = readdir( dir );
		while ( entry != NULL )
			{
			string direntryName( entry->d_name );
			if ( dex::lexicographicalCompare( direntryName.cbegin( ), direntryName.cend( ),
					currentDirectory.cbegin( ), currentDirectory.cend( ) )
					&& dex::lexicographicalCompare( direntryName.cbegin( ), direntryName.cend( ),
					parentDirectory.cbegin( ), parentDirectory.cend( ) ) 
					&& entry->d_type == DT_DIR )
				{
				
				// Decode the current file
				unsigned char *savedHtml = reinterpret_cast< unsigned char * >( dex::readFromFile( entry->d_name, 0 ) );
				unsigned char *ptr = savedHtml;

				// retrieve the saved url + html pair
				dex::Url url = dex::Url( stringDecoder( ptr, &ptr ).cStr( ) );
				dex::string html = stringDecoder( ptr, &ptr );

				dex::HTMLparser parser( url, html, true );

				dex::string titleString;
				titleString.reserve( 25 );
				for ( auto &titleWord: parser.ReturnTitle( ) )
					{
					titleString += titleWord;
					}
				
				// TODO this should go in parser but didn't want to break dependent functionality
				dex::vector < dex::AncWord > anchors = parser.ReturnAnchorText( );
				dex::vector < dex::string > anchorText( anchors.size( ) );
				for ( auto &anchor: anchors )
					{

					}
				// TODO add default argument for anchorText in index.hpp
				if ( !initializingIndexChunk->addDocument( url.completeUrl( ), { }, parser.ReturnTitle( ), titleString, 
						parser.ReturnWords( ) ) )
					{
					close( fileDescriptor );
					fileDescriptor = openFile( indexChunkCount++ );
					initializingIndexChunk = indexChunk( fileDescriptor );
					}
				if ( !initializingIndexChunk->addDocument( url.completeUrl( ), { }, parser.ReturnTitle( ), titleString,
						parser.ReturnWords( ) ) )
					{
					// TODO: Throw an exception. Should not fail to add a document to a new index chunk
					throw dex::fileWriteException( );
					}
				}
			}
		closedir( dir );
		}
	close( fileDescriptor );
	}


