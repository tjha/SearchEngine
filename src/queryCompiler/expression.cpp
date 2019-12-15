/*
 * expression.cpp
 *
 * Class implementations for expression functionality
 */

#include <cstddef>
#include "basicString.hpp"
#include "constraintSolver.hpp"
#include "expression.hpp"
#include "index.hpp"


Expression::~Expression( ) { }


NotExpression::NotExpression( Expression *value, dex::index::indexChunk *chunk ) : value( value ), chunk( chunk ) { }

NotExpression::~NotExpression( )
	{
	delete value;
	}

dex::constraintSolver::ISR *NotExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR = getEndOfDocumentISR( chunk );
	dex::constraintSolver::ISR *temp = value->eval( );
	return &dex::constraintSolver::notISR( temp, endDocISR );
	}


OrExpression::OrExpression( dex::index::indexChunk *chunk ) : chunk( chunk ) { }

OrExpression::~OrExpression( )
	{
	for ( Expression *expression : terms )
		delete expression;
	}

dex::constraintSolver::ISR *OrExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR = getEndOfDocumentISR( chunk );
	dex::vector < dex::constraintSolver::ISR * > isrs;
	for ( size_t i = 0;  i < terms.size( );  ++i )
		isrs.pushBack( terms[ i ]->eval( ) );

	return new dex::constraintSolver::orISR( isrs, endDocISR );
	}


AndExpression::AndExpression( dex::index::indexChunk *chunk ) { }

AndExpression::~AndExpression( )
	{
	for ( Expression *isr : terms )
		delete isr;
	}

dex::constraintSolver::ISR *AndExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR = getEndOfDocumentISR( chunk );
	dex::vector < dex::constraintSolver::ISR * > isrs;
	for ( size_t i = 0;  i < terms.size( );  ++i )
		isrs.pushBack( terms[ i ]->eval( ) );

	return new dex::constraintSolver::andISR( isrs, endDocISR );
	}


PhraseExpression::PhraseExpression( dex::index::indexChunk *chunk ) : chunk( chunk ) { }

PhraseExpression::~PhraseExpression( )
	{
	for ( Expression *isr : terms )
		delete isr;
	}

dex::constraintSolver::ISR *PhraseExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR = getEndOfDocumentISR( chunk );
	dex::vector < dex::constraintSolver::ISR * > isrs;
	for ( size_t i = 0;  i < terms.size( );  ++i )
		isrs.pushBack( terms[ i ]->eval( ) );

	return new dex::constraintSolver::andISR( isrs, endDocISR );
	}

Word::Word( dex::string word, dex::index::indexChunk *chunk ) : word( word ), chunk( chunk ) { }

dex::constraintSolver::ISR *Word::eval( ) const
	{
	return new dex::index::indexChunk::indexStreamReader( chunk, word );
	}
