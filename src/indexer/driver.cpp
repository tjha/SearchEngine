// Index Driver process html files into index chunks
//
// 2019-12-08: 
//

#include <iostream>
#include <unordered_set>
#include "dirent.h"
#include "index.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "vector.hpp"
#include "basicString.hpp"

using dex::string;
using dex::vector;

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

	string directory( "." );
	string parentDirectory( ".." );

	while ( entry != NULL )
		{
		string direntryName( entry->d_name );
		if ( dex::lexicographicalCompare( direntryName.cbegin( ), direntryName.cend( ),
				directory.cbegin( ), directory.cend( ) )
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
	dex::indexChunk *initializingIndexChunk = indexChunk( fileDescriptor );

	for ( auto &directory: toProcess )
		{
		dir = openDir( directory );
		entry = readdir( dir );
		while ( entry != NULL )
			{
			if ( strcmp( entry->d_name, "." != 0 && strcmp( entry->d_name, ".." != 0 && entry->d_type == DT_REG )
				{
				unsigned char *html = dex::readFromFile( entry->d_name, 0 );
				unsigned char *ptr = html;

				dex::string url = stringDecode( ptr, ptr );
				dex::string html = stringDecode( ptr, ptr );

				//...
				dex::HTMLparser parser( url, html, true );

				if ( !initializingIndexChunk->addDocument( url, parser.ReturnAnchorText( ), parser.ReturnTitle( ),
						parser.ReturnWords( ) ) )
					{
					close( fileDescriptor );
					fileDescriptor = openFile( indexChunkCount++ );
					initializingIndexChunk = indexChunk( fileDescriptor );
					}
				if ( !initializingIndexChunk->addDocument( url, parser.ReturnAnchorText( ), parser.ReturnTitle( ),
						parser.ReturnWords( ) ) )
					{
					// TODO: Throw an exception. Should not fail to add a document to a new index chunk
					throw dex::fileWriteException;
					}
				}
			}
		close( dir );
		}
	close( fileDescriptor );
	}

int openFile( int indexChunkCount )
	{
	const char filePath[ ] = dex::string( dex::toString( indexChunkCount ) + "_in.dex" ).c_str( );
	return open( filePath, O_RDWR | O_CREAT, 0777 );
	}

