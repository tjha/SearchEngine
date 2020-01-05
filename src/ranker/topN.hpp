// topN.hpp
// Linked-list implementation of topN
//
// 2019-12-29:  Port over Jonas, Medha, and Stephen's implementation instead: jasina
// 2019-12-11:  Port over from HW4 as done by Matthew, Chris, and Tejas: lougheem

#ifndef DEX_TOP_N
#define DEX_TOP_N

#include "utils/algorithm.hpp"
#include "utils/vector.hpp"

namespace dex{
	struct documentInfo
		{
		size_t documentIndex;
		double score;
		};

	documentInfo **findTopN( const dex::vector< double > &scores, const size_t N )
		{
		// This should hold a sorted array of the top at most N scores that we have seen so far
		documentInfo **topNArray = new documentInfo *[ N ]( );

		for ( size_t i = 0;  i < scores.size( );  ++i )
			{
			double currentScore = scores[ i ];

			// j is the index into topNArray for checking if we want to insert currentScore
			for ( size_t j = 0;  j < N;  ++j )
				{
				// Check whether currentScore is one of the N largest scores we've seen so far
				if ( !topNArray[ j ] || currentScore > topNArray[ j ]->score )
					{
					// Move everything less than currentScore to the right
					for ( size_t k = N - 1;  k > j;  --k )
						dex::swap( topNArray[ k ], topNArray[ k - 1 ] );

					// Insert current value into topNArray. We only allocate new memory if necessary
					if ( topNArray[ j ] )
						*topNArray[ j ] = documentInfo{ i, currentScore };
					else
						topNArray[ j ] = new documentInfo{ i, currentScore };

					// We should only do the body of this if statement for one value of j, so we break here
					break;
					}
				}
			}

		return topNArray;
		}
	}

#endif
