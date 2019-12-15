// parserQC.cpp
//
// Implementation of the parser class.

#include "expression.hpp"
#include "parserQC.hpp"
#include "../constraintSolver/constraintSolver.hpp"
#include "../ranker/rankerObjects.hpp"

dex::queryCompiler::expression *dex::queryCompiler::parser::findFactor( )
	{
	if ( stream.allConsumed( ) )
		return nullptr;

	if ( stream.match( '(' ) )
		{
		dex::queryCompiler::expression *add = dex::queryCompiler::parser::findOr( );
		if ( stream.match( ')' ) )
			return add;
		if ( add )
			delete add;
		return nullptr;
		}
	else
		if ( stream.match( '"' ) )
			{
			dex::queryCompiler::phraseExpression *phrase = new dex::queryCompiler::phraseExpression( chunk );
			while ( !stream.match( '"' ) && !stream.allConsumed( ) )
				phrase->terms.pushBack( stream.parseWord( ) );
			return phrase;
			if ( phrase )
				delete phrase;
			return nullptr;
			}
	return stream.parseWord( );
	}

dex::queryCompiler::expression *dex::queryCompiler::parser::findNot( )
	{
	size_t count;
	for ( count = 0;  stream.match( '~' );  ++count );

	if ( count % 2 == 0 )
		return dex::queryCompiler::parser::findFactor( );
	else
		{
		dex::queryCompiler::expression *factor = dex::queryCompiler::parser::findFactor( );
		if ( factor )
			return new dex::queryCompiler::notExpression( factor, chunk );
		return nullptr;
		}
	}

dex::queryCompiler::expression *dex::queryCompiler::parser::findOr( )
	{
	dex::queryCompiler::expression *left = dex::queryCompiler::parser::findAnd( );
	if ( left )
		{
		dex::queryCompiler::orExpression *self = new dex::queryCompiler::orExpression( chunk );
		self->terms.pushBack( left );
		bool termAdded = true;
		while ( termAdded )
			{
			if ( stream.match( '|' ) )
				{
				left = dex::queryCompiler::parser::findAnd( );
				if ( !left )
					return nullptr;
				self->terms.pushBack( left );
				}
			else
				termAdded = false;
			}
		return self;
		}
	return nullptr;
	}

dex::queryCompiler::expression *dex::queryCompiler::parser::findAnd( )
	{
	dex::queryCompiler::expression *left = dex::queryCompiler::parser::findNot( );
	if ( left )
		{
		dex::queryCompiler::andExpression *self = new dex::queryCompiler::andExpression( chunk );
		self->terms.pushBack( left );
		bool termAdded = true;
		while ( termAdded )
			{
			if ( stream.match( '&' ) )
				{
				left = dex::queryCompiler::parser::findNot( );
				if ( !left )
					return nullptr;
				self->terms.pushBack( left );
				}
			else
				termAdded = false;
			}
		return self;
		}
	return nullptr;
	}

dex::matchedDocuments *dex::queryCompiler::parser::parse( )
	{
	dex::queryCompiler::expression *root = dex::queryCompiler::parser::findOr( );
	if ( root )
		if ( stream.allConsumed( ) )
			{
			const dex::vector < dex::string > &flattenedQuery = root->flattenedQuery( ).first;
			dex::vector < bool > emphasizedWords.reserve( flattenedQuery.size( ) );

			if ( flattenedQuery.empty( ) )
				{
				delete root;
				return new dex::matchedDocuments
					{
					flattenedQuery,
					nullptr,
					chunk,
					emphasizedWords
					};
				}

			for( size_t index = 0;  index < flattenedQuery.size( );  ++index )
				emphasizedWords.pushBack( stream.emphasizedWords.count( flattenedQuery[ index ] ) );
			dex::constraintSolver::ISR *matchingDocumentISR = root->eval( );

			delete root;

			return new dex::matchedDocuments
				{
				flattenedQuery,  // flattened query vector of strings
				matchingDocumentISR,   // matching document ISR
				chunk,           // index chunk
				emphasizedWords  // emphasized words in order of flattenedQuery.
				};
			}
		delete root;
	return nullptr;
	}

dex::queryCompiler::parser::parser( const dex::string &in ) : stream( in, chunk ) { }
