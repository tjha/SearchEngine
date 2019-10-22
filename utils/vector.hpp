// vector.hpp
// Vector class. We don't implement emplace.
//
// 2019-10-21 - Implement iterators, clean up style, size, empty, reserve, capacity, at, front, back, erase, insert,
//              swap, maxSize, grow, pushBack, assign, data, and resize: combsc, jasina, lougheem, medhak
// 2019-10-20 - Jonas Hirshland merged AG submission and this file
// 2019-09-29 - Jonas Hirshland renamed and did some error bounds
// 2019-09-17 - Tejas Jha - created constructors and destructor
//                        - Added comment descriptions for private variables
//                        - modified ints to unsigned int
// 2019-09-17 - Jonas Hirshland implemented insert, clear, grow, size
// 2019-09-14 - Tejas Jha created basic skeleton with private members

#ifndef DEX_VECTOR
#define DEX_VECTOR

#include <cstddef>
#include <iostream>
#include "algorithm.hpp"
#include "exception.hpp"

namespace dex
	{
	template < class T >
	class vector
		{
		// TODO: Change snake_case to camelCase
		public:
			vector( );
			vector( size_t num_elements );
			vector( size_t num_elements, const T &val );
			vector( const vector < T > &other );
			vector( vector < T > &&other );
			~vector( );
			vector< T > operator=( const vector < T > &v );
			vector< T > operator=( vector < T > &&v );

			// Iterators
			class iterator;
			class constIterator;
			class reverseIterator;
			class constReverseIterator;

			iterator begin( );
			iterator end( );
			constIterator cbegin( ) const;
			constIterator cend( ) const;
			reverseIterator rbegin( );
			reverseIterator rend( );
			constReverseIterator crbegin( ) const;
			constReverseIterator crend( ) const;

			void swap( iterator &a, iterator &b );
			void swap( constIterator &a, constIterator &b );
			void swap( reverseIterator &a, reverseIterator &b );
			void swap( constReverseIterator &a, constReverseIterator &b );

			// Capacity
			size_t size( ) const;
			size_t max_size( ) const;
			void resize( size_t newVectorSize );
			void resize( size_t newVectorSize, const T &value );
			size_t capacity( ) const;
			bool empty( ) const;
			void reserve( size_t new_cap );
			void shrink_to_fit( );

			// Element access
			T& operator []( size_t index );
			const T& operator[ ] ( size_t i ) const;
			T at( size_t index );
			const T at( size_t index ) const;
			T front( );
			T back( );
			T* data( );
			const T* data( ) const;

			// Modifiers
			template < class InputIterator >
			void assign( InputIterator first, InputIterator last );
			void assign( size_t newVectorSize, const T &value );
			void push_back( const T &obj );
			void pop_back( );
			void insert( iterator index, T obj);
			void remove( size_t index );
			void erase( iterator index );
			void swap( vector v );
			void clear( );

			// Home grown
			void push_front( const T &obj );
			void pop_front( );

		private:
			// TODO: rename private variables
			// dynamic array
			T *arr;

			// amount of available space
			size_t cap;

			// number of elemets added to array
			size_t sz;

			void grow( );
		};

		template < class T >
		// default constructor
		vector < T >::vector( )
			{
			arr = new T[ 1 ]( );
			cap = 1;
			sz = 0;
			}

	template < class T >
	// constructor for vector of num elements
	vector< T >::vector( size_t num_elements )
		{
		arr = new T[ num_elements ]( );
		cap = num_elements;
		sz = num_elements;
		}

	template < class T >
	// constructor for vector of num elements each initialized to val
	vector < T >::vector( size_t num_elements, const T &val )
		{
		arr = new T[ num_elements ];
		cap = num_elements;
		sz = num_elements;
		dex::fill( begin( ), end( ), val );
		}

	// Copy Contructor
	template < class T >
	vector< T >::vector( const vector < T > &other )
		{
		arr = new T[ other.cap ];
		cap = other.cap;
		sz = other.sz;
		dex::copy( other.cbegin( ), other.cend( ), begin( ) );
		}

	// Move Constructor
	template < class T >
	vector< T >::vector( vector < T > &&other )
		{
		cap = other.cap;
		sz = other.sz;
		arr = other.arr;
		other.sz = 0;
		other.cap = 0;
		other.arr = nullptr;
		}

	// Assignment Operator
	template < class T >
	vector< T > vector< T >::operator=( const vector < T > &other )
		{
		vector < T > otherCopy( other );
		swap( otherCopy );
		return *this;
		}

	// Move Assignment Operator
	template < class T >
	vector < T > vector < T >::operator=( vector < T > &&other )
		{
		swap( other );
		return *this;
		}

	template < class T >
	// destructor
	vector < T >::~vector( )
		{
		if ( arr )
			delete[ ] arr;
		}

	template < class T >
	void vector < T >::insert( vector < T >::iterator index, T obj )
		{

		if ( size( ) == capacity( ) )
			{
			int location = index - begin( );
			grow( );
			index = begin( ) + location;
			}

		++sz;
		dex::copyBackward( index, end( ) - 1, end( ) );
		*index = obj;
		}


	template < class T >
	bool vector< T >::empty() const
		{
		return size( ) == 0;
		}

	template < class T >
	void vector< T >::reserve( size_t newCap )
		{
		if ( capacity( ) >= newCap )
			return;

		T *newArray = new T[ newCap ];
		dex::copy( begin( ), end( ), newArray );

		cap = newCap;
		delete arr;
		arr = newArray;
		}


	template < class T >
	void vector < T >::clear( )
		{
		sz = 0;
		}

	template < class T >
	void vector < T >::grow( )
		{
		reserve( dex::max( size_t( 1 ), cap << 1 ) );
		}

	template < class T >
	void vector < T >::remove( size_t index )
		{
		dex::copy( cbegin( ) + index + 1, cend( ), begin( ) + index );
		--sz;
		}

	template < class T >
	void vector < T >::erase( vector< T >::iterator index )
		{
			dex::copy( index + 1, end( ), index );
			--sz;
		}

	template < class T >
	template < class InputIterator >
	void vector < T >::assign( InputIterator first, InputIterator last )
		{
		vector temporaryVector( first, last );
		dex::swap( temporaryVector );
		}
	template < class T >
	void vector < T >::assign( size_t newVectorSize, const T &value )
		{
		vector temporaryVector( newVectorSize, value );
		dex::swap( temporaryVector );
		}

	template < class T >
	void vector < T >::push_back( const T &obj )
		{
		if ( size( ) == capacity( ) )
			grow( );
		arr[ sz++ ] = obj;
		}

	template < class T >
	void vector<T>::pop_back( )
		{
		--sz;
		}

	template < class T >
	typename vector < T >::iterator vector < T >::begin( )
		{
		return vector < T >::iterator( *this, 0 );
		}

	template < class T >
	typename vector < T >::iterator vector < T >::end( )
		{
		return vector < T >::iterator( *this, size( ) );
		}

	template < class T >
	typename vector < T >::constIterator vector < T >::cbegin( ) const
		{
		return vector < T >::constIterator( *this, 0 );
		}

	template < class T >
	typename vector < T >::constIterator vector < T >::cend( ) const
		{
		return vector < T >::constIterator( *this, size( ) );
		}

	template < class T >
	typename vector < T >::reverseIterator vector < T >::rbegin( )
		{
		return vector < T >::reverseIterator( *this, 0 );
		}

	template < class T >
	typename vector < T >::reverseIterator vector < T >::rend( )
		{
		return vector < T >::reverseIterator( *this, size( ) );
		}

	template < class T >
	typename vector < T >::constReverseIterator vector < T >::crbegin( ) const
		{
		return vector < T >::constReverseIterator( *this, 0 );
		}

	template < class T >
	typename vector < T >::constReverseIterator vector < T >::crend( ) const
		{
		return vector < T >::constReverseIterator( *this, size( ) );
		}

	template < class T >
	void vector < T >::swap( iterator &a, iterator &b )
		{
		dex::swap( a, b );
		}

	template < class T >
	void vector < T >::swap( constIterator &a, constIterator &b )
		{
		dex::swap( a, b );
		}

	template < class T >
	void vector < T >::swap( reverseIterator &a, reverseIterator &b )
		{
		dex::swap( a, b );
		}

	template < class T >
	void vector < T >::swap( constReverseIterator &a, constReverseIterator &b )
		{
		dex::swap( a, b );
		}

	template < class T >
	size_t vector < T >::capacity( ) const
		{
		return cap;
		}

	template < class T >
	size_t vector < T >::size( ) const
		{
		return sz;
		}

	template < class T >
	size_t vector < T >::max_size( ) const
		{
		return size_t( -1 );
		}

	template < class T >
	void vector< T >::resize( size_t newVectorSize )
		{
		resize( newVectorSize, T( ) );
		}
	template < class T >
	void vector< T >::resize( size_t newVectorSize, const T &val )
		{
		if ( newVectorSize > capacity( ) )
			reserve( newVectorSize );

		if ( newVectorSize > size( ) )
			dex::fill( arr + size( ), arr + newVectorSize, val );
		sz = newVectorSize;
		}

	template < class T >
	T vector < T >::at( size_t index )
		{
		if( index >= sz )
			throw dex::outOfRangeException( );
		return arr[ index ];
		}

	template < class T >
	const T vector < T >::at( size_t index ) const
		{
		if( index >= sz )
			throw dex::outOfRangeException( );
		return arr[ index ];
		}

	template < class T >
	void vector<T>::push_front( const T& obj )
		{
		insert( 0, obj );
		}

	template < class T >
	void vector< T >::pop_front( )
		{
		remove( 0 );
		}

	template < class T >
	void vector< T >::shrink_to_fit( )
		{
		if ( size( ) == capacity( ) )
			return;

		T *old = arr;
		arr = new T[ size( ) ];
		dex::copy( old, old + size( ), arr );
		delete[ ] old;
		cap = sz;
		}


	template < class T >
	void vector< T >::swap( vector other )
		{
		dex::swap( arr, other.arr );
		dex::swap( sz, other.sz );
		dex::swap( cap, other.cap );
		}

	template < class T >
	T &vector < T >::operator [ ]( size_t index )
		{
		return arr[ index ];
		}

	template < class T >
	T vector < T >::front( )
		{
		return arr[ 0 ];
		}

	template < class T >
	T vector < T >::back( )
		{
		return arr[ size( ) - 1 ];
		}

	template < class T >
	T *vector < T >::data( )
		{
		return arr;
		}

	template < class T >
	const T *vector< T >::data( ) const
		{
		return arr;
		}

	template < class T >
	const T &vector < T >::operator [ ]( size_t index ) const
		{
		return arr[ index ];
		}

	template < class T >
	class vector < T >::iterator
		{
		private:
			friend class vector < T >;
			friend class vector < T >::constIterator;
			vector < T > *vec;
			size_t position;
			iterator( vector < T > &vec, size_t position ) :
					vec( &vec ), position( position ) { }
		public:
			friend bool operator==( const iterator &a, const iterator &b )
				{
				// Only makes sense to compare iterators pointing to the same vector
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position == b.position;
				}
			friend bool operator!=( const iterator &a, const iterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position != b.position;
				}

			T &operator*( ) const
				{
				return ( *vec )[ position ];
				}
			T *operator->( ) const
				{
				return &( ( *vec )[ position ] );
				}

			iterator &operator++( )
				{
				if ( position < vec->size( ) )
					++position;
				else
					throw outOfRangeException( );
				return *this;
				}
			iterator operator++( int )
				{
				iterator toReturn( *this );
				if ( position < vec->size( ) )
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
				if ( it.position > n + it.vec->size( ) || ( n > 0 && it.position < size_t( n ) ) )
					throw outOfRangeException( );
				return iterator( *it.vec, it.position - n );
				}
			friend int operator-( const iterator &a, const iterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return int( a.position - b.position );
				}
			friend iterator operator+( const iterator &it, int n )
				{
				if ( it.position + n > it.vec->size( ) || ( n < 0 && it.position < size_t( -n ) ) )
					throw outOfRangeException( );
				return iterator( *it.vec, it.position + n );
				}

			friend bool operator<( const iterator &a, const iterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position < b.position;
				}
			friend bool operator>( const iterator &a, const iterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position > b.position;
				}
			friend bool operator<=( const iterator &a, const iterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position <= b.position;
				}
			friend bool operator>=( const iterator &a, const iterator &b )
				{
				if ( a.vec != b.vec )
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

			T &operator[ ]( const size_t index ) const
				{
				return ( *vec )[ index ];
				}
		};

	template < class T >
	class vector < T >::constIterator
		{
		private:
			friend class vector < T >;
			const vector < T > *vec;
			size_t position;
			constIterator( const vector < T > &vec, size_t position ) :
					vec( &vec ), position( position ) { }
		public:
			constIterator( const iterator &it ) : vec( it.vec ), position( it.position ) { }
			friend bool operator==( const constIterator &a, const constIterator &b )
				{
				// Only makes sense to compare iterators pointing to the same vec
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position == b.position;
				}
			friend bool operator!=( const constIterator &a, const constIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position != b.position;
				}

			const T &operator*( ) const
				{
				return ( *vec )[ position ];
				}
			const T *operator->( ) const
				{
				return &( ( *vec )[ position ] );
				}

			constIterator &operator++( )
				{
				if ( position < vec->size( ) )
					++position;
				else
					throw outOfRangeException( );
				return *this;
				}
			constIterator operator++( int )
				{
				constIterator toReturn( *this );
				if ( position < vec->size( ) )
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
				if ( it.position > n + it.vec->size( ) || ( n > 0 && it.position < size_t( n ) ) )
					throw outOfRangeException( );
				return constIterator( *it.vec, it.position - n );
				}
			friend int operator-( const constIterator &a, const constIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return int( a.position - b.position );
				}
			friend constIterator operator+( const constIterator &it, int n )
				{
				if ( it.position + n > it.vec->size( ) || ( n < 0 && it.position < size_t( -n ) ) )
					throw outOfRangeException( );
				return constIterator( *it.vec, it.position + n );
				}

			friend bool operator<( const constIterator &a, const constIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position < b.position;
				}
			friend bool operator>( const constIterator &a, const constIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position > b.position;
				}
			friend bool operator<=( const constIterator &a, const constIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position <= b.position;
				}
			friend bool operator>=( const constIterator &a, const constIterator &b )
				{
				if ( a.vec != b.vec )
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

			const T &operator[ ]( const size_t index ) const
				{
				return ( *vec )[ index ];
				}
		};

	template < class T >
	class vector < T >::reverseIterator
		{
		private:
			friend class vector < T >;
			friend class vector < T >::constReverseIterator;
			size_t position;
			vector < T > *vec;
			reverseIterator( vector < T > &vec, size_t position ) :
					vec( &vec ), position( position ) { }
		public:
			friend bool operator==( const reverseIterator &a, const reverseIterator &b )
				{
				// Only makes sense to compare iterators pointing to the same vec
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position == b.position;
				}
			friend bool operator!=( const reverseIterator &a, const reverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position != b.position;
				}

			T &operator*( ) const
				{
				return ( *vec )[ vec->size( ) - position - 1 ];
				}
			T *operator->( ) const
				{
				return &( ( *vec )[ vec->size( ) - position - 1 ] );
				}

			reverseIterator &operator++( )
				{
				if ( position < vec->size( ) )
					++position;
				else
					throw outOfRangeException( );
				return *this;
				}
			reverseIterator operator++( int )
				{
				reverseIterator toReturn( *this );
				if ( position < vec->size( ) )
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
				if ( it.position > n + it.vec->size( ) || ( n > 0 && it.position < size_t( n ) ) )
					throw outOfRangeException( );
				return reverseIterator( *it.vec, it.position - n );
				}
			friend int operator-( const reverseIterator &a, const reverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return int( a.position - b.position );
				}
			friend reverseIterator operator+( const reverseIterator &it, int n )
				{
				if ( it.position + n > it.vec->size( ) || ( n < 0 && it.position < size_t( -n ) ) )
					throw outOfRangeException( );
				return reverseIterator( *it.vec, it.position + n );
				}

			friend bool operator<( const reverseIterator &a, const reverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position < b.position;
				}
			friend bool operator>( const reverseIterator &a, const reverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position > b.position;
				}
			friend bool operator<=( const reverseIterator &a, const reverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position <= b.position;
				}
			friend bool operator>=( const reverseIterator &a, const reverseIterator &b )
				{
				if ( a.vec != b.vec )
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

			T &operator[ ]( const size_t index ) const
				{
				return ( *vec )[ index ];
				}
		};

	template < class T >
	class vector < T >::constReverseIterator
		{
		private:
			friend class vector < T >;
			const vector < T > *vec;
			size_t position;
			constReverseIterator( const vector < T > &vec, size_t position ) :
					vec( &vec ), position( position ) { }
		public:
			constReverseIterator( const reverseIterator &it ) : vec( it.vec ), position( it.position ) { }
			friend bool operator==( const constReverseIterator &a, const constReverseIterator &b )
				{
				// Only makes sense to compare iterators pointing to the same vec
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position == b.position;
				}
			friend bool operator!=( const constReverseIterator &a, const constReverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position != b.position;
				}

			const T &operator*( ) const
				{
				return ( *vec )[ vec->size( ) - position - 1 ];
				}
			const T *operator->( ) const
				{
				return &( ( *vec )[ vec->size( ) - position - 1 ] );
				}

			constReverseIterator &operator++( )
				{
				if ( position < vec->size( ) )
					++position;
				else
					throw outOfRangeException( );
				return *this;
				}
			constReverseIterator operator++( int )
				{
				constReverseIterator toReturn( *this );
				if ( position < vec->size( ) )
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
				if ( it.position > n + it.vec->size( ) || ( n > 0 && it.position < size_t( n ) ) )
					throw outOfRangeException( );
				return constReverseIterator( *it.vec, it.position - n );
				}
			friend int operator-( const constReverseIterator &a, const constReverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return int( a.position - b.position );
				}
			friend constReverseIterator operator+( const constReverseIterator &it, int n )
				{
				if ( it.position + n > it.vec->size( ) || ( n < 0 && it.position < size_t( -n ) ) )
					throw outOfRangeException( );
				return constReverseIterator( *it.vec, it.position + n );
				}

			friend bool operator<( const constReverseIterator &a, const constReverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position < b.position;
				}
			friend bool operator>( const constReverseIterator &a, const constReverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position > b.position;
				}
			friend bool operator<=( const constReverseIterator &a, const constReverseIterator &b )
				{
				if ( a.vec != b.vec )
					throw invalidArgumentException( );
				return a.position <= b.position;
				}
			friend bool operator>=( const constReverseIterator &a, const constReverseIterator &b )
				{
				if ( a.vec != b.vec )
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

			const T &operator[ ]( const size_t index ) const
				{
				return ( *vec )[ index ];
				}
		};
	}

#endif
