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
      public:
      vector( );
      vector( unsigned int num );
      vector ( unsigned int num, const T& val );
      ~vector( );
      void insert( size_t index, T obj);
      void grow( );
      void clear( );
      void remove(int index);
      void push_back(T insertThis);
      void pop_back();
      void push_front(T insertThis);
      void pop_front();
      void shrinkToFit();
      void swap(vector v);
      T operator [](int index);
      T operator=(vector v);
      T at(int index);
      T front();
      T back();
      T swap(vector x);
          
      private:
      T *arr;                  // dynamic array
      unsigned int capacity;   // amount of available space
      unsigned int size;       // number of elemets added to array
      };
   
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

      template < class T >
       void vector<T>::remove(int index){
           for (int i = index; i < size; i++)
           {
               *arr[i] = *arr[i+1];
           }
           size--;
       }
      
       template < class T >
       void vector<T>::push_back(T insertThis){
           if (size == capacity)
           {
               grow();
           }
           *arr[size] = insertThis;
           size++;
       }
       
       template < class T >
       void vector<T>::pop_back(){
           size--;
       }
       
       template < class T >
       T vector<T>::at(int index){
           return *arr[index];
       }
       
       template < class T >
       T vector<T>::front(){
           return *arr[0];
       }
       
       template < class T >
       T vector<T>::back(){
           return *arr[size-1];
       }
       
       template < class T >
       void vector<T>::push_front(T insertThis){
           if (size == capacity)
           {
               grow();
           }
           for (int i = size; i > 0; i--)
           {
               *arr[i] = *arr[i-1];
           }
           *arr[0] = insertThis;
           size++;
       }
       
       template < class T >
       void vector< T >::pop_front(){
           for(int i = 0; i < size; i++)
           {
               *arr[i] = *arr[i+1];
           }
           size--;
       }
       
       template < class T >
       void vector< T >::shrinkToFit(){
           T* old = arr;
           arr = new T[size];
           for(int i = 0; i < size; i++)
           {
               *arr[i] = *old[i];
           }
           delete old;
           capacity = size;
       }
       
       template < class T >
       void vector< T >::swap(vector v){
           for(int i = 0; i < size; i++)
           {
               *arr[i] = *arr[i+1];
           }
           size--;
       }
   
        void vector< T >::operator=(vector v){
            size = v.size;
            capacity = v.capacity;
            for(int i = 0; i < size; i++){
                *arr[i] = *v.arr[i];
            }
        }
   
       T vector< T >::operator [](int index){
           return *arr[index];
       }
   
   }
