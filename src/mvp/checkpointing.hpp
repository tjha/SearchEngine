// checkpointing.hpp
//
// This file contains functions that deal with writing data to persistent storage
// 2019-11-21: Fix html saving function, add includeGuards: combsc
// 2019-11-20: Add saving for html: combsc


#ifndef CHECKPOINTING_HPP
#define CHECKPOINTING_HPP

#include "../utils/file.hpp"
#include "../utils/basicString.hpp"
#include "../spinarak/url.hpp"

namespace dex
	{
	// Folder structure
	// Hash the URL
	// 2 layers of folders
	// bytes 1 determines the first folder
	// bytes 2 determines the second folder
	// bytes 3-4 determine the name of the files
	// This gives us 4,294,967,296 possible locations for html
	int saveHtml( dex::Url url, dex::string html )
		{
		dex::hash < dex::string > hasher;
		unsigned long h = hasher( url.completeUrl( ) );
		unsigned long first = h & 0x000000FF;
		unsigned long second = ( h & 0x0000FF00 ) >> 8;
		unsigned long name = ( h & 0xFFFF0000 ) >> 16 ;
		int err = dex::makeDirectory( "html" );
		if ( err == -1 )
			return err;
		dex::string dirName = "html/" + dex::toString( first );
		err = dex::makeDirectory( dirName.cStr( ) );
		if ( err == -1 )
			return err;
		dirName = "html/" + dex::toString( first ) + "/" + dex::toString( second );
		err = dex::makeDirectory( dirName.cStr( ) );
		if ( err == -1 )
			return err;
		dex::string filename = "html/" + dex::toString( first ) + "/" + dex::toString( second ) + "/" + dex::toString( name ) + ".html";
		err = dex::writeToFile( filename.cStr( ), html.cStr( ), html.size( ) );
		return err;
		}

	
	
	}
	#endif
	