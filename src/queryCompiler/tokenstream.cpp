// tokenstream.cpp
//
// 2019-12-15: Done: jasina, medhak

#include <cstddef>
#include "queryCompiler/expression.hpp"
#include "queryCompiler/tokenstream.hpp"
#include "utils/algorithm.hpp"
#include "utils/basicString.hpp"
#include "utils/stemming.hpp"
#include "utils/unorderedSet.hpp"

bool dex::queryCompiler::isAlpha ( char c )
	 {
	 return ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' );
	 }

bool dex::queryCompiler::isSymbol( char c )
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


dex::queryCompiler::tokenStream::tokenStream( const dex::string &in, dex::index::indexChunk *chunk ) : chunk( chunk )
	{
	dex::string semiparsed;
	semiparsed.reserve( in.size( ) );
	input.reserve( in.size( ) );

	// Erase irrelevant chars using algorithm --- gasp!!

	bool charIsAlpha;
	bool charIsSymbol;
	bool inPhrase = false;
	char previousCharacter = '\0';

	for ( size_t index = 0;  index < in.size( );  ++index )
		{
		charIsAlpha = isAlpha( in[ index ] );
		charIsSymbol = isSymbol( in[ index ] );

		if ( !( charIsAlpha || charIsSymbol ) )
			continue;

		if ( in[ index ] == '"' )
			{
			inPhrase = !inPhrase;

			if ( previousCharacter == ' ' && inPhrase )
				semiparsed.back( ) = '&';

			if ( previousCharacter == '"' )
				{
				if ( inPhrase )
					semiparsed.pushBack( '&' );
				else
					{
					semiparsed.popBack( );
					continue;
					}
				}

			semiparsed.pushBack( '"' );
			previousCharacter = '"';
			continue;
			}

		if ( inPhrase && charIsSymbol && in[ index ] != ' ' )
			continue;

		if ( in[ index ] == '(' )
			{
			if ( isAlpha( previousCharacter ) || previousCharacter == ')' || previousCharacter == '"' )
				semiparsed.pushBack( '&' );
			else
				if ( previousCharacter == ' ' )
					{
					semiparsed.back( ) = '&';
					previousCharacter = '&';
					}
			}

		if ( previousCharacter == ' ' )
			{
			if ( charIsSymbol )
				semiparsed.back( ) = in[ index ];
			else
				{
				if ( !inPhrase )
					semiparsed.back( ) = '&';

				semiparsed.pushBack( in[ index ] );
				}

			previousCharacter = in[ index ];
			continue;
			}

		if ( isSymbol( previousCharacter ) && in[ index ] == ' ' )
			continue;

		if ( !inPhrase && previousCharacter == '"' && charIsAlpha )
			semiparsed.pushBack( '&' );

		semiparsed.pushBack( in[ index ] );
		previousCharacter = in[ index ];
		}
	if ( !in.empty( ) && !semiparsed.empty( ) && in.front( ) == ' ' && semiparsed.front( ) == '&' )
		semiparsed = semiparsed.substr( 1 );
	if ( !in.empty( ) && !semiparsed.empty( ) && semiparsed.back( ) == ' ' )
		semiparsed.popBack( );

	for( size_t index = 0;  index < semiparsed.size( ) - 1;  ++index )
		if ( semiparsed[ index ] == '$' )
			{
			dex::string word = semiparsed.substr(
					index + 1, semiparsed.findFirstOf( "|&$\"~() ", index + 1, 8 ) - index - 1 );
			emphasizedWords.insert( dex::porterStemmer::stem( word ) );
			if( index != 0 && semiparsed[ index - 1] != '(' && semiparsed[ index - 1] != '|'
					&& semiparsed[ index - 1] != '&' && semiparsed[ index - 1] != '~' )
				input.pushBack( '&' );
			}
		else
			input.pushBack( semiparsed[ index ] );

	if ( semiparsed.back( ) != '$' )
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

	return new dex::queryCompiler::word( dex::porterStemmer::stem( word ), chunk );
	}
