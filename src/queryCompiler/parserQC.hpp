// parserQC.hpp
//
// Define interface for our parser based off of the third lab.

#ifndef DEX_PARSER
#define DEX_PARSER

#include "constraintSolver/constraintSolver.hpp"
#include "queryCompiler/expression.hpp"
#include "queryCompiler/tokenstream.hpp"
#include "ranker/rankerObjects.hpp"

namespace dex
	{
	namespace queryCompiler
		{
		class parser
			{
				// Stream of tokens to consume input from
				tokenStream stream;

				dex::index::indexChunk *chunk;

				/**
				 * Find the appropriate nonterminal
				 *
				 * Return nullptr if it could not be found
				 */
				expression *findAnd( );

				expression *findFactor( );

				expression *findNot( );

				expression *findOr( );

				expression *findEmph( );

			public:
				/**
				 * Construct parser based on given input
				 */
				parser( const dex::string &in, dex::index::indexChunk *chunkIn );

				/**
				 * The public interface of the parser. Call this function,
				 * rather than the private internal functions.
				 */
				dex::matchedDocuments *parse( );
			};
		}
	}

#endif
