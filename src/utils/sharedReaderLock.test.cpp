// sharedReaderLockTests.cpp
// Tests for the sharedReaderLock file

// 2019-11-29: Init Commit

#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "catch.hpp"
#include "utils/sharedReaderLock.hpp"

const size_t length = 1000000;

struct readStruct
	{
	dex::sharedReaderLock *m;
	char *a;
	};

struct writeStruct
	{
	dex::sharedReaderLock *m;
	char toWrite;
	char *a;
	};

void *read( void * arg )
	{
	struct readStruct s = *static_cast< readStruct* >(arg);
	s.m->readLock( );
	char toCheck = s.a[ 0 ];
	for ( size_t i = 0;  i < length / 2 ;  ++i )
		{
		REQUIRE( s.a[ i ] == toCheck );
		}

	usleep( 1000 );

	for ( size_t i = length / 2;  i < length ;  ++i )
		{
		REQUIRE( s.a[ i ] == toCheck );
		}
	s.m->releaseReadLock( );
	return nullptr;
	}

void *write( void * arg )
	{
	struct writeStruct s = *static_cast< writeStruct* >(arg);
	s.m->writeLock( );
	for ( size_t i = 0;  i < length / 2;  ++i )
		{
		s.a[ i ] = s.toWrite;
		}

	usleep( 1000 );

	for ( size_t i = length / 2 ;  i < length;  ++i )
		{
		s.a[ i ] = s.toWrite;
		}
	s.m->releaseWriteLock( );

	return nullptr;
	}

TEST_CASE( "Shared", "[shared]" )
	{

	char arr [ length ];
	pthread_t readers [ 6 ];
	pthread_t writers [ 2 ];
	dex::sharedReaderLock m;
	struct readStruct st;

	for ( size_t i = 0;  i < length;  ++i )
		{
		arr[ i ] = 'a';
		}
	st.a = arr;
	st.m = &m;

	struct writeStruct wst1;
	struct writeStruct wst2;
	wst1.a = arr;
	wst1.m = &m;
	wst1.toWrite = 'z';

	wst2.a = arr;
	wst2.m = &m;
	wst2.toWrite = 'b';

	for ( int i = 0;  i < 3;  ++i )
		{
		pthread_create( &readers[ i ], nullptr, read, static_cast< void * > ( &st ) );
		}
	pthread_create( &writers[ 0 ], nullptr, write, static_cast< void * > ( &wst1 ) );

	for ( int i = 3;  i < 6;  ++i )
		{
		pthread_create( &readers[ i ], nullptr, read, static_cast< void * > ( &st ) );
		}
	pthread_create( &writers[ 1 ], nullptr, write, static_cast< void * > ( &wst2 ) );

	for ( int i = 0;  i < 6;  ++i )
		{
		pthread_join( readers[ i ], nullptr );
		}
	pthread_join( writers[ 0 ], nullptr );
	pthread_join( writers[ 1 ], nullptr );
	}