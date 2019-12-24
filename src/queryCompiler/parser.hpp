// parser.hpp
//
// Define interface for our parser based off of the third lab.

#ifndef DEX_QUERY_PARSER
#define DEX_QUERY_PARSER

#include "constraintSolver/constraintSolver.hpp"
#include "queryCompiler/expression.hpp"
#include "queryCompiler/tokenstream.hpp"
#include "ranker/rankerObjects.hpp"
#include "utils/basicString.hpp"

namespace dex
	{
	namespace queryCompiler
		{
		class parser
			{
				// Stream of tokens to consume input from
				tokenStream *stream;

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
				 * The public interface of the parser. Call this function,
				 * rather than the private internal functions.
				 */
				dex::matchedDocuments *parse( dex::string &in, dex::index::indexChunk *chunk );
			};
		}
	}

#endif
