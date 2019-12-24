// unorderedMap.hpp
// Organically grown unordered map implementation that imitates the STL unordered_map. This uses linear probing.
//
// We ignore the key_equal predicate and allocator
//
// 2019-11-30: Added insertionWillRehash function: combsc
// 2019-11-04: Use typeTraits.hpp: jasina
// 2019-11-02: Change default table size, allocate memory for pair only when needed: jasina
// 2019-10-28: Implement insert: jasina
// 2019-10-27: Address PR style comments: jasina
// 2019-10-26: Wrote count, rehash, constructors, operator[ ], operator=, at, empty, size, maxSize, bucketCount, clear,
//             swap, erase, iterators, find, include guard: jasina, lougheem
// 2019-10-20: File created: jasina, lougheem

#ifndef DEX_UNORDERED_MAP
#define DEX_UNORDERED_MAP

#include <cstddef>
#include "utils/algorithm.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/typeTraits.hpp"
#include "utils/utility.hpp"

namespace dex
	{
	template < class Key >
	struct hash;

	template < class Key, class Value, class Hash = dex::hash < Key > >
	class unorderedMap
		{
		private:
			struct wrappedPair;

			wrappedPair *table;
			size_t tableSize;
			size_t numberElements;
			size_t ghostCount;

			static const size_t DEFAULT_TABLE_SIZE = 1 << 4;

			Hash hasher;

			struct wrappedPair
				{
				dex::pair < Key, Value > *pairPtr;
				bool isEmpty;
				bool isGhost;

				wrappedPair( ) : pairPtr( nullptr ), isEmpty( true ), isGhost( false ) { }
				wrappedPair( Key key, Value value ) : isEmpty( false ), isGhost( false )
					{
					pairPtr = new dex::pair < Key, Value >( key, value );
					}
				};

			size_t probe( const Key &key ) const
				{
				bool ghostFound = false;
				size_t firstGhostLocation;
				size_t location = hasher( key ) % tableSize;

				// This loop will run until either we find an empty bucket or until we find the key we want
				while ( true )
					{
					// We're done if we enter this block
					if ( table[ location ].isEmpty )
						{
						if ( ghostFound )
							return firstGhostLocation;
						else
							return location;
						}

					if ( table[ location ].isGhost )
						{
						// Keep track of the first ghost we encounter. We will overwrite this bucket if we never find the key
						if ( !ghostFound )
							{
							firstGhostLocation = location;
							ghostFound = true;
							}
						}
					else
						// We're done if we find the key we want
						if ( table[ location ].pairPtr->first == key )
							return location;

					location = ( location + 1 ) % tableSize;
					}
				}
		public:
			unorderedMap( size_t tableSize = DEFAULT_TABLE_SIZE, const Hash &hasher = Hash( ) )
				{
				this->tableSize = dex::max( size_t( 1 ), tableSize );
				numberElements = 0;
				ghostCount = 0;
				this->hasher = hasher;

				table = new wrappedPair[ this->tableSize ]( );
				}
			template< class InputIt >
			unorderedMap( InputIt first, InputIt last,
					size_t tableSize = DEFAULT_TABLE_SIZE, const Hash &hasher = Hash( ) )
				{
				this->tableSize = dex::max( size_t( 1 ), tableSize );
				numberElements = 0;
				ghostCount = 0;
				this->hasher = hasher;

				table = new wrappedPair[ this->tableSize ]( );
				insert( first, last );
				}

			unorderedMap( const unorderedMap < Key, Value, Hash > &other )
				{
				table = nullptr;
				unorderedMap < Key, Value, Hash > temp( other.cbegin( ), other.cend( ),
						other.bucketCount( ), other.hasher );
				swap( temp );
				}

			unorderedMap &operator=( const unorderedMap < Key, Value, Hash > &other )
				{
				unorderedMap < Key, Value, Hash > temp( other );
				swap( temp );
				return *this;
				}

			~unorderedMap( )
				{
				if ( table )
					{
					for ( size_t index = 0;  index != tableSize;  ++index )
						if ( !( table[ index ].isEmpty || table[ index ].isGhost ) )
							delete table[ index ].pairPtr;
					delete [ ] table;
					}
				}

		private:
			template < bool isConst >
			class _iterator
				{
				private:
					friend class unorderedMap < Key, Value, Hash >;

					typedef typename dex::conditional < isConst, const unorderedMap < Key, Value, Hash >,
							unorderedMap < Key, Value, Hash > >::type mapType;
					typedef typename dex::conditional < isConst, const dex::pair < Key, Value >,
							dex::pair < Key, Value > >::type pairType;

					mapType *map;
					size_t position;
				private:
					_iterator( mapType &map, size_t position ) : map( &map )
						{
						// Find the first filled bucket at or after position
						for ( ;  position != map.bucketCount( ) &&
								( map.table[ position ].isEmpty || map.table[ position ].isGhost );
								++( position ) );
						this->position = position;
						}
				public:
					template < typename = typename dex::enableIf < isConst > >
					_iterator( const _iterator < false > &other ) :
							map( other.map ), position( other.position ) { }

					friend bool operator==( const _iterator &a, const _iterator &b )
						{
						// Only makes sense to compare iterators pointing to the same map
						if ( a.map != b.map )
							throw invalidArgumentException( );
						return a.position == b.position;
						}
					friend bool operator!=( const _iterator &a, const _iterator &b )
						{
						if ( a.map != b.map )
							throw invalidArgumentException( );
						return a.position != b.position;
						}

					pairType &operator*( ) const
						{
						return *( map->table[ position ].pairPtr );
						}
					pairType *operator->( ) const
						{
						return map->table[ position ].pairPtr;
						}

					_iterator &operator++( )
						{
						if ( position >= map->bucketCount( ) )
							throw outOfRangeException( );

						// Find the next filled bucket
						for ( ++position ;  position != map->bucketCount( ) &&
								( map->table[ position ].isEmpty || map->table[ position ].isGhost );  ++position );
						return *this;
						}
					_iterator operator++( int )
						{
						_iterator toReturn( *this );
						++*this;
						return toReturn;
						}
				};

			template < bool isConst >
			void swap( _iterator < isConst > &a, _iterator < isConst > &b )
				{
				dex::swap( a.map, b.map );
				dex::swap( a.position, b.position );
				}
		public:
			typedef _iterator < false > iterator;
			iterator begin( )
				{
				return iterator( *this, 0 );
				}
			iterator end( )
				{
				return iterator( *this, bucketCount( ) );
				}

			typedef _iterator < true > constIterator;
			constIterator cbegin( ) const
				{
				return constIterator( *this, 0 );
				}
			constIterator cend( ) const
				{
				return constIterator( *this, bucketCount( ) );
				}

			bool empty( ) const
				{
				return size( ) == 0;
				}

			size_t size( ) const
				{
				return numberElements;
				}

			size_t maxSize( ) const
				{
				return size_t( -1 );
				}

			size_t bucketCount( ) const
				{
				return tableSize;
				}

			Value &operator[ ]( const Key &key )
				{
				return insert( dex::pair < Key, Value >{ key, Value{ } } ).first->second;
				}

			const Value &at( const Key &key ) const
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty || table[ location ].isGhost )
					throw outOfRangeException( );
				return table[ location ].pairPtr->second;
				}
			Value &at( const Key &key )
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty || table[ location ].isGhost )
					throw outOfRangeException( );
				return table[ location ].pairPtr->second;
				}

			iterator find( const Key &key )
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty || table[ location ].isGhost )
					return end( );
				return iterator( *this, location );
				}

			constIterator find( const Key &key ) const
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty || table[ location ].isGhost )
					return cend( );
				return constIterator( *this, location );
				}

			iterator erase( constIterator position )
				{
				table[ position.position ].isGhost = true;
				delete table[ position.position ].pairPtr;
				++ghostCount;
				--numberElements;
				return iterator( *this, position.position );
				}

			iterator erase( constIterator first, constIterator last )
				{
				while ( first != last )
					first = erase( first );
				return iterator( *this, last.position );
				}

			size_t erase( const Key &key )
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty || table[ location ].isGhost )
					return 0;
				table[ location ].isGhost = true;
				delete table[ location ].pairPtr;
				++ghostCount;
				--numberElements;
				return 1;
				}

			void clear( )
				{
				erase( cbegin( ), cend( ) );
				}

			dex::pair < iterator, bool > insert( const dex::pair < Key, Value > &datum )
				{
				bool inserted = false;
				const Key &key = datum.first;
				size_t location = probe( key );
				wrappedPair *bucket = &table[ location ];

				if ( bucket->isEmpty || bucket->isGhost )
					{
					inserted = true;
					bucket->pairPtr = new dex::pair < Key, Value >( datum );
					bucket->isEmpty = false;
					ghostCount -= bucket->isGhost;
					bucket->isGhost = false;
					++numberElements;

					if ( 2 * ( size( ) + ghostCount ) > bucketCount( ) )
						{
						rehash( 2 * bucketCount( ) );
						location = probe( key );
						}
					}

				return dex::pair < iterator, bool >{ iterator( *this, location ), inserted };
				}

			bool insertionWillRehash( const Key key )
				{
				if ( 2 * ( size( ) + ghostCount + 1 ) <= bucketCount( ) )
					return false;
				size_t location = probe( key );
				wrappedPair *bucket = &table[ location ];
				return bucket->isEmpty;
				}

			template < class InputIt >
			void insert( InputIt first, InputIt last )
				{
				for ( ;  first != last;  insert( *( first++ ) ) );
				}

			size_t count( const Key &key )
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty || table[ location ].isGhost )
					return 0;
				return 1;
				}

			void rehash( size_t newSize )
				{
				// TODO: Maybe reuse precalculated hashes
				swap( unorderedMap < Key, Value, Hash > ( cbegin( ), cend( ),
						dex::max( newSize, size( ) * 2 ) ) );
				}

			void swap( unorderedMap &other )
				{
				dex::swap( other.table, table );
				dex::swap( other.tableSize, tableSize );
				dex::swap( other.numberElements, numberElements );
				dex::swap( other.ghostCount, ghostCount );
				}

			void swap( unorderedMap &&other )
				{
				dex::swap( other.table, table );
				dex::swap( other.tableSize, tableSize );
				dex::swap( other.numberElements, numberElements );
				dex::swap( other.ghostCount, ghostCount );
				}

			dex::string compress( )
				{
				dex::string compressed = "";
				Key defaultKey;
				for ( auto it = begin( );  it != cend( );  ++it )
					{
					compressed += it->first + "\n" + it->second.compress( ); // TODO compress passes in a string by ref to append to
					}
					return compressed;
				}
		};

		template < class Key, class Value, class Hash >
		void swap( unorderedMap < Key, Value, Hash > &a, unorderedMap < Key, Value, Hash > &b )
			{
			a.swap( b );
			}
	}

#endif
