// vector.hpp
// 
// Vector class
//
// 2019/09/17 - Tejas Jha - created constructors and destructor
//                        - Added comment descriptions for private variables
//                        - modified ints to unsigned int
// 2019/09/14 - Tejas Jha - created basic skeleton with private members

namespace dex
   {
   template < class T >
   class vector
      {
      public:
      vector( );
      vector( unsigned int num );
      vector ( unsigned int num, const T& val );
      ~vector( );
      void insert( size_t index, T obj);
      void grow( );
      void clear( );

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
