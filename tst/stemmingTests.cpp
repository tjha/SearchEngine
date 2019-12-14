// stemmingTests
// test[s] for the Port[er] stem[mer]. Taken from http://snowball.tartarus.org/algorithms/porter/stemmer.html
//
// 2019-12-13: Add esoteric cases: jasina
// 2019-11-04: File created: jasina

#include "catch.hpp"
#include "../src/utils/basicString.hpp"
#include "../src/utils/stemming.hpp"
#include "../src/utils/vector.hpp"

using namespace dex;
TEST_CASE( "test stems", "[stemmer]" )
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
		{ "agreed", "agre" }, // This one deviates in step 5
		{ "plastered", "plaster" },
		{ "bled", "bled" },
		{ "motoring", "motor" },
		{ "sing", "sing" },
		{ "conflated", "conflat" }, // This one deviates in step 5
		{ "troubled", "troubl" }, // This one deviates in step 5
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
		{ "relational", "relat" }, // This one deviates in step 5
		{ "conditional", "condition" },
		{ "rational", "ration" }, // This one deviates in step 4
		{ "valenci", "valenc" }, // This one deviates in step 5
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
		{ "sensibiliti", "sensibl" }, // This one deviates in step 5
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
		{ "bowdlerize", "bowdler" },
		// Step 5a
		{ "probate", "probat" },
		{ "rate", "rate" },
		// Step 5b
		{ "controll", "control" },
		{ "roll", "roll" },
		// Others
		{ "s", "s" },
		{ "ess", "ess" },
		{ "", "" }
	};

	for ( const string *wordPair : wordPairs)
		REQUIRE( porterStemmer::stem( wordPair[ 0 ] ) == wordPair[ 1 ] );
	}
