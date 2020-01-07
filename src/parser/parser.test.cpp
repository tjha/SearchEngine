// parserTests.cpp
// Basic testing of Parser functionality
//
// 2019-11-26: Added stress test case for getLinks using man7_all_pages: tjha
// 2019-11-25: Added edge cases for .s and ..s: tjha
// 2019-11-24: Added .s and ..s basic tests: medhak
// 2019-11-23: Added Enneagraminstitute links tests: tjha
// 2019-11-22: Modified test case to utilize dex::Url during comparisions to
//             ensure isolation from Url implementation: tjha
// 2019-11-22: Added man7 test case and merged with changes by combsc that
//             convert links to vector of url types instead of strigs: tjha
// 2019-11-21: Created basic link and anchor text tests for amazon.html: tjha
// 2019-11-21: Created extensive checking of all anchor text words for
//             peter_chen.html: tjha
// 2019-11-20: Modified basic test to ensure content is first parsed for url
//             format: <url>\n<html_content>: tjha
// 2019-11-20: Reorganized test cases for different files
//             Each test case splits into sections to test functionality: tjha
// 2019-11-06: Made compatible with new paired struct: medhak
// 2019-11-06: AnchorText test with peter_chen.html: tjha
// 2019-11-05: Made compatible with Parser classL medhak
// 2019-11-04: GetLinks test with peter_chen.html
//             Hard-coded html test for anchor text: tjha
// 2019-10-26: File created, two GetLinks tests with hard-coded html: medhak,
//             tjha

#include <cstddef>
#include <fstream>
#include "catch.hpp"
#include "parser/parser.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/file.hpp"
#include "utils/url.hpp"


using dex::anchorPos;
using dex::HTMLparser;
using dex::outOfRangeException;
using dex::readFromFile;

using dex::string;
using dex::Url;
using dex::vector;

using std::size_t;

