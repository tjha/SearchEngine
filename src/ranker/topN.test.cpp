// topNTests.cpp
// tests for topN that will be used to get top scored documents
//
// 2019-12-11: Init commit: lougheem

#include <cstddef>
#include "catch.hpp"
#include "ranker/topN.hpp"

TEST_CASE( "topN" )
	{
	dex::vector < double > scores;

	for ( size_t index = 0;  index < 1500;  ++index )
		scores.pushBack( static_cast < double >( index ) / 2 );

	size_t N = 100;
	dex::documentInfo **topN = dex::findTopN( scores, N );
	for ( size_t index = 0;  index < N;  ++index )
		REQUIRE( topN[ index ]->score == static_cast < double >( 1499 - index ) / 2 );

	for ( size_t index = 0;  index < N;  ++index )
		delete topN[ index ];
	delete [ ] topN;
	}

