// rankerTests.cpp
// tests for the ranker class
//
// 2019-12-23: Check that we can't span across documents: combsc
// 2019-12-22: Update unsigneds to size_t: combsc
// 2019-12-10 - 14: Wrote everything else: combsc
// 2019-12-09: Init Commit: combsc + lougheem

#include <cstddef>
#include <iostream>
#include "catch.hpp"
#include "constraintSolver/constraintSolver.hpp"
#include "indexer/index.hpp"
#include "ranker/ranker.hpp"
#include "ranker/testingISRs.hpp"

TEST_CASE( "static ranking", "[ranker]" )
	{
	dex::vector < dex::pair < size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	dex::vector < dex::pair < size_t, double > > bodySpanHeuristics;
	dex::vector < dex::pair < size_t, double > > titleSpanHeuristics;
	size_t maxNumBodySpans = 0;
	size_t maxNumTitleSpans = 0;
	double emphasizedWeight = 0;
	double proportionCap = 0;
	double wordsWeight = 1000;

	dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			maxNumBodySpans, maxNumTitleSpans, emphasizedWeight, proportionCap, wordsWeight );

	SECTION( "static title scoring" )
		{
		REQUIRE( judge.rankerStatic.staticScoreTitle( "under15" ) == 50 );
		REQUIRE( judge.rankerStatic.staticScoreTitle( "googlemapsgooglemaps" ) == 40 );
		REQUIRE( judge.rankerStatic.staticScoreTitle( "googlemapsgooglemaps a new paradigm" ) == 20 );
		REQUIRE( judge.rankerStatic.staticScoreTitle( "googlemapsgooglemaps a new paradigm and a bad title" ) == 0 );
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
		for ( size_t i = 0;  i < urls.size( ) - 1;  ++i )
			{
			REQUIRE( judge.rankerStatic.staticScoreUrl( urls[ i ] ) < judge.rankerStatic.staticScoreUrl( urls[ i + 1 ] ) );
			}
		}
	}

TEST_CASE( "begin document" )
	{
	dex::vector < dex::pair < size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	dex::vector < dex::pair < size_t, double > > bodySpanHeuristics;
	dex::vector < dex::pair < size_t, double > > titleSpanHeuristics;
	double emphasizedWeight = 0;
	double proportionCap = 0;
	size_t maxNumBodySpans = 0;
	size_t maxNumTitleSpans = 0;
	double wordsWeight = 1000;

	dex::vector < size_t > ends = { 959, 6000, 7000 };
	dex::rankerTesting::endOfDocumentISR endisr( ends );
	dex::rankerTesting::ISR matchingISR( "", ends, endisr );
	dex::vector < size_t > quick = { 62, 69, 84, 311, 421, 430, 566, 619, 794, 952,
			3500, 5500,
			6500 };
	dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
	dex::vector < size_t > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957,
			3501, 5501,
			6504 };
	dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
	dex::vector < size_t > fox = { 284, 423, 580, 612, 796, 912, 958,
			3502, 5502,
			6508 };
	dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
	size_t beginDocument = 0;
	size_t endDocument = matchingISR.next( );
	REQUIRE( endDocument == 959 );

	endDocument = matchingISR.next( );
	endisr.seek( endDocument );
	beginDocument = endDocument - endisr.documentSize( );
	REQUIRE( endDocument == 6000 );
	REQUIRE( beginDocument == 960 );

	endDocument = matchingISR.next( );
	endisr.seek( endDocument );
	beginDocument = endDocument - endisr.documentSize( );
	REQUIRE( endDocument == 7000 );
	REQUIRE( beginDocument == 6001 );

	}

TEST_CASE( "sudo ISR", "[ranker]" )
	{
	SECTION ( "simple" )
		{
		dex::vector < size_t > ends = { 4001 };
		dex::vector < size_t > postingList = { 3, 42, 82, 1009, 4000 };

		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR isr( "wurd", postingList, endisr );
		size_t location = isr.next( );
		size_t iters = 0;
		while ( location != dex::rankerTesting::ISR::npos )
			{
			REQUIRE( postingList[ iters++ ] == location );
			std::cout << location << ",\t";
			location = isr.next( );
			}
		std::cout << "\n";
		}
	SECTION ( "seeking" )
		{
		dex::vector < size_t > ends = { 4001 };
		dex::vector < size_t > postingList = { 3, 42, 82, 1009, 4000 };

		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR isr( "wurd", postingList, endisr );

		for ( size_t i = 0; i < postingList.size( ) - 1; ++i )
			{
			REQUIRE( isr.seek( postingList[ i ] - 1 ) == postingList[ i ] );
			REQUIRE( isr.next( ) == postingList[ i + 1 ] );
			}

		REQUIRE( isr.seek( postingList[ 4 ] - 1 ) == postingList[ 4 ] );
		REQUIRE( isr.next( ) == size_t( -1 ) );
		REQUIRE( isr.seek( postingList[ 4 ] + 1 ) == size_t( -1 ) );
		REQUIRE( isr.next( ) == size_t( -1 ) );
		}



	}
