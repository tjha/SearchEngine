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
      void insert(int index, T obj)
         {
         if ( size + 1 > capacity / 2 ) 
            {
            grow(); // you might not want to do a simple grow when inserting
            }
         
         T old;
         for ( int i = index;  i <= size;  ++i )
            {
               old = arr[ index ];
               arr[ index ] = old;
               obj = old;
            }
         }
      void clear()
         {
         T *ptr = arr;
         for ( int i = 0;  i < size;  ++i )
            {
               *ptr = nullptr; 
            }
         size = 0;
         }
      void grow()
         {
         capacity *= 2;
         T *arr_old = &arr;
         arr = new T[capacity];
         delete(arr_old);
         }
      private:
      T *arr;
      int capacity;
      int size;
      };
   }
