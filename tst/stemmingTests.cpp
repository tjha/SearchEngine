// stemmingTests
// test[s] for the Port[er] stem[mer]
//
// 2019-11-04: File created: jasina

#include <iostream>
#include "catch.hpp"
#include "../src/utils/basicString.hpp"
#include "../src/utils/stemming.hpp"
#include "../src/utils/vector.hpp"

using namespace dex;
TEST_CASE( "test stuff", "[stemmer]" )
	{
	std::cout << porterStemmer::stem( "caresses" ) << std::endl;
	std::cout << porterStemmer::stem( "ponies" ) << std::endl;
	std::cout << porterStemmer::stem( "ties" ) << std::endl;
	std::cout << porterStemmer::stem( "caress" ) << std::endl;
	std::cout << porterStemmer::stem( "cats" ) << std::endl;
	}
