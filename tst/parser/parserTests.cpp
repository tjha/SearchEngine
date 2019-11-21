// parserTests.cpp
// Basic testing of Parser functionality
//
// 2019-11-06: Made compatible with new paired struct: medhak
// 2019-11-06: AnchorText test with peter_chen.html: tjha
// 2019-11-05: Made compatible with Parser classL medhak
// 2019-11-04: GetLinks test with peter_chen.html
//             Hard-coded html test for anchor text: tjha
// 2019-10-26: File created, two GetLinks tests with hard-coded html: medhak,
//             tjha
#include "basicString.hpp"
#include "catch.hpp"
#include "exception.hpp"
#include "parser.hpp"
#include "file.hpp"
//#include <iostream>

using dex::readFromFile;
using dex::outOfRangeException;
using dex::string;
using dex::HTMLparser;
using dex::anchorPos;
using dex::vector;

//using std::cerr;
//using std::cout;
//using std::endl;


TEST_CASE( "get links", "[parser]" )
   {

	SECTION( "parsed simple html document with one link" )
      {
      string htmlDoc = 
         "<html>\
            <title>Title</title>\
            <body>\
            <a href=\"https://web.eecs.umich.edu/~pmchen/software\">Software</a>\
            </body>\
         </html>";
      HTMLparser testParser( htmlDoc );
      // testParser.GetLinks();
      vector< string > links = testParser.ReturnLinks( );
      string expectedLink = "https://web.eecs.umich.edu/~pmchen/software";
      REQUIRE( links.size() == 1 );
      REQUIRE( links[0] == expectedLink );
      }

	SECTION( "parsed simple html document with one link" )
      {
      string htmlDoc = 
         "<html>\
            <title>Title</title>\
            <body>\
            <a href=\"https://web.eecs.umich.edu/~pmchen/software1\">Software</a>\
            <a href=\"https://web.eecs.umich.edu/~pmchen/software2\">Software</a>\
            <a href=\"https://web.eecs.umich.edu/~pmchen/software3\">Software</a>\
            </body>\
         </html>";
      HTMLparser testParser( htmlDoc );
      // testParser.GetLinks();
      vector< string > links = testParser.ReturnLinks( );
      string expectedLink1 = "https://web.eecs.umich.edu/~pmchen/software1";
      string expectedLink2 = "https://web.eecs.umich.edu/~pmchen/software2";
      string expectedLink3 = "https://web.eecs.umich.edu/~pmchen/software3";
      REQUIRE( links.size() == 3 );
      REQUIRE( links[0] == expectedLink1 );
      REQUIRE( links[1] == expectedLink2 );
      REQUIRE( links[2] == expectedLink3 );

      }
   }

TEST_CASE( "peter_chen.html page: simple format with comment tags" )
   {

   // Read and save HTML file uisng HTMLparser
   string filename = "tst/parser/peter_chen.html";
   string htmlDoc;
   htmlDoc = readFromFile( filename.cStr( ) );
 
   HTMLparser testParser( htmlDoc );

   SECTION( "Expected Links" )
      {
      vector< string > links = testParser.ReturnLinks( );

      // Validate correct number of links were extracted
      REQUIRE ( links.size() == 13 );

      // Create vector with expected links on page
      vector < string > expectedLinks;

      expectedLinks.pushBack(
         "http://www.provost.umich.edu/programs/thurnau/index.html" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu/" );
      expectedLinks.pushBack( "http://www.umich.edu/" );
      expectedLinks.pushBack( 
         "https://web.eecs.umich.edu/~pmchen/contact.html" );
      expectedLinks.pushBack( "http://web.eecs.umich.edu/virtual/" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu/~pmchen/Rio" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu/ssl" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu/cse" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu/" );
      expectedLinks.pushBack( "https://web.eecs.umich.edu/~pmchen/papers/" );
      expectedLinks.pushBack( 
         "https://web.eecs.umich.edu/~pmchen/eecs482/" );
      expectedLinks.pushBack( 
         "https://web.eecs.umich.edu/~pmchen/students.html" );
      expectedLinks.pushBack( 
         "https://web.eecs.umich.edu/~pmchen/software" );

      // Verify parsed links match expected page links
      for (size_t i = 0; i < expectedLinks.size(); i++ )
         {
         REQUIRE ( links[ i ] == expectedLinks[ i ] );
         }
      }

   SECTION( "Expected Anchor Text" )
      {
      vector < anchorPos > anchors = testParser.ReturnAnchorText();
      vector < string > words = testParser.ReturnWords();

      vector < string > AnchorWords;

      AnchorWords.pushBack( "Computer" );
      AnchorWords.pushBack("Science");
      AnchorWords.pushBack("and");
      AnchorWords.pushBack("Engineering");
      AnchorWords.pushBack("Division");
      
      for ( size_t i = 0; i < anchors[ 7 ].endPos - anchors[ 7 ].startPos; i++ )
         {
         REQUIRE( AnchorWords[ i] == words[anchors[7].startPos + i]);
         }
      }

   SECTION( "Expected Words" )
      {
      }

   }

TEST_CASE( "amazon.com html page with comment, script, div, img, and link tags" )
   {
      SECTION( "Expected Links" )
      {
      string filename = "tst/parser/amazon.html";
      string htmlDoc;
      htmlDoc = readFromFile( filename.cStr( ) );
      }
      
      SECTION ( "Expected Anchor Text" )
         {
         }
      
      SECTION( "Expected Words" )
         {
         }
   }

