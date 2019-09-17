// vector.hpp
// 
// Vector class
//
// 2019/09/14 - Tejas Jha created basic skeleton with private members

namespace dex
   {
   template < class T >
   class vector
      {
      public:
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
