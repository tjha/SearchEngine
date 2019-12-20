// urlTests.cpp
// testing for URL class

// 2019-12-10: init commit: combsc

#include "catch.hpp"
#include "crawler/url.hpp"

TEST_CASE( "basics", "[url]" )
	{
	dex::Url example( "https://www.example.com/some/long/path?query#fragment" );
	dex::Url otherExample = example;
	dex::Url thirdExample;
	thirdExample = otherExample;

	REQUIRE( example.completeUrl( ) == "https://www.example.com/some/long/path?query#fragment" );
	REQUIRE( example.getDomain( ) == "example.com" );
	REQUIRE( example.getSubdomain( ) == "www" );
	REQUIRE( example.getHost( ) == "www.example.com" );
	REQUIRE( example.getService( ) == "https" );
	REQUIRE( example.getFragment( ) == "fragment" );
	REQUIRE( example.getPath( ) == "/some/long/path" );

	REQUIRE( otherExample.completeUrl( ) == "https://www.example.com/some/long/path?query#fragment" );
	REQUIRE( otherExample.getDomain( ) == "example.com" );
	REQUIRE( otherExample.getSubdomain( ) == "www" );
	REQUIRE( otherExample.getHost( ) == "www.example.com" );
	REQUIRE( otherExample.getService( ) == "https" );
	REQUIRE( otherExample.getFragment( ) == "fragment" );
	REQUIRE( otherExample.getPath( ) == "/some/long/path" );

	REQUIRE( thirdExample.completeUrl( ) == "https://www.example.com/some/long/path?query#fragment" );
	REQUIRE( thirdExample.getDomain( ) == "example.com" );
	REQUIRE( thirdExample.getSubdomain( ) == "www" );
	REQUIRE( thirdExample.getHost( ) == "www.example.com" );
	REQUIRE( thirdExample.getService( ) == "https" );
	REQUIRE( thirdExample.getFragment( ) == "fragment" );
	REQUIRE( thirdExample.getPath( ) == "/some/long/path" );
	
	}
