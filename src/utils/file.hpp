// file.hpp
// file for dealing with our fileIO
//
// 2019-11-2: Initial Commit: combsc
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "exception.hpp"

namespace dex
	{
	// used internally
	size_t fileSize( int fd )
		{
		struct stat fileInfo;
		fstat( fd, &fileInfo );
		return fileInfo.st_size;
		}

	// Writes to the file name specified. Will overwrite whatever was there before.
	int writeToFile( const char *filePath, const char *toWrite, size_t length )
		{

		int fd = open ( filePath, O_RDWR | O_CREAT| O_TRUNC, S_IRWXU );
		if ( fd == -1 )
			return -1;
		int result = lseek( fd, length - 1, SEEK_SET );
		if ( result == -1 )
			return -1;
		
		result = write( fd, " ", 1 );

		char *map = ( char * ) mmap( nullptr, length, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 );
		memcpy( map, toWrite, length );

		close( fd );
		return 0;
		}

	// Returns a char* pointing to the beginning of memory of the file specified.
	char *readFromFile( const char *filePath )
		{
		int fd = open( filePath, O_RDONLY );
		if ( fd == -1 )
			throw outOfRangeException( );

		size_t filesize = fileSize( fd );
		char *map = ( char * ) mmap( nullptr, filesize, PROT_READ, MAP_SHARED, fd, 0 );
		close( fd );
		return map;
		}

	// Returns the size of a file
	size_t getFileSize( const char *filePath )
		{
		int fd = open( filePath, O_RDONLY );
		return fileSize( fd );
		}
	}