TEST_CASE( "basic get links with relative paths", "[parser]" )
	{
	SECTION(" Words testS :")
		{

		string filename = "tst/parserTests/hamiltoncshell.html";
		string htmlDoc;
		htmlDoc = readFromFile( filename.cStr() );
      dex::Url url( "https://en.wikipedia.org/wiki/Hamilton_C_shell" );
		HTMLparser testParser( url, htmlDoc, true );

		vector<string> words = testParser.ReturnWords();
		vector<string> titleWords = testParser.ReturnTitle();
		vector<string> firstTen;
		firstTen.pushBack( "hamilton" );
		firstTen.pushBack( "c" );
		firstTen.pushBack( "shell" );
		firstTen.pushBack( "from" );
		firstTen.pushBack( "wikipedia" );
		firstTen.pushBack( "the" );
		firstTen.pushBack( "free" );
		firstTen.pushBack( "encyclopedia" );
		firstTen.pushBack( "jump" );
		firstTen.pushBack( "to" );
		firstTen.pushBack( "navigation" );

		for ( size_t i = 0; i < 11; i++ )
			{
			REQUIRE( firstTen[ i ] == words[ i ] );
			}
		vector<string> title;
		title.pushBack("hamilton");
		title.pushBack("c");
		title.pushBack("shell");
		title.pushBack("wikipedia");

		REQUIRE( titleWords.size( ) == title.size( ) );

		for( size_t i = 0; i < titleWords.size(); i++ )
			{
			REQUIRE( titleWords[ i ] == title[ i ] );
			}

		}

	SECTION( "parsed simple html document with one link" )
		{
		string htmlDoc =
			 "https://web.eecs.umich.edu/~pmchen\n\
			  <html>\n\
				  <title>Title</title>\n\
				  <body>\n\
				  <a href=\"software\">Software</a>\n\
				  </body>\n\
			  </html>";
      dex::Url url( "https://web.eecs.umich.edu/~pmchen" );
		HTMLparser testParser( url, htmlDoc, false );
		vector< Url > links = testParser.ReturnLinks( );
		string expectedLink = "https://web.eecs.umich.edu/~pmchen/software";
		REQUIRE( links.size( ) == 1 );
		REQUIRE( links[ 0 ] == Url( expectedLink.cStr( ) ) );
		}

	SECTION( "parsed simple html document with three links" )
		{
		string htmlDoc =
			"https://web.eecs.umich.edu/~pmchen/\n\
			<html>\n\
				<title>Title</title>\n\
				<body>\n\
				<a href=\"software1\">Software</a>\n\
				<a href=\"software2\">Software</a>\n\
				<a href=\"software3\">Software</a>\n\
				</body>\n\
			</html>";
      dex::Url url( "https://web.eecs.umich.edu/~pmchen/" );
		HTMLparser testParser( url, htmlDoc, false );
		vector< Url > links = testParser.ReturnLinks( );
		string expectedLink1 = "https://web.eecs.umich.edu/~pmchen/software1";
		string expectedLink2 = "https://web.eecs.umich.edu/~pmchen/software2";
		string expectedLink3 = "https://web.eecs.umich.edu/~pmchen/software3";
		REQUIRE( links.size( ) == 3 );
		REQUIRE( links[ 0 ] == Url( expectedLink1.cStr( ) ) );
		REQUIRE( links[ 1 ] == Url( expectedLink2.cStr( ) ) );
		REQUIRE( links[ 2 ] == Url( expectedLink3.cStr( ) ) );
		}

	SECTION( "edge case with links that start with ." )
		{
		string htmlDoc =
			"https://web.eecs.umich.edu/~pmchen/\n\
			<html>\n\
				<title>Title</title>\n\
				<body>\n\
				<a href=\".software1\">Software</a>\n\
				<a href=\"../.software2\">Software</a>\n\
				<a href=\"./.software3\">Software</a>\n\
				</body>\n\
			</html>";
      dex::Url url( "https://web.eecs.umich.edu/~pmchen/" );
		HTMLparser testParser( url, htmlDoc, false );
		vector< Url > links = testParser.ReturnLinks( );
		string expectedLink1 = "https://web.eecs.umich.edu/~pmchen/.software1";
		string expectedLink2 = "https://web.eecs.umich.edu/.software2";
		string expectedLink3 = "https://web.eecs.umich.edu/~pmchen/.software3";
		REQUIRE( links.size( ) == 3 );
		REQUIRE( links[ 0 ] == Url( expectedLink1.cStr( ) ) );
		REQUIRE( links[ 1 ] == Url( expectedLink2.cStr( ) ) );
		REQUIRE( links[ 2 ] == Url( expectedLink3.cStr( ) ) );
		}

	SECTION( "working with .s " )
		{
		string filename = "tst/parserTests/man7_man_pages.html";
		string htmlDoc;
		htmlDoc = readFromFile( filename.cStr( ) );
      dex::Url url( "https://www.kernel.org/doc/man-pages/" );
		HTMLparser testParser( url, htmlDoc, false );
		vector< Url > links = testParser.ReturnLinks( );

		REQUIRE( links[ 0 ] == Url( "https://www.kernel.org/doc/man-pages/contributing.html" ) );
		REQUIRE( links[ 1 ] == Url( "https://www.kernel.org/doc/man-pages/reporting_bugs.html" ) );
		REQUIRE( links[ 2 ] == Url( "https://www.kernel.org/doc/man-pages/patches.html" ) );
		REQUIRE( links[ 3 ] == Url( "https://www.kernel.org/doc/man-pages/download.html" ) );
		}

	SECTION( "working with ..s" )
		{
		string filename = "tst/parserTests/man7_index.html";
		string htmlDoc;
		htmlDoc = readFromFile( filename.cStr( ) );
      dex::Url url( "http://man7.org/tlpi/index.html" );
		HTMLparser testParser( url, htmlDoc, false );
		vector< dex::Url > links = testParser.ReturnLinks( );

		REQUIRE(links[ 0 ] == Url( "http://man7.org/index.html" ) );
		REQUIRE(links[ 3 ] == Url( "http://man7.org/mtk/index.html" ) );
		REQUIRE(links[ 6 ] == Url( "http://man7.org/training/index.html" ) );
		}

	SECTION( "Edge cases while working with .s and ..s" )
		{
		string htmlDoc =
			"https://web.eecs.umich.edu/~pmchen/subdir1/subdir2/\n\
			<html>\n\
				<title>Title</title>\n\
				<body>\n\
				<a href=\"../software1\">Software1</a>\n\
				<a href=\"../../software2\">Software2</a>\n\
				<a href=\"/./software3\">Software3</a>\n\
				<a href=\"./software3\">Software3</a>\n\
				<a href=\"../..\">Software2</a>\n\
				<a href=\"../../\">Software2</a>\n\
				<a href=\"../../../\">Software2</a>\n\
				</body>\n\
			</html>";

		string htmlDocWithIndex =
			"https://web.eecs.umich.edu/~pmchen/subdir1/subdir2/index.html\n\
			<html>\n\
				<title>Title</title>\n\
				<body>\n\
				<a href=\"../software1\">Software1</a>\n\
				<a href=\"../../software2\">Software2</a>\n\
				<a href=\"/./software3\">Software3</a>\n\
				<a href=\"./software3\">Software3</a>\n\
				<a href=\"../..\">Software2</a>\n\
				<a href=\"../../\">Software2</a>\n\
				<a href=\"../../../\">Software2</a>\n\
				</body>\n\
			</html>";

		vector< string > expectedLink;
		expectedLink.pushBack(
			"https://web.eecs.umich.edu/~pmchen/subdir1/software1" );
		expectedLink.pushBack(
			"https://web.eecs.umich.edu/~pmchen/software2" );
		expectedLink.pushBack(
			"https://web.eecs.umich.edu/~pmchen/subdir1/subdir2/software3" );
		expectedLink.pushBack(
			"https://web.eecs.umich.edu/~pmchen/subdir1/subdir2/software3" );
		expectedLink.pushBack(
			"https://web.eecs.umich.edu/~pmchen/" );
		expectedLink.pushBack(
			"https://web.eecs.umich.edu/~pmchen/" );
		expectedLink.pushBack(
			"https://web.eecs.umich.edu/" );

      dex::Url url( "https://web.eecs.umich.edu/~pmchen/subdir1/subdir2/" );
		HTMLparser testParser( url, htmlDoc, false );
		vector< Url > links = testParser.ReturnLinks( );

		REQUIRE( links.size( ) == expectedLink.size( ) );
		for ( std::size_t i = 0; i < expectedLink.size( ); i++ )
			{
			REQUIRE( links[ i ] == Url( expectedLink[ i ].cStr( ) ) );
			}

      dex::Url url2( "https://web.eecs.umich.edu/~pmchen/subdir1/subdir2/index.html" );
		HTMLparser testParser2( url2, htmlDocWithIndex, false );
		links = testParser2.ReturnLinks( );

		REQUIRE( links.size( ) == expectedLink.size( ) );
		for ( std::size_t i = 0; i < expectedLink.size( ); i++ )
			{
			REQUIRE( links[ i ] == Url( expectedLink[ i ].cStr( ) ) );
			}
		}
	}

