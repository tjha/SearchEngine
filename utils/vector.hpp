// vector.hpp
// 
// Vector class
//
// 2019/09/14 - Tejas Jha created basic skeleton with private members

namespace dex
   {
   template <class T>
   class vector
      {
      private:
      T *arr;
      int capacity;
      int size;
      };
   }
