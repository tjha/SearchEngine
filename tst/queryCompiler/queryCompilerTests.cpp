// queryCompilerTests.cpp
//
// 2019-12-15: File created

#include "../catch.hpp"
#include "../constraintSolver/testingISRs.hpp"
#include "../../src/queryCompiler/expression.hpp"
#include "../../src/queryCompiler/parserQC.hpp"
#include "../../src/queryCompiler/tokenstream.hpp"
#include "../../src/utils/basicString.hpp"

#include <iostream>


TEST_CASE( "token stream" )
	{
	SECTION( "space delimiting" )
		{
		dex::string s = "here is check number one";
		dex::queryCompiler::tokenStream ts( s, nullptr );
		REQUIRE( ts.input == "here&is&check&number&one" );

		s = "here    is     check    number two";
		ts = dex::queryCompiler::tokenStream( s, nullptr );
		REQUIRE( ts.input == "here&is&check&number&two" );

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

	SECTION( "with Emphs" )
		{
		dex::string s = "here $is check | number one";
		dex::queryCompiler::tokenStream ts( s, nullptr );

		for ( auto it = ts.emphasizedWords.cbegin( );  it != ts.emphasizedWords.cend( );  ++it )
			std::cout << "============" << *it << "================" << std::endl;

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

// TEST_CASE( "single word check" )
// 	 {
// 	 SECTION( "basic" )
// 		{
// 		dex::string s = "First Check";
// 		dex::queryCompiler::parser P( s, nullptr );
// 		P.parse();
// 		}
// 	 }
