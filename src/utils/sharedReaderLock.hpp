// sharedReaderLock.hpp
//
// In house freshly made shared readers lock.
// 2019-12-04: Changed signal to cv: combsc
// 2019-11-29: Init Commit: combsc

#ifndef DEX_SHARED_READER_LOCK
#define DEX_SHARED_READER_LOCK

#include <pthread.h>

namespace dex
	{
	class sharedReaderLock
		{
		private:
			pthread_mutex_t numReadersLock;
			int readers;

			pthread_mutex_t readingLock;
			pthread_cond_t cv;


		public:
			sharedReaderLock( )
				{
				numReadersLock = PTHREAD_MUTEX_INITIALIZER;
				readingLock = PTHREAD_MUTEX_INITIALIZER;
				cv = PTHREAD_COND_INITIALIZER;
				readers = 0;
				}

			void readLock( )
				{
				pthread_mutex_lock( &readingLock );
				pthread_mutex_lock( &numReadersLock );
				readers++;
				pthread_mutex_unlock( &numReadersLock );
				pthread_mutex_unlock( &readingLock );
				}

			void releaseReadLock( )
				{
				pthread_mutex_lock( &numReadersLock );
				readers--;
				if( readers == 0 )
					pthread_cond_signal( &cv );
				pthread_mutex_unlock( &numReadersLock );
				}

			void writeLock( )
				{
				pthread_mutex_lock( &readingLock );
				pthread_mutex_lock( &numReadersLock );
				while ( readers != 0 )
					pthread_cond_wait( &cv, &numReadersLock );
				pthread_mutex_unlock( &numReadersLock );
				}

			void releaseWriteLock( )
				{
				pthread_mutex_unlock( &readingLock );
				}
		};
	}
# endif
