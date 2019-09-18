// vector.hpp
// 
// Vector class
//
// 2019/09/17 - Jonas Hirshland implemented insert, clear, grow, size
// 2019/09/14 - Tejas Jha created basic skeleton with private members

namespace dex
   {
   template < class T >
   class vector
      {
      public:
      void insert( int index, T obj );
      void clear();
      void grow();

      private:
      T *arr;
      int capacity;
      int size;
      };

   template < class T >
   void vector< T >::insert( int index, T obj )
      {
      // if adding one means its at least half full, then grow by 2x
      if ( size == capacity ) 
         {
         grow(); // you might not want to do a simple grow when inserting
         }
      
      T old;
      for ( int i = index;  i <= size;  ++i )
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
      for ( int i = 0;  i < size;  ++i )
         {
         arr[ i ] = arr_old[ i ];
         }
      delete[] arr_old;
      }
   }


