// vector.hpp
// 
// Vector class
//
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
      vector( size_t num );
      vector ( size_t num, const T& val );
      ~vector( );
      void insert( size_t index, T obj);
      void grow( );
      void clear( );
      void remove( size_t index );
      void push_back( T obj );
      void pop_back( );
      void push_front( T obj );
      void pop_front( );
      void shrink_to_fit( );
      void swap( vector v );
      T operator []( size_t index );
      T operator=( vector v );
      T at( size_t index );
      T front( );
      T back( );
      T swap( vector x );
          
      private:
      T *arr;                  // dynamic array
      size_t capacity;   // amount of available space
      size_t size;       // number of elemets added to array
      };

      template < class T >
      // default constructor
      vector< T >::vector( )
         {
         arr = new T[ 1 ];
         capacity = 1;
         size = 0;
         }
   
      template < class T >
      // constructor for vector of num elements
      vector< T >::vector( size_t num )
         {
         arr = new T[ num ]( );
         capacity = num ;
         size = num;
         }

      template < class T >
      // constructor for vector of num elements each initialized to val
      vector< T >::vector( size_t num, const T& val )
         {
         arr = new T[ num ] { val };
         capacity = num ;
         size = num;
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
         if ( size == capacity )
            {
            grow(); // you might not want to do a simple grow when inserting
            }
         
         T old;
         for ( size_t i = index;  i <= size;  ++i )
            {
               old = arr[ index ];
               arr[ index ] = obj;
               obj = old;
            }
             size++;
         }

      template < class T >
      void vector< T >::clear( )
         {
         size = 0;
         }

      template < class T >
      void vector< T >::grow( )
         {
         capacity *= 2;
         if ( capacity == 0 )
            {
            capacity = 1;
            }
         T *arr_old = &arr;
         arr = new T[ capacity ];
         for ( size_t i = 0;  i < size;  ++i )
            {
            arr[ i ] = arr_old[ i ];
            }
         delete[ ] arr_old;
         }

      template < class T >
      void vector<T>::remove( size_t index )
         {
         if ( size == 0 ) 
            {
            std::cerr << "CANNOT REMOVE ON VECTOR OF SIZE 0";
            exit( 1 );
            }
         for ( int i = index;  i < size;  i++ )
            {
            arr[i] = arr[i+1];
            }
         size--;
         }
      
      template < class T >
      void vector<T>::push_back( T obj )
         {
         if ( size == capacity )
            {
            grow( );
            }
         arr[ size ] = obj;
         size++;
         }
       
      template < class T >
      void vector<T>::pop_back( )
         {
         if ( size == 0 ) 
            {
            std::cerr < "CAN'T CALL POP BACK ON VECTOR OF SIZE 0\n";
            exit(1);
            }
         size--;
         }
       
      template < class T >
      T vector<T>::at( size_t index )
         {
         if( index >= size || index < 0 )
            {
            std::cerr << "OUT OF BOUNDS \n";
            exit( 1 );
            }
         return arr[ index ];
         }
       
      template < class T >
      T vector<T>::front( )
         {
         if ( size == 0 ) 
            {
            std::cerr << "VECTOR OF SIZE 0 HAS NO FRONT\n";
            exit( 1 );
            }
         return arr[ 0 ];
         }
       
      template < class T >
      T vector<T>::back( )
         {
         if ( size == 0 ) 
            {
            std::cerr << "VECTOR OF SIZE 0 HAS NO BACK\n";
            exit( 1 );
            }
         return arr[ size - 1 ];
         }
       
      template < class T >
      void vector<T>::push_front( T obj )
         {
         insert( 0, obj );
         }
       
      template < class T >
      void vector< T >::pop_front( )
         {
         if ( size == 0 ) 
            {
            std::cerr < "CAN'T CALL POP BACK ON VECTOR OF SIZE 0\n";
            exit( 1 );
            }
         remove( 0 );
         }
       
      template < class T >
      void vector< T >::shrink_to_fit( )
         {
         T* old = arr;
         arr = new T[ size ];
         for( int i = 0;  i < size;  i++ )
         {
            arr[ i ] = old[ i ];
         }
         delete[ ] old;
         capacity = size;
         }
       
      template < class T >
      void vector< T >::swap( vector v )
         {
         T* temp;
         temp = v.arr;
         v.arr = arr;
         arr = v.arr;
         unsigned int num = v.size;
         v.size = size;
         size = num;
         num = v.capacity;
         v.capacity = capacity;
         capacity = num;
         }
   
      void vector< T >::operator=( vector v )
         {
         size = v.size;
         capacity = v.capacity;
         for( size_t i = 0;  i < size;  i++ )
            {
            arr[ i ] = v.arr[ i ];
            }
         }
   
      T vector< T >::operator []( size_t index )
         {
         if ( index > capacity )
            {
            std::cerr < "INDEX OUT OF BOUNDS\n";
            exit( 1 );
            }
         return at( index );
         }
   }

      private:
      T *arr;                  // dynamic array 
      unsigned int capacity;   // amount of available space
      unsigned int size;       // number of elemets added to array 
      };

   template < class T >
   // default constructor
   vector< T >::vector( )
      {
      arr = new T[ 1 ];
      capacity = 1;
      size = 0;
      }

   template < class T >
   // constructor for vector of num elements
   vector< T >::vector( unsigned int num )
      {
      arr = new T[ num + 1 ]( );
      capacity = num + 1;
      size = num;
      }

   template < class T >
   // constructor for vector of num elements each initialized to val
   vector< T >::vector( unsigned int num, const T& val )
      {
      arr = new T[ num + 1 ] { val };
      capacity = num + 1;
      size = num;
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
      if ( size == capacity ) 
         {
         grow(); // you might not want to do a simple grow when inserting
         }
      
      T old;
      for ( size_t i = index;  i <= size;  ++i )
         {
            old = arr[ index ];
            arr[ index ] = obj;
            obj = old;
         }
      }

   template < class T >
   void vector< T >::clear()
      {
      size = 0;
      }

   template < class T >
   void vector< T >::grow()
      {
      capacity *= 2;
      T *arr_old = &arr;
      arr = new T[ capacity ];
      for ( size_t i = 0;  i < size;  ++i )
         {
         arr[ i ] = arr_old[ i ];
         }
      delete[] arr_old;
      }
   }
