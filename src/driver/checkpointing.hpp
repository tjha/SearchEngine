// checkpointing.hpp
//
// This file contains functions that deal with writing data to persistent storage
// 2019-12-06: log and performance file now rewriting old files: jhirsh
// 2019-12-04: Change how we save HTML to disk: combsc
// 2019-11-26: Added relative paths for saving: combsc
// 2019-11-21: Fix html saving function, add includeGuards: combsc
// 2019-11-20: Add saving for html: combsc


#ifndef CHECKPOINTING_HPP
#define CHECKPOINTING_HPP

#include <dirent.h>
#include <time.h>
#include "../utils/file.hpp"
#include "../utils/basicString.hpp"
#include "../crawler/robotsMap.hpp"
#include "frontier.hpp"
#include "file.hpp"
#include "utf.hpp"
#include "encode.hpp"
#include "utility.hpp"

namespace dex
	{
	
	dex::pair < size_t, size_t > getInstanceInfo( const char *fileName)
		{
		dex::pair < size_t, size_t > toReturn;
		toReturn.first = 0;
		toReturn.second = 0;
		if ( !dex::fileExists( fileName ) )
			return toReturn;


		string info( readFromFile( fileName ) );
		info = info.stripWhitespace( );
		if ( info.substr( 0, 13 ) != "numInstances=" )
			return toReturn;
		size_t delimLocation = info.find( "\n" );
		if ( delimLocation == dex::string::npos )
			return toReturn;

		if ( info.substr( delimLocation + 1 , 11 ) != "instanceId=" )
			return toReturn;
		toReturn.first = dex::stoi( info.substr( 13, delimLocation - 13 ) );
		toReturn.second = dex::stoi( info.substr( delimLocation + 12, info.size( ) - delimLocation - 12 ) );
		return toReturn;
		}
	
	// Folder structure
	// Hash the URL
	// 2 layers of folders
	// bytes 1 determines the first folder
	// bytes 2 determines the second folder
	// bytes 3-4 determine the name of the files
	// This gives us 4,294,967,296 possible locations for html
	size_t HTMLChunkSize = 100000000; // 16 MB files for htm
	int saveHtml ( const dex::string &url, const dex::string &html, int fileDescriptor)
		{
		// allocate data to store
		unsigned char *toStore = new unsigned char[ url.size( ) + html.size( ) + 14 ];
		unsigned char *toStorePointer = toStore;
		dex::utf::encoder < dex::string > stringEncoder;
		toStorePointer = stringEncoder( url, toStorePointer );
		toStorePointer = stringEncoder( html, toStorePointer );
		int toReturn = write( fileDescriptor, toStore, toStorePointer - toStore );
		delete[ ] toStore;
		if ( toReturn == -1 )
			{
			std::cout << "error writing html" << std::endl;
			}
		return toReturn;
		}

	int getCurrentFileDescriptor( dex::string folderPath )
		{
		int filenumber = 0;
		// see how many files are in the save directory
		DIR * dir = opendir( folderPath.cStr( ) );
		dirent * entry = readdir( dir );
		while ( entry != NULL )
			{
			++filenumber;
			entry = readdir( dir );
			}
		closedir( dir );

		// calculate the correct file names for html save
		dex::string htmlFilename( folderPath + dex::toString( filenumber ) + "_forIndexer" );
		return open( htmlFilename.cStr( ), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU );
		}

	dex::frontier loadFrontier ( const char * fileName, size_t size, robotsMap *robot, bool encoded = false )
		{
		dex::frontier frontier( size, robot );
		if ( !dex::fileExists( fileName ) )
			return frontier;

		if ( !encoded )
			{
			// read in the frontier file
			string frontierList( readFromFile( fileName ) );
			
			string delimiter = "\n";
			size_t found = frontierList.find( delimiter, 0 );
			size_t start = found + delimiter.size( );
			// Parse the frontier_file and add it to list of urls
			while ( start < frontierList.size( ) )
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
			}
		else
			{
			dex::encode::decoder < dex::vector < dex::Url > > UrlDecoder;
			unsigned char *frontierPtr = ( unsigned char * )readFromFile( fileName );
			dex::vector < dex::Url > out = UrlDecoder( frontierPtr );
			for ( auto it = out.cbegin( );  it != out.cend( );  ++it )
				{
				frontier.putUrl( *it );
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

	int saveVisitedLinks ( const char * fileName, dex::vector< dex::string > links )
		{
		dex::encode::encoder < dex::vector < dex::string > > VectorStringEncoder;
		dex::vector< unsigned char > encodedFrontier = VectorStringEncoder( links );
		return writeToFile( fileName, encodedFrontier.data( ), encodedFrontier.size( ) );
		}

	int saveCrawledLinks( const char * fileName, dex::unorderedSet < dex::string > * crawled )
		{
		string crawledData = "CRAWLED LINKS\n";
		for ( auto it = crawled->begin( );  it != crawled->end( );  ++it )
			{
			crawledData += *it + "\n";
			}
		return writeToFile( fileName, crawledData.cStr( ), crawledData.size( ) );
		}

	dex::unorderedSet < dex::string > * loadCrawledLinks( const char * fileName, size_t crawledLinksSize )
		{
		dex::unorderedSet < dex::string > * links = new dex::unorderedSet < dex::string >( crawledLinksSize );
		if ( !dex::fileExists( fileName ) )
			return links;
		// read in the frontier file
		string crawled( readFromFile( fileName ) );
		
		string delimiter = "\n";
		size_t found = crawled.find( delimiter, 0 );
		size_t start = found + delimiter.size( );
		// Parse the frontier_file and add it to list of urls
		while ( start < crawled.size( ) )
			{
			found = crawled.find( delimiter, start );
			if ( found < crawled.npos )
				{
				string url( crawled.begin( ) + start, crawled.begin( ) + found );
				links->insert( url );
				start = found + delimiter.size( );
				}
			else
				{
				break;
				}
			}
		return links;
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
		while ( start < linksList.size( ) )
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

	int createNewLog( dex::string logPath, dex::string &filename )
		{
		DIR * dir = opendir( ( logPath ).cStr( ) );
		dirent * entry = readdir( dir );
		
		// delete old logs
		while ( entry != NULL )
			{
			if ( strcmp( entry->d_name, "." ) && strcmp( entry->d_name, ".." ) )
				remove( entry->d_name );
			entry = readdir( dir );
			}
		closedir( dir );

		// create new log
		time_t now = time( nullptr );
		filename = logPath;
		filename += ctime( &now );
		filename.popBack( );
		filename += ".log";
		filename = filename.replaceWhitespace( "_" );

		// open the new log fileDescriptor
		int fileDescriptor = open( filename.cStr( ), O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU );
		std::cout << "Deleteing old log, creating new at " << filename << std::endl;
		if ( fileDescriptor == -1 )
			{
			std::cerr << "Couldn't open new log file " << filename << std::endl;
			exit( 1 );
			}
		
		return fileDescriptor;
		}

	int createNewPerformanceFile( dex::string performancePath, dex::string &filename )
		{
		DIR * dir = opendir( ( performancePath ).cStr( ) );
		dirent * entry = readdir( dir );
		
		// delete old logs
		while ( entry != NULL )
			{
			if ( strcmp( entry->d_name, "." ) && strcmp( entry->d_name, ".." ) )
				remove( entry->d_name );
			entry = readdir( dir );
			}
		closedir( dir );

		// create new log
		time_t now = time( nullptr );
		filename = performancePath;
		filename += ctime( &now );
		filename.popBack( );
		filename += ".txt";
		filename = filename.replaceWhitespace( "_" );

		// open the new performance fileDescriptor
		int fileDescriptor = open( filename.cStr( ), O_RDWR | O_CREAT, S_IRWXU );
		if ( fileDescriptor == -1 )
			{
			std::cerr << "Couldn't open new log file " << filename << std::endl;
			exit( 2 );
			}
		
		std::cout << "performance FD " << fileDescriptor << std::endl;
		// return file to driver
		std::cout << "Deleteing old performance file, creating new at " << filename << std::endl;
		return fileDescriptor;
		}


	}
	#endif
	
