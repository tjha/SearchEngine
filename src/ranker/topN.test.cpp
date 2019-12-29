// topNTests.cpp
// tests for topN that will be used to get top scored documents
//
// 2019-12-11: Init commit: lougheem

#include <cstddef>
#include "catch.hpp"
#include "ranker/topN.hpp"

TEST_CASE( "topN" )
	{
	SECTION( "reversed" )
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

	SECTION( "gamma" )
		{
		dex::vector < double > scores = { 0, 5, 7, 7, 2, 1, 5, 6, 6, 4, 9 };

		size_t N = 5;
		dex::documentInfo **topN = dex::findTopN( scores, N );
		REQUIRE( topN[ 0 ]->score == 9 );
		REQUIRE( topN[ 0 ]->documentIndex == 10 );
		REQUIRE( topN[ 1 ]->score == 7 );
		REQUIRE( topN[ 1 ]->documentIndex == 2 );
		REQUIRE( topN[ 2 ]->score == 7 );
		REQUIRE( topN[ 2 ]->documentIndex == 3 );
		REQUIRE( topN[ 3 ]->score == 6 );
		REQUIRE( topN[ 3 ]->documentIndex == 7 );
		REQUIRE( topN[ 4 ]->score == 6 );
		REQUIRE( topN[ 4 ]->documentIndex == 8 );

		for ( size_t index = 0;  index < N;  ++index )
			delete topN[ index ];
		delete [ ] topN;
		}
	}

