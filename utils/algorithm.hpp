// algorithm.hpp
// Artisan crafted version of <algorithm> that keeps all of the interfaces.
// 2019-10-17: Disambiguate call to search, add include guard: jasina
// 2019-10-14: Fix findEnd: jasina
// 2019-10-13: Implement find, search, findEnd, copy, copyBackward, and fill: jasina
// 2019-09-19: Implemented Min, Max, Swap: Jasina, Combsc

#ifndef DEX_ALGORITHM
#define DEX_ALGORITHM

namespace dex
	{
	template< class InputIt, class T >
	InputIt find( InputIt first, InputIt last, const T& value )
		{
		for ( ;  first != last;  ++first )
			if ( *first == value )
				return first;
		return first;
		}

	template < class ForwardIt1, class ForwardIt2 >
	ForwardIt1 search( ForwardIt1 first, ForwardIt1 last, ForwardIt2 sFirst, ForwardIt2 sLast )
		{
		for ( ;  ;  ++first )
			{
			ForwardIt1 firstCopy = first;
			for ( ForwardIt2 sFirstCopy = sFirst;  ;  ++firstCopy, ++sFirstCopy )
				{
				if ( sFirstCopy == sLast )
					return first;
				if ( firstCopy == last )
					return last;
				if ( *sFirstCopy != *firstCopy )
					break;
				}
			}
		}

	template< class ForwardIt1, class ForwardIt2 >
	ForwardIt1 findEnd( ForwardIt1 first, ForwardIt1 last, ForwardIt2 s_first, ForwardIt2 s_last )
		{
		if ( s_first == s_last )
			return last;
		ForwardIt1 location = last;
		while ( true )
			{
			ForwardIt1 newLocation = dex::search( first, last, s_first, s_last );
			if ( newLocation == last )
				return location;
			location = newLocation;
			first = newLocation;
			++first;
			}
		}

	template < class InputIt, class OutputIt >
	OutputIt copy( InputIt first, InputIt last, OutputIt dFirst )
		{
		for ( ;  first != last;  *( dFirst++ ) = *( first++ ) );
		return dFirst;
		}

	template < class BidirIt1, class BidirIt2 >
	BidirIt2 copyBackward( BidirIt1 first, BidirIt1 last, BidirIt2 dLast )
		{
		for ( ;  last != first;  *( --dLast ) = *( --last ));
		return dLast;
		}

	template < class ForwardIt, class T >
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

#endif
