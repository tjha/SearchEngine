// basicString.hpp
// Homemade version of std::basic_string that keeps all of the interfaces. Quickly navigate the code by searching for
// the phrase "CTRLF" (with no quotes)
//
// We omit the following functions:
//    std::basic_string( initializer_list < charT > );
//    std::basic_string::operator=( std::initializer_list < charT > );
//    std::basic_string::operator+=( std::initializer_list < charT > );
//    std::basic_string::append( std::initializer_list < charT > );
//    std::basic_string::assign( std::basic_string < charT > && ) noexcept;
//    std::basic_string::insert( const_iterator, initializer_list < charT > );
//    std::basic_string::replace( const_iterator, const_iterator, initializer_list < charT > );
//    std::basic_string::get_allocator( ) const;
//
// 2019-10-17: Add include guard, lots of minor bug fixes, hash function, make array always be a C-string, improve the
//             efficiency of replace, rewrite erase and insert to use replace: jasina, lougheem
// 2019-10-14: Fix insert, erase, replace, find: jasina, lougheem
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

#ifndef DEX_BASIC_STRING
#define DEX_BASIC_STRING

#include <iostream>
#include "algorithm.hpp"
#include "exception.hpp"

namespace dex
	{
	template < typename charT >
	class basicString
		{
		private:
			// A null-terminated C-string
			charT *array;
			// In order to maintain our array invariant, we must have arraySize >= stringSize + 1
			unsigned arraySize;
			unsigned stringSize;

			// Find the length of a null-terminated C-string (not including the null-terminator)
			static unsigned cStringLength( const charT *cstr )
				{
				const charT *end;
				for ( end = cstr;  *end != charT { };  ++end );
				return end - cstr;
				}

		public:
			// Maximum possible size of our string
			static const unsigned npos = unsigned ( -1 );

			// CTRLF Constructors
			basicString( )
				{
				arraySize = 1;
				stringSize = 0;
				array = new charT[ arraySize ]( );
				}
			basicString( const basicString < charT > &other, unsigned position = 0, unsigned length = npos )
				{
				if ( position > other.size( ) )
					throw outOfRangeException( );

				// Make the final length be length, unless the string is too short. In that case, make it be to the end
				// of the string.
				unsigned finalLength = min(length, other.size( ) - position );

				arraySize = finalLength + 1;
				array = new charT[ arraySize ];
				stringSize = finalLength;

				dex::copy( other.cbegin( ) + position, other.cbegin( ) + position + finalLength, array );
				array[ finalLength ] = charT { };
				}
			basicString( const charT* other, unsigned length = npos )
				{
				// We are making here the assumption that the number of appendations
				//    string will not be significant. Therefore, we will do no
				//    optimizations for allocating arraySize, allocating lazily.
				stringSize = min( cStringLength( other ), length );
				arraySize = stringSize + 1;
				array = new charT[ arraySize ];

				dex::copy( other, other + stringSize, array );
				array[ stringSize ] = charT { };
				}
			basicString( unsigned length, charT character )
				{
				arraySize = length + 1;
				array = new charT[ arraySize ];
				stringSize = length;

				dex::fill( array, array + stringSize, character );
				array[ stringSize ] = charT { };
				}
			template < class InputIterator > basicString( InputIterator first, InputIterator last )
				{
				// This is an uber-naive way to allocate memory. Maybe we should change this later.
				arraySize = 1;
				array = new charT[ arraySize ];
				stringSize = 0;

				while ( first != last )
					{
					if ( stringSize + 2 > arraySize )
						reserve( arraySize << 1 );

					array[ stringSize++ ] = *( first++ );
					}

				array[ stringSize ] = charT { };
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

			// CTRLF Destructor
			~basicString( )
				{
				if ( array )
					delete [ ] array;
				}

			// CTRLF Assignment
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
				resize( 1 );
				array[ 0 ] = c;
				return *this;
				}
			basicString < charT > &operator=( basicString < charT > &&other )
				{
				swap( other );
				return *this;
				}

			// CTRLF Capacity
			unsigned size( ) const
				{
				return stringSize;
				}

			unsigned length( ) const
				{
				return stringSize;
				}

			unsigned maxSize( ) const
				{
				return npos;
				}

			void resize( unsigned newStringSize, charT character = { } )
				{
				if ( newStringSize > capacity( ) )
					reserve( newStringSize );

				if ( newStringSize > size( ) )
					dex::fill( array + size( ), array + newStringSize, character );
				stringSize = newStringSize;
				array[ size( ) ] = charT { };
				}

			unsigned capacity( ) const
				{
				return arraySize - 1;
				}

			void reserve( unsigned newArraySize )
				{
				if ( newArraySize == capacity( ) )
					return;

				charT *newArray = new charT[ newArraySize + 1 ];
				if ( newArraySize > size( ) )
					dex::copy( array, array + size( ) + 1, newArray );
				else
					{
					// This block is actually a non-binding request, so we could delete it if need be.
					dex::copy( array, array + newArraySize, newArray );
					stringSize = newArraySize;
					array[ size( ) ] = charT { };
					}
				delete [ ] array;
				array = newArray;
				arraySize = newArraySize + 1;
				}

			void clear( )
				{
				resize( 0 );
				}

			bool empty( ) const
				{
				return size( ) == 0;
				}

			// We can alter the behavior of this function. If we want a string buffer we'll have to change this.
			void shrinkToFit( )
				{
				reserve( size( ) );
				}

			// CTRLF Iterators
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
					const basicString *string;
					unsigned position;
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

			// CTRLF Element Access
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
				if( position < 0 || position >= size( ) )
					throw outOfRangeException( );
				return array[ position ];
				}
			charT &at( unsigned position )
				{
				if( position < 0 || position >= size( ) )
					throw outOfRangeException( );
				return array[ position ];
				}

			const charT &front( ) const
				{
				return array[ 0 ];
				}
			charT &front( )
				{
				return array[ 0 ];
				}

			const charT &back( ) const
				{
				return array[ size( ) - 1 ];
				}
			charT &back( )
				{
				return array[ size( ) - 1 ];
				}

			const charT* cStr( ) const
				{
				return array;
				}

			const charT* data( ) const
				{
				return array;
				}

			// CTRLF Modifiers
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
				// Technically less efficient, but is more clear and avoids code duplication.
				return append( other, other + cStringLength( other ) );
				}
			basicString < charT > &append( const charT *other, unsigned length )
				{
				return append( other, other + length );
				}
			basicString < charT > &append( unsigned number, charT character )
				{
				resize( size( ) + number, character );
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
				resize( size( ) + 1, character );
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
			template< class InputIt >
			basicString < charT > &assign( InputIt first, InputIt last )
				{
				basicString temporaryString( first, last );
				swap( temporaryString );
				return *this;
				}

			basicString < charT > &insert( unsigned position, const basicString < charT > &other )
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				insert( cbegin( ) + position, other.cbegin( ), other.cend( ) );
				return *this;
				}
			basicString < charT > &insert( unsigned position, const basicString < charT > &other,
					unsigned subposition, unsigned sublength )
				{
				if ( position > size( ) || subposition > other.size( ) )
					throw dex::outOfRangeException( );

				insert( cbegin( ) + position, other.cbegin( ) + subposition,
						other.cbegin( ) + subposition + sublength );
				return *this;
				}
			basicString < charT > &insert( unsigned position, const charT *other )
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				// Technically less efficient, but is more clear and avoids code duplication.
				unsigned length = cStringLength( other );
				insert( cbegin( ) + position, other, other + length );
				return *this;
				}
			basicString < charT > &insert( unsigned position, const charT *other, unsigned length )
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				// Caution: we do not check if other is of length at least length
				insert( cbegin( ) + position, other, other + length );
				return *this;
				}
			basicString < charT > &insert( unsigned position, unsigned length, charT character )
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				insert( cbegin( ) + position, length, character );
				return *this;
				}
			iterator insert( constIterator first, unsigned length, charT character )
				{
				unsigned originalSize = size( );
				replace( first, first, length, character );
				return begin( ) + ( ( first - cbegin( ) ) + size( ) - originalSize );
				}
			iterator insert( constIterator first, charT character )
				{
				return insert( first, 1, character );
				}
			template < class InputIterator >
			iterator insert( constIterator insertionPoint, InputIterator first, InputIterator last )
				{
				unsigned originalSize = size( );
				replace( insertionPoint, insertionPoint, first, last );
				return begin( ) + ( ( insertionPoint - cbegin( ) ) + size( ) - originalSize );
				}

			basicString < charT > &erase( unsigned position = 0, unsigned length = npos )
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				erase( cbegin( ) + position, cbegin( ) + position + dex::min( length, size( ) - position ) );
				return *this;
				}
			iterator erase ( constIterator first )
				{
				return erase(first, cend( ) );
				}
			iterator erase ( constIterator first, constIterator last )
				{
				// Return if the iterators passed are to different strings.
				if( first.string != this || last.string != this || first > last )
					return end( );

				replace( first, last, 0, charT { } );
				return begin( ) + ( first - cbegin( ) );
				}

			basicString < charT > &replace( unsigned position, unsigned length, const basicString < charT > &other )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, other );
				}
			basicString < charT > &replace( constIterator first, constIterator last, const basicString < charT > &other )
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
				return replace( first, last, other, other + cStringLength( other ) );
				}
			basicString < charT > &replace( unsigned position, unsigned length, const charT *other, unsigned n )
				{
				replace( cbegin( ) + position, cbegin( ) + ( position + length ), other, other + n );
				return *this;
				}
			basicString < charT > &replace( constIterator first, constIterator last, const charT *other, unsigned n )
				{
				replace( first, last, other, n );
				return *this;
				}
			basicString < charT > &replace( unsigned position, unsigned length, unsigned n, charT c )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, n, c );
				}
		private:
			void shiftAtPoint( unsigned insertionLength, constIterator first, constIterator last )
				{
				unsigned removalLength = last - first;
				unsigned newStringLength = size( ) + insertionLength - removalLength;
				iterator writableFirst = begin( ) + ( first - cbegin( ) );

				if ( removalLength < insertionLength )
					{
					constIterator oldCend = cend( );
					if ( newStringLength > capacity( ) )
						reserve( newStringLength );
					stringSize = newStringLength;
					array[ size( ) ] = charT { };
					dex::copyBackward( last, oldCend, end( ) );
					}
				if ( removalLength > insertionLength )
					{
					dex::copy( last, cend( ), writableFirst + insertionLength );
					stringSize = newStringLength;
					array[ size( ) ] = charT { };
					}
				}
		public:
			basicString < charT > &replace( constIterator first, constIterator last, unsigned n, charT c )
				{
				shiftAtPoint( n, first, last );

				iterator writableFirst = begin( ) + ( first - cbegin( ) );
				dex::fill( writableFirst, writableFirst + n, c );

				return *this;
				}
			template < class InputIterator > basicString < charT > &replace( constIterator first, constIterator last,
					InputIterator inputFirst, InputIterator inputLast )
				{
				unsigned insertionLength = 0;
				for ( InputIterator it = inputFirst;  it != inputLast;  ++insertionLength, ++it );
				shiftAtPoint( insertionLength, first, last );

				dex::copy( inputFirst, inputLast, begin( ) + ( first - cbegin( ) ) );
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
				--stringSize;
				array[ stringSize ] = charT { };
				}

			// CTRLF String Operations
			unsigned copy( charT *characterArray, unsigned count, unsigned position = 0 ) const
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				if ( count == npos )
					count = size( ) - count;
				else
					count = min( size( ), position + count );

				dex::copy( begin( ) + position, begin( ) + position + count, characterArray );
				return count;
				}

			unsigned find( const basicString &other, unsigned position = 0 ) const
				{
				return find( other.cStr( ), position, other.size( ) );
				}
			unsigned find( const charT *other, unsigned position = 0 ) const
				{
				// Inefficient, but easy to understand
				return find( other, position, cStringLength( other ) );
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

			unsigned rfind( const basicString &other, unsigned position = npos ) const
				{
				return rfind( other.cStr( ), position, other.size( ) );
				}
			unsigned rfind( const charT *other, unsigned position = npos ) const
				{
				return rfind( other, position, cStringLength( other ) );
				}
			unsigned rfind( const charT *other, unsigned position, unsigned n ) const
				{
				if ( n == 0 )
					return min( position, size( ) );
				constIterator searchEnd = cbegin( ) + min( min( position, size( ) ) + n, size( ) );
				constIterator location = dex::findEnd( cbegin( ), searchEnd,
						other, other + n );
				if ( location == searchEnd )
					return npos;
				return location - cbegin( );
				}
			unsigned rfind( charT c, unsigned position = npos ) const
				{
				constReverseIterator searchStart = crend( ) - ( min( position, size( ) - 1 ) + 1 );
				constReverseIterator location = dex::find( searchStart, crend( ), c );
				if ( location == crend( ) )
					return npos;
				return size( ) - 1 - (location - crbegin( ));
				}

			unsigned findFirstOf( const basicString &other, unsigned position = 0 ) const
				{
				return findFirstOf( other.cStr( ), position, other.size( ) );
				}
			unsigned findFirstOf( const charT *other, unsigned position = 0 ) const
				{
				return findFirstOf( other, position, cStringLength( other ) );
				}
			// This seems like an inefficient implementation... if we had an unordered map we would
			// speed this up a lot
			unsigned findFirstOf( const charT *other, unsigned position, unsigned n ) const
				{
				for ( constIterator it = cbegin( ) + position;  it != cend( );  ++it )
					for ( unsigned i = 0;  i != n;  ++i )
						if ( *it == other[ i ] )
							return it - cbegin( );
				return npos;
				}
			unsigned findFirstOf( charT c, unsigned position = 0 ) const
				{
				return find( c, position );
				}

			unsigned findLastOf( const basicString &other, unsigned position = npos ) const
				{
				return findLastOf( other.cStr( ), position, other.size( ) );
				}
			unsigned findLastOf( const charT *other, unsigned position = npos ) const
				{
				return findLastOf( other, position, cStringLength( other ) );
				}
			unsigned findLastOf( const charT *other, unsigned position, unsigned n ) const
				{
				constReverseIterator searchStart = crend( ) - ( min( position, size( ) - 1 ) + 1 );
				for ( constReverseIterator it = searchStart;  it != crend( );  ++it )
					for ( unsigned i = 0;  i != n;  ++i )
						if ( *it == other[ i ] )
							return size( ) - 1 - ( it - crbegin( ) );
				return npos;
				}
			unsigned findLastOf( charT c, unsigned position = npos ) const
				{
				return rfind( c, position );
				}

			unsigned findFirstNotOf( const basicString &other, unsigned position = 0 ) const
				{
				return findFirstNotOf( other.cStr( ), position, other.size( ) );
				}
			unsigned findFirstNotOf( const charT *other, unsigned position = 0 ) const
				{
				return findFirstNotOf( other, position, cStringLength( other ) );
				}
			unsigned findFirstNotOf( const charT *other, unsigned position, unsigned n ) const
				{
				for ( constIterator it = cbegin( ) + position;  it != cend( );  ++it )
					{
					bool isInOther = false;
					for ( unsigned i = 0;  i != n;  ++i )
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

			unsigned findLastNotOf( const basicString &other, unsigned position = npos ) const
				{
				return findLastNotOf( other.cStr( ), position, other.size( ) );
				}
			unsigned findLastNotOf( const charT *other, unsigned position = npos ) const
				{
				return findLastNotOf( other, position, cStringLength( other ) );
				}
			unsigned findLastNotOf( const charT *other, unsigned position, unsigned n ) const
				{
				constReverseIterator searchStart = crend( ) - ( min( position, size( ) - 1 ) + 1 );
				for ( constReverseIterator it = searchStart;  it != crend( );  ++it )
					{
					bool isInOther = false;
					for ( unsigned i = 0;  i != n;  ++i )
						if ( *it == other[ i ] )
							{
							isInOther = true;
							break;
							}
					if ( !isInOther )
						return size( ) - 1 - ( it - crbegin( ) );
					}
				return npos;
				}
			unsigned findLastNotOf( charT c, unsigned position = npos ) const
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

		// CTRLF Non-member Functions
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

	template < class Key >
	struct hash;

	template < > struct hash < dex::string >
		{
		private:
			static const unsigned long prime = 16777619;
			static const unsigned long offsetBasis = 2166136261;
		public:
			unsigned long operator( )( dex::string str )
				{
				// Compute hash using FNV-1a algorithm
				unsigned long hash = offsetBasis;
				for ( unsigned index = 0;  index != str.length( );  ++index )
					hash = ( hash ^ str[ index ] ) * prime;

				// Constrain our hash to 32 bits
				hash &= 0xFFFFFFFF;

				return hash;
				}
		};
	}

#endif
