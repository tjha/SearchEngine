// rankerTests.cpp
// tests for the ranker class
//
// 2019-12-09: Init Commit: combsc + lougheem

#include "catch.hpp"
#include "../src/ranker/ranker.hpp"
#include <iostream>

TEST_CASE( "static ranking", "[ranker]" )
	{
	dex::vector < dex::pair < unsigned, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	dex::vector < dex::pair < unsigned, double > > bodySpanHeuristics;
	dex::vector < dex::pair < unsigned, double > > titleSpanHeuristics;
	double emphasizedWeight = 0;
	double proportionCap = 0;
	unsigned maxNumBodySpans = 0;
	unsigned maxNumTitleSpans = 0;
	dex::vector < dex::indexChunkObject * > someChunks;
	dex::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics, 
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, someChunks );

	SECTION( "static title scoring" )
		{
		REQUIRE( judge.staticScoreTitle( "under15" ) == 50 );
		REQUIRE( judge.staticScoreTitle( "googlemapsgooglemaps" ) == 40 );
		REQUIRE( judge.staticScoreTitle( "googlemapsgooglemaps a new paradigm" ) == 20 );
		REQUIRE( judge.staticScoreTitle( "googlemapsgooglemaps a new paradigm and a bad title" ) == 0 );
		}
	
	SECTION( "static url scoring" )
		{
		dex::vector < dex::Url > urls;
		urls.pushBack( "http://scam.reallylongwebsitenameholymoly.biz/a/b/c/d/e/f/g?query=1#fragment" );
		urls.pushBack( "https://scam.reallylongwebsitenameholymoly.biz/a/b/c/d/e/f/g?query=1#fragment" );
		urls.pushBack( "https://www.reallylongwebsitenameholymoly.biz/a/b/c/d/e/f/g?query=1#fragment" );
		urls.pushBack( "https://www.reallylongwebsitenameholymoly.biz/a/b/c/d/e/f/g" );
		urls.pushBack( "https://www.reallylongwebsitenameholymoly.biz/a/b" );
		urls.pushBack( "https://www.reasonable.biz/a/b" );
		urls.pushBack( "https://www.reasonable.edu/a/b" );
		for ( unsigned i = 0;  i < urls.size( ) - 1;  ++i )
			{
			REQUIRE( judge.staticScoreUrl( urls[ i ] ) < judge.staticScoreUrl( urls[ i + 1 ] ) );
			}
		}
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
	dex::vector < dex::pair < unsigned, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	dex::vector < dex::pair < unsigned, double > > bodySpanHeuristics;
	dex::vector < dex::pair < unsigned, double > > titleSpanHeuristics;
	double emphasizedWeight = 0;
	double proportionCap = 0;
	unsigned maxNumBodySpans = 0;
	unsigned maxNumTitleSpans = 0;
	dex::vector < dex::indexChunkObject * > someChunks;
	
	
	SECTION ( "simple" )
		{
		std::cout << "basic spanning, simple\n";
		dex::vector < unsigned > duo = { 1, 3, 900 };
		dex::ISR duoISR( "duo", duo );
		dex::vector < unsigned > mushu = { 2, 61, 901 };
		dex::ISR mushuISR( "mushu", mushu );

		dex::vector < dex::ISR > isrs;
		isrs.pushBack( duoISR );
		isrs.pushBack( mushuISR );
		
		dex::vector < dex::pair < unsigned, double > > heuristics = { { 1, 1 }, { 20, 1 }, { 60, 1 } };
		dex::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics, 
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, someChunks );
		dex::vector < unsigned > wordCount;
		dex::vector < unsigned > spans = judge.getDesiredSpans( isrs, 1, heuristics, 5, wordCount );
		std::cout << "Finished spanning\n";
		unsigned prevHeuristic = 0;
		for ( unsigned i = 0;  i < spans.size( );  ++i )
			{
			std::cout << spans[ i ] << " spans between " << prevHeuristic + 1 << " and " << heuristics[ i ].first * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ].first * isrs.size( );
			}
		REQUIRE( spans[ 0 ] == 2 );
		REQUIRE( spans[ 1 ] == 0 );
		REQUIRE( spans[ 2 ] == 1 );
		REQUIRE( wordCount[ 0 ] == 3 );
		REQUIRE( wordCount[ 1 ] == 3 );
		}
	SECTION( "Quick Brown Fox" )
		{
		dex::vector < unsigned > quick = { 62, 69, 84, 311, 421, 430, 566, 619, 794, 952 };
		dex::ISR quickISR( "quick", quick );
		dex::vector < unsigned > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957 };
		dex::ISR brownISR( "brown", brown );
		dex::vector < unsigned > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::ISR foxISR( "fox", fox );
		dex::vector < dex::ISR > isrs;
		isrs.pushBack( quickISR );
		isrs.pushBack( brownISR );
		isrs.pushBack( foxISR );
		dex::vector < dex::pair < unsigned, double > > heuristics = { { 1, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
		dex::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics, 
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, someChunks );
		dex::vector < unsigned > wordCount;
		dex::vector < unsigned > spans = judge.getDesiredSpans( isrs, 1, heuristics, 5, wordCount );
		std::cout << "Finished spanning\n";
		unsigned prevHeuristic = 0;
		for ( unsigned i = 0;  i < spans.size( );  ++i )
			{
			std::cout << spans[ i ] << " spans between " << prevHeuristic + 1 << " and " << heuristics[ i ].first * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ].first * isrs.size( );
			}
		REQUIRE( spans[ 0 ] == 2 );
		REQUIRE( spans[ 1 ] == 2 );
		REQUIRE( spans[ 2 ] == 0 );
		REQUIRE( spans[ 3 ] == 1 );
		REQUIRE( wordCount[ 0 ] == 10 );
		REQUIRE( wordCount[ 1 ] == 14 );
		REQUIRE( wordCount[ 2 ] == 7 );
		}
	}

