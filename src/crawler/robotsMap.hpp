// robotsMap.hpp
// Object for organizing robots.txt objects for websites
//
// 2019-12-02: Set maximum size
// 2019-12-01: Added politeToVisit: comb
// 2019-11-30: Continued working: combsc
// 2019-11-29: Init Commit: combsc

#ifndef ROBOTS_MAP_HPP
#define ROBOTS_MAP_HPP

#include "crawler/robots.hpp"
#include "utils/sharedReaderLock.hpp"
#include "utils/unorderedMap.hpp"
#include "utils/utility.hpp"

namespace dex
	{
	class robotsMap
		{
		private:
			// Takes in a domain, returns a robotTxt* and the lock pointer associated with it.
			dex::unorderedMap< dex::string, dex::pair< dex::RobotTxt*, dex::sharedReaderLock* > > mainMap;
			dex::sharedReaderLock mapLock;
			size_t maximumSize;

			bool existsNoLock( dex::string str )
				{
				return mainMap.count( str ) > 0 && !( mainMap[ str ].first && mainMap[ str ].first->hasExpired( ) );
				}

			// mapLock only needs to be writerLocked when we know that our mainMap object is changing in a
			// meaingful way. Since it's a hashmap, we know that the only time our objects will be moved in the
			// map is if they are erased or if the map is grown. Therefore, insertions into our map can be done
			// with only a reader lock as long as we know the map will not grow.
		public:

			size_t size( )
				{
				mapLock.readLock( );
				size_t s = mainMap.size( );
				mapLock.releaseReadLock( );
				return s;
				}

			size_t capacity( )
				{
				mapLock.readLock( );
				size_t s = mainMap.bucketCount( );
				mapLock.releaseReadLock( );
				return s;
				}

			bool robotExists( dex::string str )
				{
				mapLock.readLock( );
				bool toReturn = existsNoLock( str );
				mapLock.releaseReadLock( );
				return toReturn;
				}

			// This function is necessary for making crawlUrl threadsafe but also reasonably
			// quick. To understand, please look at it in tandem with crawlUrl
			// return -2 if the robot is being created by another thread
			// returns -1 if the string does not exist
			// return 0 if successfully visited and called updateLastVisited
			// return 1 if politeness error
			// return 2 if path not allowed
			int startRobotCreation( const dex::string &domain, const dex::string &path )
				{
				mapLock.writeLock( );
				if ( !existsNoLock( domain ) )
					{
					dex::sharedReaderLock *m = new dex::sharedReaderLock( );
					dex::pair< dex::RobotTxt*, dex::sharedReaderLock* > p( nullptr, m );
					m->writeLock( );
					mainMap[ domain ] = p;
					m->releaseWriteLock( );
					mapLock.releaseWriteLock( );
					// Create a lock at this domain's spot so we can visit the robots.txt and
					// insert it back into the map later.
					return -1;
					}
				dex::pair< dex::RobotTxt*, dex::sharedReaderLock* > p = mainMap[ domain ];
				p.second->writeLock( );
				if ( p.first == nullptr )
					{
					p.second->releaseWriteLock( );
					mapLock.releaseWriteLock( );
					return -2;
					}
				int visitPath = p.first->visitPathResult( path );
				if ( visitPath == 0 )
					p.first->updateLastVisited( );
				p.second->releaseWriteLock( );
				mapLock.releaseWriteLock( );
				return visitPath;
				}
			int finishRobotCreation( const dex::string &domain, const dex::RobotTxt &robot )
				{
				mapLock.writeLock( );
				if ( !existsNoLock( domain ) )
					{
					mapLock.releaseWriteLock( );
					return -1;
					}
				mainMap[ domain ].second->writeLock( );
				mainMap[ domain ].first = new dex::RobotTxt( robot );
				mainMap[ domain ].second->releaseWriteLock( );
				mapLock.releaseWriteLock( );
				return 0;
				}

			bool politeToVisit( const dex::string &domain, const dex::string &path )
				{
				mapLock.readLock( );
				if ( !existsNoLock( domain ) )
					{
					mapLock.releaseReadLock( );
					return true;
					}
				mainMap[ domain ].second->readLock( );
				if ( !mainMap[ domain ].first )
					{
					mainMap[ domain ].second->releaseReadLock( );
					mapLock.releaseReadLock( );
					return false;
					}
				bool toReturn = mainMap[ domain ].first->visitPathResult( path ) == 0;
				mainMap[ domain ].second->releaseReadLock( );
				mapLock.releaseReadLock( );
				return toReturn;
				}

			time_t timeToWait( const dex::string &domain )
				{
				mapLock.readLock( );
				if ( !existsNoLock( domain ) )
					{
					mapLock.releaseReadLock( );
					return 0;
					}
				mainMap[ domain ].second->readLock( );
				if ( !mainMap[ domain ].first )
					{
					mainMap[ domain ].second->releaseReadLock( );
                                        mapLock.releaseReadLock( );
                                        return 10;
					}
				int toReturn = mainMap[ domain ].first->timeToWait( );
				mainMap[ domain ].second->releaseReadLock( );
                                mapLock.releaseReadLock( );
                                return toReturn;
				}

			// return 0 on success, -1 on fail
			int writeUnlock( const dex::string &domain )
				{
				mapLock.writeLock( );
				if ( !existsNoLock( domain ) )
					{
					mapLock.releaseWriteLock( );
					return -1;
					}
				mainMap[ domain ].second->releaseWriteLock( );
				mapLock.releaseWriteLock( );
				return 0;
				}

			void insert( const dex::string &str, const dex::RobotTxt &robot )
				{
				dex::RobotTxt *r = new dex::RobotTxt( robot );
				mapLock.writeLock( );
				// If the robot already exists
				if ( mainMap.count( str ) > 0 )
					{
					dex::pair< dex::RobotTxt*, dex::sharedReaderLock* > p = mainMap[ str ];
					p.second->writeLock( );
					if ( p.first )
						delete p.first;
					p.first = r;
					p.second->releaseWriteLock( );
					}
				// If the robot does not exist, need to make a sharedReaderLock and writerLock
				// the map.
				else
					{
					dex::sharedReaderLock *m = new dex::sharedReaderLock( );
					dex::pair< dex::RobotTxt*, dex::sharedReaderLock* > p( r, m );
					mainMap[ str ] = p;
					}
				mapLock.releaseWriteLock( );
				}

			void erase( const dex::string &str )
				{
				if ( robotExists( str ) )
					{
					mapLock.writeLock( );
					if ( existsNoLock( str ) )
						{
						if ( mainMap[ str ].first )
							delete mainMap[ str ].first;
						if ( mainMap[ str ].second )
							delete mainMap[ str ].second;
						mainMap.erase( str );
						}

					mapLock.releaseWriteLock( );
					}
				}

			int purge( )
				{
				if ( mainMap.size( ) > maximumSize )
					{
					size_t i = 0;
					for ( auto it = mainMap.cbegin( );  it != mainMap.cend( );  ++it )
						{
						if ( i % 5 == 0 )
							erase( it->first );
						i++;
						}
					mapLock.writeLock( );
					mainMap.rehash( maximumSize * 4 );
					mapLock.releaseWriteLock( );
					return 1;
					}
				return 0;
				}

			dex::pair< dex::RobotTxt*, dex::sharedReaderLock* > getPair( const dex::string &str )
				{
				mapLock.readLock( );
				if ( existsNoLock( str ) )
					{
					mapLock.releaseReadLock( );
					return mainMap[ str ];
					}
				mapLock.releaseReadLock( );
				return dex::pair< dex::RobotTxt*, dex::sharedReaderLock* >( nullptr, nullptr );
				}

			dex::vector< dex::RobotTxt > getAllRobots( )
				{
				dex::vector< dex::RobotTxt > robots;
				mapLock.readLock( );
				for ( auto it = mainMap.begin( );  it != mainMap.end( );  ++it )
					{
					it->second.second->readLock( );
					robots.pushBack( *it->second.first );
					it->second.second->releaseReadLock( );
					}
				mapLock.releaseReadLock( );
				return robots;
				}

			robotsMap( int max )
				{
				maximumSize = max;
				mainMap.rehash( max * 4 );
				}

			~robotsMap( )
				{
				for ( auto it = mainMap.begin( );  it != mainMap.end( );  ++it )
					{
					if ( it->second.first )
						delete it->second.first;
					if ( it->second.second )
						delete it->second.second;
					}
				}

		};
	}

#endif
