/*
 * tokenstream.cpp
 *
 * Implementation of tokenstream.h
 *
 * Lab3: You do not have to modify this file, but you may choose to do so
 */

//  #include <assert.h>  - - -Why do we need this?

#include "../utils/algorithm.hpp"
#include "../utils/basicString.hpp"
#include <cstddev>
#include "expression.hpp"
#include "tokenstream.hpp"

bool isalpha ( char c )
	 {
	 if ( ( c > 64 && c < 91 ) || ( c > 96 && c < 123 ) )
		  return true;
	 return false;
	 }

bool CharIsRelevant( char c )
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
			return isalpha( c );
		}
	}

bool CharIsIrrelevant( char c )
	{
	return !CharIsRelevant( c );
	}

TokenStream::TokenStream( const dex::string &in )
	{
	// Erase irrelevant chars using algorithm
	input.reserve( in.size( ) );
	for ( size_t index = 0;  index < in.size( );  ++index )
		if ( CharIsRelevant( in[ index ] ) )
			input.pushBack( in[ index ] );
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

// Number *TokenStream::ParseNumber( )
// 	{
// 	if ( location >= input.size( ) )
// 		{
// 		return nullptr;
// 		}
// 	// Parsing is done using strtoll, rather than atoi or variants
// 	// This way, we can easily check for parsing success, since strtoll
// 	// gives us a pointer to past how many characters it has consumed
// 	char *end;
// 	int64_t val = std::strtoll( input.c_str( ) + location, &end, 10 );
// 	// Check for parse success. If we start and end at input.c_str( ) + location,
// 	// then we have not processed any characters, and have failed to find a number
// 	if ( end == input.c_str( ) + location )
// 		{
// 		return nullptr;
// 		}
// 	// Update location to the first unparsed char
// 	location = end - input.c_str( );
// 	return new Number( val );
// 	}

dex::vector < dex::constraintSolver::ISR * > TokenStream::ParsePhrase( )
	{
	dex::vector < dex::constraintSolver::ISR * > isrs;
	size_t nextSymbolLocation = input.findFirstOf( "|&$\"~()", location, 7 );
	string word;

	while ( true )
		{
		size_t nextWordEnd = input.findFirstOf( ' ', location );

		if ( nextWordEnd >= nextSymbolLocation )
			{
			if ( nextSymbolLocation != location )
				{
				isrs.pushBack( new dex::index::indexChunk::indexStreamReader
						( chunk, input.substr( location, nextSymbolLocation - location ) ) );
				location = nextSymbolLocation;
				}

			return isrs;
			}

		// update location and isrs

		if ( nextWordEnd - location > 1 )
			isrs.pushBack( new dex::index::indexChunk::indexStreamReader
					( chunk, input.substr( location, nextWordEnd - location ) ) );
		location = nextWordEnd + 1;
		}
	}
