// algorithm.hpp
// Artisan crafted version of <algorithm> that keeps all of the interfaces.
// 2019-09-19: Implemented Min, Max, Swap: Jasina, Combsc

namespace dex
	{
	
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
