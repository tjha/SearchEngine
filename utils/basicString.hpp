// basicString.hpp
// Homemade version of std::basic_string that keeps all of the interfaces.
// We omit the following functions:
//    std::basic_string( initializer_list < charT > );
//    std::basic_string::operator=( std::initializer_list < charT > );
//    std::basic_string::operator+=( std::initializer_list < charT > );
//    std::basic_string::append( std::initializer_list < charT > );
//    std::basic_string::assign( std::basic_string < charT > && ) noexcept;
//    std::basic_string::insert( const_iterator, initializer_list < charT > );
//    std::basic_string::replace( const_iterator, const_iterator, initializer_list < charT > );
//    std::basic_string::get_allocator( ) const;
// 2019-09-19: Formatting fixes: combsc, jasina
// 2019-09-16: Defined Interface: combsc, jasina, lougheem
// 2019-09-14: File created: combsc, jasina, lougheem

namespace dex
	{
	template < class charT > 
	class basicString
		{
		private:
			charT *array;
			unsigned arraySize;
			unsigned stringSize;

		public:
			static const unsigned npos = unsigned ( -1 );

			basicString( )
				{
				arraySize = 1;
				stringSize = 0;
				array = new charT[ arraySize ];
				}
			basicString( const basicString < charT > &other, unsigned position = 0, unsigned length = npos )
				{
				// Make the final length be length, unless the string is too short. In that case, make it be to the end of the string.
				unsigned finalLength = length > other.size() - position ? other.size() - position : length;

				arraySize = other.capacity();
				array = new charT[ arraySize ];
				stringSize = other.size();

				for ( unsigned i = 0;  i != finalLength;  ++i )
					{
					array[ i ] = other[ i + position ];
					}
				}
			basicString( const charT* other, unsigned n = npos )
				{
				for ( stringSize = 0;  other[ stringSize ] != charT ( { } ) && stringSize != n;  ++stringSize );
				// We are making here the assumption that the number of appendations
				//    string will not be significant. Therefore, we will do no 
				//    optimizations for allocating arraySize, allocating lazily.
				arraySize = stringSize;
				array = new charT[ arraySize ];
				for ( unsigned i = 0;  i != stringSize;  ++i )
					{
					array[ i ] = other[ i ];
					}
				}
			basicString( unsigned n, charT c )
				{
				arraySize = n;
				array = new charT[ arraySize ];
				stringSize = arraySize;

				for ( unsigned i = 0; i != arraySize; ++i )
					{
					array[ i ] = c;
					}
				}
			template < class InputIterator > basicString( InputIterator first, InputIterator last )
				{
				// TODO: This is an uber-naive way to allocate memory. Maybe we should change this later.
				arraySize = 1;
				array = new charT[ arraySize ];
				stringSize = 0;

				while ( first != last )
					{
					if ( ++stringSize > arraySize )
						{
						reserve( arraySize <<= 1 );
						}

					array[stringSize - 1] = *(first++);
					}
				}
			basicString( basicString < charT > &&other )
				{
				arraySize = other.arraySize;
				array = other.array;
				stringSize = other.stringSize;

				other.arraySize = 0;
				other.array = nullptr;
				other.stringSize = 0;
				}

			~basicString( )
				{
				if ( array )
					{
					delete [ ] array;
					}
				}

			basicString < charT > &operator=( basicString < charT > other );
			basicString < charT > &operator=( const charT* other );
			basicString < charT > &operator=( charT c );
			basicString < charT > &operator=( basicString < charT > &&other );

			// Capacity
			unsigned size( ) const
				{
				return stringSize;
				}

			unsigned length( ) const
				{
				return stringSize;
				}

			unsigned max_size( ) const
				{
				return npos;
				}

			void resize( unsigned newStringSize, charT character = { } )
				{
				reserve(newStringSize);
				for ( unsigned i = stringSize; i < newStringSize; ++i )
					{
					array[ i ] = character;
					}
				stringSize = newStringSize;
				}

			unsigned capacity( ) const
				{
				return arraySize;
				}

			void reserve( unsigned newArraySize )
				{
				if (newArraySize == stringSize )
					{
					return;
					}

				charT *newArray = new charT[ newArraySize ];
				if ( newArraySize > stringSize )
					{
					for ( unsigned i = 0;  i != stringSize;  ++i )
						{
						newArray[ i ] = array[ i ];
						}
					}
				else
					{
					for ( unsigned i = 0;  i != newArraySize;  ++i )
						{
						newArray[ i ] = array[ i ];
						}
					stringSize = newArraySize;
					}
				delete [ ] array;
				array = newArray;
				arraySize = newArraySize;

				}

			void clear( )
				{
				stringSize = 0;
				}

			bool empty( ) const
				{
				return stringSize == 0;
				}

			// We can alter the behavior of this function. If we want a string buffer we'll have to change this.
			void shrink_to_fit( )
				{
				reserve( stringSize );
				}

			// Iterators
			class iterator
				{
				};
			iterator begin( );
			iterator end( );

			class constIterator
				{
				};
			constIterator cbegin( ) const;
			constIterator cend( ) const;

			class reverseIterator
				{
				};
			reverseIterator rbegin( );
			reverseIterator rend( );

			class constReverseIterator
				{
				};
			constReverseIterator crbegin( ) const;
			constReverseIterator crend( ) const;

			// Element Access
			const charT &operator[ ]( unsigned position ) const
				{
				return array[ position ];
				}
			charT &operator[ ]( unsigned position )
				{
				return array[ position ];
				}

			const charT &at( unsigned ) const;
			charT &at( unsigned );

			const charT &back( ) const;
			charT &back();

			// Modifiers
			basicString < charT > &operator+=( const basicString &other );
			basicString < charT > &operator+=( const charT *other );
			basicString < charT > &operator+=( charT character );

			basicString < charT > &append( const basicString < charT > &other );
			basicString < charT > &append( const basicString < charT > &other, unsigned position, unsigned length );
			basicString < charT > &append( const charT *other );
			basicString < charT > &append( const charT *other, unsigned length );
			basicString < charT > &append( unsigned number, charT character );
			basicString < charT > &append( iterator first, iterator last );

			void pushBack( charT character );

			basicString < charT > &assign( const basicString < charT > &other );
			basicString < charT > &assign( const basicString < charT > &other, unsigned position, unsigned length );
			basicString < charT > &assign( const charT *other );
			basicString < charT > &assign( const charT *other, unsigned length ); 
			basicString < charT > &assign( unsigned length, charT character ); 
			basicString < charT > &assign( iterator first, iterator last );
			
			basicString < charT > &insert( unsigned position, const basicString < charT > &other );
			basicString < charT > &insert( unsigned position, const basicString < charT > &other, unsigned subposition, unsigned sublength );
			basicString < charT > &insert( unsigned position, const charT *other );
			basicString < charT > &insert( unsigned position, const charT *other, unsigned length );
			basicString < charT > &insert( unsigned position, unsigned length, charT character );

			iterator insert( constIterator first, unsigned length, charT character );
			iterator insert( constIterator first, charT character );
			template < class InputIterator >
					iterator insert( iterator insertionPoint, InputIterator first, InputIterator last );
			
			basicString < charT > &erase( unsigned position = 0, unsigned length = npos );
			iterator erase ( constIterator first );
			iterator erase ( constIterator first, constIterator last );

			basicString < charT > &replace( unsigned position, unsigned length, const basicString < charT > &other );
			basicString < charT > &replace( constIterator first, constIterator last, const basicString < charT > &other );
			basicString < charT > &replace( unsigned position, unsigned length, const basicString < charT > &other, unsigned subposition, unsigned sublength );
			basicString < charT > &replace( unsigned position, unsigned length, const charT *other );
			basicString < charT > &replace( constIterator first, constIterator Last, const charT *other );
			basicString < charT > &replace( unsigned position, unsigned length, const charT *other, unsigned n );
			basicString < charT > &replace( constIterator first, constIterator Last, const charT *other, unsigned n );
			basicString < charT > &replace( unsigned position, unsigned length, unsigned n, charT c );
			basicString < charT > &replace( constIterator first, constIterator Last, unsigned n, charT c );
			template < class InputIterator > basicString < charT > &replace( constIterator first, constIterator Last, InputIterator inputFirst, InputIterator inputLast );

			void swap( basicString &other );

			void popBack( );

			// String Operations

			const charT* cStr( ) const;

			const charT* data( ) const;

			unsigned copy( charT *characterArray, unsigned length, unsigned position = 0 ) const;

			unsigned find( const basicString &other, unsigned position = 0 ) const;
			unsigned find( const charT *other, unsigned position = 0 ) const;
			unsigned find( const charT *other, unsigned position, unsigned n ) const;
			unsigned find( charT c, unsigned position = 0 ) const;

			unsigned rfind( const basicString &other, unsigned position = 0 ) const;
			unsigned rfind( const charT *other, unsigned position = 0 ) const;
			unsigned rfind( const charT *other, unsigned position, unsigned n ) const;
			unsigned rfind( charT c, unsigned position = 0 ) const;

			unsigned findFirstOf( const basicString &other, unsigned position = 0 ) const;
			unsigned findFirstOf( const charT *other, unsigned position = 0 ) const;
			unsigned findFirstOf( const charT *other, unsigned position, unsigned n ) const;
			unsigned findFirstOf( charT c, unsigned position = 0 ) const;

			unsigned findLastOf( const basicString &other, unsigned position = 0 ) const;
			unsigned findLastOf( const charT *other, unsigned position = 0 ) const;
			unsigned findLastOf( const charT *other, unsigned position, unsigned n ) const;
			unsigned findLastOf( charT c, unsigned position = 0 ) const;

			unsigned findFirstNotOf( const basicString &other, unsigned position = 0 ) const;
			unsigned findFirstNotOf( const charT *other, unsigned position = 0 ) const;
			unsigned findFirstNotOf( const charT *other, unsigned position, unsigned n ) const;
			unsigned findFirstNotOf( charT c, unsigned position = 0 ) const;

			unsigned findLastNotOf( const basicString &other, unsigned position = 0 ) const;
			unsigned findLastNotOf( const charT *other, unsigned position = 0 ) const;
			unsigned findLastNotOf( const charT *other, unsigned position, unsigned n ) const;
			unsigned findLastNotOf( charT c, unsigned position = 0 ) const;

			basicString < charT > substr( unsigned position = 0, unsigned length = npos );

			int compare( const basicString &other ) const;
			int compare( unsigned position, unsigned length, const basicString &other ) const;
			int compare( unsigned position, unsigned length, const basicString &other, unsigned subposition, unsigned sublength ) const;
			int compare( const charT *other ) const;
			int compare( unsigned position, unsigned length, const charT *other ) const;
			int compare( unsigned position, unsigned length, const charT *other, unsigned n ) const;
		};
	}

// TODO: Non-member function interface
