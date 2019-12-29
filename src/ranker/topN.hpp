// topN.hpp
// Linked-list implementation of topN
//
// 2019-12-29:  Port over Jonas, Medha, and Stephen's implementation: jasina
// 2019-12-11:  Port over from HW4 as done by Matthew, Chris, and Tejas: lougheem

#ifndef DEX_TOP_N
#define DEX_TOP_N

#include "utils/vector.hpp"

struct documentInfo
	{
	size_t documentIndex;
	double score;
	};

documentInfo **findTopN( dex::vector < double > scores, size_t N )
	{
	// This should hold a sorted array of the top at most N scores that we have seen so far
	documentInfo **topNArray = new documentInfo *[ N ]( );

	for ( size_t i = 0;  i < scores.size( );  ++i )
		{
		double currentValue = scores[ i ];
		for ( size_t j = 0;  j < N;  ++j )
			{
			if ( !topNArray[ j ] || currentValue > topNArray[ j ]->score )
				{
				for ( size_t k = N - 1;  k > j;  --k )
					{
					documentInfo *temp = topNArray[ k ];
					topNArray[ k ] = topNArray[ k - 1 ];
					topNArray[ k - 1 ] = temp;
					}

				if ( topNArray[ j ] )
					*topNArray[ j ] = documentInfo{ i, currentValue };
				else
					topNArray[ j ] = new documentInfo{ i, currentValue };
				break;
				}
			}
		}

	return topNArray;
	}

#endif