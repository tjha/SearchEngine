// unorderedMap.hpp
// Organically grown unordered map implementation that imitates the STL unordered_map. This uses linear probing.
//
// We ignore the key_equal predicate and allocator
//
// 2019-10-26: Wrote count, rehash, constructors, operator[ ], operator=, at, empty, size, maxSize, bucketCount, clear, swap, non-iterator erase: jasina, lougheem
// 2019-10-20: File created: jasina, lougheem

#include <cstddef>
#include "algorithm.hpp"

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
				Key key;
				Value value;
				bool isEmpty;
				bool isGhost;
				size_t hash;

				wrappedPair( ) : key( Key{ } ), value( Value{ } ), isEmpty( true ), isGhost( false ), hash( 0 ) { }
				wrappedPair( Key key, Value value ) : key( key ), value( value ), isEmpty( false ), isGhost( false )
					{
					hash = hasher( value );
					}
				}

			size_t probe( const Key &key ) const
				{
				size_t location = hasher( key );
				for ( ;  table[ location ].isGhost || ( !table[ location ].isEmpty && table[ location ].key != key );
						location = ( location + 1 ) % tableSize );
				return location;
				}
		public:
			unorderedMap( size_t tableSize = 10000, const Hash &hasher = Hash( ) )
				{
				this->tableSize = dex::max( 1, tableSize );
				numberElements = 0;
				ghostCount = 0;
				
				table = new wrappedPair[ this->tableSize ];
				}
			template< class InputIt >
			unorderedMap( InputIt first, InputIt last, size_t tableSize = 10000, const Hash &hasher = Hash( ) )
				{
				this->tableSize = dex::max( 1, tableSize );
				numberElements = 0;
				ghostCount = 0;
				
				table = new wrappedPair[ this->tableSize ];

				for ( ;  first != last;  this[ first->first ] = first->second, ++first, ++numElements );
				}

			unorderedMap( const unorderedMap < Key, Value, Hash > &other )
				{
				unorderedMap < Key, Value, Hash > temp( other.cbegin( ), other.cend( ),
						other.bucketCount( ), other.hasher );
				swap( temp );
				}

			unorderedMap &operator=( const unorderedMap < Key, Value, Hash > &other )
				{
				uorderedMap < Key, Value, Hash > temp( other );
				swap( temp );
				return *this;
				}

			~unorderedMap( )
				{
				delete table;
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

			const Value &operator[ ]( const Key &key ) const
				{
				wrappedPair *bucket = &table[ probe( key ) ];
				if ( bucket->isEmpty )
					{
					bucket->key = key;
					bucket->isEmpty = false;
					ghostCount -= bucket->isGhost;
					bucket->isGhost = false;
					bucket->hash = hasher( key );
					++numberElements;

					if ( 2 * size( ) > bucketCount( ) )
						{
						rehash( 2 * bucketCount( ) );
						bucket = &table[ probe( key ) ];
						}
					}
				return bucket->value;
				}
			Value &operator[ ]( const Key &key )
				{
				wrappedPair *bucket = &table[ probe( key ) ];
				if ( bucket->isEmpty )
					{
					bucket->key = key;
					bucket->isEmpty = false;
					ghostCount -= bucket->isGhost;
					bucket->isGhost = false;
					bucket->hash = hasher( key );
					++numberElements;

					if ( 2 * size( ) > bucketCount( ) )
						{
						rehash( 2 * bucketCount( ) );
						bucket = &table[ probe( key ) ];
						}
					}
				return bucket->value;
				}

			const Value &at( const Key &key ) const
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty )
					throw outOfRangeException( );
				return table[ location ];
				}
			Value &at( const Key &key )
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty )
					throw outOfRangeException( );
				return table[ location ];
				}

			iterator erase( constIterator position )
				{
				// TODO
				}

			iterator erase( constIterator first, constIterator last )
				{
				// TODO
				}

			size_t erase( const Key &key )
				{
				size_t location = probe( key );
				if ( table[ location ].isEmpty )
					return 0;
				table[ location ].isGhost = true;
				return 1;
				}

			void clear( )
				{
				erase( this.cbegin( ), this.cend( ) );
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
				swap( unorderedMap < Key, Value, Hash > ( this.cbegin( ), this.cend( ),
						dex::max( newSize, size( ) * 2 ) ) );
				}

			void swap( unorderedMap &other )
				{
				dex::swap( other.table, table );
				dex::swap( other.tableSize, tableSize );
				dex::swap( other.numberElements, numberElements );
				dex::swap( other.ghostCount, ghostCount );
				}
		};
	}