// parserTests.cpp
// Basic testing of Parser functionality
//
// 2018-11-21: Created basic link and anchor text tests for amazon.html: tjha
//             Updated to use URLs instead of strings: combsc
// 2018-11-21: Created extensive checking of all anchor text words for
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
#include "basicString.hpp"
#include "catch.hpp"
#include "exception.hpp"
#include "parser.hpp"
#include "file.hpp"

#include <cstddef>

#include <iostream>

using dex::anchorPos;
using dex::HTMLparser;
using dex::outOfRangeException;
using dex::readFromFile;
using dex::string;
using dex::vector;

using std::size_t;
using std::cout;
using std::endl;


TEST_CASE( "basic get links with relative paths", "[parser]" )
   {

	SECTION( "parsed simple html document with one link" )
      {
      string htmlDoc = 
         "https://web.eecs.umich.edu/~pmchen/\n\
         <html>\n\
            <title>Title</title>\n\
            <body>\n\
            <a href=\"software\">Software</a>\n\
            </body>\n\
         </html>";
      HTMLparser testParser( htmlDoc );
      // testParser.GetLinks();
      vector< dex::Url > links = testParser.ReturnLinks( );
      string expectedLink = "https://web.eecs.umich.edu/~pmchen/software";
      REQUIRE( links.size() == 1 );
      REQUIRE( links[0].completeUrl( ) == expectedLink );
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
      HTMLparser testParser( htmlDoc );
      // testParser.GetLinks();
      vector< dex::Url > links = testParser.ReturnLinks( );
      string expectedLink1 = "https://web.eecs.umich.edu/~pmchen/software1";
      string expectedLink2 = "https://web.eecs.umich.edu/~pmchen/software2";
      string expectedLink3 = "https://web.eecs.umich.edu/~pmchen/software3";
      REQUIRE( links.size() == 3 );
      REQUIRE( links[0].completeUrl( ) == expectedLink1 );
      REQUIRE( links[1].completeUrl( ) == expectedLink2 );
      REQUIRE( links[2].completeUrl( ) == expectedLink3 );

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
      vector< dex::Url > links = testParser.ReturnLinks( );

      // Validate correct number of links were extracted
      REQUIRE ( links.size() == 13 );

      // Create vector with expected links on page
      vector < string > expectedLinks;

      expectedLinks.pushBack(
         "http://www.provost.umich.edu/programs/thurnau/index.html" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu" );
      expectedLinks.pushBack( "http://www.umich.edu" );
      expectedLinks.pushBack( 
         "https://web.eecs.umich.edu/~pmchen/contact.html" );
      expectedLinks.pushBack( "http://web.eecs.umich.edu/virtual/" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu/~pmchen/Rio" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu/ssl" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu/cse" );
      expectedLinks.pushBack( "http://www.eecs.umich.edu" );
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
         REQUIRE ( links[ i ].completeUrl( ) == expectedLinks[ i ] );
         }
      }

   SECTION( "Expected Anchor Text" )
      {
      vector < dex::anchorPos > anchors = testParser.ReturnAnchorText();
      vector < dex::string > words = testParser.ReturnWords();
      vector < dex::string > AnchorWords;

      // push back anchor words for each link
      AnchorWords.pushBack( "Arthur" );
      AnchorWords.pushBack( "F" );
      AnchorWords.pushBack( "Thurnau" );
      AnchorWords.pushBack( "Professor" );

      AnchorWords.pushBack( "EECS" );
      AnchorWords.pushBack( "Department" );

      AnchorWords.pushBack( "University" );
      AnchorWords.pushBack( "of" );
      AnchorWords.pushBack( "Michigan" );

      AnchorWords.pushBack( "Contact" );
      AnchorWords.pushBack( "information" );

      AnchorWords.pushBack( "Adding" );
      AnchorWords.pushBack( "security" );
      AnchorWords.pushBack( "services" );
      AnchorWords.pushBack( "through" );
      AnchorWords.pushBack( "virtual" );
      AnchorWords.pushBack( "machines" );


      AnchorWords.pushBack( "Rio" );
      AnchorWords.pushBack( "RAM" );
      AnchorWords.pushBack( "I" );
      AnchorWords.pushBack( "O" );

      AnchorWords.pushBack( "Software" );
      AnchorWords.pushBack( "Systems" );
      AnchorWords.pushBack( "Lab" );

      AnchorWords.pushBack( "Computer" );
      AnchorWords.pushBack( "Science" );
      AnchorWords.pushBack( "and" );
      AnchorWords.pushBack( "Engineering" );
      AnchorWords.pushBack( "Division" );
    
      AnchorWords.pushBack( "EECS" );

      AnchorWords.pushBack( "Publications" );

      AnchorWords.pushBack( "EECS" );
      AnchorWords.pushBack( "482" );

      AnchorWords.pushBack( "Students" );

      AnchorWords.pushBack( "Software" ); 
      REQUIRE( AnchorWords.size( ) == words.size( ) );

      size_t word_count = 0; 
      for ( size_t j = 0; j < anchors.size( ); j++ )
         {
         for ( size_t i = 0; i <= anchors[ j ].endPos - anchors[ j ].startPos; i++ )
            {
            REQUIRE( AnchorWords[ word_count++ ] == words[ anchors[ j ].startPos + i ] );
            }
         }
      }

   SECTION( "Expected Words" )
      {
      }

   }

TEST_CASE( "amazon.com html page: comment, script, div, img, and link tags" )
   {
      string filename = "tst/parser/amazon.html";
      string htmlDoc;
      htmlDoc = readFromFile( filename.cStr( ) );
      HTMLparser testParser( htmlDoc );

      SECTION( "Expected Links" )
         {
         vector< dex::Url > links = testParser.ReturnLinks( );

         // Validate correct number of links were extracted
         REQUIRE ( links.size() == 2 );

         // Create vector with expected links on page
         vector < string > expectedLinks;

         expectedLinks.pushBack(
            "https://www.amazon.com/gp/help/customer/display.html/ref=footer_cou?ie=UTF8&nodeId=508088" );
         expectedLinks.pushBack( 
            "https://www.amazon.com/gp/help/customer/display.html/ref=footer_privacy?ie=UTF8&nodeId=468496" );

         // Verify parsed links match expected page links
         for ( size_t i = 0; i < expectedLinks.size(); i++ )
            {
            REQUIRE ( links[ i ].completeUrl( ) == expectedLinks[ i ] );
            }
         }
      
      SECTION ( "Expected Anchor Text" )
         {
         vector < dex::anchorPos > anchors = testParser.ReturnAnchorText();
         vector < dex::string > words = testParser.ReturnWords();
         vector < dex::string > AnchorWords;

         // push back anchor words for each link
         AnchorWords.pushBack( "Conditions" );
         AnchorWords.pushBack( "of" );
         AnchorWords.pushBack( "Use" );

         AnchorWords.pushBack( "Privacy" );
         AnchorWords.pushBack( "Policy" );
 
         REQUIRE( AnchorWords.size( ) == words.size( ) );

         size_t word_count = 0; 
         for ( size_t j = 0; j < anchors.size( ); j++ )
            {
            for ( size_t i = 0; i <= anchors[ j ].endPos - anchors[ j ].startPos; i++ )
               {
               REQUIRE( AnchorWords[ word_count++ ] == words[ anchors[ j ].startPos + i ] );
               }
            }
         }
      
      SECTION( "Expected Words" )
         {
         }
   }

TEST_CASE( "man7.org: simple page where relative links don't have slashes" )
   {
      string filename = "tst/parser/man7.html";
      string htmlDoc;
      htmlDoc = readFromFile( filename.cStr( ) );
      HTMLparser testParser( htmlDoc );

      SECTION( "Expected Links" )
         {
         vector< dex::Url > links = testParser.ReturnLinks( );

         // Validate correct number of links were extracted
         //REQUIRE ( links.size() == 10 );

         // Create vector with expected links on page
         vector < string > expectedLinks;

         expectedLinks.pushBack( "http://man7.org/tlpi/index.html" );
         expectedLinks.pushBack( "http://man7.org/training/index.html" );
         expectedLinks.pushBack( "http://man7.org/tlpi/index.html" );
         expectedLinks.pushBack( "http://blog.man7.org" );
         expectedLinks.pushBack( "http://man7.org/articles/index.html" );
         expectedLinks.pushBack( "http://www.kernel.org/doc/man-pages/" );
         expectedLinks.pushBack( "http://man7.org/linux/man-pages/index.html" );
         expectedLinks.pushBack( "http://man7.org/mtk/index.html" );
         expectedLinks.pushBack( "http://man7.org/mtk/contact.html" );

         // Verify parsed links match expected page links
         for ( size_t i = 0; i < links.size(); i++ )
            {
            //REQUIRE ( links[ i ].completeUrl( ) == expectedLinks[ i ] );
            }
         }
      
      SECTION ( "Expected Anchor Text" )
         {
         }
      
      SECTION( "Expected Words" )
         {
         }
   }