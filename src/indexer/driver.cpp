// Index Driver process html files into index chunks
//
// 2019-12-08: 
//

#include <iostream>
#include <unordered_set>
#include "dirent.h"
#include "index.hpp"
#include "file.hpp"
#include "vector.hpp"
#include "string.hpp"

using dex::string;
using dex::vector;

int main ( int argc, char ** argv )
	{
	
	if ( argc != 3 )
		{
		std::cerr << "Usage ./build/indexer.exe <batch-name> <chunk-output-folder>;
		exit( 1 );
		}

	string batch = argv[ 1 ];
	string outputFolder = argv[ 2 ];

	unordered_set < char * > toProcess;
	Dir * dir;
	dir = opendir( batch );
	dirent * entry = readdir( dir );

	while ( entry != NULL )
		{
		if ( strcmp( entry->d_name, "." ) != 0 && strcmp( entry->d_name, "." ) != 0 && entry->d_type == DT_DIR )
			{
			// look at the directory
			toProcess.insert( entry->d_name );
			}
		}
	closedir( dir );

	dex::utf::decoder < dex::string > stringDecoder;

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
				}
			}
		
		close( dir );
		}
	}

