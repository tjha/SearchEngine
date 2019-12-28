// parser.cpp
//
// Implementation of the parser class.

#include "constraintSolver/constraintSolver.hpp"
#include "queryCompiler/expression.hpp"
#include "queryCompiler/parser.hpp"
#include "ranker/rankerObjects.hpp"

#include <iostream>

dex::queryCompiler::matchedDocumentsGenerator::matchedDocumentsGenerator(
		dex::queryCompiler::expression *root, dex::queryCompiler::tokenStream *stream ) : root( root ), stream( stream )
	{
	if ( !root || !stream )
		{
		invalid = true;
		return;
		}

	flattenedQuery = root->flattenedQuery( );

	if ( flattenedQuery.first.empty( ) )
		{
		invalid = true;
		return;
		}

	invalid = flattenedQuery.first.empty( );
	if ( invalid )
		return;

	emphasizedWords.reserve( flattenedQuery.first.size( ) );
	for( size_t index = 0;  index < flattenedQuery.first.size( );  ++index )
		emphasizedWords.pushBack( stream->emphasizedWords.count( flattenedQuery.first[ index ] ) );

	query = root->toString( );
	}

dex::queryCompiler::matchedDocumentsGenerator::~matchedDocumentsGenerator( )
	{
	if ( root )
		delete root;
	if ( stream )
		delete stream;
	}

dex::matchedDocuments *dex::queryCompiler::matchedDocumentsGenerator::operator( )( dex::index::indexChunk *chunk ) const
	{
	if ( invalid )
		return nullptr;

	dex::constraintSolver::ISR *isr = root->eval( chunk );

	if ( isr && flattenedQuery.first.size( ) + flattenedQuery.second.size( ) == 1 )
		isr = new dex::constraintSolver::phraseISR( { isr }, dex::queryCompiler::getEndOfDocumentISR( chunk ) );

	return new dex::matchedDocuments
		{
		flattenedQuery.first,  // flattened query vector of strings
		isr,            // matching document ISR
		chunk,                          // index chunk
		emphasizedWords                 // emphasized words in order of flattenedQuery.
		};
	}

dex::string dex::queryCompiler::matchedDocumentsGenerator::getQuery( ) const
	{
	if ( flattenedQuery.first.size( ) + flattenedQuery.second.size( ) == 1 )
		return "(" + query + ")";
	return query;
	}

dex::queryCompiler::expression *dex::queryCompiler::parser::findPhrase( )
	{
	if ( stream->match( '"' ) )
		{
		if ( stream->allConsumed( ) )
			return nullptr;

		dex::queryCompiler::phraseExpression *phrase = new dex::queryCompiler::phraseExpression( );
		while ( true )
			{
			phrase->terms.pushBack( stream->parseWord( ) );

			if ( stream->match( '"' ) )
				break;

			if ( stream->allConsumed( ) || !stream->match( ' ' ) )
				{
				delete phrase;
				return nullptr;
				}
			}
		return phrase;
		}

	return stream->parseWord( );
	}

dex::queryCompiler::expression *dex::queryCompiler::parser::findFactor( )
	{
	if ( stream->allConsumed( ) )
		return nullptr;

	if ( stream->match( '(' ) )
		{
		dex::queryCompiler::expression *add = dex::queryCompiler::parser::findOr( );
		if ( stream->match( ')' ) )
			return add;
		if ( add )
			delete add;
		return nullptr;
		}

	return dex::queryCompiler::parser::findPhrase( );
	}

dex::queryCompiler::expression *dex::queryCompiler::parser::findNot( )
	{
	size_t count;
	for ( count = 0;  stream->match( '~' );  ++count );

	if ( count % 2 == 0 )
		return dex::queryCompiler::parser::findFactor( );
	else
		{
		dex::queryCompiler::expression *factor = dex::queryCompiler::parser::findFactor( );
		if ( factor )
			return new dex::queryCompiler::notExpression( factor );
		return nullptr;
		}
	}

dex::queryCompiler::expression *dex::queryCompiler::parser::findOr( )
	{
	dex::queryCompiler::expression *left = dex::queryCompiler::parser::findAnd( );
	if ( left )
		{
		dex::queryCompiler::orExpression *self = new dex::queryCompiler::orExpression( );
		self->terms.pushBack( left );
		bool termAdded = true;
		while ( termAdded )
			{
			if ( stream->match( '|' ) )
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
		dex::queryCompiler::andExpression *self = new dex::queryCompiler::andExpression( );
		self->terms.pushBack( left );
		bool termAdded = true;
		while ( termAdded )
			{
			if ( stream->match( '&' ) )
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

dex::queryCompiler::expression *dex::queryCompiler::parser::parsePrefix( )
	{
	if ( stream->allConsumed( ) )
		return nullptr;

	// Elimiate leading spaces
	while ( stream->match( ' ' ) );

	if ( stream->match( '|' ) )
		{
		dex::queryCompiler::expression *left = dex::queryCompiler::parser::parsePrefix( );
		if ( !left )
			return nullptr;
		if ( stream->allConsumed( ) )
			{
			delete left;
			return nullptr;
			}

		dex::queryCompiler::expression *right = dex::queryCompiler::parser::parsePrefix( );
		if ( !right )
			{
			delete left;
			return nullptr;
			}

		dex::queryCompiler::orExpression *self = new dex::queryCompiler::orExpression( );
		self->terms.pushBack( left );
		self->terms.pushBack( right );
		return self;
		}

	if ( stream->match( '&' ) )
		{
		dex::queryCompiler::expression *left = dex::queryCompiler::parser::parsePrefix( );
		if ( !left )
			return nullptr;
		if ( stream->allConsumed( ) )
			{
			delete left;
			return nullptr;
			}

		dex::queryCompiler::expression *right = dex::queryCompiler::parser::parsePrefix( );
		if ( !right )
			{
			delete left;
			return nullptr;
			}

		dex::queryCompiler::andExpression *self = new dex::queryCompiler::andExpression( );
		self->terms.pushBack( left );
		self->terms.pushBack( right );
		return self;
		}

	size_t tildeCount;
	for ( tildeCount = 0;  stream->match( '~' );  ++tildeCount );

	if ( tildeCount > 0 )
		{
		if ( tildeCount % 2 == 0 )
			return dex::queryCompiler::parser::parsePrefix( );
		else
			{
			dex::queryCompiler::expression *factor = dex::queryCompiler::parser::parsePrefix( );
			if ( factor )
				return new dex::queryCompiler::notExpression( factor );
			return nullptr;
			}
		}

	return findPhrase( );
	}

dex::queryCompiler::matchedDocumentsGenerator dex::queryCompiler::parser::parse( dex::string &in, bool infix )
	{
	stream = new tokenStream( in, infix );
	dex::queryCompiler::expression *root;

	std::cout << stream->input << std::endl;

	if ( infix )
		root = dex::queryCompiler::parser::findOr( );
	else
		{
		root = dex::queryCompiler::parser::parsePrefix( );
		if ( !stream->allConsumed( ) )
			{
			delete root;
			root = nullptr;
			}
		}

	return dex::queryCompiler::matchedDocumentsGenerator( root, stream );
	}
