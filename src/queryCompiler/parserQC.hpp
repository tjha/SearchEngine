// parserQC.hpp
//
// Define interface for our parser based off of the third lab.

#ifndef PARSER_H_
#define PARSER_H_

#include <string>
#include "expression.hpp"
#include "../constraintSolver/constraintSolver.hpp"
#include "tokenstream.hpp"

/**
 * The actual expression parser
 */
class Parser
	{
	// Stream of tokens to consume input from
	TokenStream stream;

	dex::index::indexChunk *chunk;

	/**
	 * Find the appropriate nonterminal
	 *
	 * Return nullptr if it could not be found
	 */
	Expression *FindAND( );

	Expression *FindFactor( );

	Expression *FindNot( );

	Expression *FindOR( );

	Expression *FindEmph( );

	Expression *FindPhrase( );

public:

	/**
	 * Construct parser based on given input
	 */
	Parser( const dex::string &in );

	/**
	 * The public interface of the parser. Call this function,
	 * rather than the private internal functions.
	 */
	Expression *Parse( );
	};
// class Parser

#endif /* PARSER_H_ */
