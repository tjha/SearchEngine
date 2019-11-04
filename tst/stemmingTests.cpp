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
		{ "rational", "ration" }, // This one deviates in step 4
		{ "valenci", "valence" },
		{ "hesitanci", "hesit" }, // This one deviates in step 4
		{ "digitizer", "digit" }, // This one deviates in step 4
		{ "conformabli", "conform" }, // This one deviates in step 4
		{ "radicalli", "radic" }, // This one deviates in step 3
		{ "differentli", "differ" }, // This one deviates in step 4
		{ "vileli", "vile" },
		{ "analogousli", "analog" }, // This one deviates in step 4
		{ "vietnamization", "vietnam" },// This one deviates in step 4
		{ "predication", "predic" }, // This one deviates in step 3
		{ "operator", "oper" }, // This one deviates in step 4
		{ "feudalism", "feudal" },
		{ "decisiveness", "decis" }, // This one deviates in step 4
		{ "hopefulness", "hope" }, // This one deviates in step 3
		{ "callousness", "callous" },
		{ "formaliti", "formal" },
		{ "sensitiviti", "sensit" }, // This one deviates in step 4
		{ "sensibiliti", "sensible" },
		// Step 3
		{ "triplicate", "triplic" },
		{ "formative", "form" },
		{ "formalize", "formal" },
		{ "electriciti", "electr" }, // This one deviates in step 4
		{ "electrical", "electr" }, // This one deviates in step 4
		{ "hopeful", "hope" },
		{ "goodness", "good" },
		// Step 4
		{ "revival", "reviv" },
		{ "allowance", "allow" },
		{ "inference", "infer" },
		{ "airliner", "airlin" },
		{ "gyroscopic", "gyroscop" },
		{ "adjustable", "adjust" },
		{ "defensible", "defens" },
		{ "irritant", "irrit" },
		{ "replacement", "replac" },
		{ "adjustment", "adjust" },
		{ "dependent", "depend" },
		{ "adoption", "adopt" },
		{ "homologou", "homolog" },
		{ "communism", "commun" },
		{ "activate", "activ" },
		{ "angulariti", "angular" },
		{ "homologous", "homolog" },
		{ "effective", "effect" },
		{ "bowdlerize", "bowdler" }
	};

	for ( const string *wordPair : wordPairs)
		{
		string stemmedWord = porterStemmer::stem( wordPair[ 0 ] );
		std::cout << wordPair[ 0 ]  << " -> " << stemmedWord << ", " << wordPair[ 1 ] << std::endl;
		REQUIRE( stemmedWord == wordPair[ 1 ] );
		}
	}
