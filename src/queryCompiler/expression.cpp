// expression.cpp
//
// 2019-12-15: Done (desperate times): jasina, medhak

#include <cstddef>
#include "constraintSolver/constraintSolver.hpp"
#include "indexer/index.hpp"
#include "queryCompiler/expression.hpp"
#include "utils/basicString.hpp"
#include "utils/stemming.hpp"


dex::queryCompiler::expression::~expression( ) { }


dex::queryCompiler::notExpression::notExpression( expression *value, dex::index::indexChunk *chunk )
		: value( value ), chunk( chunk ) { }

dex::queryCompiler::notExpression::~notExpression( )
	{
	delete value;
	}

dex::constraintSolver::ISR *dex::queryCompiler::notExpression::eval( ) const
	{
	if ( !chunk )
		return nullptr;

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

void dex::queryCompiler::notExpression::print( size_t depth ) const
	{
	for ( size_t i = 0;  i != depth;  ++i )
		std::cout << '\t';
	std::cout << "NOT" << std::endl;
	if ( value )
		value->print( depth + 1 );
	}


dex::queryCompiler::orExpression::orExpression( dex::index::indexChunk *chunk ) : chunk( chunk ) { }

dex::queryCompiler::orExpression::~orExpression( )
	{
	for ( expression *expression : terms )
		delete expression;
	}

dex::constraintSolver::ISR *dex::queryCompiler::orExpression::eval( ) const
	{
	if ( !chunk )
		return nullptr;

	if ( terms.size( ) == 1 )
		return terms.front( )->eval( );

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

void dex::queryCompiler::orExpression::print( size_t depth ) const
	{
	if ( terms.size( ) == 1 )
		{
		terms.front( )->print( depth );
		return;
		}

	for ( size_t i = 0;  i != depth;  ++i )
		std::cout << '\t';
	std::cout << "OR" << std::endl;
	for ( size_t index = 0;  index != terms.size( );  ++index )
		terms[ index ]->print( depth + 1 );
	}


dex::queryCompiler::andExpression::andExpression( dex::index::indexChunk *chunk ) : chunk( chunk ) { }

dex::queryCompiler::andExpression::~andExpression( )
	{
	for ( expression *isr : terms )
		delete isr;
	}

dex::constraintSolver::ISR *dex::queryCompiler::andExpression::eval( ) const
	{
	if ( !chunk )
		return nullptr;

	if ( terms.size( ) == 1 )
		return terms.front( )->eval( );

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

void dex::queryCompiler::andExpression::print( size_t depth ) const
	{
	if ( terms.size( ) == 1 )
		{
		terms.front( )->print( depth );
		return;
		}

	for ( size_t i = 0;  i != depth;  ++i )
		std::cout << '\t';
	std::cout << "AND" << std::endl;
	for ( size_t index = 0;  index != terms.size( );  ++index )
		terms[ index ]->print( depth + 1 );
	}


dex::queryCompiler::phraseExpression::phraseExpression( dex::index::indexChunk *chunk ) : chunk( chunk ) { }

dex::queryCompiler::phraseExpression::~phraseExpression( )
	{
	for ( expression *isr : terms )
		delete isr;
	}

dex::constraintSolver::ISR *dex::queryCompiler::phraseExpression::eval( ) const
	{
	if ( !chunk )
		return nullptr;

	if ( terms.size( ) == 1 )
		return terms.front( )->eval( );

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

void dex::queryCompiler::phraseExpression::print( size_t depth ) const
	{
	if ( terms.size( ) == 1 )
		{
		terms.front( )->print( depth );
		return;
		}

	for ( size_t i = 0;  i != depth;  ++i )
		std::cout << '\t';
	std::cout << "PHRASE" << std::endl;
	for ( size_t index = 0;  index != terms.size( );  ++index )
		terms[ index ]->print( depth + 1 );
	}


dex::queryCompiler::word::word( dex::string str, dex::index::indexChunk *chunk ) : str( str ), chunk( chunk ) { }

dex::constraintSolver::ISR *dex::queryCompiler::word::eval( ) const
	{
	if ( !chunk )
		return nullptr;

	dex::string stemmedStr = dex::porterStemmer::stem( str );

	return new dex::constraintSolver::orISR( dex::vector < dex::constraintSolver::ISR * > {
			new dex::index::indexChunk::indexStreamReader( chunk, stemmedStr ),
			new dex::index::indexChunk::indexStreamReader( chunk, "#" + stemmedStr ) }, getEndOfDocumentISR( chunk ) );
	}

dex::pair < dex::vector < dex::string >, dex::vector < dex::string > > dex::queryCompiler::word::flattenedQuery( ) const
	{
	return dex::pair  < dex::vector < dex::string >, dex::vector < dex::string > >(
			dex::vector < dex::string >{ str }, dex::vector < dex::string >( ) );
	}

void dex::queryCompiler::word::print( size_t depth ) const
	{
	for ( size_t i = 0;  i != depth;  ++i )
		std::cout << '\t';
	std::cout << '[' << str << ']' << std::endl;
	}
