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
#include "utils/utility.hpp"

namespace dex
	{
	namespace queryCompiler
		{
		class matchedDocumentsGenerator
			{
			private:
				bool invalid;
				dex::queryCompiler::expression *root;
				tokenStream *stream;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery;
				dex::vector < bool > emphasizedWords;
				dex::string query;

			public:
				matchedDocumentsGenerator( dex::queryCompiler::expression *root, dex::queryCompiler::tokenStream *stream );
				~matchedDocumentsGenerator( );
				dex::matchedDocuments *operator( )( dex::index::indexChunk *chunk ) const;
				dex::string getQuery( ) const;
			};

		class parser
			{
				// Stream of tokens to consume input from
				tokenStream *stream;

				/**
				 * Find the appropriate nonterminal
				 *
				 * Return nullptr if it could not be found
				 */
				expression *findPhrase( );
				expression *findFactor( );
				expression *findNot( );
				expression *findAnd( );
				expression *findOr( );

				expression *parsePrefix( );

			public:
				/**
				 * The public interface of the parser. Call this function,
				 * rather than the private internal functions.
				 */
				matchedDocumentsGenerator parse( dex::string &in, bool infix = true );
			};
		}
	}

#endif
