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
// 2019-10-26: Rewrite iterators to not duplicate code: jasina
// 2019-10-24: Call lexicographicalCompare from algorithm, fix tempaltes with inputIterators: jasina
// 2019-10-22: Fix iterator-based constructor: jasina
// 2019-10-21: Added cstddef, fix iterator templating: combsc, jasina
// 2019-10-20: Add lexicographicalCompare, improved compare, change unsigned to size_t: combsc, jasina
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

#include <cstddef>
#include <iostream>
#include <type_traits>
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
			size_t arraySize;
			size_t stringSize;

			// Find the length of a null-terminated C-string (not including the null-terminator)
			static size_t cStringLength( const charT *cstr )
				{
				const charT *end;
				for ( end = cstr;  *end != charT { };  ++end );
				return end - cstr;
				}

		public:
			// Maximum possible size of our string
			static const size_t npos = size_t ( -1 );

			// CTRLF Constructors
			basicString( )
				{
				arraySize = 1;
				stringSize = 0;
				array = new charT[ arraySize ]( );
				}
			basicString( const basicString < charT > &other, size_t position = 0, size_t length = npos )
				{
				if ( position > other.size( ) )
					throw outOfRangeException( );

				// Make the final length be length, unless the string is too short. In that case, make it be to the end
				// of the string.
				size_t finalLength = min(length, other.size( ) - position );

				arraySize = finalLength + 1;
				array = new charT[ arraySize ];
				stringSize = finalLength;

				dex::copy( other.cbegin( ) + position, other.cbegin( ) + position + finalLength, array );
				array[ finalLength ] = charT { };
				}
			basicString( const charT* other, size_t length = npos )
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
			basicString( size_t length, charT character )
				{
				arraySize = length + 1;
				array = new charT[ arraySize ];
				stringSize = length;

				dex::fill( array, array + stringSize, character );
				array[ stringSize ] = charT { };
				}
			template < class InputIt,
					typename = typename std::enable_if < !std::is_integral< InputIt >::value >::type >
			basicString( InputIt first, InputIt last )
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
			size_t size( ) const
				{
				return stringSize;
				}

			size_t length( ) const
				{
				return stringSize;
				}

			size_t maxSize( ) const
				{
				return npos;
				}

			void resize( size_t newStringSize, charT character = { } )
				{
				if ( newStringSize > capacity( ) )
					reserve( newStringSize );

				if ( newStringSize > size( ) )
					dex::fill( array + size( ), array + newStringSize, character );
				stringSize = newStringSize;
				array[ size( ) ] = charT { };
				}

			size_t capacity( ) const
				{
				return arraySize - 1;
				}

			void reserve( size_t newArraySize )
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

		private:
			// CTRLF Iterators
			template < bool isConst, bool isForward >
			class _iterator
				{
				private:
					friend class basicString < charT >;
					typename
							std::conditional < isConst, const basicString < charT > *, basicString < charT > * >::type
							string;
					size_t position;
					_iterator(
							typename
									std::conditional < isConst, const basicString < charT > &, basicString < charT > & >::type
									string,
							size_t position ) :
							string( &string ), position( position ) { }
				public:
					template < typename = typename std::enable_if < isConst > >
					_iterator( const _iterator < false, isForward > &other ) :
							string( other.string ), position( other.position ) { }

					friend bool operator==( const _iterator &a, const _iterator &b )
						{
						// Only makes sense to compare iterators pointing to the same string
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position == b.position;
						}
					friend bool operator!=( const _iterator &a, const _iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position != b.position;
						}

					typename std::conditional < isConst, const charT &, charT & >::type operator*( ) const
						{
						if ( isForward )
							return ( *string )[ position ];
						return ( *string )[ string->size( ) - position - 1 ];
						}
					typename std::conditional < isConst, const charT *, charT * >::type operator->( ) const
						{
						if ( isForward )
							return &( ( *string )[ position ] );
						return &( ( *string )[ string->size( ) - position - 1 ] );
						}

					_iterator &operator++( )
						{
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return *this;
						}
					_iterator operator++( int )
						{
						_iterator toReturn( *this );
						if ( position < string->size( ) )
							++position;
						else
							throw outOfRangeException( );
						return toReturn;
						}

					_iterator &operator--( )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}
					_iterator operator--( int )
						{
						if ( position > 0 )
							--position;
						else
							throw outOfRangeException( );
						return *this;
						}

					friend _iterator operator-( const _iterator &it, int n )
						{
						if ( it.position > n + it.string->size( ) || ( n > 0 && it.position < size_t( n ) ) )
							throw outOfRangeException( );
						return _iterator( *it.string, it.position - n );
						}
					friend int operator-( const _iterator &a, const _iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return int( a.position - b.position );
						}
					friend _iterator operator+( const _iterator &it, int n )
						{
						if ( it.position + n > it.string->size( ) || ( n < 0 && it.position < size_t( -n ) ) )
							throw outOfRangeException( );
						return _iterator( *it.string, it.position + n );
						}

					friend bool operator<( const _iterator &a, const _iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position < b.position;
						}
					friend bool operator>( const _iterator &a, const _iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position > b.position;
						}
					friend bool operator<=( const _iterator &a, const _iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position <= b.position;
						}
					friend bool operator>=( const _iterator &a, const _iterator &b )
						{
						if ( a.string != b.string )
							throw invalidArgumentException( );
						return a.position >= b.position;
						}

					_iterator &operator+=( int n )
						{
						return *this = *this + n;
						}
					_iterator &operator-=( int n )
						{
						return *this = *this - n;
						}

					typename
							std::conditional < isConst, const charT &, charT & >::type operator[ ]( const size_t index ) const
						{
						return ( *string )[ index ];
						}
				};

			template < bool isConst, bool isForward >
			void swap( _iterator < isConst, isForward > &a, _iterator < isConst, isForward > &b )
				{
				dex::swap( a, b );
				}
		public:
			typedef _iterator < false, true > iterator;
			iterator begin( )
				{
				return iterator( *this, 0 );
				}
			iterator end( )
				{
				return iterator( *this, size( ) );
				}

			typedef _iterator < true, true > constIterator;
			constIterator cbegin( ) const
				{
				return constIterator( *this, 0 );
				}
			constIterator cend( ) const
				{
				return constIterator( *this, size( ) );
				}

			typedef _iterator < false, false > reverseIterator;
			reverseIterator rbegin( )
				{
				return reverseIterator( *this, 0 );
				}
			reverseIterator rend( )
				{
				return reverseIterator( *this, size( ) );
				}
			
			typedef _iterator < true, false > constReverseIterator;
			constReverseIterator crbegin( ) const
				{
				return constReverseIterator( *this, 0 );
				}
			constReverseIterator crend( ) const
				{
				return constReverseIterator( *this, size( ) );
				}

			// CTRLF Element Access
			const charT &operator[ ]( size_t position ) const
				{
				return array[ position ];
				}
			charT &operator[ ]( size_t position )
				{
				return array[ position ];
				}

			const charT &at( size_t position) const
				{
				if( position < 0 || position >= size( ) )
					throw outOfRangeException( );
				return array[ position ];
				}
			charT &at( size_t position )
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
			basicString < charT > &append( const basicString < charT > &other, size_t position, size_t length )
				{
				return append( other.cbegin( ) + position, other.cbegin( ) + position + length );
				}
			basicString < charT > &append( const charT *other )
				{
				// Technically less efficient, but is more clear and avoids code duplication.
				return append( other, other + cStringLength( other ) );
				}
			basicString < charT > &append( const charT *other, size_t length )
				{
				return append( other, other + length );
				}
			basicString < charT > &append( size_t number, charT character )
				{
				resize( size( ) + number, character );
				return *this;
				}
			template < class InputIt,
					typename = typename std::enable_if < !std::is_integral< InputIt >::value >::type >
			basicString < charT > &append( InputIt first, InputIt last )
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
			basicString < charT > &assign( const basicString < charT > &other, size_t position, size_t length )
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
			basicString < charT > &assign( const charT *other, size_t length )
				{
				basicString temporaryString( other, length );
				swap( temporaryString );
				return *this;
				}
			basicString < charT > &assign( size_t length, charT character )
				{
				basicString temporaryString( length, character );
				swap( temporaryString );
				return *this;
				}
			template < class InputIt,
					typename = typename std::enable_if < !std::is_integral< InputIt >::value >::type >
			basicString < charT > &assign( InputIt first, InputIt last )
				{
				basicString temporaryString( first, last );
				swap( temporaryString );
				return *this;
				}

			basicString < charT > &insert( size_t position, const basicString < charT > &other )
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				insert( cbegin( ) + position, other.cbegin( ), other.cend( ) );
				return *this;
				}
			basicString < charT > &insert( size_t position, const basicString < charT > &other,
					size_t subposition, size_t sublength )
				{
				if ( position > size( ) || subposition > other.size( ) )
					throw dex::outOfRangeException( );

				insert( cbegin( ) + position, other.cbegin( ) + subposition,
						other.cbegin( ) + subposition + sublength );
				return *this;
				}
			basicString < charT > &insert( size_t position, const charT *other )
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				// Technically less efficient, but is more clear and avoids code duplication.
				size_t length = cStringLength( other );
				insert( cbegin( ) + position, other, other + length );
				return *this;
				}
			basicString < charT > &insert( size_t position, const charT *other, size_t length )
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				// Caution: we do not check if other is of length at least length
				insert( cbegin( ) + position, other, other + length );
				return *this;
				}
			basicString < charT > &insert( size_t position, size_t length, charT character )
				{
				if ( position > size( ) )
					throw dex::outOfRangeException( );

				insert( cbegin( ) + position, length, character );
				return *this;
				}
			iterator insert( constIterator first, size_t length, charT character )
				{
				size_t originalSize = size( );
				replace( first, first, length, character );
				return begin( ) + ( ( first - cbegin( ) ) + size( ) - originalSize );
				}
			iterator insert( constIterator first, charT character )
				{
				return insert( first, 1, character );
				}
			template < class InputIt,
					typename = typename std::enable_if < !std::is_integral< InputIt >::value >::type >
			iterator insert( constIterator insertionPoint, InputIt first, InputIt last )
				{
				size_t originalSize = size( );
				replace( insertionPoint, insertionPoint, first, last );
				return begin( ) + ( ( insertionPoint - cbegin( ) ) + size( ) - originalSize );
				}

			basicString < charT > &erase( size_t position = 0, size_t length = npos )
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

			basicString < charT > &replace( size_t position, size_t length, const basicString < charT > &other )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, other );
				}
			basicString < charT > &replace( constIterator first, constIterator last, const basicString < charT > &other )
				{
				return replace( first, last, other.cbegin( ), other.cend( ) );
				}
			basicString < charT > &replace( size_t position, size_t length,
					const basicString < charT > &other, size_t subposition, size_t sublength )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length,
						other.cbegin( ) + subposition, other.cbegin( ) + subposition + sublength );
				}
			basicString < charT > &replace( size_t position, size_t length, const charT *other )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, other );
				}
			basicString < charT > &replace( constIterator first, constIterator last, const charT *other )
				{
				return replace( first, last, other, other + cStringLength( other ) );
				}
			basicString < charT > &replace( size_t position, size_t length, const charT *other, size_t n )
				{
				replace( cbegin( ) + position, cbegin( ) + ( position + length ), other, other + n );
				return *this;
				}
			basicString < charT > &replace( constIterator first, constIterator last, const charT *other, size_t n )
				{
				replace( first, last, other, n );
				return *this;
				}
			basicString < charT > &replace( size_t position, size_t length, size_t n, charT c )
				{
				return replace( cbegin( ) + position, cbegin( ) + position + length, n, c );
				}
		private:
			void shiftAtPoint( size_t insertionLength, constIterator first, constIterator last )
				{
				size_t removalLength = last - first;
				size_t newStringLength = size( ) + insertionLength - removalLength;
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
			basicString < charT > &replace( constIterator first, constIterator last, size_t n, charT c )
				{
				shiftAtPoint( n, first, last );

				iterator writableFirst = begin( ) + ( first - cbegin( ) );
				dex::fill( writableFirst, writableFirst + n, c );

				return *this;
				}
			template < class InputIt,
					typename = typename std::enable_if < !std::is_integral< InputIt >::value >::type >
			basicString < charT > &replace( constIterator first, constIterator last,
					InputIt inputFirst, InputIt inputLast )
				{
				size_t insertionLength = 0;
				for ( InputIt it = inputFirst;  it != inputLast;  ++insertionLength, ++it );
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
			size_t copy( charT *characterArray, size_t count, size_t position = 0 ) const
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

			size_t find( const basicString &other, size_t position = 0 ) const
				{
				return find( other.cStr( ), position, other.size( ) );
				}
			size_t find( const charT *other, size_t position = 0 ) const
				{
				// Inefficient, but easy to understand
				return find( other, position, cStringLength( other ) );
				}
			size_t find( const charT *other, size_t position, size_t n ) const
				{
				constIterator location = dex::search( cbegin( ) + position, cend( ), other, other + n );
				if ( location == cend( ) )
					return npos;
				return location - cbegin( );
				}
			size_t find( charT c, size_t position = 0 ) const
				{
				constIterator location = dex::find( cbegin( ) + position, cend( ), c );
				if ( location == cend( ) )
					return npos;
				return location - cbegin( );
				}

			size_t rfind( const basicString &other, size_t position = npos ) const
				{
				return rfind( other.cStr( ), position, other.size( ) );
				}
			size_t rfind( const charT *other, size_t position = npos ) const
				{
				return rfind( other, position, cStringLength( other ) );
				}
			size_t rfind( const charT *other, size_t position, size_t n ) const
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
			size_t rfind( charT c, size_t position = npos ) const
				{
				constReverseIterator searchStart = crend( ) - ( min( position, size( ) - 1 ) + 1 );
				constReverseIterator location = dex::find( searchStart, crend( ), c );
				if ( location == crend( ) )
					return npos;
				return size( ) - 1 - (location - crbegin( ));
				}

			size_t findFirstOf( const basicString &other, size_t position = 0 ) const
				{
				return findFirstOf( other.cStr( ), position, other.size( ) );
				}
			size_t findFirstOf( const charT *other, size_t position = 0 ) const
				{
				return findFirstOf( other, position, cStringLength( other ) );
				}
			// This seems like an inefficient implementation... if we had an unordered map we would
			// speed this up a lot
			size_t findFirstOf( const charT *other, size_t position, size_t n ) const
				{
				for ( constIterator it = cbegin( ) + position;  it != cend( );  ++it )
					for ( size_t i = 0;  i != n;  ++i )
						if ( *it == other[ i ] )
							return it - cbegin( );
				return npos;
				}
			size_t findFirstOf( charT c, size_t position = 0 ) const
				{
				return find( c, position );
				}

			size_t findLastOf( const basicString &other, size_t position = npos ) const
				{
				return findLastOf( other.cStr( ), position, other.size( ) );
				}
			size_t findLastOf( const charT *other, size_t position = npos ) const
				{
				return findLastOf( other, position, cStringLength( other ) );
				}
			size_t findLastOf( const charT *other, size_t position, size_t n ) const
				{
				constReverseIterator searchStart = crend( ) - ( min( position, size( ) - 1 ) + 1 );
				for ( constReverseIterator it = searchStart;  it != crend( );  ++it )
					for ( size_t i = 0;  i != n;  ++i )
						if ( *it == other[ i ] )
							return size( ) - 1 - ( it - crbegin( ) );
				return npos;
				}
			size_t findLastOf( charT c, size_t position = npos ) const
				{
				return rfind( c, position );
				}

			size_t findFirstNotOf( const basicString &other, size_t position = 0 ) const
				{
				return findFirstNotOf( other.cStr( ), position, other.size( ) );
				}
			size_t findFirstNotOf( const charT *other, size_t position = 0 ) const
				{
				return findFirstNotOf( other, position, cStringLength( other ) );
				}
			size_t findFirstNotOf( const charT *other, size_t position, size_t n ) const
				{
				for ( constIterator it = cbegin( ) + position;  it != cend( );  ++it )
					{
					bool isInOther = false;
					for ( size_t i = 0;  i != n;  ++i )
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
			size_t findFirstNotOf( charT c, size_t position = 0 ) const
				{
				return findFirstNotOf( &c, position, 1 );
				}

			size_t findLastNotOf( const basicString &other, size_t position = npos ) const
				{
				return findLastNotOf( other.cStr( ), position, other.size( ) );
				}
			size_t findLastNotOf( const charT *other, size_t position = npos ) const
				{
				return findLastNotOf( other, position, cStringLength( other ) );
				}
			size_t findLastNotOf( const charT *other, size_t position, size_t n ) const
				{
				constReverseIterator searchStart = crend( ) - ( min( position, size( ) - 1 ) + 1 );
				for ( constReverseIterator it = searchStart;  it != crend( );  ++it )
					{
					bool isInOther = false;
					for ( size_t i = 0;  i != n;  ++i )
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
			size_t findLastNotOf( charT c, size_t position = npos ) const
				{
				return findLastNotOf( &c, position, 1 );
				}

			basicString < charT > substr( size_t position = 0, size_t length = npos ) const
				{
				return basicString( *this, position, length );
				}

		private:
			template < class InputIt1 >
			int lexicographicalCompare( InputIt1 first1, InputIt1 last1, const charT *first2 ) const
				{
				for ( ;  first1 != last1 && *first2 != charT { };  ++first1, ++first2 )
					{
					if ( *first1 < *first2 )
						return -1;
					if ( *first1 > *first2 )
						return 1;
					}
				return ( *first2 == charT { } ) - ( *first1 == *last1 );
				}
		public:
			int compare( const basicString &other ) const
				{
				return dex::lexicographicalCompare( cbegin( ), cend( ), other.cbegin( ), other.cend( ) );
				}
			int compare( size_t position, size_t length, const basicString &other ) const
				{
				return dex::lexicographicalCompare( cbegin( ) + position, cbegin( ) + position + length,
						other.cbegin( ), other.cend( ) );
				}
			int compare( size_t position, size_t length,
					const basicString &other, size_t subposition, size_t sublength ) const
				{
				return dex::lexicographicalCompare( cbegin( ) + position, cbegin( ) + position + length,
						other.cbegin( ) + subposition, other.cbegin( ) + subposition + sublength);
				}
			int compare( const charT *other ) const
				{
				return lexicographicalCompare( cbegin( ), cend( ), other );
				}
			int compare( size_t position, size_t length, const charT *other ) const
				{
				return lexicographicalCompare( cbegin( ) + position, cbegin( ) + position + length, other );
				}
			int compare( size_t position, size_t length, const charT *other, size_t n ) const
				{
				return dex::lexicographicalCompare( cbegin( ) + position, cbegin( ) + position + length, other, other + n );
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
				for ( size_t index = 0;  index != str.length( );  ++index )
					hash = ( hash ^ str[ index ] ) * prime;

				// Constrain our hash to 32 bits
				hash &= 0xFFFFFFFF;

				return hash;
				}
		};
	}

#endif
