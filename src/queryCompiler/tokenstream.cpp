// tokenstream.cpp
//
// 2019-12-15: Done: jasina, medhak

#include <cstddef>
#include "queryCompiler/expression.hpp"
#include "queryCompiler/tokenstream.hpp"
#include "utils/algorithm.hpp"
#include "utils/basicString.hpp"
#include "utils/unorderedSet.hpp"

bool dex::queryCompiler::isAlpha ( char c )
	 {
	 return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' );
	 }

bool dex::queryCompiler::isSymbol( char c, bool infix )
	{
	switch ( c )
		{
		case '|':
		case '&':
		case '$':
		case '"':
		case '~':
			return true;
		case '(':
		case ')':
			return infix;
		default:
			return isWhitespace( c );
		}
	}

bool dex::queryCompiler::isWhitespace( char c )
	{
	switch ( c )
		{
		case ' ':
		case '\f':
		case '\n':
		case '\r':
		case '\t':
		case '\v':
			return true;
		}
	return false;
	}

char dex::queryCompiler::toLower( char c )
	{
	if ( c >= 'A' && c <= 'Z' )
		return c + ( 'a' - 'A' );
	return c;
	}

dex::queryCompiler::tokenStream::tokenStream( const dex::string &in, bool infix )
	{
	input.reserve( in.size( ) );

	// Intermediate value to help us figure out which words are emphasized
	dex::string semiparsed;
	semiparsed.reserve( in.size( ) );

	bool charIsAlpha;
	bool charIsSymbol;
	bool charIsWhitespace;
	bool inPhrase = false;
	char currentChar;
	char previousCharacter = '&';

	for ( size_t index = 0;  index < in.size( );  ++index )
		{
		currentChar = toLower( in[ index ] );
		charIsAlpha = isAlpha( currentChar );
		charIsSymbol = isSymbol( currentChar, infix );
		charIsWhitespace = isWhitespace( currentChar );

		// If we encounter an unknown character, ignore it
		if ( !( charIsAlpha || charIsSymbol ) )
			continue;

		if ( currentChar == '"' )
			{
			inPhrase = !inPhrase;

			// If we have a quote preceeded by a space, replace the space with the necessary character
			if ( isWhitespace( previousCharacter ) )
				{
				if ( inPhrase && infix )
					semiparsed.back( ) = '&';
				if ( !inPhrase )
					{
					semiparsed.back( ) = '"';
					previousCharacter = '"';
					continue;
					}
				}

			if ( previousCharacter == '"' )
				{
				// If we encounter an empty quoted phrase, our input should be considered invalid
				if ( !inPhrase )
					return;

				// If we have two quotes back to back, delimit them somehow
				if ( infix )
					semiparsed.pushBack( '&' );
				else
					semiparsed.pushBack( ' ' );
				}

			semiparsed.pushBack( '"' );
			previousCharacter = '"';
			continue;
			}

		// Skip symbols found in phrases
		if ( inPhrase && charIsSymbol && !charIsWhitespace )
			continue;

		// If we see an implied ampersand preceeding an open parenthesis, insert it
		if ( currentChar == '(' )
			{
			if ( isAlpha( previousCharacter ) || previousCharacter == ')' || previousCharacter == '"' )
				{
				if ( infix )
					semiparsed.pushBack( '&' );
				else
					semiparsed.pushBack( ' ' );
				}
			else
				if ( isWhitespace( previousCharacter ) && infix )
					{
					semiparsed.back( ) = '&';
					previousCharacter = '&';
					}
			}

		if ( isWhitespace( previousCharacter ) )
			{
			if ( charIsSymbol )
				// Remove spaces that are adjacent to symbols
				semiparsed.back( ) = currentChar;
			else
				{
				// If we see an implied ampersand preceeding a word, insert it
				if ( !inPhrase && infix )
					semiparsed.back( ) = '&';

				semiparsed.pushBack( currentChar );
				}

			previousCharacter = currentChar;
			continue;
			}

		// Ignore extraneous spaces after symbols
		if ( isSymbol( previousCharacter ) && charIsWhitespace )
			continue;

		// If we see an implied ampersand preceeding a word, insert it
		if ( !inPhrase && previousCharacter == '"' && charIsAlpha )
			{
			if ( infix )
				semiparsed.pushBack( '&' );
			else
				semiparsed.pushBack( ' ' );
			}

		semiparsed.pushBack( currentChar );
		previousCharacter = currentChar;
		}
	if ( !semiparsed.empty( ) && isWhitespace( semiparsed.back( ) ) )
		semiparsed.popBack( );

	for ( size_t index = 0;  index < semiparsed.size( ) - 1;  ++index )
		if ( semiparsed[ index ] == '$' )
			{
			dex::string word = semiparsed.substr(
					index + 1, semiparsed.findFirstOf( "|&$\"~() ", index + 1, 8 ) - index - 1 );
			emphasizedWords.insert( word );
			if ( index != 0 && semiparsed[ index - 1 ] != '(' && semiparsed[ index - 1 ] != '|'
					&& semiparsed[ index - 1 ] != '&' && semiparsed[ index - 1 ] != '~' )
				input.pushBack( '&' );
			}
		else
			input.pushBack( semiparsed[ index ] );

	if ( semiparsed.back( ) == '$' )
		input.clear( );
	else
		input.pushBack( semiparsed.back( ) );
	}

bool dex::queryCompiler::tokenStream::match( char c )
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

bool dex::queryCompiler::tokenStream::allConsumed( ) const
	{
	return location == input.size( );
	}

dex::queryCompiler::word *dex::queryCompiler::tokenStream::parseWord( )
	{
	if ( location >= input.size( ) )
		return nullptr;

	size_t nextSymbolLocation = input.findFirstOf( "|&$\"~() ", location, 8 );

	if ( nextSymbolLocation == location )
		return nullptr;
	dex::string word = input.substr( location, nextSymbolLocation - location );
	location = dex::min( nextSymbolLocation, input.size( ) );

	return new dex::queryCompiler::word( word );
	}
