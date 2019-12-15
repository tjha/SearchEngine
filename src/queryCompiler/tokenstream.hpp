/*
 * tokenstream.h
 *
 * Declaration of a stream of tokens that you can read from
 *
 * Lab3: You do not have to modify this file, but you may choose to do so
 */

#ifndef TOKENSTREAM_H_
#define TOKENSTREAM_H_


#include "expression.hpp"

#include "../constraintSolver/constraintSolver.hpp"
#include "../indexer/index.hpp"
#include "../utils/basicString.hpp"

/**
 * Check if a character is relevant to a math expression
 *
 * Relevant characters are defined as
 * +
 * -
 * *
 * (
 * )
 *
 * and all digits
 */
bool CharIsRelevant( char c );

/**
 * Opposite of char is relevant. Needed for filtering the input
 */
bool CharIsIrrelevant( char c );

/**
 * The token stream, which you can both Match( ) a single character from,
 * or ParseNumber( ) to consume a whole int64_t
 *
 * The input string by default is filtered of any characters that are
 * deemed "irrelevant" by the CharIsIrrelevant function above
 */
class TokenStream
	{
	// The input we receive, with only relevant characters left
	dex::string input;
	// Where we currently are in the input
	size_t location { 0 };

	dex::index::indexChunk *chunk;

public:

	/**
	 * Construct a token stream that uses a copy of the input
	 * that contains only characters relevant to math expressions
	 */
	TokenStream( const dex::string &in, dex::index::indexChunk *chunk );

	/**
	 * Attempt to match and consume a specific character
	 *
	 * Returns true if the char was matched and consumed, false otherwise
	 */
	bool Match( char c );

	/**
	 * Check whether all the input was consumed
	 */
	bool AllConsumed( ) const;

	/**
	 * Attempt to match and consume a whole word
	 *
	 * Return a dynamically allocated Word if successful, nullptr otherwise
	 */
	Word *ParsePhrase( );
	};

#endif /* TOKENSTREAM_H_ */
