// basicString.hpp
// Homemade version of std::basic_string that keeps all of the interfaces.
// We omit the following functions:
//    std::basic_string::operator+=( std::initializer_list < charT > );
//    std::basic_string::append( std::initializer_list < charT > );
//    std::basic_string::assign( std::basic_string < charT > && ) noexcept;
//    std::basic_string::insert( const_iterator, initializer_list < charT > );
//    std::basic_string::replace( const_iterator, const_iterator, initializer_list < charT > );
//    std::basic_string::get_allocator( ) const;
// 2019-09-16: Defined Interface: combsc, jasina, lougheem
// 2019-09-14: File created: combsc, jasina, lougheem

namespace dex
	{
	template < class charT > 
	class basicString
		{
		private:
			charT *array;
			unsigned arraySize;
			unsigned stringSize;

		public:
			static const unsigned npos = unsigned ( -1 );
			basicString( );
			~basicString( );
			basicString < charT > operator=(basicString < charT > other);

			// Capacity
			unsigned size( ) const;
			unsigned length( ) const;
			unsigned max_size( ) const;
			void resize(unsigned newStringLength, charT character);
			unsigned capacity( ) const;
			void reserve(unsigned newStringLength);
			void clear( );
			bool empty( ) const;
			void shrink_to_fit( );

			// Iterators
			class iterator
				{
				};

			class constIterator
				{
				};

			class reverseIterator
				{
				};

			class constReverseIterator
				{
				};

			iterator begin( );
			iterator end( );

			constIterator cbegin( ) const;
			constIterator cend( ) const;

			reverseIterator rbegin( );
			reverseIterator rend( )

			constReverseIterator crbegin( ) const;
			constReverseIterator crend( ) const;

			// Element Access
			const charT &operator[ ]( unsigned );
			charT &operator[ ]( unsigned );
			const charT &at( unsigned );
			charT &at( unsigned );
			const charT &back( );
			charT &back();

			// Modifiers
			basicString < charT > &operator+=( const basicString &other );
			basicString < charT > &operator+=( const charT *other );
			basicString < charT > &operator+=( charT character );

			basicString < charT > &append( const basicString < charT > &other );
			basicString < charT > &append( const basicString < charT > &other, unsigned position, unsigned length );
			basicString < charT > &append( const charT *other );
			basicString < charT > &append( const charT *other, unsigned length );
			basicString < charT > &append( unsigned number, charT character );
			basicString < charT > &append( iterator first, iterator last );
			void pushBack( charT character );

			basicString < charT > &assign( const basicString < charT > &other );
			basicString < charT > &assign( const basicString < charT > &other, unsigned position, unsigned length );
			basicString < charT > &assign( const charT *other );
			basicString < charT > &assign( const charT *other, unsigned length ); 
			basicString < charT > &assign( unsigned length, charT character ); 
			basicString < charT > &assign( iterator first, iterator last );
			
			basicString < charT > &insert( unsigned position, const basicString < charT > &other );
			basicString < charT > &insert( unsigned position, const basicString < charT > &other, unsigned position, unsigned length );
			basicString < charT > &insert( unsigned position, const charT *other );
			basicString < charT > &insert( unsigned position, const charT *other, unsigned length );
			basicString < charT > &insert( unsigned position, unsigned length, charT character );
			iterator insert( constIterator first, unsigned length, charT character );
			iterator insert( constIterator first, charT character );
			template < class InputIterator >
					iterator insert( iterator insertionPoint, InputIterator first, InputIterator last );
			
			basicString < charT > &erase( unsigned position = 0, unsigned length = npos );
			iterator erase ( constIterator first );
			iterator erase ( constIterator first, constIterator last );

			basicString < charT > &replace( unsigned position, unsigned length, const basicString < charT > &other );
			basicString < charT > &replace( constIterator first, constIterator last, const basicString < charT > &other );
			basicString < charT > &replace( unsigned position, unsigned length, const basicString < charT > &other, unsigned subposition, unsigned sublength );
			basicString < charT > &replace( unsigned position, unsigned length, const charT *other );
			basicString < charT > &replace( constIterator first, constIterator Last, const charT *other );
			basicString < charT > &replace( unsigned position, unsigned length, const charT *other, unsigned n );
			basicString < charT > &replace( constIterator first, constIterator Last, const charT *other, unsigned n );
			basicString < charT > &replace( unsigned position, unsigned length, unsigned n, charT c );
			basicString < charT > &replace( constIterator first, constIterator Last, unsigned n, charT c );
			template < class InputIterator > basicString < charT > &replace( constIterator first, constIterator Last, InputIterator inputFirst, InputIterator inputLast );

			void swap( basicString &other );

			void popBack( );

			// String Operations

			const charT* cStr( ) const;

			const chart* data( ) const;

			unsigned copy( charT *characterArray, unsigned length, unsigned position = 0 ) const;

			unsigned find( const basicString &other, unsigned position = 0 ) const;
			unsigned find( const charT *other, unsigned position = 0 ) const;
			unsigned find( const charT *other, unsigned position, unsigned n ) const;
			unsigned find( charT c, unsigned position = 0 ) const;

			unsigned rfind( const basicString &other, unsigned position = 0 ) const;
			unsigned rfind( const charT *other, unsigned position = 0 ) const;
			unsigned rfind( const charT *other, unsigned position, unsigned n ) const;
			unsigned rfind( charT c, unsigned position = 0 ) const;

			unsigned findFirstOf( const basicString &other, unsigned position = 0 ) const;
			unsigned findFirstOf( const charT *other, unsigned position = 0 ) const;
			unsigned findFirstOf( const charT *other, unsigned position, unsigned n ) const;
			unsigned findFirstOf( charT c, unsigned position = 0 ) const;

			unsigned findLastOf( const basicString &other, unsigned position = 0 ) const;
			unsigned findLastOf( const charT *other, unsigned position = 0 ) const;
			unsigned findLastOf( const charT *other, unsigned position, unsigned n ) const;
			unsigned findLastOf( charT c, unsigned position = 0 ) const;

			unsigned findFirstNotOf( const basicString &other, unsigned position = 0 ) const;
			unsigned findFirstNotOf( const charT *other, unsigned position = 0 ) const;
			unsigned findFirstNotOf( const charT *other, unsigned position, unsigned n ) const;
			unsigned findFirstNotOf( charT c, unsigned position = 0 ) const;

			unsigned findLastNotOf( const basicString &other, unsigned position = 0 ) const;
			unsigned findLastNotOf( const charT *other, unsigned position = 0 ) const;
			unsigned findLastNotOf( const charT *other, unsigned position, unsigned n ) const;
			unsigned findLastNotOf( charT c, unsigned position = 0 ) const;

			basicString < charT > substr( unsigned position = 0, unsigned length = npos );

			int compare( const basicString &other ) const;
			int compare( unsigned position, unsigned length, const basicString &other ) const;
			int compare( unsigned position, unsigned length, const basicString &other, unsigned subposition, unsigned sublength ) const;
			int compare( const charT *other ) const;
			int compare( unsigned position, unsigned length, const charT *other ) const;
			int compare( unsigned position, unsigned length, const charT *other, unsigned n ) const;
		};
	}
