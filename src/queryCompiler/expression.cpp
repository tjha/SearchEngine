// expression.cpp
//
// 2019-12-15: Done (desperate times): jasina, medhak

#include <cstddef>
#include "basicString.hpp"
#include "constraintSolver.hpp"
#include "expression.hpp"
#include "index.hpp"


dex::queryCompiler::expression::~expression( ) { }


dex::queryCompiler::notExpression::notExpression( expression *value, dex::index::indexChunk *chunk )
		: value( value ), chunk( chunk ) { }

dex::queryCompiler::notExpression::~notExpression( )
	{
	delete value;
	}

dex::constraintSolver::ISR *dex::queryCompiler::notExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR = getEndOfDocumentISR( chunk );
	dex::constraintSolver::ISR *temp = value->eval( );
	return &dex::constraintSolver::notISR( temp, endDocISR );
	}


dex::queryCompiler::orExpression::orExpression( dex::index::indexChunk *chunk ) : chunk( chunk ) { }

dex::queryCompiler::orExpression::~orExpression( )
	{
	for ( expression *expression : terms )
		delete expression;
	}

dex::constraintSolver::ISR *dex::queryCompiler::orExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR = getEndOfDocumentISR( chunk );
	dex::vector < dex::constraintSolver::ISR * > isrs;
	for ( size_t i = 0;  i < terms.size( );  ++i )
		isrs.pushBack( terms[ i ]->eval( ) );

	return new dex::constraintSolver::orISR( isrs, endDocISR );
	}


dex::queryCompiler::andExpression::andExpression( dex::index::indexChunk *chunk ) { }

dex::queryCompiler::andExpression::~andExpression( )
	{
	for ( expression *isr : terms )
		delete isr;
	}

dex::constraintSolver::ISR *dex::queryCompiler::andExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR = getEndOfDocumentISR( chunk );
	dex::vector < dex::constraintSolver::ISR * > isrs;
	for ( size_t i = 0;  i < terms.size( );  ++i )
		isrs.pushBack( terms[ i ]->eval( ) );

	return new dex::constraintSolver::andISR( isrs, endDocISR );
	}


dex::queryCompiler::phraseExpression::phraseExpression( dex::index::indexChunk *chunk ) : chunk( chunk ) { }

dex::queryCompiler::phraseExpression::~phraseExpression( )
	{
	for ( expression *isr : terms )
		delete isr;
	}

dex::constraintSolver::ISR *dex::queryCompiler::phraseExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR = getEndOfDocumentISR( chunk );
	dex::vector < dex::constraintSolver::ISR * > isrs;
	for ( size_t i = 0;  i < terms.size( );  ++i )
		isrs.pushBack( terms[ i ]->eval( ) );

	return new dex::constraintSolver::andISR( isrs, endDocISR );
	}

dex::queryCompiler::word::word( dex::string str, dex::index::indexChunk *chunk ) : str( str ), chunk( chunk ) { }

dex::constraintSolver::ISR *dex::queryCompiler::word::eval( ) const
	{
	return new dex::index::indexChunk::indexStreamReader( chunk, str );
	}
