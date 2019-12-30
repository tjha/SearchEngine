// tokenstream.hpp
//
// 2019-12-15: Done: jasina, medhak

#ifndef DEX_TOKENSTREAM
#define DEX_TOKENSTREAM

#include "constraintSolver/constraintSolver.hpp"
#include "queryCompiler/expression.hpp"
#include "utils/basicString.hpp"
#include "utils/unorderedSet.hpp"

namespace dex
	{
	namespace queryCompiler
		{
		// Check if the character is in [a-zA-Z]
		bool isAlpha( char c );

		// Check if character is one of the symbols from our language
		bool isSymbol( char c, bool infix = true );

		// Check if character is whitespace
		bool isWhitespace( char c );

		// Convert c to lower case if it is upper case
		char toLower( char c );

		class tokenStream
			{
			public:
				// The input we receive, with only relevant characters left
				dex::string input;
				// Where we currently are in the input
				size_t location { 0 };
				// Which words in the input were emphasized
				dex::unorderedSet < dex::string > emphasizedWords;

				/**
				 * Construct a token stream that uses a copy of the input
				 * that contains only characters relevant to math expressions
				 */
				tokenStream( const dex::string &in, bool infix = true );

				/**
				 * Attempt to match and consume a specific character
				 *
				 * Returns true if the char was matched and consumed, false otherwise
				 */
				bool match( char c );

				/**
				 * Check whether all the input was consumed
				 */
				bool allConsumed( ) const;

				/**
				 * Attempt to match and consume a whole word
				 *
				 * Return a dynamically allocated Word if successful, nullptr otherwise
				 */
				dex::queryCompiler::word *parseWord( );
			};
		}
	}
#endif
