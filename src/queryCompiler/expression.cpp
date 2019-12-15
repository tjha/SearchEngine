// expression.cpp
//
// 2019-12-15: Done (desperate times): jasina, medhak

#include <cstddef>
#include "../constraintSolver/constraintSolver.hpp"
#include "../indexer/index.hpp"
#include "../queryCompiler/expression.hpp"
#include "../utils/basicString.hpp"


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
	return new dex::constraintSolver::notISR( temp, endDocISR );
	}

dex::pair < dex::vector < dex::string >, dex::vector < dex::string > >
		dex::queryCompiler::notExpression::flattenedQuery( ) const
	{
	const dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > &valueFlattenedQuery
			= value->flattenedQuery( );
	return dex::pair < dex::vector < dex::string >, dex::vector < dex::string > >(
			valueFlattenedQuery.second, valueFlattenedQuery.first );
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

dex::pair < dex::vector < dex::string >, dex::vector < dex::string > >
		dex::queryCompiler::orExpression::flattenedQuery( ) const
	{
	dex::vector < dex::string > left, right;
	for ( size_t index = 0;  index < terms.size( );  ++index )
		{
		const dex::vector < dex::string > &subLeft = terms[ index ]->flattenedQuery( ).first;
		const dex::vector < dex::string > &subRight = terms[ index ]->flattenedQuery( ).second;
		for ( size_t jndex = 0;  jndex < subLeft.size( );  ++jndex )
			left.pushBack( subLeft[ jndex ] );
		for ( size_t jndex = 0;  jndex < subRight.size( );  ++jndex )
			right.pushBack( subRight[ jndex ] );
		}

	return dex::pair < dex::vector < dex::string >, dex::vector < dex::string > >( left, right );
	}


dex::queryCompiler::andExpression::andExpression( dex::index::indexChunk *chunk ) : chunk( chunk ) { }

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

dex::pair < dex::vector < dex::string >, dex::vector < dex::string > >
		dex::queryCompiler::andExpression::flattenedQuery( ) const
	{
	dex::vector < dex::string > left, right;
	for ( size_t index = 0;  index < terms.size( );  ++index )
		{
		const dex::vector < dex::string > &subLeft = terms[ index ]->flattenedQuery( ).first;
		const dex::vector < dex::string > &subRight = terms[ index ]->flattenedQuery( ).second;
		for ( size_t jndex = 0;  jndex < subLeft.size( );  ++jndex )
			left.pushBack( subLeft[ jndex ] );
		for ( size_t jndex = 0;  jndex < subRight.size( );  ++jndex )
			right.pushBack( subRight[ jndex ] );
		}

	return dex::pair < dex::vector < dex::string >, dex::vector < dex::string > >( left, right );
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

dex::pair < dex::vector < dex::string >, dex::vector < dex::string > >
		dex::queryCompiler::phraseExpression::flattenedQuery( ) const
	{
	dex::vector < dex::string > left, right;
	for ( size_t index = 0;  index < terms.size( );  ++index )
		{
		const dex::vector < dex::string > &subLeft = terms[ index ]->flattenedQuery( ).first;
		const dex::vector < dex::string > &subRight = terms[ index ]->flattenedQuery( ).second;
		for ( size_t jndex = 0;  jndex < subLeft.size( );  ++jndex )
			left.pushBack( subLeft[ jndex ] );
		for ( size_t jndex = 0;  jndex < subRight.size( );  ++jndex )
			right.pushBack( subRight[ jndex ] );
		}

	return dex::pair < dex::vector < dex::string >, dex::vector < dex::string > >( left, right );
	}

dex::queryCompiler::word::word( dex::string str, dex::index::indexChunk *chunk ) : str( str ), chunk( chunk ) { }

dex::constraintSolver::ISR *dex::queryCompiler::word::eval( ) const
	{
	return new dex::constraintSolver::orISR( dex::vector < dex::constraintSolver::ISR * >{
			new dex::index::indexChunk::indexStreamReader( chunk, str ),
			new dex::index::indexChunk::indexStreamReader( chunk, "#" + str ) }, getEndOfDocumentISR( chunk ) );
	}

dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > dex::queryCompiler::word::flattenedQuery( ) const
	{
	return dex::pair  < dex::vector < dex::string >, dex::vector < dex::string > >(
			dex::vector < dex::string >{ str }, dex::vector < dex::string >( ) );
	}