TEST_CASE( "man7.org: simple page where relative links don't have slashes" )
	{
		string filename = "tst/parserTests/man7.html";
		string htmlDoc;
		htmlDoc = readFromFile( filename.cStr( ) );
      dex::Url url( "http://man7.org" );
		HTMLparser testParser( url, htmlDoc, false );

		SECTION( "Expected Links" )
			{
			vector< dex::Url > links = testParser.ReturnLinks( );

			// Create vector with expected links on page
			vector< string > expectedLinks;

			expectedLinks.pushBack( "http://man7.org/tlpi/index.html" );
			expectedLinks.pushBack( "http://man7.org/training/index.html" );
			expectedLinks.pushBack( "http://man7.org/tlpi/index.html" );
			expectedLinks.pushBack( "http://blog.man7.org" );
			expectedLinks.pushBack( "http://man7.org/articles/index.html" );
			expectedLinks.pushBack( "http://man7.org/conf/index.html" );
			expectedLinks.pushBack( "http://www.kernel.org/doc/man-pages/" );
			expectedLinks.pushBack( "http://man7.org/linux/man-pages/index.html" );
			expectedLinks.pushBack( "http://man7.org/mtk/index.html" );
			expectedLinks.pushBack( "http://man7.org/mtk/contact.html" );
			expectedLinks.pushBack( "http://statcounter.com" );

			// Validate correct number of links were extracted
			REQUIRE ( links.size( ) == expectedLinks.size( ) );

			// Verify parsed links match expected page links
			for ( size_t i = 0; i < links.size(); i++ )
				{
				REQUIRE ( links[ i ] == Url( expectedLinks[ i ].cStr( ) ) );
				}
			}

		SECTION ( "Expected Anchor Text" )
			{
			}

		SECTION( "Expected Words" )
			{
			}
	}

