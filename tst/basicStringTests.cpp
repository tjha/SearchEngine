// basicStringTests.cpp
// Testing for the basicString class
//
// 2019-09-25: Created File: combsc

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../utils/basicString.hpp"
#include "../utils/exception.hpp"

TEST_CASE( "Constructing strings" )
	{
	dex::basicString<char> s0();
	REQUIRE( s0 == "" );

	dex::basicString<char> s1 = "";
	REQUIRE( s1 == "" );

	dex::basicString<char> s2 = "asd";
	REQUIRE( s2 == "asd" );

	dex::basicString<char> s3 = s2;
	REQUIRE( s3 == s2 );
	REQUIRE( s3 == "asd" );

	s2 = "dsa";
	REQUIRE( s3 == "asd" );
	REQUIRE( s2 == "dsa" );

	dex::basicString<char> s4(s3);
	REQUIRE( s4 == s3 );
	REQUIRE( s4 == "asd" );

	s3 = "Test String";
	REQUIRE( s4 == "asd" );
	REQUIRE( s3 == "Test String" );

	dex::basicString<char> s5(s3, 2, 5);
	REQUIRE( s3 == "Test String" );
	REQUIRE( s4 == "st St" );

	dex::basicString<char> s6(s3, 2, 500);
	REQUIRE( s3 == "Test String" );
	REQUIRE( s4 == "st String" );

	dex::basicString<char> s7("Test String", 3);
	REQUIRE( s7 == "Tes" );
	dex::basicString<char> s8("Test String", 500);
	REQUIRE( s8 == "Test String" );
	dex::basicString<char> s9("Test String");
	REQUIRE( s9 == "Test String" );

	dex::basicString<char> s10(0, 'b');
	REQUIRE( s10 == "" );
	dex::basicString<char> s11(3, 'b');
	REQUIRE( s11 == "bbb");
	}

TEST_CASE( "Throwing exceptions" )
	{
	dex::basicString<char> s0 = "Test String";
	bool caught = false;
	try
		{
		s0.at(500);
		}
	catch() dex::outOfRangeException )
		{
		caught = true;
		}
	REQUIRE(caught);
	caught = false;

	try
		{
		dex::basicString<char> s1(s0, 500);
		}
	catch( dex::outOfRangeException )
		{
		caught = true;
		}
	REQUIRE(caught);
	caught = false;

	dex::basicString<char>::iterator i0 = s0.begin();
	dex::basicString<char>::iterator i1 = s0.end();
	}

