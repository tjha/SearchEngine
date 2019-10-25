// vector.hpp
// Vector class. We don't implement emplace.
//
// 2019-10-24 - Added operators <, <=, >, >=, !=, == medha, jhirshey, loghead,
// jasina
// 2019-10-22 - Fix styling issues, add iterator-based constructor, and fix at, front, and back: jasina
//            - overloaded insert and erase, added more options: combsc
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
#include <type_traits>
#include "algorithm.hpp"
#include "exception.hpp"

namespace dex
    {
    template < class T >
    class vector
        {
        public:
            vector( );
            vector( size_t numElements );
            vector( size_t numElements, const T &val );
            // Only use the next constructor if InputIterator isn't integral type
            template < class InputIterator,
                  typename = typename std::enable_if < !std::is_integral< InputIterator >::value >::type >
            vector( InputIterator first, InputIterator last )
                {
                array = new T[ 1 ];
                arraySize = 1;
                vectorSize = 0;
                for ( ;  first != last;  ++first )
                    {
                    if ( size( ) == capacity( ) )
                        grow( );
                    array[ vectorSize++ ] = *first;
                    }
                }
            vector( const vector < T > &other );
            vector( vector < T > &&other );
            ~vector( );
            vector < T > operator=( const vector < T > &v );
            vector < T > operator=( vector < T > &&v );
            bool operator>( const vector < T > &v );
            bool operator>=( const vector < T > &v );
            bool operator<( const vector < T > &v );
            bool operator<=( const vector < T > &v );
            bool operator!=( const vector < T > &v );
            bool operator==( const vector < T > &v );
            

            // Iterators
            class iterator;
         class constIterator
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
            size_t maxSize( ) const;
            void resize( size_t newVectorSize );
            void resize( size_t newVectorSize, const T &value );
            size_t capacity( ) const;
            bool empty( ) const;
            void reserve( size_t newCapacity );
            void shrinkToFit( );

            // Element access
            T &operator []( size_t index );
            const T &operator[ ] ( size_t i ) const;
            T &at( size_t index );
            const T &at( size_t index ) const;
            T &front( );
            const T &front( ) const;
            T &back( );
            const T &back( ) const;
            T *data( );
            const T *data( ) const;

            // Modifiers
            template < class InputIterator >
            void assign( InputIterator first, InputIterator last );
            void assign( size_t newVectorSize, const T &value );
            void pushBack( const T &obj );
            void pushBack( T &&obj );
            void popBack( );
            void insert( constIterator index, const T &obj );
            void insert( constIterator index, size_t count, const T &obj );
            template < class InputIterator,
                  typename = typename std::enable_if < !std::is_integral< InputIterator >::value >::type >
            void insert( constIterator index, InputIterator first, InputIterator last )
                {
            size_t count = 0;
            InputIterator tmp = first;
            while ( tmp != last )
               {
               ++tmp;
               ++count;
               }

                if ( size( ) + count > capacity ( ) )
                    {
                    int location = index - cbegin( );
                    if ( size( ) + count > capacity( ) * 2 )
                        reserve( size ( ) + count );
                    else
                        grow( );
                    index = cbegin( ) + location;
                    }

                vectorSize += count;
                size_t location = index - cbegin( );
                iterator writableLocation = begin( ) + location;
                dex::copyBackward( writableLocation, end( ) - count, end( ) );
                dex::copy( first, last, writableLocation );
                }
            void erase( size_t index );
            void erase( constIterator index );
            void erase( constIterator first, constIterator last );
            void swap( vector v );
            void clear( );

            // Home grown
            void pushFront( const T &obj );
            void popFront( );

        private:
            // dynamic array
            T *array;

            // amount of available space
            size_t arraySize;

            // number of elemets added to array
            size_t vectorSize;

            void grow( );
        };

    template < class T >
    // default constructor
    vector < T >::vector( )
        {
        array = new T[ 1 ]( );
        arraySize = 1;
        vectorSize = 0;
        }

    template < class T >
    // constructor for vector of num elements
    vector < T >::vector( size_t numElements )
        {
        array = new T[ numElements ]( );
        arraySize = numElements;
        vectorSize = numElements;
        }

    template < class T >
    // constructor for vector of num elements each initialized to val
    vector < T >::vector( size_t numElements, const T &val )
        {
        array = new T[ numElements ];
        arraySize = numElements;
        vectorSize = numElements;
        dex::fill( begin( ), end( ), val );
        }

    // Copy Contructor
    template < class T >
    vector < T >::vector( const vector < T > &other )
        {
        array = new T[ other.arraySize ];
        arraySize = other.arraySize;
        vectorSize = other.vectorSize;
        dex::copy( other.cbegin( ), other.cend( ), begin( ) );
        }

    // Move Constructor
    template < class T >
    vector < T >::vector( vector < T > &&other )
        {
        arraySize = other.arraySize;
        vectorSize = other.vectorSize;
        array = other.array;
        other.vectorSize = 0;
        other.arraySize = 0;
        other.array = nullptr;
        }

    // Assignment Operator
    template < class T >
    vector < T > vector < T >::operator=( const vector < T > &other )
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
        if ( array )
            delete[ ] array;
        }

    template < class T >
    void vector < T >::insert( vector< T >::constIterator index, const T &obj )
        {
        insert( index, 1, obj );
        }

    template < class T >
    void vector < T >::insert( vector< T >::constIterator index, size_t count, const T &obj )
        {
        if ( size( ) + count > capacity ( ) )
            {
            int location = index - cbegin( );
            if ( size( ) + count > capacity( ) * 2 )
                reserve( size ( ) + count );
            else
                grow( );
            index = cbegin( ) + location;
            }
        
        vectorSize += count;
        int location = index - cbegin( );
        iterator writeLocation = begin( ) + location;
        dex::copyBackward( writeLocation, end( ) - count, end( ) );
        dex::fill( writeLocation, writeLocation + count, obj);
        }


    template < class T >
    bool vector < T >::empty( ) const
        {
        return size( ) == 0;
        }

    template < class T >
    void vector < T >::reserve( size_t newCapacity )
        {
        if ( capacity( ) >= newCapacity )
            return;

        T *newArray = new T[ newCapacity ];
        dex::copy( begin( ), end( ), newArray );

        arraySize = newCapacity;
        delete array;
        array = newArray;
        }


    template < class T >
    void vector < T >::clear( )
        {
        vectorSize = 0;
        }

    template < class T >
    void vector < T >::grow( )
        {
        reserve( dex::max( size_t( 1 ), arraySize << 1 ) );
        }

    template < class T >
    void vector < T >::erase( size_t index )
        {
        dex::copy( cbegin( ) + index + 1, cend( ), begin( ) + index );
        --vectorSize;
        }

    template < class T >
    void vector < T >::erase( vector < T >::constIterator index )
        {
        erase( index, index + 1 );
        }
    
    template < class T >
    void vector < T >::erase( vector < T >::constIterator first, vector < T > ::constIterator last )
        {
        size_t count = last - first;
        size_t location = first - cbegin( );
        iterator i = begin( ) + location;
        dex::copy( i + count, end( ), i );
        vectorSize -= count;
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
    void vector < T >::pushBack( const T &obj )
        {
        if ( size( ) == capacity( ) )
            grow( );
        array[ vectorSize++ ] = obj;
        }

    template < class T >
    void vector < T >::pushBack( T &&obj )
        {
        if ( size( ) == capacity( ) )
            grow( );
        dex::swap( array[ vectorSize++ ], obj );
        }

    template < class T >
    void vector < T >::popBack( )
        {
        --vectorSize;
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
        return arraySize;
        }

    template < class T >
    size_t vector < T >::size( ) const
        {
        return vectorSize;
        }

    template < class T >
    size_t vector < T >::maxSize( ) const
        {
        return size_t( -1 );
        }

    template < class T >
    void vector < T >::resize( size_t newVectorSize )
        {
        resize( newVectorSize, T( ) );
        }
    template < class T >
    void vector < T >::resize( size_t newVectorSize, const T &val )
        {
        if ( newVectorSize > capacity( ) )
            reserve( newVectorSize );

        if ( newVectorSize > size( ) )
            dex::fill( array + size( ), array + newVectorSize, val );
        vectorSize = newVectorSize;
        }

    template < class T >
    T &vector < T >::at( size_t index )
        {
        if( index >= vectorSize )
            throw dex::outOfRangeException( );
        return array[ index ];
        }

    template < class T >
    const T &vector < T >::at( size_t index ) const
        {
        if( index >= vectorSize )
            throw dex::outOfRangeException( );
        return array[ index ];
        }

    template < class T >
    void vector < T >::pushFront( const T &obj )
        {
        insert( 0, obj );
        }

    template < class T >
    void vector < T >::popFront( )
        {
        remove( 0 );
        }

    template < class T >
    void vector < T >::shrinkToFit( )
        {
        if ( size( ) == capacity( ) )
            return;

        T *old = array;
        array = new T[ size( ) ];
        dex::copy( old, old + size( ), array );
        delete[ ] old;
        arraySize = vectorSize;
        }


    template < class T >
    void vector < T >::swap( vector other )
        {
        dex::swap( array, other.array );
        dex::swap( vectorSize, other.vectorSize );
        dex::swap( arraySize, other.arraySize );
        }

    template < class T >
    T &vector < T >::operator [ ]( size_t index )
        {
        return array[ index ];
        }

    template < class T >
    T &vector < T >::front( )
        {
        return array[ 0 ];
        }
    template < class T >
    const T &vector < T >::front( ) const
        {
        return array[ 0 ];
        }

    template < class T >
    T &vector < T >::back( )
        {
        return array[ size( ) - 1 ];
        }
    template < class T >
    const T &vector < T >::back( ) const
        {
        return array[ size( ) - 1 ];
        }

    template < class T >
    T *vector < T >::data( )
        {
        return array;
        }

    template < class T >
    const T *vector < T >::data( ) const
        {
        return array;
        }

    template < class T >
    const T &vector < T >::operator [ ]( size_t index ) const
        {
        return array[ index ];
        }
    
    template < class T >
    bool vector < T >::operator>( const vector < T > &v )
        {
        int val = dex::lexicographicalCompare( begin( ), end( ), v.begin( ), v.end( ) );
        return val == 1 ? true : false;
        }
    
    template < class T >
    bool vector < T >::operator>=( const vector < T > &v )
        {
        int val = dex::lexicographicalCompare( begin( ), end( ), v.begin( ), v.end( ) );
        return ( (val == 1)|( val == 0 ) ) ? true : false;
        }
    
    template < class T >
    bool vector < T >::operator<( const vector < T > &v )
        {
        int val = dex::lexicographicalCompare( begin( ), end( ), v.begin( ), v.end( ) );
        return val == -1 ? true : false;
        }
    
    template < class T >
    bool vector < T >::operator<=( const vector < T > &v )
        {
        int val = dex::lexicographicalCompare( begin( ), end( ), v.begin( ), v.end( ) );
        return ( (val == -1)|( val == 0 ) ) ? true : false;
        }
    
    template < class T >
    bool vector < T >::operator!=( const vector < T > &v )
        {
        int val = dex::lexicographicalCompare( begin( ), end( ), v.begin( ), v.end( ) );
        return val == 0 ? false : true;
        }
   
    template < class T >
    bool vector < T >::operator==( const vector < T > &v )
        {
        int val = dex::lexicographicalCompare( begin( ), end( ), v.begin( ), v.end( ) );
        return val == 0 ? true : false;
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
