// redirectCacheTests.cpp
// Testing for our redirectCache
//
// 2019-12-09: REMOVED FROM CODEBASE
// 2019-11-20: Init Commit: combsc

#include "catch.hpp"
#include "../driver/redirectCache.hpp"
#include <stdio.h>
#include <unistd.h>

using dex::redirectCache;
using dex::string;

TEST_CASE( "basic insertion", "[redirect]" )
	{
	string a = "https://www.a.com";
	string b = "https://www.b.com";
	string c = "https://www.c.com";
	string d = "https://www.d.com";
	string e = "https://www.e.com";

	redirectCache r( 20 );
	REQUIRE( r.updateUrl( a.cStr( ), c.cStr( ) ) == 0 );
	REQUIRE( r.updateUrl( b.cStr( ), c.cStr( ) ) == 0 );
	REQUIRE( r.updateUrl( c.cStr( ), d.cStr( ) ) == 0 );
	REQUIRE( r.updateUrl( d.cStr( ), e.cStr( ) ) == 0 );

	REQUIRE( r.getEndpoint( a.cStr( ) ).completeUrl( ) == e ); 
	REQUIRE( r.getEndpoint( b.cStr( ) ).completeUrl( ) == e ); 
	REQUIRE( r.getEndpoint( c.cStr( ) ).completeUrl( ) == e ); 
	REQUIRE( r.getEndpoint( d.cStr( ) ).completeUrl( ) == e ); 
	REQUIRE( r.getEndpoint( e.cStr( ) ).completeUrl( ) == e ); 


	r.removeUrl( c.cStr( ) );
	REQUIRE( r.getEndpoint( a.cStr( ) ).completeUrl( ) == c ); 
	REQUIRE( r.getEndpoint( b.cStr( ) ).completeUrl( ) == c ); 

	}

TEST_CASE( "loops", "[redirect]" )
	{
	string a = "https://www.a.com";
	string b = "https://www.b.com";
	string c = "https://www.c.com";

	redirectCache r( 100 );
	REQUIRE( r.updateUrl( a.cStr( ), b.cStr( ) ) == 0 );
	REQUIRE( r.updateUrl( b.cStr( ), c.cStr( ) ) == 0 );
	REQUIRE( r.updateUrl( c.cStr( ), a.cStr( ) ) == -1 );
	}

TEST_CASE( "expire", "[redirect]" )
	{
	string a = "https://www.a.com";
	string b = "https://www.b.com";
	string c = "https://www.c.com";

	redirectCache r( 100 );
	REQUIRE( r.updateUrl( a.cStr( ), b.cStr( ) ) == 0 );
	REQUIRE( r.updateUrl( b.cStr( ), c.cStr( ), 1 ) == 0 );
	sleep( 1 );
	REQUIRE( r.getEndpoint( a.cStr( ) ).completeUrl( ) == b );
	}
