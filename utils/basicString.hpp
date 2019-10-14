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
// 2019-10-13: Let iterators be cast to const, clean up reverse iterators, fix insert, use copy/fill from algorithm,
//             fix styling errors, use find/search/find_end from algorithm, implement findLastOf and findLastNotOf:
//             jasina
// 2019-10-08: Fix assign, fix append, fix iterator operator-: combsc, jasina
// 2019-10-07: Fix iterator; implement comparison operators: combsc, jasina
// 2019-10-02: Define findFirstOf, findFirstNotOf, compare: combsc, lougheem
// 2019-10-01: Fix syntax errors: lougheem
// 2019-09-27: Define insert, replace, popBack, find, rfind: combsc, lougheem
// 2019-09-25: Define operator=, swap, iterator, constIterator, append, insert, at, and back: combsc, jasina, lougheem
// 2019-09-19: Formatting fixes; define constructors, capacity fuctions, and operator[]: combsc, jasina
// 2019-09-16: Defined Interface: combsc, jasina, lougheem
// 2019-09-14: File created: combsc, jasina, lougheem

#include <iostream>
#include "algorithm.hpp"
#include "exception.hpp"

namespace dex
	{
	template < typename charT >
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
					throw outOfRangeException( );
				// Make the final length be length, unless the string is too short. In that case, make it be to the end
				// of the string.
				unsigned finalLength = min(length, other.size( ) - position );

				arraySize = other.capacity( );
				array = new charT[ arraySize ];
				stringSize = finalLength;

				dex::copy( other.cbegin( ) + position, other.cbegin( ) + position + finalLength, array );
				}
			basicString( const charT* other, unsigned length = npos )
				{
				// Setting stringSize to the minimum of length and the size of other.
				for ( stringSize = 0;  other[ stringSize ] != charT { } && stringSize != length;  ++stringSize );
				// We are making here the assumption that the number of appendations
				//    string will not be significant. Therefore, we will do no 
				//    optimizations for allocating arraySize, allocating lazily.
				arraySize = stringSize;
				array = new charT[ arraySize ];
				
				dex::copy( other, other + stringSize, array );
				}
			basicString( unsigned length, charT character )
				{
				arraySize = length;
				array = new charT[ arraySize ];
				stringSize = arraySize;

				dex::fill( array, array + stringSize, character );
				}
			template < class InputIterator > basicString( InputIterator first, InputIterator last )
				{
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
				return assign( other );
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
				array[ 0 ] = c;
				return *this;
				}
			basicString < charT > &operator=( basicString < charT > &&other )
				{
				swap( other );
				return *this;
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
				dex::fill( array + stringSize, array + newStringSize, character );
				stringSize = newStringSize;
				}

			unsigned capacity( ) const
				{
				return arraySize;
				}

			void reserve( unsigned newArraySize )
				{
				if (newArraySize == arraySize )
					{
					return;
					}

				charT *newArray = new charT[ newArraySize ];
				if ( newArraySize > stringSize )
					dex::copy( array, array + stringSize, newArray );
				else
					{
					dex::copy( array, array + newArraySize, newArray );
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
			void shrinkToFit( )
				{
				reserve( stringSize );
				}

			// Iterators
			class constIterator;
			class constReverseIterator;

			class iterator
				{
				private:
					template < typename T >
					friend class basicString;
					friend class constIterator;
					basicString <charT > *string;
					unsigned position;
					iterator( basicString < charT > &string, unsigned position ) :
							string( &string ), position( position ) { }
				public:
					friend bool operator==( const iterator &a, const iterator &b )
						{
						// Only makes sense to compare iterators pointing to the same string
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position == b.position;
						}
					friend bool operator!=( const iterator &a, const iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position != b.position;
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
					iterator operator++( int )
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
					iterator operator--( int )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}

					friend iterator operator-( const iterator &it, int n )
						{
						if ( it.position > n + it.string->size( ) || ( n > 0 && it.position < unsigned( n ) ) )
							throw outOfRangeException( );
						return iterator( *it.string, it.position - n );
						}
					friend int operator-( const iterator &a, const iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return int( a.position - b.position );
						}
					friend iterator operator+( const iterator &it, int n )
						{
						if ( it.position + n > it.string->size( ) || ( n < 0 && it.position < unsigned( -n ) ) )
							throw outOfRangeException( );
						return iterator( *it.string, it.position + n );
						}

					friend bool operator<( const iterator &a, const iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position < b.position;
						}
					friend bool operator>( const iterator &a, const iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position > b.position;
						}
					friend bool operator<=( const iterator &a, const iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position <= b.position;
						}
					friend bool operator>=( const iterator &a, const iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position >= b.position;
						}

					iterator &operator+=( int n )
						{
						return *this = *this + n;
						}
					iterator &operator-=( int n )
						{
						return *this = *this - n;
						}

					charT &operator[ ]( const unsigned index ) const
						{
						return ( *string )[ index ];
						}

					friend class basicString < charT >;
				};
			void swap( iterator &a, iterator &b )
				{
				dex::swap( a, b );
				}
			iterator begin( )
				{
				return iterator( *this, 0 );
				}
			iterator end( )
				{
				return iterator( *this, size( ) ); 
				}

			class constIterator
				{
				private:
					template < typename T >
					friend class basicString;
					const basicString < charT > *string;
					unsigned position;
					constIterator( const basicString < charT > &string, unsigned position ) :
							string( &string ), position( position ) { }
				public:
					constIterator( const iterator &it ) : string( it.string ), position( it.position ) { }
					friend bool operator==( const constIterator &a, const constIterator &b )
						{
						// Only makes sense to compare iterators pointing to the same string
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position == b.position;
						}
					friend bool operator!=( const constIterator &a, const constIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position != b.position;
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
					constIterator operator++( int )
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
					constIterator operator--( int )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}

					friend constIterator operator-( const constIterator &it, int n )
						{
						if ( it.position > n + it.string->size( ) || ( n > 0 && it.position < unsigned( n ) ) )
							throw outOfRangeException( );
						return constIterator( *it.string, it.position - n );
						}
					friend int operator-( const constIterator &a, const constIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return int( a.position - b.position );
						}
					friend constIterator operator+( const constIterator &it, int n )
						{
						if ( it.position + n > it.string->size( ) || ( n < 0 && it.position < unsigned( -n ) ) )
							throw outOfRangeException( );
						return constIterator( *it.string, it.position + n );
						}

					friend bool operator<( const constIterator &a, const constIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position < b.position;
						}
					friend bool operator>( const constIterator &a, const constIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position > b.position;
						}
					friend bool operator<=( const constIterator &a, const constIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position <= b.position;
						}
					friend bool operator>=( const constIterator &a, const constIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position >= b.position;
						}

					constIterator &operator+=( int n )
						{
						return *this = *this + n;
						}
					constIterator &operator-=( int n )
						{
						return *this = *this - n;
						}

					const charT &operator[ ]( const unsigned index ) const
						{
						return ( *string )[ index ];
						}
				};
			void swap( constIterator &a, constIterator &b )
				{
				dex::swap( a, b );
				}
			constIterator cbegin( ) const
				{
				return constIterator( *this, 0 );
				}
			constIterator cend( ) const
				{
				return constIterator( *this, size( ) );
				}

			class reverseIterator
				{
				private:
					template < typename T >
					friend class basicString;
					friend class constReverseIterator;
					unsigned position;
					basicString *string;
					reverseIterator( basicString < charT > &string, unsigned position ) :
							string( &string ), position( position ) { }
				public:
					friend bool operator==( const reverseIterator &a, const reverseIterator &b )
						{
						// Only makes sense to compare iterators pointing to the same string
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position == b.position;
						}
					friend bool operator!=( const reverseIterator &a, const reverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position != b.position;
						}

					charT &operator*( ) const
						{
						return ( *string )[ string->size( ) - position - 1 ];
						}
					charT *operator->( ) const
						{
						return &( ( *string )[ string->size( ) - position - 1 ] );
						}

					reverseIterator &operator++( )
						{
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return *this;
						}
					reverseIterator operator++( int )
						{
						reverseIterator toReturn( *this );
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return toReturn;
						}

					reverseIterator &operator--( )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}
					reverseIterator operator--( int )
						{
						reverseIterator toReturn( *this );
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return toReturn;
						}

					friend reverseIterator operator-( const reverseIterator &it, int n )
						{
						if ( it.position > n + it.string->size( ) || ( n > 0 && it.position < unsigned( n ) ) )
							throw outOfRangeException( );
						return reverseIterator( *it.string, it.position - n );
						}
					friend int operator-( const reverseIterator &a, const reverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return int( a.position - b.position );
						}
					friend reverseIterator operator+( const reverseIterator &it, int n )
						{
						if ( it.position + n > it.string->size( ) || ( n < 0 && it.position < unsigned( -n ) ) )
							throw outOfRangeException( );
						return reverseIterator( *it.string, it.position + n );
						}

					friend bool operator<( const reverseIterator &a, const reverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position < b.position;
						}
					friend bool operator>( const reverseIterator &a, const reverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position > b.position;
						}
					friend bool operator<=( const reverseIterator &a, const reverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position <= b.position;
						}
					friend bool operator>=( const reverseIterator &a, const reverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position >= b.position;
						}

					reverseIterator &operator+=( int n )
						{
						return *this = *this + n;
						}
					reverseIterator &operator-=( int n )
						{
						return *this = *this - n;
						}

					charT &operator[ ]( const unsigned index ) const
						{
						return ( *string )[ index ];
						}
				};
			void swap( reverseIterator &a, reverseIterator &b )
				{
				dex::swap( a, b );
				}
			reverseIterator rbegin( )
				{
				return reverseIterator( *this, 0 );
				}
			reverseIterator rend( )
				{
				return reverseIterator( *this, size( ) );
				}

			class constReverseIterator
				{
				private:
					template < typename T >
					friend class basicString;
					unsigned position;
					const basicString *string;
					constReverseIterator( const basicString < charT > &string, unsigned position ) :
							string( &string ), position( position ) { }
				public:
					constReverseIterator( const reverseIterator &it ) : string( it.string ), position( it.position ) { }
					friend bool operator==( const constReverseIterator &a, const constReverseIterator &b )
						{
						// Only makes sense to compare iterators pointing to the same string
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position == b.position;
						}
					friend bool operator!=( const constReverseIterator &a, const constReverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position != b.position;
						}

					const charT &operator*( ) const
						{
						return ( *string )[ string->size( ) - position - 1 ];
						}
					const charT *operator->( ) const
						{
						return &( ( *string )[ string->size( ) - position - 1 ] );
						}

					constReverseIterator &operator++( )
						{
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return *this;
						}
					constReverseIterator operator++( int )
						{
						constReverseIterator toReturn( *this );
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return toReturn;
						}

					constReverseIterator &operator--( )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}
					constReverseIterator operator--( int )
						{
						constReverseIterator toReturn( *this );
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return toReturn;
						}

					friend constReverseIterator operator-( const constReverseIterator &it, int n )
						{
						if ( it.position > n + it.string->size( ) || ( n > 0 && it.position < unsigned( n ) ) )
							throw outOfRangeException( );
						return constReverseIterator( *it.string, it.position - n );
						}
					friend int operator-( const constReverseIterator &a, const constReverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return int( a.position - b.position );
						}
					friend constReverseIterator operator+( const constReverseIterator &it, int n )
						{
						if ( it.position + n > it.string->size( ) || ( n < 0 && it.position < unsigned( -n ) ) )
							throw outOfRangeException( );
						return constReverseIterator( *it.string, it.position + n );
						}

					friend bool operator<( const constReverseIterator &a, const constReverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position < b.position;
						}
					friend bool operator>( const constReverseIterator &a, const constReverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position > b.position;
						}
					friend bool operator<=( const constReverseIterator &a, const constReverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position <= b.position;
						}
					friend bool operator>=( const constReverseIterator &a, const constReverseIterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position >= b.position;
						}

					constReverseIterator &operator+=( int n )
						{
						return *this = *this + n;
						}
					constReverseIterator &operator-=( int n )
						{
						return *this = *this - n;
						}

					const charT &operator[ ]( const unsigned index ) const
						{
						return ( *string )[ index ];
						}
				};
			void swap( constReverseIterator &a, constReverseIterator &b )
				{
				dex::swap( a, b );
				}
			constReverseIterator crbegin( ) const
				{
				return constReverseIterator( *this, 0 );
				}
			constReverseIterator crend( ) const
				{
				return constReverseIterator( *this, size( ) ); 
				}

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
				return array[ position ];
				}
			charT &at( unsigned position )
				{
				if( position < 0 || position >= stringSize )
					throw outOfRangeException( );
				return array[ position ];
				}

			const charT &back( ) const
				{
				if( stringSize == 0 )
					throw outOfRangeException( );
				return array[ stringSize - 1 ];
				}
			charT &back( )
				{
				if( stringSize == 0 )
					throw outOfRangeException( );
				return array[ stringSize - 1 ];
				}

			// Modifiers
			basicString < charT > &operator+=( const basicString &other )
				{
				return append( other );
				}
			basicString < charT > &operator+=( const charT *other )
				{
				return append( other );
				}
			basicString < charT > &operator+=( charT character )
				{
				return append( 1, character );
				}

			basicString < charT > &append( const basicString < charT > &other )
				{
				return append( other.cbegin( ), other.cend( ) );
				}
			basicString < charT > &append( const basicString < charT > &other, unsigned position, unsigned length )
				{
				return append( other.cbegin( ) + position, other.cbegin( ) + position + length );
				}
			basicString < charT > &append( const charT *other )
				{
				const charT *otherEnd;
				// Technically less efficient, but is more clear and avoids code duplication.
				for ( otherEnd = other;  *otherEnd != '\0';  ++otherEnd );
				return append( other, otherEnd );
				}
			basicString < charT > &append( const charT *other, unsigned length )
				{
				return append( other, other + length );
				}
			basicString < charT > &append( unsigned number, charT character )
				{
				resize( stringSize + number, character );
				return *this;
				}
			template < class InputIterator >
			basicString < charT > &append( InputIterator first, InputIterator last )
				{
				insert( cend( ), first, last );
				return *this;
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
				insert( cbegin( ) + position, other.cbegin( ), other.cend( ) );
				return *this;
				}
			basicString < charT > &insert( unsigned position, const basicString < charT > &other,
					unsigned subposition, unsigned sublength )
				{
				insert( cbegin( ) + position, other.cbegin( ) + subposition,
						other.cbegin( ) + subposition + sublength );
				return *this;
				}
			basicString < charT > &insert( unsigned position, const charT *other )
				{
				// Technically less efficient, but is more clear and avoids code duplication.
				iterator otherEnd;
				for ( otherEnd = other;  otherEnd != '\0';  ++otherEnd );
				insert( cbegin( ) + position, other, otherEnd );
				return *this;
				}
			basicString < charT > &insert( unsigned position, const charT *other, unsigned length )
				{
				// Caution: we do not check if other is of length at least length
				insert( cbegin( ) + position, other, other + length );
				return *this;
				}
			basicString < charT > &insert( unsigned position, unsigned length, charT character )
				{
				insert( cbegin( ) + position, length, character );
				return *this;
				}
			iterator insert( constIterator first, unsigned length, charT character )
				{
				resize( stringSize + length );
				// Shift right part of the string
				dex::copy_backward( first, cend( ) - length, end( ) );
				// Fill in characters
				dex::fill( first, first + length, character );
				return begin( ) + ( first - cbegin( ) );
				}
			iterator insert( constIterator first, charT character )
				{
				return insert( first, 1, character );
				}
			template < class InputIterator >
			iterator insert( constIterator insertionPoint, InputIterator first, InputIterator last )
				{
				iterator fore = begin( ) + (insertionPoint - cbegin( ));
				if ( first > last )
					throw outOfRangeException( );
				unsigned length = last - first;
				resize( stringSize + length, 'u' );
				// Shift right part of the string
				dex::copy_backward( insertionPoint, cend( ) - length, end( ) );
				// Fill in characters
				dex::copy( first, last, fore );
				return fore + length;
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

				dex::copy( last( ), end( ), first( ) );
				// Decrease string size
				resize( stringSize - ( last - first ) );
				return first;
				}

			basicString < charT > &replace( unsigned position, unsigned length, const basicString < charT > &other )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, other );
				}
			basicString < charT > &replace( constIterator first, constIterator last,
					const basicString < charT > &other )
				{
				return replace( first, last, other.cbegin( ), other.cend( ) );
				}
			basicString < charT > &replace( unsigned position, unsigned length,
					const basicString < charT > &other, unsigned subposition, unsigned sublength )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length,
						other.cbegin( ) + subposition, other.cbegin( ) + subposition + sublength );
				}
			basicString < charT > &replace( unsigned position, unsigned length, const charT *other )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, other );
				}
			basicString < charT > &replace( constIterator first, constIterator last, const charT *other )
				{
				unsigned stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT { };  ++stringSize );
				return replace( first, last, other, stringSize );
				}
			basicString < charT > &replace( unsigned position, unsigned length, const charT *other, unsigned n )
				{
				iterator first = erase( cbegin( ) + position, cbegin( ) + position + length );
				insert( first, other, n );
				return *this;
				}
			basicString < charT > &replace( constIterator first, constIterator last, const charT *other, unsigned n )
				{
				first = erase( first, last );
				insert( first, other, n );
				return *this;
				}
			basicString < charT > &replace( unsigned position, unsigned length, unsigned n, charT c )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, n, c );
				}
			basicString < charT > &replace( constIterator first, constIterator last, unsigned n, charT c )
				{
				first = erase( first, last );
				insert( first, n, c );
				return *this;
				}
			template < class InputIterator > basicString < charT > &replace( constIterator first, constIterator last,
					InputIterator inputFirst, InputIterator inputLast )
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

			unsigned copy( charT *characterArray, unsigned count, unsigned position = 0 ) const
				{
				count = count == npos ? stringSize - count : min( stringSize, position + count );
				dex::copy( begin( ) + position, begin( ) + position + count, characterArray );
				return count;
				}

			unsigned find( const basicString &other, unsigned position = 0 ) const
				{
				return find( other.cStr( ), position, other.stringSize );
				}
			unsigned find( const charT *other, unsigned position = 0 ) const
				{
				// Inefficient, but easy to understand
				int stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT { };  ++stringSize );
				return find( other, position, stringSize );
				}
			unsigned find( const charT *other, unsigned position, unsigned n ) const
				{
				constIterator location = dex::search( cbegin( ) + position, cend( ), other, other + n );
				if ( location == cend( ) )
					return npos;
				return location - cbegin( );
				}
			unsigned find( charT c, unsigned position = 0 ) const
				{
				constIterator location = dex::find( cbegin( ) + position, cend( ), c );
				if ( location == cend( ) )
					return npos;
				return location - cbegin( );
				}

			unsigned rfind( const basicString &other, unsigned position = 0 ) const
				{
				return rfind( other.cStr( ), position, other.stringSize );
				}
			unsigned rfind( const charT *other, unsigned position = 0 ) const
				{
				int stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT { };  ++stringSize );
				return rfind( other, position, stringSize );
				}
			unsigned rfind( const charT *other, unsigned position, unsigned n ) const
				{
				constIterator location = dex::find_end( cbegin( ) + position, cend( ), other, other + n );
				if ( location == cend( ) )
					return npos;
				return location - cbegin( );
				}
			unsigned rfind( charT c, unsigned position = 0 ) const
				{
				constReverseIterator location = dex::find( crbegin( ) + position, crend( ), c );
				if ( location == crend( ) )
					return npos;
				return stringSize - 1 - (location - crbegin( ));
				}

			unsigned findFirstOf( const basicString &other, unsigned position = 0 ) const
				{
				return findFirstOf( other.cStr( ), position, other.size( ) );
				}
			unsigned findFirstOf( const charT *other, unsigned position = 0 ) const
				{
				unsigned stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT { };  ++stringSize );
				return findFirstOf( other, position, stringSize );
				}
			// This seems like an inefficient implementation... if we had an unordered map we would
			// speed this up a lot
			unsigned findFirstOf( const charT *other, unsigned position, unsigned n ) const
				{
				for ( constIterator it = cbegin( ) + position; it != cend( ); ++it )
					for ( unsigned i = 0; i != n; ++i )
						if ( *it == other[ i ] )
							return it - cbegin( );
				return npos;
				}
			unsigned findFirstOf( charT c, unsigned position = 0 ) const
				{
				return find( c, position );
				}

			unsigned findLastOf( const basicString &other, unsigned position = 0 ) const
				{
				return findLastOf( other.cStr( ), position, other.size( ) );
				}
			unsigned findLastOf( const charT *other, unsigned position = 0 ) const
				{
				unsigned stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT { };  ++stringSize );
				return findLastOf( other, position, stringSize );
				}
			unsigned findLastOf( const charT *other, unsigned position, unsigned n ) const
				{
				for ( constReverseIterator it = crbegin( ) + position; it != crend( ); ++it )
					for ( unsigned i = 0; i != n; ++i )
						if ( *it == other[ i ] )
							return stringSize - 1 - ( it - crbegin( ) );
				return npos;
				}
			unsigned findLastOf( charT c, unsigned position = 0 ) const
				{
				return rfind( c, position );
				}

			unsigned findFirstNotOf( const basicString &other, unsigned position = 0 ) const
				{
				return findFirstNotOf( other.cStr( ), position, other.size( ) );
				}
			unsigned findFirstNotOf( const charT *other, unsigned position = 0 ) const
				{
				unsigned stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT { };  ++stringSize );
				return findFirstNotOf( other, position, stringSize );
				}
			unsigned findFirstNotOf( const charT *other, unsigned position, unsigned n ) const
				{
				for ( constIterator it = cbegin( ) + position; it != cend( ); ++it )
					{
					bool isInOther = false;
					for ( unsigned i = 0; i < n; ++i )
						if ( *it == other[ i ] )
							{
							isInOther = true;
							break;
							}
					if ( !isInOther )
						return it - cbegin( );
					}
				return npos;
				}
			unsigned findFirstNotOf( charT c, unsigned position = 0 ) const
				{
				return findFirstNotOf( &c, position, 1 );
				}

			unsigned findLastNotOf( const basicString &other, unsigned position = 0 ) const
				{
				return findLastNotOf( other.cStr( ), position, other.size( ) );
				}
			unsigned findLastNotOf( const charT *other, unsigned position = 0 ) const
				{
				unsigned stringSize;
				for ( stringSize = 0;  other[ stringSize ] != charT { };  ++stringSize );
				return findLastNotOf( other, position, stringSize );
				}
			unsigned findLastNotOf( const charT *other, unsigned position, unsigned n ) const
				{
				for ( constReverseIterator it = crbegin( ) + position; it != crend( ); ++it )
					{
					bool isInOther = false;
					for ( unsigned i = 0; i < n; ++i )
						if ( *it == other[ i ] )
							{
							isInOther = true;
							break;
							}
					if ( !isInOther )
						return stringSize - 1 - ( it - crbegin( ) );
					}
				return npos;
				}
			unsigned findLastNotOf( charT c, unsigned position = 0 ) const
				{
				return findLastNotOf( &c, position, 1 );
				}

			basicString < charT > substr( unsigned position = 0, unsigned length = npos )
				{
				return basicString( *this, position, length );
				}

			int compare( const basicString &other ) const
				{
				return compare( 0, size( ), other, 0, other.size( ) );
				}
			int compare( unsigned position, unsigned length, const basicString &other ) const
				{
				return compare( position, length, other, 0, other.size( ) );
				}
			int compare( unsigned position, unsigned length,
					const basicString &other, unsigned subposition, unsigned sublength ) const
				{
				constIterator first = cbegin( ) + position;
				constIterator otherFirst = other.cbegin( ) + subposition;

				constIterator last = cbegin( ) + position + length;
				constIterator otherLast = other.cbegin( ) + subposition + sublength;

				while ( first != last && otherFirst != otherLast )
					{
					if ( *first < *otherFirst )
						return -1;
					if ( *first > *otherFirst )
						return 1;
					++first;
					++otherFirst;
					}
				if ( otherFirst != otherLast )
					return -1;
				if ( first != last )
					return 1;
				return 0;
				}
			int compare( const charT *other ) const
				{
				basicString otherBasicString = basicString( other );
				return compare( 0, size( ), otherBasicString, 0, otherBasicString.size( ) );
				}
			int compare( unsigned position, unsigned length, const charT *other ) const
				{
				basicString otherBasicString = basicString( other );
				return compare( position, length, other, 0, other.size( ) );
				}
			int compare( unsigned position, unsigned length, const charT *other, unsigned n ) const
				{
				basicString otherBasicString = basicString( other );
				return compare( position, length, other, 0, n );
				}
		};

		template < class charT >
		std::ostream &operator<<( std::ostream &os, const basicString < charT > &str )
			{
			for ( typename basicString < charT >::constIterator it = str.cbegin( );  it != str.cend( );  ++it )
				os << *it;
			return os;
			}

		template < class charT >
		bool operator==( const basicString < charT > &lhs, const basicString < charT > &rhs )
			{
			return !lhs.compare( rhs );
			}
		template < class charT >
		bool operator==( const charT *lhs, const basicString < charT > &rhs )
			{
			return !rhs.compare( lhs );
			}
		template < class charT >
		bool operator==( const basicString < charT > &lhs, const charT *rhs )
			{
			return !lhs.compare( rhs );
			}
		template < class charT >
		bool operator!=( const basicString < charT > &lhs, const basicString < charT > &rhs )
			{
			return lhs.compare( rhs );
			}
		template < class charT >
		bool operator!=( const charT *lhs, const basicString < charT > &rhs )
			{
			return rhs.compare( lhs );
			}
		template < class charT >
		bool operator!=( const basicString < charT > &lhs, const charT *rhs )
			{
			return lhs.compare( rhs );
			}
		template < class charT >
		bool operator>( const basicString < charT > &lhs, const basicString < charT > &rhs )
			{
			return lhs.compare( rhs ) > 0;
			}
		template < class charT >
		bool operator>( const charT *lhs, const basicString < charT > &rhs )
			{
			return rhs.compare( lhs ) < 0;
			}
		template < class charT >
		bool operator>( const basicString < charT > &lhs, const charT *rhs )
			{
			return lhs.compare( rhs ) > 0;
			}
		template < class charT >
		bool operator<( const basicString < charT > &lhs, const basicString < charT > &rhs )
			{
			return lhs.compare( rhs ) < 0;
			}
		template < class charT >
		bool operator<( const charT *lhs, const basicString < charT > &rhs )
			{
			return rhs.compare( lhs ) > 0;
			}
		template < class charT >
		bool operator<( const basicString < charT > &lhs, const charT *rhs )
			{
			return lhs.compare( rhs ) < 0;
			}
		template < class charT >
		bool operator>=( const basicString < charT > &lhs, const basicString < charT > &rhs )
			{
			return lhs.compare( rhs ) >= 0;
			}
		template < class charT >
		bool operator>=( const charT *lhs, const basicString < charT > &rhs )
			{
			return rhs.compare( lhs ) <= 0;
			}
		template < class charT >
		bool operator>=( const basicString < charT > &lhs, const charT *rhs )
			{
			return lhs.compare( rhs ) >= 0;
			}
		template < class charT >
		bool operator<=( const basicString < charT > &lhs, const basicString < charT > &rhs )
			{
			return lhs.compare( rhs ) <= 0;
			}
		template < class charT >
		bool operator<=( const charT *lhs, const basicString < charT > &rhs )
			{
			return rhs.compare( lhs ) >= 0;
			}
		template < class charT >
		bool operator<=( const basicString < charT > &lhs, const charT *rhs )
			{
			return lhs.compare( rhs ) <= 0;
			}
	
	typedef dex::basicString < char > string;
	}
