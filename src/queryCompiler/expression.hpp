#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "basicString.hpp"
#include "index.hpp"
#include "vector.hpp"
#include "constraintSolver.hpp"

class Expression
	{
	public:
		virtual ~Expression( );
		virtual dex::constraintSolver::ISR *eval( ) const = 0;
	};

dex::index::indexChunk::endOfDocumentIndexStreamReader *getEndOfDocumentISR( dex::index::indexChunk *chunk )
	{
	return new dex::index::indexChunk::endOfDocumentIndexStreamReader( chunk, "" );
	}

class NotExpression: public Expression
	{
	protected:
		Expression* value;
		dex::index::indexChunk *chunk;
	public:
		NotExpression( Expression *value, dex::index::indexChunk *chunk );
		~NotExpression( );
		dex::constraintSolver::ISR *eval( ) const override;
	};

class OrExpression: public Expression
	{
	protected:
		dex::vector < Expression * > terms;
		dex::index::indexChunk *chunk;
	public:
		OrExpression( dex::index::indexChunk *chunk );
		~OrExpression( );
		dex::constraintSolver::ISR *eval( ) const override;
	};

class AndExpression: public Expression
	{
	protected:
		dex::vector < Expression * > terms;
		dex::index::indexChunk *chunk;
	public:
		AndExpression( dex::index::indexChunk *chunk );
		~AndExpression( );
		dex::constraintSolver::ISR *eval( ) const override;
	};

class PhraseExpression : public Expression
	{
	protected:
		dex::vector < Expression * > terms;
		dex::index::indexChunk *chunk;
	public:
		PhraseExpression( dex::index::indexChunk *chunk );
		~PhraseExpression( );
		dex::constraintSolver::ISR *eval( ) const override;
	};

class Word: public Expression
	{
	protected:
		dex::string word;
		dex::index::indexChunk *chunk;
	public:
		Word( dex::string word, dex::index::indexChunk *chunk );
		dex::constraintSolver::ISR *eval( ) const override;
	}

#endif /* EXPRESSION_H_ */
