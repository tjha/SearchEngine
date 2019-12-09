// rankerTests.cpp
// tests for the ranker class
//
// 2019-12-09: Init Commit: combsc + lougheem

#include "catch.hpp"
#include "../src/ranker/ranker.hpp"
#include <iostream>

TEST_CASE( "basic spanning", "[ranker]" )
	{
	SECTION ( "simple" )
		{
		// { 1 , 3, 60, 900 };
		dex::vector < unsigned > duo = { 1 };
		dex::ISR duoISR( "duo", duo );
		std::cout << "duo" << std::endl;
		// { 2, 61, 901 };
		dex::vector < unsigned > mushu = { 2 };
		dex::ISR mushuISR( "mushu", mushu );

		dex::vector < dex::ISR > isrs;
		isrs.pushBack( duoISR );
		isrs.pushBack( mushuISR );
		dex::ranker judge;
		dex::vector < double > heuristics = { 1, 20 };
		dex::vector < bool > emphasized = { false, false };
		dex::vector < unsigned > spans = judge.getDesiredSpans( heuristics, emphasized, isrs, 1 );
		for ( unsigned i = 0;  i < spans.size( );  ++i )
			{
			std::cout << spans[ i ] << std::endl;
			}
		}
	SECTION( "Hamilton" )
		{
		dex::vector < unsigned > quick = { 62, 69, 84, 311, 421, 430, 559, 619, 794, 952 };
		dex::ISR quickISR( "quick", quick );
		dex::vector < unsigned > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 516, 795, 826, 828, 957 };
		dex::ISR brownISR( "brown", brown );
		dex::vector < unsigned > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::ISR foxISR( "fox", fox );
		dex::ranker judge;
		}
	
	}
