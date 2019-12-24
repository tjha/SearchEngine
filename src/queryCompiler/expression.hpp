// expression.hpp
//
// 2019-12-15: Done (Desperate times): jasina, medhak

#ifndef DEX_EXPRESSION
#define DEX_EXPRESSION

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
				virtual dex::constraintSolver::ISR *eval( dex::index::indexChunk *chunk ) const = 0;
				virtual dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const = 0;
				virtual dex::string toString( ) const = 0;
			};

		inline dex::index::indexChunk::endOfDocumentIndexStreamReader *getEndOfDocumentISR( dex::index::indexChunk *chunk )
			{
			return new dex::index::indexChunk::endOfDocumentIndexStreamReader( chunk, "" );
			}

		class notExpression: public expression
			{
			public:
				expression *value;
				notExpression( expression *value );
				~notExpression( );
				dex::constraintSolver::ISR *eval( dex::index::indexChunk *chunk ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				dex::string toString( ) const override;
			};

		class orExpression: public expression
			{
			public:
				dex::vector < expression * > terms;
				~orExpression( );
				dex::constraintSolver::ISR *eval( dex::index::indexChunk *chunk ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				dex::string toString( ) const override;
			};

		class andExpression: public expression
			{
			public:
				dex::vector < expression * > terms;
				~andExpression( );
				dex::constraintSolver::ISR *eval( dex::index::indexChunk *chunk ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				dex::string toString( ) const override;
			};

		class phraseExpression : public expression
			{
			public:
				dex::vector < expression * > terms;
				~phraseExpression( );
				dex::constraintSolver::ISR *eval( dex::index::indexChunk *chunk ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				dex::string toString( ) const override;
			};

		class word: public expression
			{
			public:
				dex::string str;
				word( dex::string str );
				dex::constraintSolver::ISR *eval( dex::index::indexChunk *chunk ) const override;
				dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > flattenedQuery( ) const override;
				dex::string toString( ) const override;
			};
		}
	}

#endif
