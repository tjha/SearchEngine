// rankerTests.cpp
// tests for the ranker class
//
// 2020-01-01: Changed getDesiredSpans, so change literally everything: combsc
// 2019-12-23: Check that we can't span across documents: combsc
// 2019-12-22: Update unsigneds to size_t: combsc
// 2019-12-10 - 14: Wrote everything else: combsc
// 2019-12-09: Init Commit: combsc + lougheem

#include <cstddef>
#include <iostream>
#include "catch.hpp"
#include "constraintSolver/constraintSolver.hpp"
#include "indexer/indexer.hpp"
#include "ranker/ranker.hpp"
#include "ranker/testingISRs.hpp"
#include "utils/file.hpp"

TEST_CASE( "score", "[ranker]" )
	{
	dex::ranker::score score;
	REQUIRE( score.getTotalScore( ) == 0 );
	score.setStaticUrlScore( 15 );
	REQUIRE( score.getTotalScore( ) == 15 );
	REQUIRE( score.staticUrlScore == 15 );
	score.setStaticUrlScore( 10 );
	REQUIRE( score.getTotalScore( ) == 10 );
	REQUIRE( score.staticUrlScore == 10 );
	}

TEST_CASE( "static ranking", "[ranker]" )
	{
	dex::vector< dex::pair< size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	double maxBodySpanScore = 0;
	double maxTitleSpanScore = 0;
	double emphasizedWeight = 0;
	double maxBagOfWordsScore = 0;
	double wordsWeight = 1000;

	dex::ranker::ranker judge( 3, titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );

	SECTION( "static title scoring" )
		{
		REQUIRE( judge.rankerStatic.staticScoreTitle( "under15" ) == 50 );
		REQUIRE( judge.rankerStatic.staticScoreTitle( "googlemapsgooglemaps" ) == 40 );
		REQUIRE( judge.rankerStatic.staticScoreTitle( "googlemapsgooglemaps a new paradigm" ) == 20 );
		REQUIRE( judge.rankerStatic.staticScoreTitle( "googlemapsgooglemaps a new paradigm and a bad title" ) == 0 );
		}

	SECTION( "static url scoring" )
		{
		dex::vector< dex::Url > urls;
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
	dex::vector< size_t > ends = { 959, 6000, 7000 };
	dex::rankerTesting::endOfDocumentISR endisr( ends );
	dex::rankerTesting::ISR matchingISR( "", ends, endisr );
	dex::vector< size_t > quick = { 62, 69, 84, 311, 421, 430, 566, 619, 794, 952,
			3500, 5500,
			6500 };
	dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
	dex::vector< size_t > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957,
			3501, 5501,
			6504 };
	dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
	dex::vector< size_t > fox = { 284, 423, 580, 612, 796, 912, 958,
			3502, 5502,
			6508 };
	dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
	size_t beginDocument = 0;
	size_t endDocument = matchingISR.next( );
	REQUIRE( endDocument == 959 );

	endDocument = matchingISR.next( );
	endisr.seek( endDocument );
	beginDocument = endDocument - endisr.documentSize( ) + 1;
	REQUIRE( endDocument == 6000 );
	REQUIRE( beginDocument == 960 );

	endDocument = matchingISR.next( );
	endisr.seek( endDocument );
	beginDocument = endDocument - endisr.documentSize( ) + 1;
	REQUIRE( endDocument == 7000 );
	REQUIRE( beginDocument == 6001 );

	}

