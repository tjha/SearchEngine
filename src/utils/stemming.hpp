// stemming.hpp
// Library implementing the Porter stemmer
//
// 2019-11-04: File created

#ifndef DEX_STEMMING
#define DEX_STEMMING

#include <cstddef>
#include <iostream>
#include "../utils/basicString.hpp"
#include "../utils/vector.hpp"

namespace dex
	{
	class porterStemmer
		{
		private:
			static const string step1aTruncations[ ];
			static const string step1aAppendations[ ];

			struct wordForm
				{
				string word;
				vector < size_t > deltas;
				bool firstIsVowel;
				bool lastIsVowel;

				bool endsWith( const string &str ) const
					{
					return word.size( ) >= str.size( ) && !word.compare( word.size( ) - str.size( ), str.size( ), str );
					}

				bool stemEndsWith( size_t suffixLength, const char c ) const
					{
					return word.size( ) > suffixLength && word[ word.size( ) - suffixLength - 1 ] == c;
					}

				size_t stemMeasure( size_t suffixLength ) const
					{
					if ( word.size( ) <= suffixLength || deltas.size( ) < 2 )
						return 0;

					bool previousWasVowel = firstIsVowel;
					size_t deltasIndex = 0; // Technically one less than the index
					size_t wordIndex = deltas.front( );
					size_t measure = 0;
					while ( wordIndex < word.size( ) - suffixLength )
						{
						measure += previousWasVowel;
						wordIndex += deltas[ ++deltasIndex ];
						previousWasVowel = !previousWasVowel;
						}

					return measure;
					}

				bool stemContainsVowel( size_t suffixLength ) const
					{
					return !word.empty( ) && word.size( ) > suffixLength
							&& ( firstIsVowel || ( deltas.size( ) != 1 && deltas.front( ) < word.size( ) - suffixLength ) );
					}

				bool stemEndsWithDoubleConsonant( size_t suffixLength ) const
					{
					return word.size( ) - suffixLength > 1
						&& word[ word.size( ) - suffixLength - 1 ] == word[ word.size( ) - suffixLength - 2 ]
						&& !isVowel( word[ word.size( ) - suffixLength - 1 ] );
					}

				bool stemEndsWithCVCAndNotWXY( size_t suffixLength )
					{
					if ( word.size( ) <= suffixLength || deltas.size( ) < 3 )
						return false;

					bool previousWasVowel = firstIsVowel;
					size_t deltasIndex = 0;
					size_t wordIndex = deltas.front( );
					while ( wordIndex < word.size( ) - suffixLength )
						{
						wordIndex += deltas[ ++deltasIndex ];
						previousWasVowel = !previousWasVowel;
						}

					char lastCharacter = word[ word.size( ) - suffixLength - 1 ];
					return deltasIndex > 1 && !previousWasVowel
							&& lastCharacter != 'w' && lastCharacter != 'x' && lastCharacter != 'y';
					}

				bool truncate( const string &str )
					{
					if ( !endsWith( str ) )
						return false;

					size_t deltasNewSize = deltas.size( );
					size_t charactersRemoved = 0;
					while ( charactersRemoved < str.size( ) )
						charactersRemoved += deltas[ --deltasNewSize ];
					charactersRemoved -= deltas[ deltasNewSize++ ];

					if ( ( deltas.size( ) - deltasNewSize ) % 2 )
						lastIsVowel = !lastIsVowel;

					word.resize( word.size( ) - str.size( ) );
					deltas.resize( deltasNewSize );
					if ( charactersRemoved != str.size( ) )
						deltas.back( ) -= str.size( ) - charactersRemoved;

					return true;
					}

				void append( const string &str )
					{
					if ( word.empty( ) )
						{
						*this = makeForm( str );
						return;
						}
					if ( str.empty( ) )
						return;

					wordForm newForm = makeForm( str );

					word.append( str );
					if ( lastIsVowel == newForm.firstIsVowel )
						{
						deltas.back( ) += newForm.deltas.front( );
						deltas.insert( deltas.end( ), newForm.deltas.begin( ) + 1, newForm.deltas.end( ) );
						}
					else
						{
						deltas.insert( deltas.end( ), newForm.deltas.begin( ), newForm.deltas.end( ) );
						}
					lastIsVowel = newForm.lastIsVowel;
					}
				};

			static bool isVowel( const char c )
				{
				return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
				}
			static bool isVowel( const string &word, const size_t index )
				{
				const char c = word[ index ];
				return isVowel( c ) || ( c == 'y' && index != 0 && !isVowel( word[ index - 1 ] ) );
				}

			static wordForm makeForm( const string &word )
				{
				bool firstIsVowel;
				bool previousWasVowel;
				vector < size_t > deltas;

				if ( word.empty( ) )
					return wordForm{ word, deltas, false, false };

				deltas.reserve( word.size( ) );
				deltas.pushBack( 1 );

				firstIsVowel = previousWasVowel = isVowel( word[ 0 ] );

				for ( size_t index = 1;  index != word.size( ); ++index )
					{
					bool currentIsVowel = isVowel( word, index );

					if ( previousWasVowel != currentIsVowel )
						deltas.pushBack( 0 );
					++deltas.back( );

					previousWasVowel = currentIsVowel;
					}

				return wordForm{ word, deltas, firstIsVowel, previousWasVowel };
				}

			static unsigned measure( const wordForm &form )
				{
				return ( form.deltas.size( ) - !form.firstIsVowel - form.lastIsVowel ) / 2;
				}

		public:
			static string stem( const string &word )
				{
				wordForm form = makeForm( word );

				// Step 1a
				for ( unsigned i = 0;  i != 4;  ++i )
					if ( form.truncate( step1aTruncations[ i ] ) )
						{
						form.append( step1aAppendations[ i ] );
						break;
						}

				// Step 1b
				if ( form.truncate( "eed" ) )
					form.append( "ee" );
				else
					{
					
					}

				/*
				for ( size_t delta : form.deltas )
					std::cout << delta << ", ";
				std::cout << std::endl;
				*/
				return form.word;
				}
		};

		const string porterStemmer::step1aTruncations[ ] = {
			"sses",
			"ies",
			"ss",
			"s"
		};
		const string porterStemmer::step1aAppendations[ ] = {
			"ss",
			"i",
			"ss",
			""
		};
	}

#endif
