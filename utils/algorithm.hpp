// algorithm.hpp
// Artisan crafted version of <algorithm> that keeps all of the interfaces.
// 2019-10-13: Implement copy, copy_backward, and fill: jasina
// 2019-09-19: Implemented Min, Max, Swap: Jasina, Combsc

namespace dex
	{
	template< class InputIt, class OutputIt >
	OutputIt copy( InputIt first, InputIt last, OutputIt d_first )
		{
		for ( ;  first != last;  *( d_first++ ) = *( first++ ) );
		return d_first;
		}

	template< class BidirIt1, class BidirIt2 >
	BidirIt2 copy_backward( BidirIt1 first, BidirIt1 last, BidirIt2 d_last )
		{
		for ( ;  last != first;  *( --d_last ) = *( --last ));
		return d_last;
		}

	template< class ForwardIt, class T >
	void fill( ForwardIt first, ForwardIt last, const T& value )
		{
		for ( ;  first != last;  *( first++ ) = value );
		}
	
	template < typename T >
	T min( const T &first, const T &second )
		{
		return first <= second ? first : second;
		}

	template < typename T >
	T max( const T &first, const T &second )
		{
		return first >= second ? first : second;
		}

	template < typename T >
	void swap( T &first, T &second )
		{
		// Naive implementation. We think rvalue references can make this more 
		// efficient but we're scared to implement it.
		T other = first;
		first = second;
		second = other;
		}
	}
