// queryCompilerTests.cpp
//
// 2019-12-15: File created

#include "../catch.hpp"
#include "../constraintSolver/testingISRs.hpp"
#include "../../src/queryCompiler/expression.hpp"
#include "../../src/queryCompiler/parserQC.hpp"
#include "../../src/queryCompiler/tokenstream.hpp"
#include "../../src/ranker/rankerObjects.hpp"
#include "../../src/utils/basicString.hpp"


TEST_CASE( "token stream" )
	{
	SECTION( "space delimiting" )
		{
		dex::queryCompiler::word *wordy;

		dex::string s = "here is check number one";
		dex::queryCompiler::tokenStream ts( s, nullptr );
		REQUIRE( ts.input == "here&is&check&number&one" );

		s = "here    is     check    number two";
		ts = dex::queryCompiler::tokenStream( s, nullptr );
		REQUIRE( ts.input == "here&is&check&number&two" );
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == "here" );
		REQUIRE( ts.match( '&' ) );
		delete wordy;
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == "is" );
		REQUIRE( ts.match( '&' ) );
		delete wordy;
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == "check" );
		REQUIRE( ts.match( '&' ) );
		delete wordy;
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == "number" );
		REQUIRE( ts.match( '&' ) );
		delete wordy;
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == "two" );
		delete wordy;

		s = "      here    is     check    number three";
		ts = dex::queryCompiler::tokenStream( s, nullptr );
		REQUIRE( ts.input == "here&is&check&number&three" );

		s = "here    is     check    number four     ";
		ts = dex::queryCompiler::tokenStream( s, nullptr );
		REQUIRE( ts.input == "here&is&check&number&four" );

		s = "     here    is     check    number five     ";
		ts = dex::queryCompiler::tokenStream( s, nullptr );
		REQUIRE( ts.input == "here&is&check&number&five" );
		}

	SECTION( "with ORs" )
		{
		dex::string s = "here is check | number one";
		dex::queryCompiler::tokenStream ts( s, nullptr );
		REQUIRE( ts.input == "here&is&check|number&one" );
		}

	SECTION( "with ANDS" )
		{
		dex::string s = "here is check & number & one";
		dex::queryCompiler::tokenStream ts( s, nullptr );
		REQUIRE( ts.input == "here&is&check&number&one");
		}

	SECTION( "with Phrases" )
		{
		dex::string s = "here \"is check\" | number one";
		dex::queryCompiler::tokenStream ts( s, nullptr );
		REQUIRE( ts.input == "here&\"is check\"|number&one" );

		s = "here    \"is     check\"    number two";
		ts = dex::queryCompiler::tokenStream( s, nullptr );
		REQUIRE( ts.input == "here&\"is check\"&number&two" );
		}

	SECTION( "with Emphasis" )
		{
		dex::string s = "here $is check | number one";
		dex::queryCompiler::tokenStream ts( s, nullptr );

		REQUIRE( ts.input == "here&is&check|number&one" );
		REQUIRE( ts.emphasizedWords.size( ) == 1 );
		REQUIRE( ts.emphasizedWords.count( "is" ) == 1 );

		s = "$here    $ is     check    number $two";
		ts = dex::queryCompiler::tokenStream( s, nullptr );
		REQUIRE( ts.input == "here&is&check&number&two" );
		REQUIRE( ts.emphasizedWords.size( ) == 3 );
		REQUIRE( ts.emphasizedWords.count( "here" ) );
		REQUIRE( ts.emphasizedWords.count( "is" ) );
		REQUIRE( ts.emphasizedWords.count( "two" ) );
		}
	}

TEST_CASE( "single word check" )
	 {
	dex::string query = "First    ($check   & stuff ) &~badness";
	dex::queryCompiler::parser parsyMcParseface( query, nullptr );
	dex::matchedDocuments *md = parsyMcParseface.parse( );
	REQUIRE( md->flattenedQuery.size( ) == 3 );
	REQUIRE( md->flattenedQuery[ 0 ] == "First" );
	REQUIRE( md->flattenedQuery[ 1 ] == "check" );
	REQUIRE( md->flattenedQuery[ 2 ] == "stuff" );
	REQUIRE( md->emphasizedWords.size( ) == 3 );
	REQUIRE( md->emphasizedWords[ 0 ] == false );
	REQUIRE( md->emphasizedWords[ 1 ] == true );
	REQUIRE( md->emphasizedWords[ 2 ] == false );
	delete md;

	query = "two words";
	parsyMcParseface = dex::queryCompiler::parser( query, nullptr );
	md = parsyMcParseface.parse( );
	REQUIRE( md->flattenedQuery.size( ) == 2 );
	REQUIRE( md->flattenedQuery[ 0 ] == "two" );
	REQUIRE( md->flattenedQuery[ 1 ] == "words" );
	delete md;
	 }