TEST_CASE( "enneagraminstitute.com: simple page using Squarespace" )
	{
		string filename = "tst/parserTests/enneagraminstitute.html";
		string htmlDoc;
		htmlDoc = readFromFile( filename.cStr( ) );
      dex::Url url( "https://www.enneagraminstitute.com/" );
		HTMLparser testParser( url, htmlDoc, false );

		SECTION( "Expected Links" )
			{
			vector< dex::Url > links = testParser.ReturnLinks( );

			// Create vector with expected links on page
			vector< string > expectedLinks;

			expectedLinks.pushBack( "https://www.enneagraminstitute.com/" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/how-the-enneagram-system-works" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/type-descriptions" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/the-enneagram-type-combinations" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/misidentifications-of-enneagram-personality-types" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/the-traditional-enneagram" );
			expectedLinks.pushBack( "https://subscriptions.enneagraminstitute.com" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/events" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/course-offerings" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/workshops" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/store" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/about" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/contact" );
			expectedLinks.pushBack( "https://tests.enneagraminstitute.com" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/rheti" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/ivq" );
			expectedLinks.pushBack( "https://tests.enneagraminstitute.com/business-login" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/business-accounts" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/how-the-enneagram-system-works" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/type-descriptions" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/the-enneagram-type-combinations" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/misidentifications-of-enneagram-personality-types" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/the-traditional-enneagram" );
			expectedLinks.pushBack( "https://subscriptions.enneagraminstitute.com" );

			expectedLinks.pushBack( "https://www.enneagraminstitute.com/events" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/course-offerings" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/workshops" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/store" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/about" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/contact" );
			expectedLinks.pushBack( "https://tests.enneagraminstitute.com" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/rheti" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/ivq" );
			expectedLinks.pushBack( "https://tests.enneagraminstitute.com/business-login" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/business-accounts" );


			expectedLinks.pushBack( "https://www.enneagraminstitute.com/#" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/#" );
			expectedLinks.pushBack( "http://tests.enneagraminstitute.com/" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/type-descriptions" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/events" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/#" );

			expectedLinks.pushBack( "https://www.facebook.com/EnneagramInstitute" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/contact" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/privacy-policy" );
			expectedLinks.pushBack( "https://www.enneagraminstitute.com/terms-of-use" );

			// Validate correct number of links were extracted
			REQUIRE ( links.size( ) == expectedLinks.size( ) );

			// Verify parsed links match expected page links
			for ( size_t i = 0; i < links.size(); i++ )
				{
				REQUIRE ( links[ i ] == Url( expectedLinks[ i ].cStr( ) ) );
				}
			}

		SECTION ( "Expected Anchor Text" )
			{
			}

		SECTION( "Expected Words" )
			{
			}

	}

TEST_CASE( "man7_all_pages: stress testing for GetLinks function" )
	{
		string filename = "tst/parserTests/man7_all_pages.html";
		string htmlDoc;
		htmlDoc = readFromFile( filename.cStr( ) );
		dex::Url url( "http://man7.org/linux/man-pages/dir_all_alphabetic.html" );
		HTMLparser testParser( url, htmlDoc, false );

		SECTION( "Expected Links" )
			{
			vector< dex::Url > links = testParser.ReturnLinks( );

			// Create vector with expected links on page
			vector< string > expectedLinks;

			// Validate correct number of links were extracted
			REQUIRE ( links.size( ) == 10813 );

			}
	}
