// topNTests.cpp
// tests for topN that will be used to get top scored documents
//
// 2019-12-11: Init commit: lougheem

#include "catch.hpp"
#include "../src/ranker/topN.hpp"
#include <iostream>

TEST_CASE( "topN" )
	{
	dex::vector< double > scores;
	dex::vector< double > topScores;
	int m = 1000;

	for ( double index = 0;  index < m + 1;  index++ )
		{
		scores.pushBack( index );
		//std::cout << "score: " << index << "\n";
		}
	
	REQUIRE( scores.size( ) >= m );
	int n = m/10;
	documentInfo **topN, *p;
	topN = findTopN( scores, n );
	for ( int index = 0;  index < n - 1;  index++ )
		{
		REQUIRE( topN[ index ]->score > topN[ index + 1 ]->score );
		}

	}

