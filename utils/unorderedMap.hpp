// unorderedMap.hpp
// Organically grown unordered map implementation that imitates the STL unordered_map
//
// We ignore the key_equal predicate and allocator
//
// 2019-10-20: File created: jasina, lougheem

namespace dex
	{
	template < class Key >
	struct hash;

	template < class Key, class Value, class Hash = dex::hash < Key > >
	class unorderedMap
		{
		private:
			struct wrappedValue;

			wrappedValue *table;
			unsigned tableSize;
			unsigned numberElements;

			Hash hasher;

			struct wrappedValue
				{
				Value value;
				bool isEmpty;
				bool isGhost;
				unsigned hash = hasher( value );

				wrappedValue( ) : value( Value{ } ), isEmpty( true ), isGhost( false ), hash( 0 ) { }
				wrappedValue( value ) : value( value ), isEmpty( false ), isGhost( false )
					{
					hash = hasher( value );
					}
				}

		public:
			unorderedMap( unsigned tableSize = 10000, const Hash &hasher = Hash( ) )
				{
				this->tableSize = tableSize;
				numberElements = 0;
				
				table = new wrappedValue[ this->tableSize ];
				}
			template< class InputIt >
			unorderedMap( InputIt first, InputIt last, unsigned tableSize = 10000, const Hash &hasher = Hash( ) )
				{
				this->tableSize = tableSize;
				numberElements = 0;
				
				table = new wrappedValue[ this->tableSize ];

				for ( ;  first != last;  this[ first.second ] = first.first, ++first, ++numElements );
				}

			~unorderedMap( )
				{
				delete table;
				}

			// TODO: DON'T HASH TWICE
			const Value &operator[ ]( const Key &key ) const
				{
				if ( this->count( key ) == 0 )
					{
					// TODO: grow map
					}
				return table[ hasher( key ) ];
				}
			Value &operator[ ]( const Key &key )
				{
				if ( this->count( key ) == 0 )
					{
					// TODO: grow map
					}
				return table[ hasher( key ) ];
				}

			const Value &at( const Key &key ) const
				{
				if ( this->count( key ) == 0 )
					throw outOfRangeException( );
				return table[ hasher( key ) ];
				}
			Value &at( const Key &key )
				{
				if ( this->count( key ) == 0 )
					throw outOfRangeException( );
				return table[ hasher( key ) ];
				}
		};
	}