// queryCompilerTests.cpp
//
// 2019-12-15: File created

#include "catch.hpp"
#include "constraintSolver/testingISRs.hpp"
#include "queryCompiler/expression.hpp"
#include "queryCompiler/parser.hpp"
#include "queryCompiler/tokenstream.hpp"
#include "ranker/rankerObjects.hpp"
#include "utils/basicString.hpp"
#include "utils/stemming.hpp"


TEST_CASE( "token stream", "[queryCompiler]" )
	{
	SECTION( "space delimiting" )
		{
		dex::queryCompiler::word *wordy;

		dex::string s = "here is check number one";
		dex::queryCompiler::tokenStream ts( s );
		REQUIRE( ts.input == "here&is&check&number&one" );

		s = "here    is     check    number two";
		ts = dex::queryCompiler::tokenStream( s );
		REQUIRE( ts.input == "here&is&check&number&two" );
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == dex::porterStemmer::stem( "here" ) );
		REQUIRE( ts.match( '&' ) );
		delete wordy;
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == dex::porterStemmer::stem( "is" ) );
		REQUIRE( ts.match( '&' ) );
		delete wordy;
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == dex::porterStemmer::stem( "check" ) );
		REQUIRE( ts.match( '&' ) );
		delete wordy;
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == dex::porterStemmer::stem( "number" ) );
		REQUIRE( ts.match( '&' ) );
		delete wordy;
		wordy = ts.parseWord( );
		REQUIRE( wordy->str == dex::porterStemmer::stem( "two" ) );
		delete wordy;

		s = "      here    is     check    number three";
		ts = dex::queryCompiler::tokenStream( s );
		REQUIRE( ts.input == "here&is&check&number&three" );

		s = "here    is     check    number four     ";
		ts = dex::queryCompiler::tokenStream( s );
		REQUIRE( ts.input == "here&is&check&number&four" );

		s = "     here    is     check    number five     ";
		ts = dex::queryCompiler::tokenStream( s );
		REQUIRE( ts.input == "here&is&check&number&five" );
		}

	SECTION( "with ORs" )
		{
		dex::string s = "here is check | number one";
		dex::queryCompiler::tokenStream ts( s );
		REQUIRE( ts.input == "here&is&check|number&one" );
		}

	SECTION( "with ANDS" )
		{
		dex::string s = "here is check & number & one";
		dex::queryCompiler::tokenStream ts( s );
		REQUIRE( ts.input == "here&is&check&number&one");
		}

	SECTION( "with Phrases" )
		{
		dex::string s = "here \"is check\" | number one";
		dex::queryCompiler::tokenStream ts( s );
		REQUIRE( ts.input == "here&\"is check\"|number&one" );

		s = "here    \"is     check\"    number two";
		ts = dex::queryCompiler::tokenStream( s );
		REQUIRE( ts.input == "here&\"is check\"&number&two" );

		s = "here    \"is     check\"    \"number   three\"";
		ts = dex::queryCompiler::tokenStream( s );
		REQUIRE( ts.input == "here&\"is check\"&\"number three\"" );

		s = "here    \"is     check\"\"number   four\"";
		ts = dex::queryCompiler::tokenStream( s );
		REQUIRE( ts.input == "here&\"is check\"&\"number four\"" );
		}

	SECTION( "with Emphasis" )
		{
		dex::string s = "here $is check | number one";
		dex::queryCompiler::tokenStream ts( s );

		REQUIRE( ts.input == "here&is&check|number&one" );
		REQUIRE( ts.emphasizedWords.size( ) == 1 );
		REQUIRE( ts.emphasizedWords.count( dex::porterStemmer::stem( "is" ) ) == 1 );

		s = "$here    $ is     check    number $two";
		ts = dex::queryCompiler::tokenStream( s );
		REQUIRE( ts.input == "here&is&check&number&two" );
		REQUIRE( ts.emphasizedWords.size( ) == 3 );
		REQUIRE( ts.emphasizedWords.count( dex::porterStemmer::stem( "here" ) ) );
		REQUIRE( ts.emphasizedWords.count( dex::porterStemmer::stem( "is" ) ) );
		REQUIRE( ts.emphasizedWords.count( dex::porterStemmer::stem( "two" ) ) );
		}
	}

