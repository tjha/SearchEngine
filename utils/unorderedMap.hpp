// unorderedMap.hpp
// Organically grown unordered map implementation that imitates the STL unordered_map. This uses linear probing.
//
// We ignore the key_equal predicate and allocator
//
// 2019-10-26: Wrote count, rehash, constructors, operator[ ], operator=, at, empty, size, maxSize, bucketCount, clear,
//             swap, erase, iterators, find, include guard: jasina, lougheem
// 2019-10-20: File created: jasina, lougheem

#ifndef DEX_UNORDERD_MAP
#define DEX_UNORDERD_MAP

#include <cstddef>
#include <type_traits>
#include "algorithm.hpp"
#include "exception.hpp"
#include "utility.hpp"

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

			Hash hasher;

			struct wrappedPair
				{
				dex::pair < Key, Value > pair;
				bool isEmpty;
				bool isGhost;
				size_t hash;

				wrappedPair( ) : pair( Key{ }, Value{ } ), isEmpty( true ), isGhost( false ), hash( 0 ) { }
				wrappedPair( Key key, Value value ) : pair( key, value ), isEmpty( false ), isGhost( false )
					{
					hash = hasher( key );
					}
				};

			size_t probe( const Key &key ) const
				{
				size_t location = hasher( key ) % tableSize;
				for ( ;  table[ location ].isGhost || ( !table[ location ].isEmpty && table[ location ].pair.first != key );
						location = ( location + 1 ) % tableSize );
				return location;
				}
		public:
			unorderedMap( size_t tableSize = 10000, const Hash &hasher = Hash( ) )
				{
				this->tableSize = dex::max( size_t( 1 ), tableSize );
				numberElements = 0;
				ghostCount = 0;

				table = new wrappedPair[ this->tableSize ]( );
				}
			template< class InputIt >
			unorderedMap( InputIt first, InputIt last, size_t tableSize = 10000, const Hash &hasher = Hash( ) )
				{
				this->tableSize = dex::max( size_t( 1 ), tableSize );
				numberElements = 0;
				ghostCount = 0;

				table = new wrappedPair[ this->tableSize ]( );

				for ( ;  first != last;  ( *this )[ first->first ] = first->second, ++first );
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
					delete [ ] table;
				}

		private:
			template < bool isConst >
			class _iterator
				{
				private:
					friend class unorderedMap < Key, Value, Hash >;

					typedef typename std::conditional < isConst, const unorderedMap < Key, Value, Hash >,
							unorderedMap < Key, Value, Hash > >::type mapType;
					typedef typename std::conditional < isConst, const dex::pair < Key, Value >,
							dex::pair < Key, Value > >::type pairType;

					mapType *map;
					size_t position;
				private:
					_iterator( mapType &map, size_t position ) : map( &map )
						{
						for ( ;  position != map.bucketCount( ) &&
								( map.table[ position ].isEmpty || map.table[ position ].isGhost );
								++( position ) );
						this->position = position;
						}
				public:
					template < typename = typename std::enable_if < isConst > >
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
						return map->table[ position ].pair;
						}
					pairType *operator->( ) const
						{
						return &( map->table[ position ].pair );
						}

					_iterator &operator++( )
						{
						if ( position >= map->bucketCount( ) )
							throw outOfRangeException( );
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
				wrappedPair *bucket = &table[ probe( key ) ];
				if ( bucket->isEmpty )
					{
					bucket->pair = dex::pair < Key, Value >( key, Value { } );
					bucket->isEmpty = false;
					ghostCount -= bucket->isGhost;
					bucket->isGhost = false;
					bucket->hash = hasher( key );
					++numberElements;

					if ( 2 * ( size( ) + ghostCount ) > bucketCount( ) )
						{
						rehash( 2 * bucketCount( ) );
						bucket = &table[ probe( key ) ];
						}
					}
				return bucket->pair.second;
				}

			const Value &at( const Key &key ) const
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty )
					throw outOfRangeException( );
				return table[ location ].pair.second;
				}
			Value &at( const Key &key )
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty )
					throw outOfRangeException( );
				return table[ location ].pair.second;
				}

			iterator find( const Key &key )
				{
				return iterator( *this, probe( key ) );
				}

			constIterator find( const Key &key ) const
				{
				return constIterator( *this, probe( key ) );
				}

			iterator erase( constIterator position )
				{
				table[ position.position ].isGhost = true;
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
				if ( table[ location ].isEmpty )
					return 0;
				table[ location ].isGhost = true;
				++ghostCount;
				--numberElements;
				return 1;
				}

			void clear( )
				{
				erase( cbegin( ), cend( ) );
				}

			size_t count( const Key &key )
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty )
					return 0;
				return 1;
				}

			void rehash( size_t newSize )
				{
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
		};
	}

#endif
