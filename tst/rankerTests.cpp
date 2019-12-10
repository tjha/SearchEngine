// rankerTests.cpp
// tests for the ranker class
//
// 2019-12-09: Init Commit: combsc + lougheem

#include "catch.hpp"
#include "../src/ranker/ranker.hpp"
#include <iostream>

TEST_CASE( "static ranking", "[ranker]" )
	{
	dex::ranker judge( { { 15, 50 }, { 25, 40 }, { 50, 20 } } );
	REQUIRE( judge.getStaticScoreTitle( "under15" ) == 50 );
	REQUIRE( judge.getStaticScoreTitle( "googlemapsgooglemaps" ) == 40 );
	REQUIRE( judge.getStaticScoreTitle( "googlemapsgooglemaps a new paradigm" ) == 20 );
	REQUIRE( judge.getStaticScoreTitle( "googlemapsgooglemaps a new paradigm and a bad title" ) == 0 );
	}

TEST_CASE( "sudo ISR", "[ranker]" )
	{
	dex::vector < unsigned > postingList = { 3, 42, 82, 1009, 4000 };
	dex::ISR isr( "wurd", postingList );
	unsigned location = isr.next( );
	unsigned iters = 0;
	while ( location != dex::ISR::npos )
		{
		REQUIRE( postingList[ iters++ ] == location );
		std::cout << location << ",\t";
		location = isr.next( );
		}
	std::cout << "\n";
	}
TEST_CASE( "basic spanning", "[ranker]" )
	{
	SECTION ( "simple" )
		{
		std::cout << "basic spanning, simple\n";
		// { 1 , 3, 60, 900 };
		dex::vector < unsigned > duo = { 1, 3, 900 };
		dex::ISR duoISR( "duo", duo );
		// { 2, 61, 901 };
		dex::vector < unsigned > mushu = { 2, 61, 901 };
		dex::ISR mushuISR( "mushu", mushu );

		dex::vector < dex::ISR > isrs;
		isrs.pushBack( duoISR );
		isrs.pushBack( mushuISR );

		dex::ranker judge;
		dex::vector < double > heuristics = { 1, 20, 40 };
		dex::vector < bool > emphasized = { false, false };
		dex::vector < unsigned > spans = judge.getDesiredSpans( heuristics, emphasized, isrs, 1 );
		std::cout << "Finished spanning\n";
		unsigned prevHeuristic = 0;
		for ( unsigned i = 0;  i < spans.size( );  ++i )
			{
			std::cout << spans[ i ] << " spans between " << prevHeuristic << " and " << heuristics[ i ] * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ];
			}
		}
	SECTION( "Quick Brown Fox" )
		{
		dex::vector < unsigned > quick = { 62, 69, 84, 311, 421, 430, 559, 619, 794, 952 };
		dex::ISR quickISR( "quick", quick );
		dex::vector < unsigned > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 516, 795, 826, 828, 957 };
		dex::ISR brownISR( "brown", brown );
		dex::vector < unsigned > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::ISR foxISR( "fox", fox );
		dex::vector < dex::ISR > isrs;
		isrs.pushBack( quickISR );
		isrs.pushBack( brownISR );
		isrs.pushBack( foxISR );
		dex::ranker judge;
		dex::vector < double > heuristics = { 1, 3, 4, 5 };
		dex::vector < bool > emphasized = { false, false, true };
		dex::vector < unsigned > spans = judge.getDesiredSpans( heuristics, emphasized, isrs, 1 );
		std::cout << "Finished spanning\n";
		unsigned prevHeuristic = 0;
		for ( unsigned i = 0;  i < spans.size( );  ++i )
			{
			std::cout << spans[ i ] << " spans between " << prevHeuristic << " and " << heuristics[ i ] * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ];
			}
		}
	}

TEST_CASE( "edge cases", "[ranker]" )
	{
	SECTION( "Short ISR" )
		{
		dex::vector < unsigned > quick = { 300 };
		dex::ISR quickISR( "quick", quick );
		dex::vector < unsigned > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 516, 795, 826, 828, 957 };
		dex::ISR brownISR( "brown", brown );
		dex::vector < unsigned > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::ISR foxISR( "fox", fox );
		dex::vector < dex::ISR > isrs;
		isrs.pushBack( quickISR );
		isrs.pushBack( brownISR );
		isrs.pushBack( foxISR );
		dex::ranker judge;
		dex::vector < double > heuristics = { 1, 3, 4, 5 };
		dex::vector < bool > emphasized = { false, false, true };
		dex::vector < unsigned > spans = judge.getDesiredSpans( heuristics, emphasized, isrs, 1 );
		std::cout << "Finished spanning\n";
		unsigned prevHeuristic = 0;
		for ( unsigned i = 0;  i < spans.size( );  ++i )
			{
			std::cout << spans[ i ] << " spans between " << prevHeuristic << " and " << heuristics[ i ] * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ];
			}
		}
	SECTION( "Empty ISR" )
		{
		dex::vector < unsigned > quick;
		dex::ISR quickISR( "quick", quick );
		dex::vector < unsigned > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 516, 795, 826, 828, 957 };
		dex::ISR brownISR( "brown", brown );
		dex::vector < unsigned > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::ISR foxISR( "fox", fox );
		dex::vector < dex::ISR > isrs;
		isrs.pushBack( quickISR );
		isrs.pushBack( brownISR );
		isrs.pushBack( foxISR );
		dex::ranker judge;
		dex::vector < double > heuristics = { 1, 3, 4, 5 };
		dex::vector < bool > emphasized = { false, false, true };
		dex::vector < unsigned > spans = judge.getDesiredSpans( heuristics, emphasized, isrs, 1 );
		std::cout << "Finished spanning\n";
		unsigned prevHeuristic = 0;
		for ( unsigned i = 0;  i < spans.size( );  ++i )
			{
			std::cout << spans[ i ] << " spans between " << prevHeuristic << " and " << heuristics[ i ] * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ];
			}
		}
		
	}