// vector_tests.cpp
//
// Perform tests on vector implementation
//
// 2019/09/17 Tejas Jha - Implemented basic constructor test cases

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../utils/vector.hpp"

TEST_CASE( "Vectors are constructed", "[vector]")
   {
   dex::vector< int > v1;           // default constructor
   dex::vector< int > v2( 5 );      // vector with specified size
   dex::vector< int > v3( 0 );      // vector with size specified to 0
   dex::vector< int > v4( 5, 2 );   // vector of specified size and default val
   }

