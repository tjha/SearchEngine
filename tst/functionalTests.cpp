// functional.cpp
// Tests for the functional.hpp file
//
// 2019-11-02: File created, some hash tests: jasina

#include "catch.hpp"
#include "functional.hpp"

using dex::hash;

TEST_CASE( "hash", "[functional]" )
	{
	hash < int > intHasher;
	REQUIRE( intHasher( 1 ) != 1 );
	REQUIRE( intHasher( 1 ) != intHasher( 2 ) );
	}
