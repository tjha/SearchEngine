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
	int n = 4;

	for ( double index = 0;  index < n + 1;  index++ )
		{
		scores.pushBack( index );
		std::cout << "score: " << index << "\n";
		}
	
	REQUIRE( scores.size( ) >= n );

	documentInfo **topN, *p;
	topN = findTopN( scores, n );
	for ( int index = 0;  index < n && ( p = topN[ index ] );  index++ )
		{
		std::cout << "index: " << index;
		REQUIRE( p );
		topScores.pushBack( scores[ p->documentIndex ] );
		std::cout << "\t" << scores[ p->documentIndex ] << "\n";
		}

	}