TEST_CASE( "edge cases", "[ranker]" )
	{
	dex::vector < dex::pair < unsigned, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	dex::vector < dex::pair < unsigned, double > > bodySpanHeuristics;
	dex::vector < dex::pair < unsigned, double > > titleSpanHeuristics;
	double emphasizedWeight = 0;
	double proportionCap = 0;
	unsigned maxNumBodySpans = 0;
	unsigned maxNumTitleSpans = 0;
	dex::vector < dex::indexChunkObject * > someChunks;
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
		dex::vector < dex::pair < unsigned, double > > heuristics = { { 1, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
		dex::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics, 
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, someChunks );
		dex::vector < unsigned > wordCount;
		dex::vector < unsigned > spans = judge.getDesiredSpans( isrs, 1, heuristics, 5, wordCount );
		std::cout << "Finished spanning\n";
		unsigned prevHeuristic = 0;
		for ( unsigned i = 0;  i < spans.size( );  ++i )
			{
			std::cout << spans[ i ] << " spans between " << prevHeuristic << " and " << heuristics[ i ].first * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ].first;
			}
		REQUIRE( spans[ 0 ] == 0 );
		REQUIRE( spans[ 1 ] == 0 );
		REQUIRE( spans[ 2 ] == 0 );
		REQUIRE( spans[ 3 ] == 0 );
		REQUIRE( wordCount[ 0 ] == 1 );
		REQUIRE( wordCount[ 1 ] == 14 );
		REQUIRE( wordCount[ 2 ] == 7 );
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
		dex::vector < dex::pair < unsigned, double > > heuristics = { { 1, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
		dex::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics, 
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, someChunks );
		dex::vector < unsigned > wordCount;
		dex::vector < unsigned > spans = judge.getDesiredSpans( isrs, 1, heuristics, 5, wordCount );
		std::cout << "Finished spanning\n";
		unsigned prevHeuristic = 0;
		for ( unsigned i = 0;  i < spans.size( );  ++i )
			{
			std::cout << spans[ i ] << " spans between " << prevHeuristic << " and " << heuristics[ i ].first * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ].first;
			}
		REQUIRE( spans[ 0 ] == 0 );
		REQUIRE( spans[ 1 ] == 0 );
		REQUIRE( spans[ 2 ] == 0 );
		REQUIRE( spans[ 3 ] == 0 );
		REQUIRE( wordCount[ 0 ] == 0 );
		REQUIRE( wordCount[ 1 ] == 14 );
		REQUIRE( wordCount[ 2 ] == 7 );
		}
		
	}