TEST_CASE( "sudo ISR", "[ranker]" )
	{
	SECTION ( "simple" )
		{
		dex::vector< size_t > ends = { 4001 };
		dex::vector< size_t > postingList = { 3, 42, 82, 1009, 4000 };

		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR isr( "wurd", postingList, endisr );
		size_t location = isr.next( );
		size_t iters = 0;
		while ( location != dex::rankerTesting::ISR::npos )
			{
			REQUIRE( postingList[ iters++ ] == location );
			location = isr.next( );
			}
		}
	SECTION ( "seeking" )
		{
		dex::vector< size_t > ends = { 4001 };
		dex::vector< size_t > postingList = { 3, 42, 82, 1009, 4000 };

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

	SECTION ( "get" )
		{
		dex::vector< size_t > ends = { 50, 100, 150, 200 };
		dex::vector< size_t > postingList = { 3, 42, 82, 101, 103, 105, 199 };

		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR isr( "wurd", postingList, endisr );

		for ( size_t i = 0; i < postingList.size( ) - 1; ++i )
			{
			REQUIRE( isr.seek( postingList[ i ] - 1 ) == postingList[ i ] );
			REQUIRE( isr.get( ) == postingList[ i ] );
			REQUIRE( isr.next( ) == postingList[ i + 1 ] );
			REQUIRE( isr.get( ) == postingList[ i + 1 ] );
			}

		REQUIRE( isr.seek( postingList[ 6 ] ) == postingList[ 6 ] );
		REQUIRE( isr.get( ) == postingList[ 6 ] );
		REQUIRE( isr.next( ) == size_t( -1 ) );
		REQUIRE( isr.get( ) == size_t( -1 ) );
		REQUIRE( isr.seek( postingList[ 6 ] + 1 ) == size_t( -1 ) );
		REQUIRE( isr.get( ) == size_t( -1 ) );
		REQUIRE( isr.next( ) == size_t( -1 ) );
		REQUIRE( isr.get( ) == size_t( -1 ) );
		REQUIRE( isr.seek( 0 ) == postingList[ 0 ] );
		REQUIRE( isr.get( ) == postingList[ 0 ] );
		}
	}

TEST_CASE( "kendall's tau", "[ranker]" )
	{
	dex::vector< dex::pair< size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	double emphasizedWeight = 0;
	double maxBagOfWordsScore = 0;
	double maxBodySpanScore = 0;
	double maxTitleSpanScore = 0;
	double wordsWeight = 1000;
	dex::ranker::ranker judge( 3, titleWeights, urlWeight,
				maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
	dex::vector< size_t > ordering0 = { 0, 1, 2, 3 };
	REQUIRE( judge.rankerDynamic.kendallsTau( ordering0 ) == 1 );
	dex::vector< size_t > ordering1 = { 3, 2, 1, 0 };
	REQUIRE( judge.rankerDynamic.kendallsTau( ordering1 ) ==  -1 );
	dex::vector< size_t > ordering2 = { 0, 10, 20, 30, 40 };
	REQUIRE( judge.rankerDynamic.kendallsTau( ordering2 ) ==  1 );
	dex::vector< size_t > ordering3 = { 30, 29, 1, 0 };
	REQUIRE( judge.rankerDynamic.kendallsTau( ordering3 ) ==  -1 );

	dex::vector< size_t > ordering4 = { 4, 1, 2, 3 };
	REQUIRE( judge.rankerDynamic.kendallsTau( ordering4 ) ==  0 );
	dex::vector< size_t > ordering5 = { 1, 2, 4, 3 };
	REQUIRE( judge.rankerDynamic.kendallsTau( ordering5 ) ==  2.0/3 );
	}

TEST_CASE( "basic spanning", "[ranker]" )
	{
	dex::vector< dex::pair< size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	double emphasizedWeight = 0;
	double maxBagOfWordsScore = 0;
	double maxBodySpanScore = 0;
	double maxTitleSpanScore = 0;
	double wordsWeight = 1000;

	SECTION ( "simple" )
		{
		dex::vector< size_t > end = { 951 };
		dex::rankerTesting::endOfDocumentISR endisr( end );
		dex::rankerTesting::ISR matchingISR( "", end, endisr );
		dex::vector< size_t > duo = { 1, 3, 900, 950 };
		dex::rankerTesting::ISR duoISR( "duo", duo, endisr );
		dex::vector< size_t > mushu = { 2, 61, 901 };
		dex::rankerTesting::ISR mushuISR( "mushu", mushu, endisr );

		dex::vector< dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &duoISR );
		isrs.pushBack( &mushuISR );
		dex::ranker::ranker judge( 3, titleWeights, urlWeight,
				maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
		dex::vector< dex::vector< size_t > > wordCount;
		dex::vector< dex::string > titles;
		dex::vector< dex::Url > urls;
		wordCount = judge.rankerDynamic.getDocumentInfo( isrs, &matchingISR, &endisr, nullptr, titles, urls );
		dex::vector< dex::vector< dex::pair< size_t, double > > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				wordCount );
		REQUIRE( spans[ 0 ][ 0 ].first == 2 );
		REQUIRE( spans[ 0 ][ 0 ].second == 1 );
		REQUIRE( spans[ 0 ][ 1 ].first == 59 );
		REQUIRE( spans[ 0 ][ 1 ].second == 1 );
		REQUIRE( spans[ 0 ][ 2 ].first == 2 );
		REQUIRE( spans[ 0 ][ 2 ].second == 1 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 4 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 3 );
		}
	SECTION( "Quick Brown Fox" )
		{
		dex::vector< size_t > ends = { 959 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector< size_t > quick = { 62, 69, 84, 311, 421, 430, 566, 619, 794, 952 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector< size_t > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector< size_t > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector< dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::ranker::ranker judge( 3, titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
		dex::vector< dex::vector< size_t > > wordCount;
		dex::vector< dex::string > titles;
		dex::vector< dex::Url > urls;
		wordCount = judge.rankerDynamic.getDocumentInfo( isrs, &matchingISR, &endisr, nullptr, titles, urls );
		dex::vector< dex::vector< dex::pair< size_t, double > > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				wordCount );
		REQUIRE( spans[ 0 ][ 0 ].first == 85 );
		REQUIRE( spans[ 0 ][ 0 ].second == -1.0/3 );
		REQUIRE( spans[ 0 ][ 1 ].first == 3 );
		REQUIRE( spans[ 0 ][ 1 ].second == 1 );
		REQUIRE( spans[ 0 ][ 2 ].first == 15 );
		REQUIRE( spans[ 0 ][ 2 ].second == 1 );
		REQUIRE( spans[ 0 ][ 3 ].first == 45 );
		REQUIRE( spans[ 0 ][ 3 ].second == -1.0/3 );
		REQUIRE( spans[ 0 ][ 4 ].first == 3 );
		REQUIRE( spans[ 0 ][ 4 ].second == 1 );
		REQUIRE( spans[ 0 ][ 5 ].first == 46 );
		REQUIRE( spans[ 0 ][ 5 ].second == -1.0/3 );
		REQUIRE( spans[ 0 ][ 6 ].first == 7 );
		REQUIRE( spans[ 0 ][ 6 ].second == 1 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 10 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 14 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 7 );
		}
	}
	
TEST_CASE( "edge cases", "[ranker]" )
	{
	dex::vector< dex::pair< size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	double emphasizedWeight = 0;
	double maxBagOfWordsScore = 0;
	double maxBodySpanScore = 0;
	double maxTitleSpanScore = 0;
	double wordsWeight = 1000;


	SECTION( "Short ISR" )
		{
		dex::vector< size_t > ends = { 959 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector< size_t > quick = { 300 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector< size_t > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 516, 795, 826, 828, 957 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector< size_t > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector< dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );
		dex::vector< size_t > heuristics = { 1, 3, 4, 5 };
		dex::ranker::ranker judge( 3, titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
		dex::vector< dex::vector< size_t > > wordCount;
		dex::vector< dex::string > titles;
		dex::vector< dex::Url > urls;
		wordCount = judge.rankerDynamic.getDocumentInfo( isrs, &matchingISR, &endisr, nullptr, titles, urls );
		dex::vector< dex::vector< dex::pair< size_t, double > > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				wordCount );

		REQUIRE( spans[ 0 ][ 0 ].first == 74 );
		REQUIRE( spans[ 0 ][ 0 ].second == -1.0/3 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 14 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 7 );
		}
	SECTION( "Empty ISR" )
		{
		dex::vector< size_t > ends = { 959 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector< size_t > quick;
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector< size_t > brown = { 83, 94, 170, 179, 216, 227, 400, 417, 422, 516, 795, 826, 828, 957 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector< size_t > fox = { 284, 423, 580, 612, 796, 912, 958 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector< dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );
		dex::vector< size_t > heuristics = { 1, 3, 4, 5 };
		dex::ranker::ranker judge( 3, titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
		dex::vector< dex::vector< size_t > > wordCount;
		dex::vector< dex::string > titles;
		dex::vector< dex::Url > urls;
		wordCount = judge.rankerDynamic.getDocumentInfo( isrs, &matchingISR, &endisr, nullptr, titles, urls );
		dex::vector< dex::vector< dex::pair< size_t, double > > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				wordCount );
		REQUIRE( spans.size( ) == 1 );
		REQUIRE( spans[ 0 ].size( ) == 0 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 14 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 7 );
		}

	}

TEST_CASE( "spanning multiple documents" )
	{
	dex::vector< dex::pair< size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	double emphasizedWeight = 0;
	double maxBagOfWordsScore = 0;
	double maxBodySpanScore = 0;
	double maxTitleSpanScore = 0;
	double wordsWeight = 1000;



	SECTION( "quick brown fox simple" )
		{
		dex::vector< size_t > ends = { 959, 6000, 7000 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector< size_t > quick = { 311, 421, 430,
				3500, 5500,
				6500 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector< size_t > brown = { 227, 400, 422,
				3501, 5502,
				6504 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector< size_t > fox = { 284, 423,
				3502, 5501,
				6508 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector< dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::vector< size_t > heuristics = { 1, 3, 4, 5 };
		dex::ranker::ranker judge( 3, titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
		dex::vector< dex::vector< size_t > > wordCount;
		dex::vector< dex::string > titles;
		dex::vector< dex::Url > urls;
		wordCount = judge.rankerDynamic.getDocumentInfo( isrs, &matchingISR, &endisr, nullptr, titles, urls );
		dex::vector< dex::vector< dex::pair< size_t, double > > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				wordCount );


		REQUIRE( spans[ 0 ][ 0 ].first == 85 );
		REQUIRE( spans[ 0 ][ 0 ].second == -1.0/3 );
		REQUIRE( spans[ 0 ][ 1 ].first == 3 );
		REQUIRE( spans[ 0 ][ 1 ].second == 1 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 3 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 3 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 2 );

		REQUIRE( spans[ 1 ][ 0 ].first == 3 );
		REQUIRE( spans[ 1 ][ 0 ].second == 1 );
		REQUIRE( spans[ 1 ][ 1 ].first == 3 );
		REQUIRE( spans[ 1 ][ 1 ].second == 1.0/3 );
		REQUIRE( wordCount[ 1 ][ 0 ] == 2 );
		REQUIRE( wordCount[ 1 ][ 1 ] == 2 );
		REQUIRE( wordCount[ 1 ][ 2 ] == 2 );

		REQUIRE( spans[ 2 ][ 0 ].first == 9 );
		REQUIRE( spans[ 2 ][ 0 ].second == 1 );
		REQUIRE( wordCount[ 2 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 2 ] == 1 );
		}

	SECTION( "No Cross Document Spans 1" )
		{
		dex::vector< size_t > ends = { 10, 20, 30 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector< size_t > quick = { 9 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector< size_t > brown = { 11 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector< size_t > fox = { 12 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector< dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::ranker::ranker judge( 3, titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
		dex::vector< dex::vector< size_t > > wordCount;
		dex::vector< dex::string > titles;
		dex::vector< dex::Url > urls;
		wordCount = judge.rankerDynamic.getDocumentInfo( isrs, &matchingISR, &endisr, nullptr, titles, urls );
		dex::vector< dex::vector< dex::pair< size_t, double > > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				wordCount );


		REQUIRE( spans.size( ) == 3 );

		REQUIRE( spans[ 0 ].size( ) == 0 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 0 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 0 );

		REQUIRE( spans[ 1 ].size( ) == 0 );
		REQUIRE( wordCount[ 1 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 1 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 1 ][ 2 ] == 1 );

		REQUIRE( spans[ 2 ].size( ) == 0 );
		REQUIRE( wordCount[ 2 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 2 ][ 1 ] == 0 );
		REQUIRE( wordCount[ 2 ][ 2 ] == 0 );
		}

	SECTION( "No Cross Document Spans 2" )
		{
		dex::vector< size_t > ends = { 10, 20, 30 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector< size_t > quick = { 9,
				19,
				29 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector< size_t > brown = { 1,
				11,
				21 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector< size_t > fox = { 2,
				12,
				22 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector< dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::ranker::ranker judge( 3, titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
		dex::vector< dex::vector< size_t > > wordCount;
		dex::vector< dex::string > titles;
		dex::vector< dex::Url > urls;
		wordCount = judge.rankerDynamic.getDocumentInfo( isrs, &matchingISR, &endisr, nullptr, titles, urls );
		dex::vector< dex::vector< dex::pair< size_t, double > > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				wordCount );


		REQUIRE( spans[ 0 ][ 0 ].first == 9 );
		REQUIRE( spans[ 0 ][ 0 ].second == -1.0/3 );
		REQUIRE( wordCount[ 0 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 0 ][ 2 ] == 1 );

		REQUIRE( spans[ 1 ][ 0 ].first == 9 );
		REQUIRE( spans[ 1 ][ 0 ].second == -1.0/3 );
		REQUIRE( wordCount[ 1 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 1 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 1 ][ 2 ] == 1 );

		REQUIRE( spans[ 2 ][ 0 ].first == 9 );
		REQUIRE( spans[ 2 ][ 0 ].second == -1.0/3 );
		REQUIRE( wordCount[ 2 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 2 ] == 1 );
		}

	SECTION( "quick brown fox someMissing" )
		{
		dex::vector< size_t > ends = { 959, 6000, 7000, 8000, 9000 };
		dex::rankerTesting::endOfDocumentISR endisr( ends );
		dex::rankerTesting::ISR matchingISR( "", ends, endisr );
		dex::vector< size_t > quick = {
				952,

				6500,

				8001, 8005, 8010, 8089 };
		dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
		dex::vector< size_t > brown = {
				828, 957,
				3501, 5501,
				6504,
				7050, 7060, 7500, 7800,
				8004, 8006, 8020 };
		dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
		dex::vector< size_t > fox = {
				912, 958,
				3502, 5502,
				6508,
				7051, 7061,
				8002 };
		dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
		dex::vector< dex::constraintSolver::ISR * > isrs;
		isrs.pushBack( &quickISR );
		isrs.pushBack( &brownISR );
		isrs.pushBack( &foxISR );

		dex::vector< size_t > heuristics = { 1, 3, 4, 5 };
		dex::ranker::ranker judge( 3, titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
		dex::vector< dex::vector< size_t > > wordCount;
		dex::vector< dex::string > titles;
		dex::vector< dex::Url > urls;
		wordCount = judge.rankerDynamic.getDocumentInfo( isrs, &matchingISR, &endisr, nullptr, titles, urls );
		dex::vector< dex::vector< dex::pair< size_t, double > > > spans = judge.rankerDynamic.getDesiredSpans( isrs, &matchingISR, &endisr,
				wordCount );


		REQUIRE( spans[ 0 ][ 0 ].first == 7 );
		REQUIRE( spans[ 0 ][ 0 ].second == 1 );

		REQUIRE( spans[ 1 ].size( ) == 0 );
		REQUIRE( wordCount[ 1 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 1 ][ 1 ] == 2 );
		REQUIRE( wordCount[ 1 ][ 2 ] == 2 );

		REQUIRE( spans[ 2 ][ 0 ].first == 9 );
		REQUIRE( spans[ 2 ][ 0 ].second == 1 );
		REQUIRE( wordCount[ 2 ][ 0 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 1 ] == 1 );
		REQUIRE( wordCount[ 2 ][ 2 ] == 1 );

		REQUIRE( spans[ 3 ].size( ) == 0 );
		REQUIRE( wordCount[ 3 ][ 0 ] == 0 );
		REQUIRE( wordCount[ 3 ][ 1 ] == 4 );
		REQUIRE( wordCount[ 3 ][ 2 ] == 2 );

		REQUIRE( spans[ 4 ][ 0 ].first == 4 );
		REQUIRE( spans[ 4 ][ 0 ].second == 1.0/3 );
		REQUIRE( wordCount[ 4 ][ 0 ] == 4 );
		REQUIRE( wordCount[ 4 ][ 1 ] == 3 );
		REQUIRE( wordCount[ 4 ][ 2 ] == 1 );
		}
	}

TEST_CASE( "scoring" )
	{
	dex::vector< dex::pair< size_t, double > > titleWeights = { { 15, 50 }, { 25, 40 }, { 50, 20 } };
	double urlWeight = 1;
	double emphasizedWeight = 3;
	double maxBagOfWordsScore = 100;
	size_t maxBodySpanScore = 100;
	size_t maxTitleSpanScore = 100;
	double wordsWeight = 1000;
	double maxUrlScore = 500;

	SECTION( "url scores" )
		{
		dex::ranker::ranker judge( static_cast< size_t > ( -1 ), titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight, maxUrlScore );
		
		dex::Url url = "https://www.washingtonpost.com/";
		dex::vector< dex::string > flattenedQuery = { "Washingtonpost" };
		REQUIRE( judge.rankerDynamic.scoreUrl( flattenedQuery, url ) == maxUrlScore );
		flattenedQuery = { "Washingtonpost.com" };
		REQUIRE( judge.rankerDynamic.scoreUrl( flattenedQuery, url ) == maxUrlScore );
		flattenedQuery = { "Washington", "Post" };
		REQUIRE( judge.rankerDynamic.scoreUrl( flattenedQuery, url ) == maxUrlScore );
		flattenedQuery = { "Washington" , "Post", ".Com" };
		REQUIRE( judge.rankerDynamic.scoreUrl( flattenedQuery, url ) == maxUrlScore );

		flattenedQuery = { "I", "love", "cooking", "mapo", "tofu", "recipe" };
		dex::vector< dex::Url > urls;
		urls.pushBack( "https://www.ilovecookingmapotofurecipe.com/" );
		urls.pushBack( "https://www.ilovecookingmapo.com/tofurecipe" );
		urls.pushBack( "https://www.ilovecooking.com/chinese/amazing-mapo-tofu-recipe" );
		urls.pushBack( "https://www.ilovecooking.com/korean/tofu-stew-recipe" );
		urls.pushBack( "https://www.ilovecooking.com/japanese/simple-udon-recipe" );
		urls.pushBack( "https://www.amytriescooking.com/new-tofu-trend" );
		urls.pushBack( "https://www.randomforum.com/unrelated-information" );
		for ( size_t i = 0;  i < urls.size( );  ++i )
			REQUIRE( judge.rankerDynamic.scoreUrl( flattenedQuery, urls[ i ] ) > judge.rankerDynamic.scoreUrl( flattenedQuery, urls[ i + 1] ) );
		}
	dex::vector< size_t > ends = { 1000, 6000, 7000 };
	dex::rankerTesting::endOfDocumentISR endisr( ends );
	dex::rankerTesting::ISR matchingISR( "", ends, endisr );
	dex::vector< size_t > quick = {
			62, 69, 84, 311, 421, 430, 566, 619, 794, 952,
			3500, 5500,
			6500 };
	dex::rankerTesting::ISR quickISR( "quick", quick, endisr );
	dex::vector< size_t > brown = {
			83, 94, 170, 179, 216, 227, 400, 417, 422, 575, 795, 826, 828, 957,
			3501, 5501,
			6504 };
	dex::rankerTesting::ISR brownISR( "brown", brown, endisr );
	dex::vector< size_t > fox = {
			284, 423, 580, 612, 796, 912, 958,
			3502, 5502,
			6508 };
	dex::rankerTesting::ISR foxISR( "fox", fox, endisr );
	dex::vector< dex::constraintSolver::ISR * > isrs;
	isrs.pushBack( &quickISR );
	isrs.pushBack( &brownISR );
	isrs.pushBack( &foxISR );

	dex::vector< size_t > titlequick = {
			60,
			2000,
			6500 };
	dex::rankerTesting::ISR titlequickISR( "quick", titlequick, endisr );
	dex::vector< size_t > titlebrown = {
			61,
			2002,
			6504 };
	dex::rankerTesting::ISR titlebrownISR( "brown", titlebrown, endisr );
	dex::vector< size_t > titlefox = {
			62,
			2004,
			6508 };
	dex::rankerTesting::ISR titlefoxISR( "fox", titlefox, endisr );
	dex::vector< dex::constraintSolver::ISR * > titleisrs;
	titleisrs.pushBack( &titlequickISR );
	titleisrs.pushBack( &titlebrownISR );
	titleisrs.pushBack( &titlefoxISR );

	SECTION( "simple dynamic scoring" )
		{
		
		dex::ranker::ranker judge( 3, titleWeights, urlWeight,
			maxBodySpanScore, maxTitleSpanScore, emphasizedWeight, maxBagOfWordsScore, wordsWeight );
		dex::vector< bool > emphasized = { false, false, false };
		dex::vector< dex::string > titles;
		dex::vector< dex::Url > urls;
		dex::vector< dex::ranker::score > dynamicScores = judge.rankerDynamic.getDynamicScores( isrs, titleisrs, &matchingISR, &endisr, nullptr,
				emphasized, titles, urls, { "quick", "brown", "fox" } );
		REQUIRE( dynamicScores[ 0 ].getTotalScore( ) > dynamicScores[ 1 ].getTotalScore( ) );
		REQUIRE( dynamicScores[ 1 ].getTotalScore( ) > dynamicScores[ 2 ].getTotalScore( ) );
		}
	}

TEST_CASE( "Real Chunks" )
	{
	dex::vector < dex::index::indexChunk * > pointers;
	dex::string filePath = "rankertest0_in.dex";
	int fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	REQUIRE( fd != -1 );
		// Scope to make sure we call the destructor
		{
		dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );
		dex::string url = "www.robinhood.com/shortthehousingmarket";
		dex::vector< dex::string > title = { "learn", "to", "short", "the", "housing", "market" };
		dex::string titleString = "Learn to short the housing market";
		dex::vector< dex::string > body = { "short", "the", "housing", "market" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}
	dex::index::indexChunk *chunkPointer = new dex::index::indexChunk( fd, false );
	close( fd );
	pointers.pushBack( chunkPointer );

	filePath = "rankertest1_in.dex";
	fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	REQUIRE( fd != -1 );
		// Scope to make sure we call the destructor
		{
		dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );
		dex::string url = "goldmansachs.com/unrelated/garbo/garbo/stuff";
		dex::vector< dex::string > title = { "buy", "bonds", "in", "the", "housing", "market" };
		dex::string titleString = "buy bonds in the housing market";
		dex::vector< dex::string > body = { "short", "the", "not", "a", "a", "a", "housing", "market" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}
	chunkPointer = new dex::index::indexChunk( fd, false );
	close( fd );
	pointers.pushBack( chunkPointer );

	filePath = "rankertest2_in.dex";
	fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	REQUIRE( fd != -1 );
		// Scope to make sure we call the destructor
		{
		dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );
		dex::string url = "goldmansachs.com/unrelated/garbo/garbo/stuff/other";
		dex::vector< dex::string > title = { "buy", "bonds", "in", "the", "housing", "market" };
		dex::string titleString = "buy bonds in the housing market";
		dex::vector< dex::string > body = { "short", "the", "not", "a", "a", "a", "housing", "market", "market" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}
	chunkPointer = new dex::index::indexChunk( fd, false );
	close( fd );
	pointers.pushBack( chunkPointer );

	

	SECTION( "dynamic scoring, no doc info" )
		{
		dex::ranker::ranker rankerObject( 0x11110 );
		dex::string query = "short the";
		dex::queryCompiler::parser parser;
		dex::queryCompiler::matchedDocumentsGenerator generator = parser.parse( query );
		pthread_mutex_t generatorLock = PTHREAD_MUTEX_INITIALIZER;
		dex::pair< dex::vector< dex::ranker::searchResult >, int > *result;
		dex::ranker::scoreRequest request;
		request.generatorPointer = &generator;
		request.generatorLockPointer = &generatorLock;
		request.chunkPointer = pointers[ 0 ];
		request.rankerPointer = &rankerObject;
		request.printInfo = false;
		result = dex::ranker::findAndScoreDocuments( &request );

		REQUIRE( result->second == 0 );
		REQUIRE( result->first.size( ) == 1 );
		REQUIRE( result->first[ 0 ].title == "Learn to short the housing market" );
		REQUIRE( result->first[ 0 ].url == "http://www.robinhood.com/shortthehousingmarket" );
		delete result;
		}

	SECTION( "getTopN basic functionality" )
		{
		dex::ranker::ranker rankerObject;
		dex::string query = "short the housing market";
		dex::pair< dex::vector< dex::ranker::searchResult >, int > results = dex::ranker::getTopN( 10, query,
				&rankerObject, pointers );
		REQUIRE( results.second == 0 );
		REQUIRE( results.first.size( ) == 3 );

		query = "buy bonds";
		results = dex::ranker::getTopN( 10, query, &rankerObject, pointers );
		REQUIRE( results.second == 0 );
		REQUIRE( results.first.size( ) == 2 );

		query = "buy bonds";
		results = dex::ranker::getTopN( 10, query, &rankerObject, pointers, 1, { }, false );
		REQUIRE( results.second == 0 );
		REQUIRE( results.first.size( ) == 2 );

		query = "short the housing market";
		results = dex::ranker::getTopN( 10, query, &rankerObject, pointers, 1, { }, false );
		REQUIRE( results.second == 0 );
		REQUIRE( results.first.size( ) == 3 );

		query = "short the housing market";
		results = dex::ranker::getTopN( 10, query, &rankerObject, pointers, 2 );
		REQUIRE( results.second == 0 );
		REQUIRE( results.first.size( ) == 3 );

		query = "short the housing market";
		results = dex::ranker::getTopN( 10, query, &rankerObject, pointers, 2, { "goldman" }, false );
		REQUIRE( results.second == 0 );
		REQUIRE( results.first.size( ) == 1 );

		query = "short the housing market";
		results = dex::ranker::getTopN( 10, query, &rankerObject, pointers, 2, { "goldman", "hood.com" }, false );
		REQUIRE( results.second == 0 );
		REQUIRE( results.first.size( ) == 0 );
		}

	SECTION( "malformed query" )
		{
		dex::ranker::ranker rankerObject;
		dex::string query = "";
		dex::pair< dex::vector< dex::ranker::searchResult >, int > results = dex::ranker::getTopN( 10, query,
				&rankerObject, pointers );
		REQUIRE( results.second == -1 );
		}
	for ( auto chunkPointer : pointers )
		delete chunkPointer;
	
	}
