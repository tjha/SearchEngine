// checkpointing.hpp
//
// This file contains functions that deal with writing data to persistent storage
// 2019-11-26: Added relative paths for saving: combsc
// 2019-11-21: Fix html saving function, add includeGuards: combsc
// 2019-11-20: Add saving for html: combsc


#ifndef CHECKPOINTING_HPP
#define CHECKPOINTING_HPP

#include <dirent.h>
#include "../utils/file.hpp"
#include "../utils/basicString.hpp"
#include "../spinarak/url.hpp"
#include "frontier.hpp"
#include "file.hpp"
#include "encode.hpp"

namespace dex
	{
	// Folder structure
	// Hash the URL
	// 2 layers of folders
	// bytes 1 determines the first folder
	// bytes 2 determines the second folder
	// bytes 3-4 determine the name of the files
	// This gives us 4,294,967,296 possible locations for html
	size_t HTMLChunkSize = 100000000; // 16 MB files for htm
	int filenumber = 0;
	int urlsFileDescriptor = -1;
	int htmlFileDescriptor = -1;
	int currentDocumentCount = 0;
	int saveHtml ( dex::Url url, dex::string html, dex::string folderPath )
		{
		int err = dex::makeDirectory( folderPath.cStr( ) );
		if ( err == -1 )
			return -1;

		++currentDocumentCount;

		// when chunk is filled, close and move onto next file
		if ( dex::fileSize( htmlFileDescriptor ) > HTMLChunkSize )
			{
			close( htmlFileDescriptor ); // close filled chunk
			close( urlsFileDescriptor );
			++filenumber;
			dex::string urlsFilename( folderPath + "html/" + dex::toString( filenumber ) + ".urls" );
			dex::string htmlFilename( folderPath + "html/" + dex::toString( filenumber ) + "_html" );
			std::cout << "crawled " << currentDocumentCount << " sites." << std::endl;
			std::cout << "switching to file " << htmlFilename << std::endl;

			urlsFileDescriptor = open( urlsFilename.cStr( ), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU );
			htmlFileDescriptor = open( htmlFilename.cStr( ), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU );

			currentDocumentCount = 0;
			}

		err = write( urlsFileDescriptor, ( url.completeUrl( ) + "\n" ).cStr( ), url.completeUrl( ).size( ) );
		if ( err == -1 )
			{
			std::cerr << "Writing saved Urls failed.\n";
			return -1;
			}

		return write( htmlFileDescriptor, html.cStr( ), html.size( ) );
		}

	int getCurrentFileDescriptor( dex::string folderPath )
		{
		int err = dex::makeDirectory( folderPath.cStr( ) );
		if ( err == -1 )
			{
			std::cerr << "could not make directory" << std::endl;
			return -1;
			}

		// see how many files are in the save directory
		DIR * dir = opendir( folderPath.cStr( ) );
		dirent * entry = readdir( dir );
		while ( entry != NULL )
			{
			++filenumber;
			entry = readdir( dir );
			}
		closedir( dir );

		// calculate the correct file names for url and html saves
		dex::string urlsFilename( folderPath + dex::toString( filenumber ) + ".urls" );
		dex::string htmlFilename( folderPath + dex::toString( filenumber ) + "_html" );
		// open up those files and set them to vars
		urlsFileDescriptor = open( urlsFilename.cStr( ), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU );
		htmlFileDescriptor = open( htmlFilename.cStr( ), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU );

		// check errors
		return ( urlsFileDescriptor == -1 || htmlFileDescriptor == -1 ) ? -1 : 0;
		}
	
	void closeHtmlFile( )
		{
		close( urlsFileDescriptor );
		close( htmlFileDescriptor );
		}

	dex::frontier loadFrontier ( const char * fileName, size_t size )
		{
		dex::frontier frontier( size );
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
		dex::encode::encoder < dex::vector < dex::Url > > UrlEncoder;
		dex::vector< unsigned char > encodedFrontier = UrlEncoder( frontier.getFrontier( ) );
		return writeToFile( fileName, encodedFrontier.data( ), encodedFrontier.size( ) );
		}
	
	/*dex::frontier loadFrontier( const char * filename )
		{
		dex::frontier;
		}*/

	int saveVisitedLinks ( const char * fileName, dex::vector< dex::string > links )
		{
		dex::encode::encoder < dex::vector < dex::string > > VectorStringEncoder;
		dex::vector< unsigned char > encodedFrontier = VectorStringEncoder( links );
		return writeToFile( fileName, encodedFrontier.data( ), encodedFrontier.size( ) );
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

	int saveLinksToShip( const char * folderPath, dex::vector < dex::Url > *linksToShip, int len )
		{
		
		for ( int i = 0;  i < len;  ++i )
			{
			string linksToShipData = "LINKS FOR " + dex::toString( i ) + "\n";
			for ( auto it = linksToShip[ i ].begin( );  it != linksToShip[ i ].end( );  ++it )
				{
				linksToShipData += it->completeUrl( ) + "\n";
				}
			writeToFile( ( dex::string( folderPath ) + "linksFor" + dex::toString( i ) ).cStr( ), linksToShipData.cStr( ), linksToShipData.size( ) );
			}
		
		return 0;
		}

	
	
	}
	#endif
	