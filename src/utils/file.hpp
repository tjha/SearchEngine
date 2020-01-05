// file.hpp
// file for dealing with our fileIO
//
// 2019-12-11: Added functionality to get file paths matching end-pattern: tjha
// 2019-11-26: Silence warning: combsc
// 2019-11-23 Added fileExists: combsc
// 2019-11-21: Added includeGuards: combsc
// 2019-11-20: Added appendToFile, add offset, add create, add makeDirectory: combsc
// 2019-11-4: Swapped out memcpy for dex::copy: combsc
// 2019-11-2: Initial Commit: combsc

#ifndef DEX_FILE_HPP
#define DEX_FILE_HPP

#include <dirent.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "utils/algorithm.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/unorderedSet.hpp"
#include "utils/vector.hpp"


namespace dex
	{
	// used internally
	size_t fileSize( int fd )
		{
		struct stat fileInfo;
		fstat( fd, &fileInfo );
		return fileInfo.st_size;
		}

	bool fileExists( const char *filePath )
		{
			struct stat fileInfo;
			return (stat (filePath, &fileInfo) == 0);
		}

	long getPageSize( )
		{
		return sysconf( _SC_PAGE_SIZE );
		}

	// Returns the size of a file
	size_t getFileSize( const char *filePath )
		{
		int fd = open( filePath, O_RDONLY );
		return fileSize( fd );
		}

	int makeDirectory( const char *directoryName )
		{
		struct stat st { };
		if ( stat( directoryName, &st ) == -1 )
			return mkdir( directoryName, 0700 );
		return 0;
		}

	// Writes to the file name specified. Will overwrite whatever was there before.
	int writeToFile( const char *filePath, const char *toWrite, size_t length )
		{
		int fd = open( filePath, O_RDWR | O_CREAT| O_TRUNC, S_IRWXU );
		if ( fd == -1 )
			return -1;
		int result = lseek( fd, length - 1, SEEK_SET );
		if ( result == -1 )
			{
			close( fd );
			return -1;
			}


		result = write( fd, " ", 1 );

		char *map = ( char * ) mmap( nullptr, length, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 );
		copy( toWrite, toWrite + length, map );

		close( fd );
		return 0;
		}

	// Appends to the file name specified. Will not overwrite whatever was there before.
	int appendToFile( const char *filePath, const char *toWrite, size_t length )
		{
		int fd = open( filePath, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU );
		if ( fd == - 1 )
			return -1;
		int result = write( fd, toWrite, length );
		close( fd );
		return result;
		}

	// Returns a char* pointing to the beginning of memory of the file specified.
	// offset MUST be a multiple of pagesize
	char *readFromFile( const char *filePath, size_t offset = 0 )
		{
		if ( offset % getPageSize( ) != 0 )
			throw dex::invalidArgumentException( );

		int fd = open( filePath, O_RDONLY );
		if ( fd == -1 )
			throw outOfRangeException( );

		size_t filesize = fileSize( fd );
		char *map = ( char * ) mmap( nullptr, filesize, PROT_READ, MAP_SHARED, fd, offset );
		close( fd );
		return map;
		}

	char *readFromFileForIndex( const char *filePath, size_t &filesize, size_t offset = 0 )
		{
		if ( offset % getPageSize( ) != 0 )
			throw dex::invalidArgumentException( );

		int fd = open( filePath, O_RDONLY );
		if ( fd == -1 )
			throw outOfRangeException( );

		filesize = fileSize( fd );

		std::cerr << filesize << std::endl;

		char *map = ( char * ) mmap( nullptr, filesize, PROT_READ, MAP_SHARED, fd, offset );
		close( fd );
		return map;
		}

	// Provides vector of all filenames within path that match end-pattern
	vector< string > matchingFilenames( string dirPath, const string &pattern )
		{
		// using vector as stack for DFS (pushBack and popBack)
		vector< string > Frontier;
		unorderedSet< string > SeenSet;
		Frontier.pushBack( dirPath );

		vector< string > results;

		// Keep looping until Frontier is empty
		while ( !Frontier.empty( ) )
			{
			string directory = Frontier.back( );
			Frontier.popBack( );
			SeenSet.insert( directory );

			DIR *dir = opendir( directory.cStr( ) );
			if ( !dir )
				return results;

			struct dirent* entry;

			// Loop through all files and directories in current directory
			while ( dir != nullptr && ( entry = readdir( dir ) ) != NULL )
				{
				// Ignore files that begin with a '.'
				if ( entry->d_name[ 0 ] == '.' )
					continue;

				string filename( entry->d_name );
				if ( directory.back( ) == '/' )
					filename = directory + filename;
				else
					filename = directory + "/" + filename;

				// Add subdirectory to Frontier
				if ( entry->d_type == DT_DIR )
					{
					Frontier.pushBack( filename );
					}
				// Check file
				else
					{
					// Add file to SeenSet
					SeenSet.insert( filename );

					// Add to vector to return if filename matches end-pattern
					size_t pos = filename.find( pattern );
					if ( pos != string::npos && pos + pattern.length( ) == filename.length( ) )
						results.pushBack( filename );
					}
				}
			closedir( dir );
			}

		return results;

		}
	}

#endif
