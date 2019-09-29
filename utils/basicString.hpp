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
// 2019-09-27: Define insert, replace, popBack, find, rfind: combsc, lougheem
// 2019-09-25: Define operator=, swap, iterator, constIterator, append, insert, at, and back: combsc, jasina, lougheem
// 2019-09-19: Formatting fixes; define constructors, capacity fuctions, and operator[]: combsc, jasina
// 2019-09-16: Defined Interface: combsc, jasina, lougheem
// 2019-09-14: File created: combsc, jasina, lougheem

#include "algorithm.hpp"
#include "exception.hpp"

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
				if( position > other.size( ) )
					{
					throw outOfRangeException();
					}
				// Make the final length be length, unless the string is too short. In that case, make it be to the end of the string.
				unsigned finalLength = min(length, other.size() - position );

				arraySize = other.capacity();
				array = new charT[ arraySize ];
				stringSize = other.size();

				for ( unsigned i = 0;  i != finalLength;  ++i )
					{
					array[ i ] = other[ i + position ];
					}
				}
			basicString( const charT* other, unsigned length = npos )
				{
				// Setting stringSize to the minimum of n and the size of other.
				for ( stringSize = 0;  other[ stringSize ] != charT ( { } ) && stringSize != length;  ++stringSize );
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
			basicString( unsigned length, charT character )
				{
				arraySize = length;
				array = new charT[ arraySize ];
				stringSize = arraySize;

				for ( unsigned i = 0; i != arraySize; ++i )
					{
					array[ i ] = character;
					}
				}
			template < class InputIterator > basicString( InputIterator first, InputIterator last )
				{
				if ( first.string != last.string )
					{
					throw invalidArgumentException( );
					}
				if ( first > last )
					{
					throw outOfRangeException( );
					}
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

			basicString < charT > &operator=( const basicString < charT > &other )
				{
				assign( other );
				}
			basicString < charT > &operator=( const charT *other )
				{
				basicString temporaryString( other );
				swap( temporaryString );
				return *this;
				}
			basicString < charT > &operator=( charT c )
				{
				stringSize = 1;
				array[ 1 ] = c;
				}
			basicString < charT > &operator=( basicString < charT > &&other )
				{
				swap( other );
				}

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
				private:
					unsigned position;
					basicString *string;
					iterator( basicString < charT > &string, unsigned position ) : string( &string ), position( position ) { }
				public:
					bool operator==( const iterator &other) const
						{
						// Only makes sense to compare iterators pointing to the same string
						if ( string != other.string )
							throw invalidArgumentException( );
						return position == other.position;
						}
					bool operator!=( const iterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException( );
						return position != other.position;
						}

					charT &operator*( ) const
						{
						return ( *string )[ position ];
						}
					charT *operator->( ) const
						{
						return &( ( *string )[ position ] );
						}

					iterator &operator++( )
						{
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return *this;
						}
					iterator &operator++( int )
						{
						iterator toReturn( *this );
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return toReturn;
						}

					iterator &operator--( )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}
					iterator &operator--( int )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}

					friend iterator &operator+( const iterator &, int );
					iterator &operator-( int n ) const
						{
						if ( position - n >= string->size( ) || position - n < 0 )
							throw outOfRangeException( );
						return iterator( string, position - n );
						}
					int operator-( const iterator &other ) const
						{
						if ( string == other.string )
							throw invalidArgumentException( );
						return int( position - other.position );
						}

					bool operator<( const iterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException();
						return position < other.position;
						}
					bool operator>( const iterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException();
						return position > other.position;
						}
					bool operator<=( const iterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException();
						return position <= other.position;
						}
					bool operator>=( const iterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException();
						return position >= other.position;
						}

					iterator &operator+=( int n )
						{
						if ( position + n >= string->size( ) || position + n < 0 )
							throw outOfRangeException( );
						position += n;
						return *this;
						}
					iterator &operator-=( int n )
						{
						if ( position - n >= string->size( ) || position - n < 0 )
							throw outOfRangeException( );
						position -= n;
						return *this;
						}

					charT &operator[ ]( const unsigned index ) const
						{
						return ( *string )[ index ];
						}

					friend class basicString < charT >;
				};
			friend iterator &operator+( const iterator &it, int n )
				{
				if ( it.position + n >= it.string->size( ) || it.position + n < 0 )
					throw outOfRangeException( );
				return iterator( it.string, it.position + n );
				}
			void swap( iterator &a, iterator &b )
				{
				dex::swap( a, b );
				}
			iterator begin( ) 
				{
				return iterator( this, 0 );
				}
			iterator end( ) 
				{
				return iterator( this, size( ) ); 
				}

			class constIterator
				{
				private:
					unsigned position;
					const basicString *string;
					constIterator( const basicString < charT > &string, unsigned position ) : string( &string ), position( position ) { }
				public:
					bool operator==( const constIterator &other) const
						{
						// Only makes sense to compare iterators pointing to the same string
						if ( string != other.string )
							throw invalidArgumentException( );
						return position == other.position;
						}
					bool operator!=( const constIterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException( );
						return position != other.position;
						}

					const charT &operator*( ) const
						{
						return ( *string )[ position ];
						}
					const charT *operator->( ) const
						{
						return &( ( *string )[ position ] );
						}

					constIterator &operator++( )
						{
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return *this;
						}
					constIterator &operator++( int )
						{
						constIterator toReturn( *this );
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return toReturn;
						}

					constIterator &operator--( )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}
					constIterator &operator--( int )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}

					friend iterator &operator+( const constIterator &, int );
					constIterator &operator-( int n ) const
						{
						if ( position - n >= string->size( ) || position - n < 0 )
							throw outOfRangeException( );
						return constIterator( string, position - n );
						}
					int operator-( const constIterator &other ) const
						{
						if ( string == other.string )
							throw invalidArgumentException( );
						return int( position - other.position );
						}

					bool operator<( const constIterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException();
						return position < other.position;
						}
					bool operator>( const constIterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException();
						return position > other.position;
						}
					bool operator<=( const constIterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException();
						return position <= other.position;
						}
					bool operator>=( const constIterator &other) const
						{
						if ( string != other.string )
							throw invalidArgumentException();
						return position >= other.position;
						}

					constIterator &operator+=( int n )
						{
						if ( position + n >= string->size( ) || position + n < 0 )
							throw outOfRangeException( );
						position += n;
						return *this;
						}
					constIterator &operator-=( int n )
						{
						if ( position - n >= string->size( ) || position - n < 0 )
							throw outOfRangeException( );
						position -= n;
						return *this;
						}

					const charT &operator[ ]( const unsigned index ) const
						{
						return ( *string )[ index ];
						}

					friend class basicString < charT >;
				};
			friend iterator &operator+( const constIterator &it, int n )
				{
				if ( it.position + n >= it.string->size( ) || it.position + n < 0 )
					throw outOfRangeException( );
				return constIterator( it.string, it.position + n );
				}
			void swap( constIterator &a, constIterator &b )
				{
				dex::swap( a, b );
				}
			constIterator cbegin( ) const
				{
				return constIterator( this, 0 );
				}
			constIterator cend( ) const
				{
				return constIterator( this, size( ) );
				}

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

			const charT &at( unsigned position) const
				{
				if( position < 0 || position >= stringSize )
					{
					throw outOfRangeException();
					}
				return array[ position ];
				}
			charT &at( unsigned position )
				{
				if( position < 0 || position >= stringSize )
					{
					throw outOfRangeException();
					}
				return array[ position ];
				}

			const charT &back( ) const
				{
				if( stringSize == 0 )
					{
					throw outOfRangeException();
					}
				return array[ stringSize - 1 ];
				}
			charT &back()
				{
				if( stringSize == 0 )
					{
					throw outOfRangeException();
					}
				return array[ stringSize - 1 ];
				}

			// Modifiers
			basicString < charT > &operator+=( const basicString &other )
				{
				append( other );
				}
			basicString < charT > &operator+=( const charT *other )
				{
				append( other );
				}
			basicString < charT > &operator+=( charT character )
				{
				append( other );
				}

			basicString < charT > &append( const basicString < charT > &other )
				{
				append( other.begin( ), other.end( ) );
				}
			basicString < charT > &append( const basicString < charT > &other, unsigned position, unsigned length )
				{
				append( other.begin( ) + position, other.begin( ) + position + length );
				}
			basicString < charT > &append( const charT *other )
				{
				const charT *otherEnd;
				// Technically less efficient, but is more clear and avoids code duplication.
				for ( otherEnd = other;  *otherEnd != '\0';  ++otherEnd );
				append( other, otherEnd );
				}
			basicString < charT > &append( const charT *other, unsigned length )
				{
				append( other, other + length );
				}
			basicString < charT > &append( unsigned number, charT character )
				{
				resize( stringSize + number, character );
				}
			template < class InputIterator > basicString < charT > &append( InputIterator first, InputIterator last )
				{
				if ( first.string != last.string )
					{
					throw invalidArgumentException( );
					}
				if ( first >= last )
					{
					throw outOfRangeException( );
					}

				resize( unsigned( stringSize + ( last - first ) ) );

				for ( insertionLocation = end( );  first != last;  *( insertionLocation++ ) = *( first++ ) );
				}

			void pushBack( charT character )
				{
				resize( stringSize + 1, character );
				}

			basicString < charT > &assign( const basicString < charT > &other )
				{
				basicString temporaryString( other );
				swap( temporaryString );
				return *this;
				}
			basicString < charT > &assign( const basicString < charT > &other, unsigned position, unsigned length )
				{
				basicString temporaryString = other.substr( position, length );
				swap( temporaryString );
				return *this;
				}
			basicString < charT > &assign( const charT *other )
				{
				basicString temporaryString( other );
				swap( temporaryString );
				return *this;
				}
			basicString < charT > &assign( const charT *other, unsigned length )
				{
				basicString temporaryString( other, length );
				swap( temporaryString );
				return *this;
				}
			basicString < charT > &assign( unsigned length, charT character )
				{
				basicString temporaryString( length, character );
				swap( temporaryString );
				return *this;
				}
			basicString < charT > &assign( iterator first, iterator last )
				{
				basicString temporaryString( first, last );
				swap( temporaryString );
				return *this;
				}
			
			basicString < charT > &insert( unsigned position, const basicString < charT > &other )
				{
				insert( iterator( this, position ), other.begin( ), other.end( ) );
				return *this;
				}
			basicString < charT > &insert( unsigned position, const basicString < charT > &other, unsigned subposition, unsigned sublength )
				{
				insert( iterator( this, position ), other.begin( ) + subposition, other.begin() + subposition + sublength );
				return *this;
				}
			basicString < charT > &insert( unsigned position, const charT *other )
				{
				// Technically less efficient, but is more clear and avoids code duplication.
				for ( otherEnd = other;  otherEnd != '\0';  ++otherEnd );
				insert( iterator( this, position ), other, otherEnd );
				return *this;
				}
			basicString < charT > &insert( unsigned position, const charT *other, unsigned length )
				{
				// Caution: we do not check if other is of length at least length
				insert( iterator( this, position ), other, other + length );
				return *this;
				}
			basicString < charT > &insert( unsigned position, unsigned length, charT character )
				{
				insert( iterator( *this, position ), length, character );
				return *this;
				}

			iterator insert( constIterator first, unsigned length, charT character )
				{
				resize( stringSize + length );
				// Shift right part of the string
				for ( iterator segmentEnd = first + length;  segmentEnd != first;  --segmentEnd )
					*( segmentEnd + (length - 1) ) = *( segmentEnd - 1 );
				// Fill in characters
				for ( iterator segmentEnd = first;  segmentEnd != first + length;  array[ segmentEnd++ ] = character );
				return iterator( first );
				}
			iterator insert( constIterator first, charT character )
				{
				return insert( first, 1, character );
				}
			template < class InputIterator >
					iterator insert( iterator insertionPoint, InputIterator first, InputIterator last )
				{
				if ( first.string != last.string )
					{
					throw invalidArgumentException( );
					}
				if ( first > last )
					{
					throw outOfRangeException( );
					}
				unsigned length = 0;
				for ( InputIterator counter = first;  counter != last;  ++counter )
					++length;
				resize( stringSize + last - first );
				// Shift right part of the string
				for ( iterator segmentEnd = ( insertionPoint + length );  segmentEnd != first;  --segmentEnd )
					*( segmentEnd + ( length - 1 ) ) = * ( segmentEnd - 1 );
				// Fill in characters
				for ( ;  first != last;  *( insertionPoint++ ) = *( first++ ) );
				return insertionPoint - length;
				}
			
			basicString < charT > &erase( unsigned position = 0, unsigned length = npos )
				{
				erase( cbegin( ) + position, cbegin( ) + position + length );
				return *this;
				}
			iterator erase ( constIterator first )
				{
				return erase(first, cend( ) );
				}
			iterator erase ( constIterator first, constIterator last )
				{
				// Throw an error if the iterators passed are to different strings.
				if( first.string != this || last.string != this )
					{
					throw invalidArgumentException( );
					}
				if( first > last )
					{
					throw outOfRangeException( );
					}
				
				constIterator lastIncrementer = last;
				for( iterator copier = first; copier != last && lastIncremeneter != cend( ); *( copier++ ) = *( lastIncrementer++ ) );
				// Decrease string size
				resize( stringSize - ( last - first ) );
				return first;
				}

			basicString < charT > &replace( unsigned position, unsigned length, const basicString < charT > &other )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, other );
				}
			basicString < charT > &replace( constIterator first, constIterator last, const basicString < charT > &other )
				{
				return replace( first, last, other.cbegin( ), other.cend( ) );
				}
			basicString < charT > &replace( unsigned position, unsigned length, const basicString < charT > &other, unsigned subposition, unsigned sublength )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, other.cbegin( ) + subposition, other.cbegin( ) + subposition + sublength );
				}
			basicString < charT > &replace( unsigned position, unsigned length, const charT *other )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, other );
				}
			basicString < charT > &replace( constIterator first, constIterator Last, const charT *other )
				{
				int stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT ( { } ) && stringSize != length;  ++stringSize );
				return replace( first, last, other, stringSize );
				}
			basicString < charT > &replace( unsigned position, unsigned length, const charT *other, unsigned n )
				{
				first = erase( cbegin( ) + position, cbegin( ) + position + length );
				insert( first, other, n );
				return *this;
				}
			basicString < charT > &replace( constIterator first, constIterator Last, const charT *other, unsigned n )
				{
				first = erase( first, last );
				insert( first, other, n );
				return *this;
				}
			basicString < charT > &replace( unsigned position, unsigned length, unsigned n, charT c )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, n, c );
				}
			basicString < charT > &replace( constIterator first, constIterator Last, unsigned n, charT c )
				{
				first = erase( first, last )
				insert( first, n, c );
				return *this;
				}
			template < class InputIterator > basicString < charT > &replace( constIterator first, constIterator Last, InputIterator inputFirst, InputIterator inputLast )
				{
				first = erase( first, last );
				insert( first, inputFirst, inputLast );
				return *this;
				}

			void swap( basicString &other )
				{
				dex::swap( array, other.array );
				dex::swap( arraySize, other.arraySize );
				dex::swap( stringSize, other.stringSize );
				}

			void popBack( )
				{
				stringSize--;
				array[ stringSize ] = '\0';
				}

			// String Operations

			const charT* cStr( ) const
				{
				return array;
				}

			const charT* data( ) const
				{
				return cStr( );
				}

			unsigned copy( charT *characterArray, unsigned length, unsigned position = 0 ) const
				{
				int index;
				for ( index = 0; index < length && position + index < stringSize; ++index )
					{
					characterArray[ index ] = array[ index + position ]
					}
				}
				return index;

			unsigned find( const basicString &other, unsigned position = 0 ) const
				{
				return find( other.cStr( ), position );
				}
			unsigned find( const charT *other, unsigned position = 0 ) const
				{
				int stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT ( { } ) && stringSize != length;  ++stringSize );
				return find( other, position, stringSize );
				}
			unsigned find( const charT *other, unsigned position, unsigned n ) const
				{
				for ( unsigned leftSearchPosition = position; leftSearchPosition < stringSize - n; ++leftSearchPosition )
					{
					unsigned searchWindowLength;
					for ( searchWindowLength = 0; searchWindowLength < n; ++searchWindowLength )
						{
						if ( array[ leftSearchPosition + searchWindowLength ] != other[ searchWindowLength ] )
							{
							break;
							}
						}
					if ( searchWindowLength == n )
						{
						return leftSearchPosition;
						}
					}
				return npos;
				}
			unsigned find( charT c, unsigned position = 0 ) const
				{
				for ( unsigned leftSearchPosition = position; leftSearchPosition < stringSize; ++leftSearchPosition )
					{
					if ( array[ leftSearchPosition ] == c ) 
						{
						return leftSearchPosition;
						}
					}
				return npos;
				}

			unsigned rfind( const basicString &other, unsigned position = 0 ) const
				{
				return rfind( other.cStr( ), position );
				}
			unsigned rfind( const charT *other, unsigned position = 0 ) const
				{
				int stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT ( { } ) && stringSize != length;  ++stringSize );
				return rfind( other, position, stringSize );
				}
			unsigned rfind( const charT *other, unsigned position, unsigned n ) const
				{
				for ( unsigned leftSearchPosition = position - n; leftSearchPosition >= 0; leftSearchPosition--)
					{
					unsigned searchWindowLength;
					for ( searchWindowLength = 0; searchWindowLength < n; ++searchWindowLength )
						{
						if ( array[ leftSearchPosition + searchWindowLength ] != other[ searchWindowLength ] )
							{
							break;
							}
						}
					if ( searchWindowLength == n )
						{
						return leftSearchPosition;
						}
					}
				return npos;
				}
			unsigned rfind( charT c, unsigned position = 0 ) const
				{
				for ( unsigned rightSearchPosition = stringSize - 1; rightSearchPosition >= position; -- rightSearchPosition )
					{
					if ( array[ rightSearchPosition ] == c )
						{
						return rightSearchPosition;
						}
					}
				return npos;
				}

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

			basicString < charT > substr( unsigned position = 0, unsigned length = npos )
				{
				return basicString( *this, position, length );
				}

			int compare( const basicString &other ) const;
			int compare( unsigned position, unsigned length, const basicString &other ) const;
			int compare( unsigned position, unsigned length, const basicString &other, unsigned subposition, unsigned sublength ) const;
			int compare( const charT *other ) const;
			int compare( unsigned position, unsigned length, const charT *other ) const;
			int compare( unsigned position, unsigned length, const charT *other, unsigned n ) const;
		};
	}

// TODO: Non-member function interface