TEST_CASE( "parsing check", "[queryCompiler]" )
	{
	SECTION( "Infix notation" )
		{
		SECTION( "[First    ($check   & stuff ) &~badness]" )
			{
			dex::string query = "First    ($check   & stuff ) &~badness";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "(First&(check&stuff)&~bad)");
			REQUIRE( md->flattenedQuery.size( ) == 3 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "First" ) );
			REQUIRE( md->flattenedQuery[ 1 ] == dex::porterStemmer::stem( "check" ) );
			REQUIRE( md->flattenedQuery[ 2 ] == dex::porterStemmer::stem( "stuff" ) );
			REQUIRE( md->emphasizedWords.size( ) == 3 );
			REQUIRE( md->emphasizedWords[ 0 ] == false );
			REQUIRE( md->emphasizedWords[ 1 ] == true );
			REQUIRE( md->emphasizedWords[ 2 ] == false );
			delete md;
			}

		SECTION( "[two words]" )
			{
			dex::string query = "two words";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "(two&word)");
			REQUIRE( md->flattenedQuery.size( ) == 2 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "two" ) );
			REQUIRE( md->flattenedQuery[ 1 ] == dex::porterStemmer::stem( "words" ) );
			delete md;
			}

		SECTION( "[alpha | ~( beta & ~gamma )]" )
			{
			dex::string query = "alpha | ~( beta & ~gamma )";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "(alpha|~(beta&~gamma))" );
			REQUIRE( md->flattenedQuery.size( ) == 2 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "alpha" ) );
			REQUIRE( md->flattenedQuery[ 1 ] == dex::porterStemmer::stem( "gamma" ) );
			delete md;
			}

		SECTION( "[$now \"with some\" | phrases]" )
			{
			dex::string query = "$now \"with some\" | phrases";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "((now&\"with some\")|phrase)" );
			REQUIRE( md->flattenedQuery.size( ) == 4 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "now" ) );
			REQUIRE( md->flattenedQuery[ 1 ] == dex::porterStemmer::stem( "with" ) );
			REQUIRE( md->flattenedQuery[ 2 ] == dex::porterStemmer::stem( "some" ) );
			REQUIRE( md->flattenedQuery[ 3 ] == dex::porterStemmer::stem( "phrases" ) );
			REQUIRE( md->emphasizedWords.size( ) == 4 );
			REQUIRE( md->emphasizedWords[ 0 ] == true );
			REQUIRE( md->emphasizedWords[ 1 ] == false );
			REQUIRE( md->emphasizedWords[ 2 ] == false );
			REQUIRE( md->emphasizedWords[ 3 ] == false );
			delete md;
			}

		SECTION( "[alpha]" )
			{
			dex::string query = "alpha";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "(alpha)" );
			REQUIRE( md->flattenedQuery.size( ) == 1 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "alpha" ) );
			REQUIRE( md->emphasizedWords.size( ) == 1 );
			REQUIRE( md->emphasizedWords[ 0 ] == false );
			delete md;
			}
		}

	SECTION( "Prefix notation" )
		{
		SECTION( "[     &   basic and  ]" )
			{
			dex::string query = "     &   basic and  ";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query, false );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "(basic&and)");
			REQUIRE( md->flattenedQuery.size( ) == 2 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "basic" ) );
			REQUIRE( md->flattenedQuery[ 1 ] == dex::porterStemmer::stem( "and" ) );
			REQUIRE( md->emphasizedWords.size( ) == 2 );
			REQUIRE( md->emphasizedWords[ 0 ] == false );
			REQUIRE( md->emphasizedWords[ 1 ] == false );
			delete md;
			}

		SECTION( "[     |   basic or  ]" )
			{
			dex::string query = "     |   basic or  ";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query, false );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "(basic|or)");
			REQUIRE( md->flattenedQuery.size( ) == 2 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "basic" ) );
			REQUIRE( md->flattenedQuery[ 1 ] == dex::porterStemmer::stem( "or" ) );
			REQUIRE( md->emphasizedWords.size( ) == 2 );
			REQUIRE( md->emphasizedWords[ 0 ] == false );
			REQUIRE( md->emphasizedWords[ 1 ] == false );
			delete md;
			}

		SECTION( "[| alpha ~ & $beta ~ gamma]" )
			{
			dex::string query = "| alpha ~ & $beta ~ gamma";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query, false );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "(alpha|~(beta&~gamma))");
			REQUIRE( md->flattenedQuery.size( ) == 2 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "alpha" ) );
			REQUIRE( md->flattenedQuery[ 1 ] == dex::porterStemmer::stem( "gamma" ) );
			REQUIRE( md->emphasizedWords.size( ) == 2 );
			REQUIRE( md->emphasizedWords[ 0 ] == false );
			REQUIRE( md->emphasizedWords[ 1 ] == false );
			delete md;
			}

		SECTION( "[  &  &  |  $  this  is  \"  a  comprehensive  test  \"  |  $  that  ~  \"  does stuff  \"  ]" )
			{
			dex::string query = "  &  &   |  $this  is \"  a  comprehensive test\" | $ that ~   \"does stuff  \" ";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query, false );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "(((thi|i)&\"a comprehens test\")&(that|~\"doe stuff\"))");
			REQUIRE( md->flattenedQuery.size( ) == 6 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "this" ) );
			REQUIRE( md->flattenedQuery[ 1 ] == dex::porterStemmer::stem( "is" ) );
			REQUIRE( md->flattenedQuery[ 2 ] == dex::porterStemmer::stem( "a" ) );
			REQUIRE( md->flattenedQuery[ 3 ] == dex::porterStemmer::stem( "comprehensive" ) );
			REQUIRE( md->flattenedQuery[ 4 ] == dex::porterStemmer::stem( "test" ) );
			REQUIRE( md->flattenedQuery[ 5 ] == dex::porterStemmer::stem( "that" ) );
			REQUIRE( md->emphasizedWords.size( ) == 6 );
			REQUIRE( md->emphasizedWords[ 0 ] == true );
			REQUIRE( md->emphasizedWords[ 1 ] == false );
			REQUIRE( md->emphasizedWords[ 2 ] == false );
			REQUIRE( md->emphasizedWords[ 3 ] == false );
			REQUIRE( md->emphasizedWords[ 4 ] == false );
			REQUIRE( md->emphasizedWords[ 5 ] == true );
			delete md;
			}

		SECTION( "[&&|$thisis\"a comprehensive test\"|$that~\"does stuff\"]" )
			{
			dex::string query = "&&|$this is\"a comprehensive test\"|$that~\"does stuff\"";
			dex::queryCompiler::parser parsyMcParseface;
			dex::queryCompiler::matchedDocumentsGenerator mdg = parsyMcParseface.parse( query, false );
			dex::matchedDocuments *md = mdg( nullptr );
			REQUIRE( mdg.getQuery( ) == "(((thi|i)&\"a comprehens test\")&(that|~\"doe stuff\"))");
			REQUIRE( md->flattenedQuery.size( ) == 6 );
			REQUIRE( md->flattenedQuery[ 0 ] == dex::porterStemmer::stem( "this" ) );
			REQUIRE( md->flattenedQuery[ 1 ] == dex::porterStemmer::stem( "is" ) );
			REQUIRE( md->flattenedQuery[ 2 ] == dex::porterStemmer::stem( "a" ) );
			REQUIRE( md->flattenedQuery[ 3 ] == dex::porterStemmer::stem( "comprehensive" ) );
			REQUIRE( md->flattenedQuery[ 4 ] == dex::porterStemmer::stem( "test" ) );
			REQUIRE( md->flattenedQuery[ 5 ] == dex::porterStemmer::stem( "that" ) );
			REQUIRE( md->emphasizedWords.size( ) == 6 );
			REQUIRE( md->emphasizedWords[ 0 ] == true );
			REQUIRE( md->emphasizedWords[ 1 ] == false );
			REQUIRE( md->emphasizedWords[ 2 ] == false );
			REQUIRE( md->emphasizedWords[ 3 ] == false );
			REQUIRE( md->emphasizedWords[ 4 ] == false );
			REQUIRE( md->emphasizedWords[ 5 ] == true );
			delete md;
			}
		}
	}