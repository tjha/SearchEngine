// basicParserTests.cpp
// Basic testing of Parser functionality
//
//

#include "catch.hpp"
#include "../src/parser/parser.hpp"
#include "../utils/basicString.hpp"
#include "../utils/exception.hpp"
#include <iostream>

using dex::outOfRangeException;
using dex::string;

using std::cout;
using std::endl;

TEST_CASE( "get links", "[parser]" )
   {

	SECTION( "parsed simple html document with one link" )
      {
      string html_doc = 
         "<html>\
            <title>Title</title>\
            <body>\
            <a href=\"https://web.eecs.umich.edu/~pmchen/software\">Software</a>\
            </body>\
         </html>";
      vector< string > links = get_links( html_doc );
      string expected_link = "https://web.eecs.umich.edu/~pmchen/software";
      REQUIRE( links.size() == 1);
      REQUIRE( links[0] == expected_link);
      }

	SECTION( "parsed simple html document with one link" )
      {
      string html_doc = 
         "<html>\
            <title>Title</title>\
            <body>\
            <a href=\"https://web.eecs.umich.edu/~pmchen/software1\">Software</a>\
            <a href=\"https://web.eecs.umich.edu/~pmchen/software2\">Software</a>\
            <a href=\"https://web.eecs.umich.edu/~pmchen/software3\">Software</a>\
            </body>\
         </html>";
      vector< string > links = get_links( html_doc );
      string expected_link1 = "https://web.eecs.umich.edu/~pmchen/software1";
      string expected_link2 = "https://web.eecs.umich.edu/~pmchen/software2";
      string expected_link3 = "https://web.eecs.umich.edu/~pmchen/software3";
      REQUIRE( links.size() == 3);
      REQUIRE( links[0] == expected_link1);
      REQUIRE( links[1] == expected_link2);
      REQUIRE( links[2] == expected_link3);
      }

   }