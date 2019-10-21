// vector.hpp
// 
// Vector class
//
// 2019/10/20 - Jonas Hirshland merged AG submission and this file
// 2019/09/29 - Jonas Hirshland renamed and did some error bounds
// 2019/09/17 - Tejas Jha - created constructors and destructor
//                        - Added comment descriptions for private variables
//                        - modified ints to unsigned int
// 2019/09/17 - Jonas Hirshland implemented insert, clear, grow, size
// 2019/09/14 - Tejas Jha created basic skeleton with private members

namespace dex
   {
   template < class T >
   class vector
      {
      public:
      vector( );
      vector( size_t num_elements );
      vector( size_t num_elements, const T& val );
      vector< T > operator=( const vector<T>& v );
      vector( const vector<T>& other );
      vector< T > operator=( vector<T>&& v );
      vector( vector<T>&& other );
      ~vector( );
      void insert( size_t index, T obj);
      void grow( );
      void clear( );
      void remove( size_t index );
      void push_back( const T& obj );
      void pop_back( );
      void push_front( const T& obj );
      void pop_front( );
      void shrink_to_fit( );
      void swap( vector v );
      T& operator []( size_t index );
      const T& operator[ ] ( size_t i ) const;
      T at( size_t index );
      T* begin( );
      T* end( );
      const T* begin( ) const;
      const T* end( ) const;
          
      private:
      T *arr;                  // dynamic array
      size_t cap;   // amount of available space
      size_t sz;       // number of elemets added to array
      };

      template < class T >
      // default constructor
      vector< T >::vector( )
         {
         arr = new T[ 0 ];
         cap = 0;
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
      vector< T >::vector( size_t num_elements, const T& val )
         {
         arr = new T[ num_elements ];
         cap = num_elements;
         sz = num_elements;
         for ( size_t i = 0; i < sz; ++i )
            {
            arr[ i ] = val;
            }
         }

      // Copy Contructor
      template < class T >
      vector< T >::vector( const vector<T>& other )
         {
         arr = new T[ other.cap ];
         cap = other.cap;
         sz = other.sz;
         for ( size_t i = 0;  i < other.sz;  ++i )
            {
            arr[ i ] = other[ i ];
            }
         }
      
      // Assignment Operator
      template < class T >
      vector< T > vector< T >::operator=( const vector<T>& other )
         {
         delete[ ] arr;
         cap = other.cap;
         sz = other.sz;
         arr = new T[ cap ];
         for ( size_t i = 0;  i < sz;  ++i )
            {
            arr[ i ] = other.arr[ i ];
            }
         return *this;
         }

      // Move Constructor
      template < class T >
      vector< T >::vector( vector<T>&& other )
         {
         cap = other.cap;
         sz = other.sz;
         arr = other.arr;
         other.sz = 0;
         other.cap = 0;
         other.arr = nullptr;
         }

      // Move Assignment Operator
      template < class T >
      vector< T > vector< T >::operator=( vector<T>&& other )
         {
         sz = other.sz;
         cap = other.cap;
         arr = other.arr;
         return *this;
         }

      template < class T >
      // destructor
      vector< T >::~vector( )
         {
         delete[ ] arr;
         }

      template < class T >
      void vector< T >::insert( size_t index, T obj )
         {
         // if adding one means its at least half full, then grow by 2x
         if ( sz == cap )
            {
            grow(); // you might not want to do a simple grow when inserting
            }
         
         T old;
         for ( size_t i = index;  i <= sz;  ++i )
            {
               old = arr[ index ];
               arr[ index ] = obj;
               obj = old;
            }
            sz++;
         }

      template < class T >
      void vector< T >::clear( )
         {
         sz = 0;
         }

      template < class T >
      void vector< T >::grow( )
         {
         if ( cap == 0 )
            {
            cap = 1;
            }
         else 
            {
            cap *= 2;
            }

         T *arr_old = arr;
         arr = new T[ cap ];
         for ( size_t i = 0;  i < sz;  ++i )
            {
            arr[ i ] = arr_old[ i ];
            }
         delete[ ] arr_old;
         }

      template < class T >
      void vector<T>::remove( size_t index )
         {
         if ( sz == 0 ) 
            {
            std::cerr << "CANNOT REMOVE ON VECTOR OF SIZE 0";
            exit( 1 );
            }
         for ( int i = index;  i < sz;  i++ )
            {
            arr[i] = arr[i+1];
            }
         sz--;
         }
      
      template < class T >
      void vector<T>::push_back( const T& obj )
         {
         if ( sz == cap )
            {
            grow( );
            }
         arr[ sz ] = obj;
         sz++;
         }
       
      template < class T >
      void vector<T>::pop_back( )
         {
         if ( sz == 0 ) 
            {
            std::cerr << "CAN'T CALL POP BACK ON VECTOR OF SIZE 0\n";
            exit(1);
            }
         sz--;
         }

      template < class T > 
      T* vector<T>::begin( )
         {
         return arr;
         }

      template < class T >
      T* vector<T>::end( )
         {
         return arr + sz;
         }

      template < class T >
      const T* vector<T>::begin( ) const
         {
         return arr;
         }

      template < class T > 
      const T* vector<T>::end( ) const
         {
         return arr + sz;
         }
       
      template < class T >
      T vector<T>::at( size_t index )
         {
         if( index >= sz || index < 0 )
            {
            std::cerr << "OUT OF BOUNDS \n";
            exit( 1 );
            }
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
         if ( sz == 0 ) 
            {
            std::cerr << "CAN'T CALL POP BACK ON VECTOR OF SIZE 0\n";
            exit( 1 );
            }
         remove( 0 );
         }
       
      template < class T >
      void vector< T >::shrink_to_fit( )
         {
         T* old = arr;
         arr = new T[ sz ];
         for( int i = 0;  i < sz;  i++ )
         {
            arr[ i ] = old[ i ];
         }
         delete[ ] old;
         cap = sz;
         }
       
      template < class T >
      void vector< T >::swap( vector v )
         {
         T* temp;
         temp = v.arr;
         v.arr = arr;
         arr = v.arr;
         unsigned int num = v.sz;
         v.sz = sz;
         sz = num;
         num = v.cap;
         v.cap = cap;
         cap = num;
         }
   
   
      template < class T >
      T& vector< T >::operator [ ]( size_t index )
         {
         if ( index >= sz || index < 0 )
            {
            std::cerr << "INDEX OUT OF BOUNDS\n";
            exit( 1 );
            }
         return arr[ index ];
         }

      template < class T >
      const T& vector< T >::operator [ ]( size_t index ) const
         {
         if ( index >= sz || index < 0 )
            {
            std::cerr << "INDEX OUT OF BOUNDS\n";
            exit( 1 );
            }
         return arr[ index ];
         }
   }
