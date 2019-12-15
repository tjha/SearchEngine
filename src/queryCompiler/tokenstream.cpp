/*
 * tokenstream.cpp
 *
 * Implementation of tokenstream.h
 *
 * Lab3: You do not have to modify this file, but you may choose to do so
 */

//  #include <assert.h>  - - -Why do we need this?

#include <cstddef>
#include "../utils/algorithm.hpp"
#include "../utils/basicString.hpp"
#include "expression.hpp"
#include "tokenstream.hpp"

bool isAlpha ( char c )
	 {
	 return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' );
	 }

bool isSymbol( char c )
	{
	switch ( c )
		{
		case '|':
		case '&':
		case '$':
		case '"':
		case '~':
		case '(':
		case ')':
		case ' ':
			return true;
		default:
			return false;
		}
	}


TokenStream::TokenStream( const dex::string &in, dex::index::indexChunk *chunk ) : chunk( chunk )
	{
	input.reserve( in.size( ) );

	// Erase irrelevant chars using algorithm --- gasp!!

	bool charIsAlpha;
	bool charIsSymbol;
	bool inPhrase = false;
	char previousCharacter = 'a';

	for ( size_t index = 0;  index < in.size( );  ++index )
		{
		charIsAlpha = isAlpha( in[ index ] );
		charIsSymbol = isSymbol( in[ index ] );

		if ( !( isAlpha || isSymbol ) )
			continue;

		if ( in[ index ] == '\"' )
			inPhrase = !inPhrase;

		if ( inPhrase && charIsSymbol && in[ index ] != ' ' )
			continue;

		if ( previousCharacter == ' ' )
			{
			if ( charIsSymbol )
				input.back( ) = in[ index ];
			else
				{
				if ( !inPhrase )
					input.back( ) = '&';

				input.pushBack( in[ index ] );
				}

			previousCharacter = in[ index ];
			continue;
			}

		if ( isSymbol( previousCharacter ) && in[ index ] == ' ' )
			continue;

		input.pushBack( in[ index ] );
		}
	}

bool TokenStream::Match( char c )
	{
	if ( location >= input.size( ) )
		{
		return false;
		}
	if ( input[ location ] == c )
		{
		++location;
		return true;
		}
	return false;
	}

bool TokenStream::AllConsumed( ) const
	{
	return location == input.size( );
	}

Word *TokenStream::ParsePhrase( )
	{
	if ( location >= input.size( ) )
		{
		return nullptr;
		}

	size_t nextSymbolLocation = input.findFirstOf( "|&$\"~() ", location, 7 );

	if ( nextSymbolLocation == location )
		return nullptr;

	dex::string word = input.substr( location, nextSymbolLocation - location );
	location = nextSymbolLocation;

	return new Word( word, chunk );
	}