TEST_CASE( "basic spanning", "[ranker]" )
	{
	dex::vector < dex::pair < size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	dex::vector < dex::pair < size_t, double > > bodySpanHeuristics;
	dex::vector < dex::pair < size_t, double > > titleSpanHeuristics;
	double emphasizedWeight = 0;
	double proportionCap = 0;
	size_t maxNumBodySpans = 0;
	size_t maxNumTitleSpans = 0;
	double wordsWeight = 1000;

	SECTION ( "simple" )
		{
		dex::vector < size_t > end = { 951 };
		dex::rankerTesting::endOfDocumentISR endisr( end );
		dex::rankerTesting::ISR matchingISR( "", end, endisr );
		std::cout << "basic spanning, simple\n";
		dex::vector < size_t > duo = { 1, 3, 900, 950 };
		dex::rankerTesting::ISR duoISR( "duo", duo, endisr );
		dex::vector < size_t > mushu = { 2, 61, 901 };
		dex::rankerTesting::ISR mushuISR( "mushu", mushu, endisr );

		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &duoISR );
		isrs.pushBack( &mushuISR );

		dex::vector < dex::pair < size_t, double > > heuristics = { { 1, 1 }, { 20, 1 }, { 60, 1 } };
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
				maxNumBodySpans, maxNumTitleSpans, emphasizedWeight, proportionCap, wordsWeight );
		dex::vector < dex::vector < size_t > > wordCount;
		dex::vector < dex::string > titles;
		dex::vector < dex::string > urls;
		dex::vector < dex::vector < size_t > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				nullptr, heuristics, 5, wordCount, titles, urls );
		std::cout << "Finished spanning\n";
		size_t prevHeuristic = 0;
		for ( size_t i = 0;  i < spans[ 0 ].size( );  ++i )
			{
			std::cout << spans[ 0 ][ i ] << " spans between " << prevHeuristic + 1 << " and "
					<< heuristics[ i ].first * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ].first * isrs.size( );
			}
		REQUIRE( spans[ 0 ][ 0 ] == 2 );
		REQUIRE( spans[ 0 ][ 1 ] == 0 );
		REQUIRE( spans[ 0 ][ 2 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 4 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 3 );
		}
	SECTION( "Quick Brown Fox" )
		{
		dex::vector < size_t > ends = { 959 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector < size_t > quick = { 62, 69, 84, 311, 421, 430, 566, 619, 794, 952 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::vector < dex::pair < size_t, double > > heuristics = { { 1, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < dex::vector < size_t > > wordCount;
		dex::vector < dex::string > titles;
		dex::vector < dex::string > urls;
		dex::vector < dex::vector < size_t > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				nullptr, heuristics, 5, wordCount, titles, urls );
		std::cout << "Finished spanning\n";
		size_t prevHeuristic = 0;
		for ( size_t i = 0;  i < spans[ 0 ].size( );  ++i )
			{
			std::cout << spans[ 0 ][ i ] << " spans between " << prevHeuristic + 1 << " and "
					<< heuristics[ i ].first * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ].first * isrs.size( );
			}
		REQUIRE( spans[ 0 ][ 0 ] == 2 );
		REQUIRE( spans[ 0 ][ 1 ] == 1 );
		REQUIRE( spans[ 0 ][ 2 ] == 0 );
		REQUIRE( spans[ 0 ][ 3 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 10 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 14 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 7 );
		}
	}

TEST_CASE( "edge cases", "[ranker]" )
	{
	dex::vector < dex::pair < size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	dex::vector < dex::pair < size_t, double > > bodySpanHeuristics;
	dex::vector < dex::pair < size_t, double > > titleSpanHeuristics;
	double emphasizedWeight = 0;
	double proportionCap = 0;
	size_t maxNumBodySpans = 0;
	size_t maxNumTitleSpans = 0;
	double wordsWeight = 1000;


	SECTION( "Short ISR" )
		{
		dex::vector < size_t > ends = { 959 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector < size_t > quick = { 300 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 516, 795, 826, 828, 957 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );
		dex::vector < dex::pair < size_t, double > > heuristics = { { 1, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < dex::vector < size_t > > wordCount;
		dex::vector < dex::string > titles;
		dex::vector < dex::string > urls;
		dex::vector < dex::vector < size_t > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				nullptr, heuristics, 5, wordCount, titles, urls );
		std::cout << "Finished spanning\n";
		size_t prevHeuristic = 0;
		for ( size_t i = 0;  i < spans[ 0 ].size( );  ++i )
			{
			std::cout << spans[ 0 ][ i ] << " spans between " << prevHeuristic << " and "
					<< heuristics[ i ].first * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ].first;
			}
		REQUIRE( spans[ 0 ][ 0 ] == 0 );
		REQUIRE( spans[ 0 ][ 1 ] == 0 );
		REQUIRE( spans[ 0 ][ 2 ] == 0 );
		REQUIRE( spans[ 0 ][ 3 ] == 0 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 14 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 7 );
		}
	SECTION( "Empty ISR" )
		{
		dex::vector < size_t > ends = { 959 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector < size_t > quick;
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 516, 795, 826, 828, 957 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );
		dex::vector < dex::pair < size_t, double > > heuristics = { { 1, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < dex::vector < size_t > > wordCount;
		dex::vector < dex::string > titles;
		dex::vector < dex::string > urls;
		dex::vector < dex::vector < size_t > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				nullptr, heuristics, 5, wordCount, titles, urls );
		std::cout << "Finished spanning\n";
		size_t prevHeuristic = 0;
		for ( size_t i = 0;  i < spans[ 0 ].size( );  ++i )
			{
			std::cout << spans[ 0 ][ i ] << " spans between " << prevHeuristic << " and " <<
					heuristics[ i ].first * isrs.size( ) << std::endl;
			prevHeuristic = heuristics[ i ].first;
			}
		REQUIRE( spans[ 0 ][ 0 ] == 0 );
		REQUIRE( spans[ 0 ][ 1 ] == 0 );
		REQUIRE( spans[ 0 ][ 2 ] == 0 );
		REQUIRE( spans[ 0 ][ 3 ] == 0 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 14 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 7 );
		}

	}

TEST_CASE( "spanning multiple documents" )
	{
	dex::vector < dex::pair < size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	dex::vector < dex::pair < size_t, double > > bodySpanHeuristics;
	dex::vector < dex::pair < size_t, double > > titleSpanHeuristics;
	double emphasizedWeight = 0;
	double proportionCap = 0;
	size_t maxNumBodySpans = 0;
	size_t maxNumTitleSpans = 0;
	double wordsWeight = 1000;



	SECTION( "quick brown fox simple" )
		{
		dex::vector < size_t > ends = { 959, 6000, 7000 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector < size_t > quick = { 62, 69, 84, 311, 421, 430, 566, 619, 794, 952,
				3500, 5500,
				6500 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957,
				3501, 5501,
				6504 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = { 284, 423, 580, 612, 796, 912, 958,
				3502, 5502,
				6508 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::vector < dex::pair < size_t, double > > heuristics = { { 1, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < dex::vector < size_t > > wordCount;
		dex::vector < dex::string > titles;
		dex::vector < dex::string > urls;
		dex::vector < dex::vector < size_t > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				nullptr, heuristics, 5, wordCount, titles, urls );


		REQUIRE( spans[ 0 ][ 0 ] == 2 );
		REQUIRE( spans[ 0 ][ 1 ] == 1 );
		REQUIRE( spans[ 0 ][ 2 ] == 0 );
		REQUIRE( spans[ 0 ][ 3 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 10 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 14 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 7 );

		REQUIRE( spans[ 1 ][ 0 ] == 2 );
		REQUIRE( spans[ 1 ][ 1 ] == 0 );
		REQUIRE( spans[ 1 ][ 2 ] == 0 );
		REQUIRE( spans[ 1 ][ 3 ] == 0 );
		REQUIRE( wordCount[ 1 ][ 0 ] == 2 );
		REQUIRE( wordCount[ 1 ][ 1 ] == 2 );
		REQUIRE( wordCount[ 1 ][ 2 ] == 2 );

		REQUIRE( spans[ 2 ][ 0 ] == 0 );
		REQUIRE( spans[ 2 ][ 1 ] == 1 );
		REQUIRE( spans[ 2 ][ 2 ] == 0 );
		REQUIRE( spans[ 2 ][ 3 ] == 0 );
		REQUIRE( wordCount[ 2 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 2 ] == 1 );
		}

	SECTION( "No Cross Document Spans 1" )
		{
		dex::vector < size_t > ends = { 10, 20, 30 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector < size_t > quick = { 9 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = { 11 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = { 12 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::vector < dex::pair < size_t, double > > heuristics = { { 10, 1 } };
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < dex::vector < size_t > > wordCount;
		dex::vector < dex::string > titles;
		dex::vector < dex::string > urls;
		dex::vector < dex::vector < size_t > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				nullptr, heuristics, 5, wordCount, titles, urls );


		REQUIRE( spans[ 0 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 0 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 0 );

		REQUIRE( spans[ 1 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 1 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 1 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 1 ][ 2 ] == 1 );

		REQUIRE( spans[ 2 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 2 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 2 ][ 1 ] == 0 );
		REQUIRE( wordCount[ 2 ][ 2 ] == 0 );
		}

	SECTION( "No Cross Document Spans 2" )
		{
		dex::vector < size_t > ends = { 10, 20, 30 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector < size_t > quick = { 9,
				19,
				29 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = { 1,
				11,
				21 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = { 2,
				12,
				22 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::vector < dex::pair < size_t, double > > heuristics = { { 2, 1 }, { 4, 1 } };
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < dex::vector < size_t > > wordCount;
		dex::vector < dex::string > titles;
		dex::vector < dex::string > urls;
		dex::vector < dex::vector < size_t > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				nullptr, heuristics, 5, wordCount, titles, urls );


		REQUIRE( spans[ 0 ][ 0 ] == 0 );
		REQUIRE( spans[ 0 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 1 );

		REQUIRE( spans[ 1 ][ 0 ] == 0 );
		REQUIRE( spans[ 1 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 1 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 1 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 1 ][ 2 ] == 1 );

		REQUIRE( spans[ 2 ][ 0 ] == 0 );
		REQUIRE( spans[ 2 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 2 ] == 1 );
		}

		// TODO:: Add a test case where not alll documents match

	SECTION( "quick brown fox someMissing" )
		{
		dex::vector < size_t > ends = { 959, 6000, 7000, 8000, 9000 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector < size_t > quick = {
				62, 69, 84, 311, 421, 430, 566, 619, 794, 952,
				3500, 5500,
				6500,

				8001, 8005, 8010, 8089 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = {
				83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957,
				3501, 5501,
				6504,
				7050, 7060, 7500, 7800,
				8004, 8006, 8020 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = {
				284, 423, 580, 612, 796, 912, 958,
				3502, 5502,
				6508,
				7051, 7061,
				8003, 8008, 8024, 8090, 8100 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::vector < dex::pair < size_t, double > > heuristics = { { 1, 1 }, { 3, 1 }, { 4, 1 }, { 5, 1 } };
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < dex::vector < size_t > > wordCount;
		dex::vector < dex::string > titles;
		dex::vector < dex::string > urls;
		dex::vector < dex::vector < size_t > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				nullptr, heuristics, 5, wordCount, titles, urls );


		REQUIRE( spans[ 0 ][ 0 ] == 2 );
		REQUIRE( spans[ 0 ][ 1 ] == 1 );
		REQUIRE( spans[ 0 ][ 2 ] == 0 );
		REQUIRE( spans[ 0 ][ 3 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 10 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 14 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 7 );

		REQUIRE( spans[ 1 ][ 0 ] == 2 );
		REQUIRE( spans[ 1 ][ 1 ] == 0 );
		REQUIRE( spans[ 1 ][ 2 ] == 0 );
		REQUIRE( spans[ 1 ][ 3 ] == 0 );
		REQUIRE( wordCount[ 1 ][ 0 ] == 2 );
		REQUIRE( wordCount[ 1 ][ 1 ] == 2 );
		REQUIRE( wordCount[ 1 ][ 2 ] == 2 );

		REQUIRE( spans[ 2 ][ 0 ] == 0 );
		REQUIRE( spans[ 2 ][ 1 ] == 1 );
		REQUIRE( spans[ 2 ][ 2 ] == 0 );
		REQUIRE( spans[ 2 ][ 3 ] == 0 );
		REQUIRE( wordCount[ 2 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 2 ] == 1 );

		REQUIRE( spans[ 3 ][ 0 ] == 0 );
		REQUIRE( spans[ 3 ][ 1 ] == 0 );
		REQUIRE( spans[ 3 ][ 2 ] == 0 );
		REQUIRE( spans[ 3 ][ 3 ] == 0 );
		REQUIRE( wordCount[ 3 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 3 ][ 1 ] == 4 );
		REQUIRE( wordCount[ 3 ][ 2 ] == 2 );

		REQUIRE( spans[ 4 ][ 0 ] == 1 );
		REQUIRE( spans[ 4 ][ 1 ] == 1 );
		REQUIRE( spans[ 4 ][ 2 ] == 0 );
		REQUIRE( spans[ 4 ][ 3 ] == 1 );
		REQUIRE( wordCount[ 4 ][ 0 ] == 4 );
		REQUIRE( wordCount[ 4 ][ 1 ] == 3 );
		REQUIRE( wordCount[ 4 ][ 2 ] == 5 );
		}
	}

TEST_CASE( "scoring" )
	{
	dex::vector < dex::pair < size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	dex::vector < dex::pair < size_t, double > > bodySpanHeuristics = { { 1, 50 }, { 3, 25 }, { 4, 20 }, { 5, 5 } };
	dex::vector < dex::pair < size_t, double > > titleSpanHeuristics = { { 1, 250 }, { 2, 50 } };
	double emphasizedWeight = 3;
	double proportionCap = 0.15;
	size_t maxNumBodySpans = 5;
	size_t maxNumTitleSpans = 1;
	double wordsWeight = 1000;


	SECTION( "simple dynamic scoring" )
		{
		dex::vector < size_t > ends = { 1000, 6000, 7000 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector < size_t > quick = {
				62, 69, 84, 311, 421, 430, 566, 619, 794, 952,
				3500, 5500,
				6500 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = {
				83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957,
				3501, 5501,
				6504 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = {
				284, 423, 580, 612, 796, 912, 958,
				3502, 5502,
				6508 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::vector < size_t > titlequick = {
				60,
				2000,
				6500 };
		dex::rankerTesting::ISR titlequickISR( "quick", titlequick, endisr );
		dex::vector < size_t > titlebrown = {
				61,
				2002,
				6504 };
		dex::rankerTesting::ISR titlebrownISR( "brown", titlebrown, endisr );
		dex::vector < size_t > titlefox = {
				62,
				2004,
				6508 };
		dex::rankerTesting::ISR titlefoxISR( "fox", titlefox, endisr );
		dex::vector < dex::constraintSolver::ISR * > titleisrs;
		titleisrs.pushBack( &titlequickISR );
		titleisrs.pushBack( &titlebrownISR );
		titleisrs.pushBack( &titlefoxISR );
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < bool > emphasized = { false, false, false };
		dex::vector < dex::string > titles;
		dex::vector < dex::string > urls;
		dex::vector < double > dynamicScores = judge.rankerDynamic.getDynamicScores( isrs, titleisrs, &matchingISR, &endisr, nullptr,
				emphasized, titles, urls, true );
		std::cout << dynamicScores[ 0 ] << std::endl;
		std::cout << dynamicScores[ 1 ] << std::endl;
		std::cout << dynamicScores[ 2 ] << std::endl;
		REQUIRE( dynamicScores[ 0 ] > dynamicScores[ 1 ] );
		REQUIRE( dynamicScores[ 1 ] > dynamicScores[ 2 ] );
		}


	// TODO: MAKE SURE WE CAN'T SPAN ACROSS DOCUMENTS
	/*
	SECTION( "total scoring" )
		{
		dex::vector < size_t > ends = { 1000, 6000, 7000 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector < dex::Url > urls = { "https://www.good.com", "http://www.good.com/somethingabitlonger", "http://bad.good.bad" };
		dex::vector < size_t > quick = {
				62, 69, 84, 311, 421, 430, 566, 619, 794, 952,
				3500, 5500,
				6500 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = {
				83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957,
				3501, 5501,
				6504 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = {
				284, 423, 580, 612, 796, 912, 958,
				3502, 5502,
				6508 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );
		dex::vector < dex::string > titles = { "good title", "slightly longer but reasonable",
				"much longer and insanely unreasonable by any standard for a title" };
		dex::vector < size_t > titlequick = {
				60,
				2000,
				6500 };
		dex::rankerTesting::ISR titlequickISR( "quick", titlequick, endisr );
		dex::vector < size_t > titlebrown = {
				61,
				2002,
				6504 };
		dex::rankerTesting::ISR titlebrownISR( "brown", titlebrown, endisr );
		dex::vector < size_t > titlefox = {
				62,
				2004,
				6508 };
		dex::rankerTesting::ISR titlefoxISR( "fox", titlefox, endisr );
		dex::vector < dex::constraintSolver::ISR * > titleisrs;
		titleisrs.pushBack( &titlequickISR );
		titleisrs.pushBack( &titlebrownISR );
		titleisrs.pushBack( &titlefoxISR );
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < bool > emphasized = { false, false, false };


		dex::matchedDocuments documentsObject;
		dex::matchedDocuments *documents = &documentsObject;
		documents->matchingDocumentISR = &matchingISR;
		documents->emphasizedWords = emphasized;
		documents->chunk = nullptr;
		dex::vector < dex::string > nulltitles;
		dex::vector < dex::string > nullurls;
		dex::pair < dex::vector < double >, int > totalScoresPair = judge.scoreDocuments( documents, &endisr, nulltitles, nullurls, true );
		dex::vector < double > totalScores = totalScoresPair.first;
		std::cout << totalScores[ 0 ] << std::endl;
		std::cout << totalScores[ 1 ] << std::endl;
		std::cout << totalScores[ 2 ] << std::endl;
		REQUIRE( totalScores[ 0 ] > totalScores[ 1 ] );
		REQUIRE( totalScores[ 1 ] > totalScores[ 2 ] );
		}

	SECTION( "total scoring w/ more ends than matching" )
		{
		dex::vector < size_t > ends = { 1000, 2000, 3000, 4000, 5000, 6000, 7000 };
		dex::vector < size_t > matching = { 1000, 6000, 7000 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", matching, endisr );
		dex::vector < dex::Url > urls = { "https://www.good.com", "http://www.good.com/somethingabitlonger", "http://bad.good.bad" };
		dex::vector < size_t > quick = {
				62, 69, 84, 311, 421, 430, 566, 619, 794, 952,
				3500, 5500,
				6500 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector < size_t > brown = {
				83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957,
				3501, 5501,
				6504 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector < size_t > fox = {
				284, 423, 580, 612, 796, 912, 958,
				3502, 5502,
				6508 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector < dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );
		dex::vector < dex::string > titles = { "good title", "slightly longer but reasonable",
				"much longer and insanely unreasonable by any standard for a title" };
		dex::vector < size_t > titlequick = {
				60,
				2000,
				6500 };
		dex::rankerTesting::ISR titlequickISR( "quick", titlequick, endisr );
		dex::vector < size_t > titlebrown = {
				61,
				2002,
				6504 };
		dex::rankerTesting::ISR titlebrownISR( "brown", titlebrown, endisr );
		dex::vector < size_t > titlefox = {
				62,
				2004,
				6508 };
		dex::rankerTesting::ISR titlefoxISR( "fox", titlefox, endisr );
		dex::vector < dex::constraintSolver::ISR * > titleisrs;
		titleisrs.pushBack( &titlequickISR );
		titleisrs.pushBack( &titlebrownISR );
		titleisrs.pushBack( &titlefoxISR );
		dex::ranker::ranker judge( titleWeights, urlWeight, bodySpanHeuristics, titleSpanHeuristics,
			emphasizedWeight, proportionCap, maxNumBodySpans, maxNumTitleSpans, wordsWeight );
		dex::vector < bool > emphasized = { false, false, false };


		dex::matchedDocuments documents;
		documents.titleISRs = titleisrs;
		documents.bodyISRs = isrs;
		documents.matchingDocumentISR = &matchingISR;
		documents.emphasizedWords = emphasized;
		documents.urls = urls;
		documents.titles = titles;
		documents.chunk = nullptr;
		dex::vector < dex::string > nulltitles;
		dex::vector < dex::string > nullurls;
		dex::pair < dex::vector < double >, int > totalScoresPair = judge.scoreDocuments( documents, &endisr, nulltitles, nullurls, true );
		dex::vector < double > totalScores = totalScoresPair.first;
		std::cout << totalScores[ 0 ] << std::endl;
		std::cout << totalScores[ 1 ] << std::endl;
		std::cout << totalScores[ 2 ] << std::endl;
		REQUIRE( totalScores[ 0 ] > totalScores[ 1 ] );
		REQUIRE( totalScores[ 1 ] > totalScores[ 2 ] );
		}
	*/
	}
