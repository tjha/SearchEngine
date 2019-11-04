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
	const string wordPairs[ ][ 2 ] = {
		// Step 1a
		{ "caresses", "caress" },
		{ "ponies", "poni" },
		{ "ties", "ti" },
		{ "caress", "caress" },
		{ "cats", "cat" },
		// Step 1b
		{ "feed", "feed" },
		{ "agreed", "agree" },
		{ "plastered", "plaster" },
		{ "bled", "bled" },
		{ "motoring", "motor" },
		{ "sing", "sing" },
		{ "conflated", "conflate" },
		{ "troubled", "trouble" },
		{ "sized", "size" },
		{ "hopping", "hop" },
		{ "tanned", "tan" },
		{ "falling", "fall" },
		{ "hissing", "hiss" },
		{ "fizzed", "fizz" },
		{ "failing", "fail" },
		{ "filing", "file" },
		// Step 1c
		{ "happy", "happi" },
		{ "sky", "sky" },
		// Step 2
		{ "relational", "relate" },
		{ "conditional", "condition" },
		{ "rational", "rational" },
		{ "valenci", "valence" },
		{ "hesitanci", "hesitance" },
		{ "digitizer", "digitize" },
		{ "conformabli", "conformable" },
		{ "radicalli", "radic" }, // This one fails in step 3
		{ "differentli", "different" },
		{ "vileli", "vile" },
		{ "analogousli", "analogous" },
		{ "vietnamization", "vietnamize" },
		{ "predication", "predic" }, // This one fails in step 3
		{ "operator", "operate" },
		{ "feudalism", "feudal" },
		{ "decisiveness", "decisive" },
		{ "hopefulness", "hope" }, // This one fails in step 3
		{ "callousness", "callous" },
		{ "formaliti", "formal" },
		{ "sensitiviti", "sensitive" },
		{ "sensibiliti", "sensible" },
		// Step 3
		{ "triplicate", "triplic" },
		{ "formative", "form" },
		{ "formalize", "formal" },
		{ "electriciti", "electric" },
		{ "electrical", "electric" },
		{ "hopeful", "hope" },
		{ "goodness", "good" }
	};

	for ( const string *wordPair : wordPairs)
		{
		string stemmedWord = porterStemmer::stem( wordPair[ 0 ] );
		std::cout << wordPair[ 0 ]  << " -> " << stemmedWord << ", " << wordPair[ 1 ] << std::endl;
		REQUIRE( stemmedWord == wordPair[ 1 ] );
		}
	}
