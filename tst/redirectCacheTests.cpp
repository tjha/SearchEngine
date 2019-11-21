// redirectCacheTests.cpp
// Testing for our redirectCache
//
// 2019-11-20: Init Commit: combsc

#include "catch.hpp"
#include "../mvp/redirectCache.hpp"

using dex::redirectCache;
using dex::string;

TEST_CASE( "basic insertion", "[redirect]" )
	{
	string a = "https://www.a.com";
	string b = "https://www.b.com";
	string c = "https://www.c.com";
	string d = "https://www.d.com";
	string e = "https://www.e.com";

	redirectCache r;
	r.updateUrl( a.cStr( ), c.cStr( ) );
	r.updateUrl( b.cStr( ), c.cStr( ) );
	r.updateUrl( c.cStr( ), d.cStr( ) );
	r.updateUrl( d.cStr( ), e.cStr( ) );

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

	redirectCache r;
	REQUIRE( r.updateUrl( a.cStr( ), b.cStr( ) ) == 0 );
	REQUIRE( r.updateUrl( b.cStr( ), c.cStr( ) ) == 0 );
	REQUIRE( r.updateUrl( c.cStr( ), a.cStr( ) ) == -1 );
	}