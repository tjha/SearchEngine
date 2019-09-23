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
      public:
      void remove(int index);
      void push_back(T insertThis);
      void pop_back();
      void push_front(T insertThis);
      void pop_front();
      void shrinkToFit();
      void swap(vector v);
      T at(int index);
      T front();
      T back();
      T swap(vector x);
      };
      
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
   }
