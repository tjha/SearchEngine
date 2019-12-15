// parserQC.cpp
//
// Implementation of the parser class.

#include "../constraintSolver/constraintSolver.hpp"
#include "expression.hpp"
#include "parserQC.hpp"

Expression *Parser::FindFactor( )
	{
	if ( stream.AllConsumed( ) )
		return nullptr;

	if ( stream.Match( '(' ) )
		{
		Expression *add = FindOR( );
		if ( stream.Match( ')' ) )
			return add;
		if ( add )
			delete add;
		return nullptr;
		}
	else
		if ( stream.Match( '"' ) )
			{
			PhraseExpression *phrase = new PhraseExpression( chunk );
			while ( !stream.Match( '"' ) && !stream.AllConsumed( ) )
				phrase->terms.pushBack( stream.ParseWord( ) );
			return phrase;
			if ( phrase )
				delete phrase;
			return nullptr;
			}
	return stream.ParseWord( );
	}

Expression *Parser::FindNot( )
	{
	size_t count;
	for ( count = 0;  stream.Match( '~' );  ++count );

	if ( count % 2 == 0 )
		return FindFactor( );
	else
		{
		Expression *factor = FindFactor( );
		if ( factor )
			return new NotExpression( factor, chunk );
		return nullptr;
		}
	}

Expression *Parser::FindOR( )
	{
	Expression *left = FindAND( );
	if ( left )
		{
		OrExpression *self = new OrExpression( chunk );
		self->terms.pushBack( left );
		bool termAdded = true;
		while ( termAdded )
			{
			if ( stream.Match( '|' ) )
				{
				left = FindAND( );
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

Expression *Parser::FindAND( )
	{
	Expression *left = FindNot( );
	if ( left )
		{
		AndExpression *self = new AndExpression( chunk );
		self->terms.pushBack( left );
		bool termAdded = true;
		while ( termAdded )
			{
			if ( stream.Match( '&' ) )
				{
				left = FindNot( );
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

Expression *Parser::Parse( )
	{
	Expression *root = FindOR( );
	if ( root )
		if ( stream.AllConsumed( ) )
			return root;
		delete root;

	return nullptr;
	}

Parser::Parser( const dex::string &in ) : stream( in, chunk ) { }
