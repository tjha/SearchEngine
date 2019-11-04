// basicParserTests.cpp
// Basic testing of Parser functionality
//
//

#include "catch.hpp"
#include "parser.hpp"
#include "basicString.hpp"
#include "exception.hpp"

using dex::outOfRangeException;
using dex::string;


// helper function to convert file to string for use in tests

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
      REQUIRE( links.size() == 1);
      REQUIRE( links[0] == expectedLink);
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
      REQUIRE( links.size() == 3);
      REQUIRE( links[0] == expectedLink1);
      REQUIRE( links[1] == expectedLink2);
      REQUIRE( links[2] == expectedLink3);
      }

   }
