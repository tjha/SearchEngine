// utility.hpp
// Semi-complete replica of the STL's utility header
//
// 2019-12-09: Add default constructor: combsc
// 2019-10-16: Add include guard: jasina
// 2019-10-20: File created, pair implemented: jasina, lougheem

#ifndef DEX_UTILITY
#define DEX_UTILITY

#include "algorithm.hpp"

namespace dex
	{
	template < class T1, class T2 >
	struct pair
		{
		T1 first;
		T2 second;
		pair( )
			{
			}
		pair( const T1& first, const T2& second ) : first( first ), second( second ) { }
		};

	template < class T1, class T2 >
	bool operator==( const pair < T1, T2 > &lhs, const pair < T1, T2 > &rhs )
		{
		return lhs.first == rhs.first && lhs.second == rhs.second ;
		}
	template < class T1, class T2 >
	bool operator!=( const pair < T1, T2 > &lhs, const pair < T1, T2 > &rhs )
		{
		return lhs.first != rhs.first || lhs.second != rhs.second ;
		}
	template < class T1, class T2 >
	bool operator<( const pair < T1, T2 > &lhs, const pair < T1, T2 > &rhs )
		{
		return lhs.first < rhs.first || ( lhs.first == rhs.first && lhs.second < rhs.second );
		}
	template < class T1, class T2 >
	bool operator>( const pair < T1, T2 > &lhs, const pair < T1, T2 > &rhs )
		{
		return rhs < lhs;
		}
	template < class T1, class T2 >
	bool operator<=( const pair < T1, T2 > &lhs, const pair < T1, T2 > &rhs )
		{
		return !( rhs < lhs );
		}
	template < class T1, class T2 >
	bool operator>=( const pair < T1, T2 > &lhs, const pair < T1, T2 > &rhs )
		{
		return !( lhs < rhs );
		}
	template < class T1, class T2 >
	void swap( pair < T1, T2 > &x, pair < T1, T2 > &y )
		{
		dex::swap( x.first, y.first );
		dex::swap( x.second, y.second );
		}
	}

#endif
