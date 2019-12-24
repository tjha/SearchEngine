// expression.hpp
//
// 2019-12-15: Done (Desperate times): jasina, medhak

#ifndef DEX_EXPRESSION
#define DEX_EXPRESSION

#include <iostream>
#include "constraintSolver/constraintSolver.hpp"
#include "indexer/index.hpp"
#include "utils/basicString.hpp"
#include "utils/utility.hpp"
#include "utils/vector.hpp"

namespace dex
	{
	namespace queryCompiler
		{
		class expression
			{
			public:
				virtual ~expression( );
				virtual dex::constraintSolver::ISR *eval( ) const = 0;
				virtual dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const = 0;
				virtual void print( size_t depth = 0 ) const = 0;
			};

		inline dex::index::indexChunk::endOfDocumentIndexStreamReader *getEndOfDocumentISR( dex::index::indexChunk *chunk )
			{
			return new dex::index::indexChunk::endOfDocumentIndexStreamReader( chunk, "" );
			}

		class notExpression: public expression
			{
			public:
				expression *value;
				dex::index::indexChunk *chunk;
				notExpression( expression *value, dex::index::indexChunk *chunk );
				~notExpression( );
				dex::constraintSolver::ISR *eval( ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				void print( size_t depth = 0 ) const override;
			};

		class orExpression: public expression
			{
			public:
				dex::vector < expression * > terms;
				dex::index::indexChunk *chunk;
				orExpression( dex::index::indexChunk *chunk );
				~orExpression( );
				dex::constraintSolver::ISR *eval( ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				void print( size_t depth = 0 ) const override;
			};

		class andExpression: public expression
			{
			public:
				dex::vector < expression * > terms;
				dex::index::indexChunk *chunk;
				andExpression( dex::index::indexChunk *chunk );
				~andExpression( );
				dex::constraintSolver::ISR *eval( ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				void print( size_t depth = 0 ) const override;
			};

		class phraseExpression : public expression
			{
			public:
				dex::vector < expression * > terms;
				dex::index::indexChunk *chunk;
				phraseExpression( dex::index::indexChunk *chunk );
				~phraseExpression( );
				dex::constraintSolver::ISR *eval( ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				void print( size_t depth = 0 ) const override;
			};

		class word: public expression
			{
			public:
				dex::string str;
				dex::index::indexChunk *chunk;
				word( dex::string str, dex::index::indexChunk *chunk );
				dex::constraintSolver::ISR *eval( ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				void print( size_t depth = 0 ) const override;
			};
		}
	}

#endif
