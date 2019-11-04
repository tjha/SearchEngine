// basicParserTests.cpp
// Basic testing of Parser functionality
//
//

#include "basicString.hpp"
#include "catch.hpp"
#include "exception.hpp"
#include "parser.hpp"
#include "file.hpp"
#include <iostream>

using dex::readFromFile;
using dex::outOfRangeException;
using dex::string;

using std::cerr;
using std::cout;
using std::endl;


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
      vector< string > links = GetLinks( htmlDoc );
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
      vector< string > links = GetLinks( htmlDoc );
      string expectedLink1 = "https://web.eecs.umich.edu/~pmchen/software1";
      string expectedLink2 = "https://web.eecs.umich.edu/~pmchen/software2";
      string expectedLink3 = "https://web.eecs.umich.edu/~pmchen/software3";
      REQUIRE( links.size() == 3 );
      REQUIRE( links[0] == expectedLink1 );
      REQUIRE( links[1] == expectedLink2 );
      REQUIRE( links[2] == expectedLink3 );
      }

   SECTION ( "peter chen html page with commented out html" )
      {
      string filename = "tst/parser/peter_chen.html";
      string htmlDoc;
      try
         {
         htmlDoc = readFromFile( filename.cStr() );
         vector < string > links = GetLinks ( htmlDoc );

         REQUIRE ( links.size() == 13 );

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

         for (size_t i = 0; i < expectedLinks.size(); i++ )
            {
            REQUIRE ( links[i] == expectedLinks[i] );
            }
         } 
      catch ( outOfRangeException &e )
         {
         cerr << "Cannot read file: " << filename << endl;
         REQUIRE( 1 == 0 ); // force test failure
         }
      }
   }
