// checkpointing.hpp
//
// This file contains functions that deal with writing data to persistent storage
// 2019-11-26: Added relative paths for saving: combsc
// 2019-11-21: Fix html saving function, add includeGuards: combsc
// 2019-11-20: Add saving for html: combsc


#ifndef CHECKPOINTING_HPP
#define CHECKPOINTING_HPP

#include "../utils/file.hpp"
#include "../utils/basicString.hpp"
#include "../spinarak/url.hpp"
#include "frontier.hpp"

namespace dex
	{
	// Folder structure
	// Hash the URL
	// 2 layers of folders
	// bytes 1 determines the first folder
	// bytes 2 determines the second folder
	// bytes 3-4 determine the name of the files
	// This gives us 4,294,967,296 possible locations for html
	int saveHtml ( dex::Url url, dex::string html, dex::string folderPath )
		{
		dex::hash < dex::string > hasher;
		unsigned long h = hasher( url.completeUrl( ) );
		unsigned long first = h & 0x000000FF;
		unsigned long second = ( h & 0x0000FF00 ) >> 8;
		unsigned long name = ( h & 0xFFFF0000 ) >> 16 ;
		int err = dex::makeDirectory( ( folderPath + "html" ).cStr( ) );
		if ( err == -1 )
			return err;
		dex::string dirName = folderPath + "html/" + dex::toString( first );
		err = dex::makeDirectory( dirName.cStr( ) );
		if ( err == -1 )
			return err;
		dirName = folderPath + "html/" + dex::toString( first ) + "/" + dex::toString( second );
		err = dex::makeDirectory( dirName.cStr( ) );
		if ( err == -1 )
			return err;
		dex::string filename = folderPath + "html/" + dex::toString( first ) + "/" + dex::toString( second ) + "/" + dex::toString( name ) + ".html";
		err = dex::writeToFile( filename.cStr( ), html.cStr( ), html.size( ) );
		return err;
		}

	dex::frontier loadFrontier ( const char * fileName )
	{
		dex::frontier frontier;
		if ( !dex::fileExists( fileName ) )
			return frontier;
		// read in the frontier file
		string frontierList( readFromFile( fileName ) );
		
		string delimiter = "\n";
		size_t found = frontierList.find( delimiter, 0 );
		size_t start = found + delimiter.size( );
		// Parse the frontier_file and add it to list of urls
		while ( start >= 0 && start < frontierList.size( ) )
			{
			found = frontierList.find( delimiter, start );
			if ( found < frontierList.npos )
				{
				string url( frontierList.begin( ) + start, frontierList.begin( ) + found );
				frontier.putUrl( url.cStr( ) );
				start = found + delimiter.size( );
				}
			else
				{
				break;
				}
			}

		return frontier;
	}

	int saveFrontier ( const char * fileName, dex::frontier frontier )
		{
		string frontierData = "FRONTIER\n";
		for ( auto it = frontier.begin( );  it != frontier.end( );  ++it )
			{
			frontierData += it->completeUrl( ) + "\n";
			}
		return writeToFile( fileName, frontierData.cStr( ), frontierData.size( ) );
		}

	dex::unorderedSet < dex::Url > loadBrokenLinks ( const char * fileName )
		{
		dex::unorderedSet < dex::Url > brokenLinks;
		if ( !dex::fileExists( fileName ) )
			return brokenLinks;
		string linksList( readFromFile( fileName ) );
		
		string delimiter = "\n";
		size_t found = linksList.find( delimiter, 0 );
		size_t start = found + delimiter.size( );
		// Parse the frontier_file and add it to list of urls
		while ( start >= 0 && start < linksList.size( ) )
			{
			found = linksList.find( delimiter, start );
			if ( found < linksList.npos )
				{
				string url( linksList.begin( ) + start, linksList.begin( ) + found );
				brokenLinks.insert( url.cStr( ) );
				start = found + delimiter.size( );
				}
			else
				{
				break;
				}
			}

		return brokenLinks;
		}

	int saveBrokenLinks ( const char * fileName, dex::unorderedSet < dex::Url > brokenLinks)
		{
		string brokenLinksData = "BROKEN LINKS\n";
		for ( auto it = brokenLinks.begin( );  it != brokenLinks.end( );  ++it )
			{
			brokenLinksData += it->completeUrl( ) + "\n";
			}
		return writeToFile( fileName, brokenLinksData.cStr( ), brokenLinksData.size( ) );
		}

	
	
	}
	#endif
	