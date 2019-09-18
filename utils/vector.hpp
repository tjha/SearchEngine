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
   }
