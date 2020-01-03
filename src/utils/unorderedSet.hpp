// unorderedSet.hpp
// All natural (no preservative) unordered set implementation that imitates the STL unordered_set using linear probing.
//
// We ignore the key_equal predicate and allocator
//
// 2019-11-04: Use typeTraits.hpp: jasina
// 2019-11-02: Change default table size, allocate memory for pair only when needed: jasina
// 2019-10-28: File created: jasina

#ifndef DEX_UNORDERED_SET
#define DEX_UNORDERED_SET

#include <cstddef>
#include "utils/algorithm.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/typeTraits.hpp"
#include "utils/utility.hpp"

namespace dex
	{
	template< class Key >
	struct hash;

	template< class Key, class Hash = dex::hash< Key > >
	class unorderedSet
		{
		private:
			struct wrappedDatum;

			wrappedDatum *table;
			size_t tableSize;
			size_t numberElements;
			size_t ghostCount;

			static const size_t DEFAULT_TABLE_SIZE = 1 << 4;

			Hash hasher;

			struct wrappedDatum
				{
				Key *keyPtr;
				bool isEmpty;
				bool isGhost;

				wrappedDatum( ) : keyPtr( nullptr ), isEmpty( true ), isGhost( false ) { }
				wrappedDatum( Key key ) : isEmpty( false ), isGhost( false )
					{
					keyPtr = new Key( key );
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
						if ( *( table[ location ].keyPtr ) == key )
							return location;

					location = ( location + 1 ) % tableSize;
					}
				}
		public:
			unorderedSet( size_t tableSize = DEFAULT_TABLE_SIZE, const Hash &hasher = Hash( ) )
				{
				this->tableSize = dex::max( size_t( 1 ), tableSize );
				numberElements = 0;
				ghostCount = 0;
				this->hasher = hasher;

				table = new wrappedDatum[ this->tableSize ]( );
				}
			template< class InputIt >
			unorderedSet( InputIt first, InputIt last,
					size_t tableSize = DEFAULT_TABLE_SIZE, const Hash &hasher = Hash( ) )
				{
				this->tableSize = dex::max( size_t( 1 ), tableSize );
				numberElements = 0;
				ghostCount = 0;
				this->hasher = hasher;

				table = new wrappedDatum[ this->tableSize ]( );
				insert( first, last );
				}

			unorderedSet( const unorderedSet< Key, Hash > &other )
				{
				table = nullptr;
				unorderedSet< Key, Hash > temp( other.cbegin( ), other.cend( ), other.bucketCount( ), other.hasher );
				swap( temp );
				}

			unorderedSet &operator=( const unorderedSet< Key, Hash > &other )
				{
				unorderedSet< Key, Hash > temp( other );
				swap( temp );
				return *this;
				}

			~unorderedSet( )
				{
				if ( table )
					{
					for ( size_t index = 0;  index != tableSize;  ++index )
						if ( !( table[ index ].isEmpty || table[ index ].isGhost ) )
							delete table[ index ].keyPtr;
					delete [ ] table;
					}
				}

		private:
			template< bool isConst >
			class _iterator
				{
				private:
					friend class unorderedSet< Key, Hash >;

					typedef typename dex::conditional< isConst, const unorderedSet< Key, Hash >,
							unorderedSet< Key, Hash > >::type setType;
					typedef typename dex::conditional< isConst, const Key, Key >::type datumType;

					setType *set;
					size_t position;
				private:
					_iterator( setType &set, size_t position ) : set( &set )
						{
						// Find the first filled bucket at or after position
						for ( ;  position != set.bucketCount( ) &&
								( set.table[ position ].isEmpty || set.table[ position ].isGhost );
								++( position ) );
						this->position = position;
						}
				public:
					template< typename = typename dex::enableIf< isConst > >
					_iterator( const _iterator< false > &other ) :
							set( other.set ), position( other.position ) { }

					friend bool operator==( const _iterator &a, const _iterator &b )
						{
						// Only makes sense to compare iterators pointing to the same set
						if ( a.set != b.set )
							throw invalidArgumentException( );
						return a.position == b.position;
						}
					friend bool operator!=( const _iterator &a, const _iterator &b )
						{
						if ( a.set != b.set )
							throw invalidArgumentException( );
						return a.position != b.position;
						}

					datumType &operator*( ) const
						{
						return *( set->table[ position ].keyPtr );
						}
					datumType *operator->( ) const
						{
						return set->table[ position ].keyPtr;
						}

					_iterator &operator++( )
						{
						if ( position >= set->bucketCount( ) )
							throw outOfRangeException( );

						// Find the next filled bucket
						for ( ++position ;  position != set->bucketCount( ) &&
								( set->table[ position ].isEmpty || set->table[ position ].isGhost );  ++position );
						return *this;
						}
					_iterator operator++( int )
						{
						_iterator toReturn( *this );
						++*this;
						return toReturn;
						}
				};

			template< bool isConst >
			void swap( _iterator< isConst > &a, _iterator< isConst > &b )
				{
				dex::swap( a.set, b.set );
				dex::swap( a.position, b.position );
				}
		public:
			typedef _iterator< false > iterator;
			iterator begin( )
				{
				return iterator( *this, 0 );
				}
			iterator end( )
				{
				return iterator( *this, bucketCount( ) );
				}

			typedef _iterator< true > constIterator;
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
				delete table[ position.position ].keyPtr;
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
				delete table[ location ].keyPtr;
				++ghostCount;
				--numberElements;
				return 1;
				}

			void clear( )
				{
				erase( cbegin( ), cend( ) );
				}

			dex::pair< iterator, bool > insert( const Key &key )
				{
				bool inserted = false;
				size_t location = probe( key );
				wrappedDatum *bucket = &table[ location ];

				if ( bucket->isEmpty || bucket->isGhost )
					{
					inserted = true;
					bucket->keyPtr = new Key( key );
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

				return dex::pair< iterator, bool >{ iterator( *this, location ), inserted };
				}

			template< class InputIt >
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
				swap( unorderedSet< Key, Hash > ( cbegin( ), cend( ), dex::max( newSize, size( ) * 2 ) ) );
				}

			void swap( unorderedSet &other )
				{
				dex::swap( other.table, table );
				dex::swap( other.tableSize, tableSize );
				dex::swap( other.numberElements, numberElements );
				dex::swap( other.ghostCount, ghostCount );
				}

			void swap( unorderedSet &&other )
				{
				dex::swap( other.table, table );
				dex::swap( other.tableSize, tableSize );
				dex::swap( other.numberElements, numberElements );
				dex::swap( other.ghostCount, ghostCount );
				}

			dex::string compress( )
				{
				dex::string compressed = "";
				for ( auto it = begin();  it != cend();  ++it )
					{
					// tabs are formatting decisions
					compressed += "\t\t" + *it + "\n";
					}
				return compressed;
				}
		};

		template< class Key, class Hash >
		void swap( unorderedSet< Key, Hash > &a, unorderedSet< Key, Hash > &b )
			{
			a.swap( b );
			}
	}

#endif
