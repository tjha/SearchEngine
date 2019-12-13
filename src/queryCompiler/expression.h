/*
 * expression.h
 *
 * Class declarations for expressions
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include "basicString.hpp"
#include "index.hpp"
#include "vector.hpp"

/**
 * Just a plain old expression
 */
class Expression
	{
	public:
		virtual ~Expression( );
		virtual int64_t Eval( ) const = 0;
		virtual dex::constraintSolver::ISR *eval( string word ) const = 0;
	};
// class Expression

/**
 * <Neg> ::= '-' <AddSub>
 */

class PhraseExpression : public Expression
	{
	protected:
		dex::vector < dex::string > words;
		dex::index::indexChunk::endOfDocumentIndexStreamReader *endOfDocISR;
	public:
		PhraseExpression( );
		~PhraseExpression( );
		dex::constraintSolverISR *eval( ) const override;
	};

class NotExpression: public Expression
	{
	protected:
		Expression *value;
		dex::index::indexChunk::endOfDocumentIndexStreamReader *endOfDocISR;
	public:
		NotExpression( Expression *value );
		~NotExpression( );
		dex::constraintSolver::ISR *eval( ) const override;
	};
// class AddSub

/**
 * <AddSub> ::= <MultDiv> { ‘+’ <MultDiv> }
 */
class OrExpression: public Expression
	{
	protected:
		std::vector < Expression * > terms;
		dex::index::indexChunk::endOfDocumentIndexStreamReader *endOfDocISR;
		friend class Parser;
	public:
		OrExpression( );
		~OrExpression( );
		dex::constraintSolver::ISR *eval( ) const override;
	};
// class AddSub

/**
 * <MultDiv> ::= <FactorExpression> { ‘*’ <FactorExpression> }
 */
class AndExpression: public Expression
	{
	protected:
		std::vector < Expression * > terms;
		dex::index::indexChunk::endOfDocumentIndexStreamReader *endOfDocISR;
		friend class Parser;
	public:
		AndExpression( );
		~AndExpression( );
		dex::constraintSolver::ISR *eval( ) const override;
	};
// class MultDiv

#endif /* EXPRESSION_H_ */
