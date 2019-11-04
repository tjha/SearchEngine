// stemming.hpp
// Library implementing the Porter stemmer. See http://snowball.tartarus.org/algorithms/porter/stemmer.html
//
// 2019-11-04: File created

#ifndef DEX_STEMMING
#define DEX_STEMMING

#include <cstddef>
#include "../utils/basicString.hpp"
#include "../utils/vector.hpp"

namespace dex
	{
	class porterStemmer
		{
		private:
			static const string step1aTruncations[ ];
			static const string step1aAppendations[ ];
			static const string step2Truncations[ ];
			static const string step2Appendations[ ];
			static const string step3Truncations[ ];
			static const string step3Appendations[ ];

			// struct representing a word when split into alternating consonant and vowel parts
			struct wordForm
				{
				string word;
				vector < size_t > deltas; // lengths of each part
				bool firstIsVowel;
				bool lastIsVowel;

				bool endsWith( const char c ) const
					{
					return !word.empty( ) && word[ word.size( ) - 1 ] == c;
					}

				bool endsWith( const string &str ) const
					{
					return word.size( ) >= str.size( ) && !word.compare( word.size( ) - str.size( ), str.size( ), str );
					}

				bool stemEndsWith( size_t suffixLength, const char c ) const
					{
					return word.size( ) > suffixLength && word[ word.size( ) - suffixLength - 1 ] == c;
					}

				// Calculate m
				size_t measure( ) const
					{
					if ( word.empty( ) )
						return 0;
					return ( deltas.size( ) - !firstIsVowel - lastIsVowel ) / 2;
					}

				// Calculate m
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

				// Determine *v*
				bool stemContainsVowel( size_t suffixLength ) const
					{
					return !word.empty( ) && word.size( ) > suffixLength
							&& ( firstIsVowel || ( deltas.size( ) != 1 && deltas.front( ) < word.size( ) - suffixLength ) );
					}

				// Determine *d
				bool endsWithDoubleConsonant( ) const
					{
					char lastCharacter = word[ word.size( ) - 1 ];
					return word.size( ) > 1 && lastCharacter == word[ word.size( ) - 2 ] && !isVowel( lastCharacter );
					}

				// Determine *o
				bool stemEndsWithCVCAndNotWXY( size_t suffixLength )
					{
					if ( word.size( ) <= suffixLength || deltas.size( ) < 3 )
						return false;

					char lastCharacter = word[ word.size( ) - suffixLength - 1 ];
					return !isVowel( word, word.size( ) - suffixLength - 3 )
							&& isVowel( word, word.size( ) - suffixLength - 2 )
							&& !isVowel( word, word.size( ) - suffixLength - 1 )
							&& lastCharacter != 'w' && lastCharacter != 'x' && lastCharacter != 'y';
					}

				void truncate( size_t suffixLength )
					{
					if ( suffixLength >= word.size( ) )
						{
						word.clear( );
						deltas.clear( );
						}

					size_t deltasNewSize = deltas.size( );
					size_t charactersRemoved = 0;
					while ( charactersRemoved <= suffixLength )
						charactersRemoved += deltas[ --deltasNewSize ];
					charactersRemoved -= deltas[ deltasNewSize++ ];

					if ( ( deltas.size( ) - deltasNewSize ) % 2 )
						lastIsVowel = !lastIsVowel;

					word.resize( word.size( ) - suffixLength );
					deltas.resize( deltasNewSize );
					if ( charactersRemoved != suffixLength )
						deltas.back( ) -= suffixLength - charactersRemoved;
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

		public:
			static string stem( const string &word )
				{
				wordForm form = makeForm( word );

				// Step 1a
				for ( unsigned i = 0;  i != 4;  ++i )
					if ( form.endsWith( step1aTruncations[ i ] ) )
						{
						form.truncate( step1aTruncations[ i ].size( ) );
						form.append( step1aAppendations[ i ] );
						break;
						}

				// Step 1b
				bool secondOrThirdStepSuccessful = false;;
				if ( form.endsWith( "eed" ) )
					{
					if ( form.stemMeasure( 3 ) > 0 )
						{
						form.truncate( 3 );
						form.append( "ee" );
						}
					}
				else
					{
					if ( form.endsWith( "ed" ) )
						{
						if ( form.stemContainsVowel( 2 ) )
							{
							form.truncate( 2 );
							secondOrThirdStepSuccessful = true;
							}
						}
					else
						if ( form.endsWith( "ing" ) )
							if ( form.stemContainsVowel( 3 ) )
								{
								form.truncate( 3 );
								secondOrThirdStepSuccessful = true;
								}
					}
				if ( secondOrThirdStepSuccessful )
					{
					if ( form.endsWith( "at" ) || form.endsWith( "bl" ) || form.endsWith( "iz" ) )
						form.append( "e" );
					else
						{
						if ( form.endsWithDoubleConsonant( )
								&& !form.endsWith( 'l' ) && !form.endsWith( 's' ) && !form.endsWith( 'z' ) )
							form.truncate( 1 );
						else
							if ( form.measure( ) == 1 && form.stemEndsWithCVCAndNotWXY( 0 ) )
								form.append( "e" );
						}
					}


				// Step 1c
				if ( form.endsWith( 'y' ) && form.stemContainsVowel( 1 ) )
					{
					form.truncate( 1 );
					form.append( "i" );
					}

				// Step 2
				for ( unsigned i = 0;  i != 20;  ++i )
					if ( form.endsWith( step2Truncations[ i ] ) )
						{
						if ( form.stemMeasure( step2Truncations[ i ].size( ) ) > 0)
							{
							form.truncate( step2Truncations[ i ].size( ) );
							form.append( step2Appendations[ i ] );
							}
						break;
						}

				// Step 3
				for ( unsigned i = 0;  i != 7;  ++i )
					if ( form.endsWith( step3Truncations[ i ] ) )
						{
						if ( form.stemMeasure( step3Truncations[ i ].size( ) ) > 0)
							{
							form.truncate( step3Truncations[ i ].size( ) );
							form.append( step3Appendations[ i ] );
							}
						break;
						}

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
		const string porterStemmer::step2Truncations[ ] = {
			"ational",
			"tional",
			"enci",
			"anci",
			"izer",
			"abli",
			"alli",
			"entli",
			"eli",
			"ousli",
			"ization",
			"ation",
			"ator",
			"alism",
			"iveness",
			"fulness",
			"ousness",
			"aliti",
			"iviti",
			"biliti"
		};
		const string porterStemmer::step2Appendations[ ] = {
			"ate",
			"tion",
			"ence",
			"ance",
			"ize",
			"able",
			"al",
			"ent",
			"e",
			"ous",
			"ize",
			"ate",
			"ate",
			"al",
			"ive",
			"ful",
			"ous",
			"al",
			"ive",
			"ble"
		};
		const string porterStemmer::step3Truncations[ ] {
			"icate",
			"ative",
			"alize",
			"iciti",
			"ical",
			"ful",
			"ness"
		};
		const string porterStemmer::step3Appendations[ ] {
			"ic",
			"",
			"al",
			"ic",
			"ic",
			"",
			""
		};
	}

#endif
