// ranker.hpp
// This ranks the stuff

// 2019-12-23: Updated to use beginDocument properly: combsc
// 2019-12-22: Updated the getDesiredSpan function to use ISRs efficiently: combsc
// 2019-12-14 and 15: moved everything to pointers, removed prints: combsc
// 2019-12-12 and 13: Get dynamic ranking working with the constraint solver interface
//                    Also get static and dynamic scoring working in tandem: combsc
// 2019-12-11: Implement more dynamic ranking: combsc
// 2019-12-10: Implement topN: lougheem
// 2019-12-10: Implement dynamic ranking + pthread stuff: combsc
// 2019-12-09: getDesiredSpans: lougheem
// 2019-12-09: Implementing static title ranking: combsc
// 2019-12-09: Init Commit: combsc + lougheem

#ifndef RANKER_HPP
#define RANKER_HPP

#include <cstddef>
#include <pthread.h>
#include "constraintSolver/constraintSolver.hpp"
#include "indexer/index.hpp"
#include "queryCompiler/parser.hpp"
#include "ranker/rankerConfig.hpp"
#include "ranker/rankerObjects.hpp"
#include "ranker/topN.hpp"

namespace dex
	{
	void *parseAndScore( void *args )
			{
			dex::queryRequest queryRequest = *( ( dex::queryRequest * ) args );
			std::cout << "[" << queryRequest.query << "]" << std::endl;
			dex::queryCompiler::parser parser( queryRequest.query, queryRequest.chunkPointer );
			dex::matchedDocuments *documents = parser.parse( );
			std::cout << "Parse and Score isr: ";
			std::cout << documents->matchingDocumentISR << std::endl;
			for ( int i = 0;  i < 10; ++i )
				std::cout << " Matched document found that ends at: " << documents->matchingDocumentISR->next( ) << std::endl;
			documents->matchingDocumentISR->seek( 0 );
			std::cout << "NPOS: " << dex::constraintSolver::ISR::npos << std::endl;
			return ( void * ) documents;
			}
	class ranker
		{
		private:
			// pair of < maximum length of title, score awarded to title >
			dex::vector < dex::pair < size_t, double > > staticTitleWeights;
			// score weighting for URL
			double staticUrlWeight;
			// pair of < inclusive upper bound on span range, score weighting of that range >
			dex::vector < dex::pair < size_t, double > > dynamicBodySpanHeuristics;
			dex::vector < dex::pair < size_t, double > > dynamicTitleSpanHeuristics;
			size_t maxNumBodySpans;
			size_t maxNumTitleSpans;
			double emphasizedWeight;
			double proportionCap;
			double wordsWeight;
			dex::vector < dex::index::indexChunk * > chunkPointers;


		public:
			ranker( dex::vector < dex::pair < size_t, double > > titleWeights, double urlWeight,
					dex::vector < dex::pair < size_t, double > > bodySpanHeuristics,
					dex::vector < dex::pair < size_t, double > > titleSpanHeuristics,
					double emphasizedWeightIn, double proportionCapIn,
					size_t bodySpans, size_t titleSpans, double wordsWeightIn, dex::vector < dex::index::indexChunk * > someChunks )
				{
				staticTitleWeights = titleWeights;
				staticUrlWeight = urlWeight;
				dynamicBodySpanHeuristics = bodySpanHeuristics;
				dynamicTitleSpanHeuristics = titleSpanHeuristics;
				chunkPointers = someChunks;
				maxNumBodySpans = bodySpans;
				maxNumTitleSpans = titleSpans;
				emphasizedWeight = emphasizedWeightIn;
				proportionCap = proportionCapIn;
				wordsWeight = wordsWeightIn;
				}

			ranker ( dex::vector < dex::index::indexChunk * > someChunks )
				{
				chunkPointers = someChunks;
				RankerConfiguration config;
				staticTitleWeights = config.StaticTitleWeights;
				staticUrlWeight = config.StaticUrlWeight;
				dynamicBodySpanHeuristics = config.BodySpanHeuristics;
				dynamicTitleSpanHeuristics = config.TitleSpanHeuristics;
				maxNumBodySpans = config.MaxBodySpans;
				maxNumTitleSpans = config.MaxTitleSpans;
				emphasizedWeight = config.EmphasizedWeight;
				proportionCap = config.ProportionCap;
				wordsWeight = config.wordWeight;
				}

			double staticScoreUrl( const dex::Url &url )
				{
				double score = 0;
				// Promote good tlds ( .com, .org, .gov )
				dex::string host = url.getHost( );
				if ( host.size( ) > 4 )
					{
					if ( host.substr( 0, 4 ) == "www." )
						{
						score += 8;
						}
					host = host.substr( host.size( ) - 4, 4);
					if ( host == ".com" )
						{
						score += 10;
						}
					else
						{
						if ( host == ".org" )
							{
							score += 10;
							}
						else
							{
							if ( host == ".gov" )
								{
								score += 10;
								}
							else
								{
								if ( host == ".edu" )
									{
									score += 10;
									}
								}
							}
						}
					}
				// Promote short URLs
				// max of this should not give more than a good TLD
				// completeUrls of size 45 or lower get maximum score ( of 9 ).
				// decay after that

				// TODO: Make this more binary
				dex::string completeUrl = url.completeUrl( );
				int urlSize = dex::max( size_t( 45 ), completeUrl.size( ) );
				score += 9 * 45 / urlSize;

				if ( url.getService( ) == "https" )
					{
					score += 8;
					}

				// Promote not a ton of /'s
				// take off points for every / you have over 6 ( take off 3 for the slashes in every url )
				int numSlashes = 0;
				for ( size_t i = 0;  i < completeUrl.size( );  ++i )
					{
					if ( completeUrl[ i ] == '/' )
						++numSlashes;
					}
				numSlashes -= 6;
				if ( numSlashes > 0 )
					{
					score -= numSlashes * 2;
					}
				// Promote no queries or fragments
				if ( !url.getFragment( ).empty( ) || !url.getQuery( ).empty( ) )
					score -= 5;

				return staticUrlWeight * score;
				}

			// Title weights contains vector of pairs
			// pair.first is the number of characters in the title
			// pair.second is the number of points awarded if the size is less than or equal to first
			double staticScoreTitle( const dex::string &title )
				{
				size_t size = title.size( );

				for ( auto it = staticTitleWeights.cbegin( );  it != staticTitleWeights.cend( );  ++it )
					{
					if ( size <= it->first )
						return it->second;
					}

				return 0;
				}

			double getStaticScore( const dex::string &title, const dex::Url &url, bool printInfo = false )
				{
				double titleScore = staticScoreTitle( title );
				if ( printInfo )
					{
					std::cout << "Static Score Title: " << titleScore << std::endl;
					}
				double urlScore = staticScoreUrl( url ) * staticUrlWeight;
				if ( printInfo )
					{
					std::cout << "Static Score Url: " << urlScore << std::endl;
					}
				return titleScore + urlScore;
				}

			// Body ISRs are assumed to be pointing to the beginning of the document.
			// Heuristics is a vector contianing the lengths of the spans you're looking for in the document
			// Emphasized is a vector containing whether or not the word at that index was emphasized
			// Vector of ISRs should be arranged such that the words of the ISRs line up with the order of the query
			// rarest should be the index of the ISR of the rarest word in the query
			// { 1, 2, 4, 8 }
			// ISRs will not be moved back to the beginning
			vector < vector < size_t > > getDesiredSpans( vector < constraintSolver::ISR * > &isrs, constraintSolver::ISR *matching,
					constraintSolver::endOfDocumentISR *ends, dex::index::indexChunk *chunk,
					const vector < pair < size_t, double > > &heuristics, const size_t maxNumSpans,
					vector < vector < size_t > > &wordCount, vector < string > &titles, vector < string > &urls )
				{
				vector < vector < size_t > > documentSpans;

				wordCount.clear( );
				size_t size = isrs.size( );
				vector < size_t > isrCurrentValue;
				isrCurrentValue.resize( size );
				size_t endDocument;
				size_t beginDocument;
				// Find the word counts for all the documents that match
				endDocument = matching->seek( 0 );
				ends->seek( endDocument );
				beginDocument = endDocument - ends->documentSize( );
				while ( endDocument != dex::endOfDocumentISR::npos )
					{
					vector < size_t > currentWordCount;
					currentWordCount.resize( size );
					for ( size_t isrIndex = 0;  isrIndex < size;  ++isrIndex )
						{
						isrCurrentValue[ isrIndex ] = isrs[ isrIndex ]->seek( beginDocument );
						currentWordCount[ isrIndex ] = 0;
						// Check to see if our current next word is a part of the document that we're on
						// If it isn't, then we know this word doesn't appear in the current document.
						if ( isrCurrentValue[ isrIndex ] < endDocument )
							{
							++currentWordCount[ isrIndex ];
							isrCurrentValue[ isrIndex ] = isrs[ isrIndex ]->next( );
							while ( isrCurrentValue[ isrIndex ] < endDocument )
								{
								++currentWordCount[ isrIndex ];
								isrCurrentValue[ isrIndex ] = isrs[ isrIndex ]->next( );
								}
							}
						}
					wordCount.pushBack( currentWordCount );
					endDocument = matching->next( );
					ends->seek( endDocument );
					beginDocument = endDocument - ends->documentSize( );
					}

				// find spans
				vector < size_t > current;
				current.clear( );
				current.resize( size );
				endDocument = matching->seek( 0 );
				ends->seek( endDocument );
				beginDocument = endDocument - ends->documentSize( );
				size_t documentNumber = 0;
				while ( endDocument != dex::endOfDocumentISR::npos )
					{
					// std::cout << "Begin Document: " << beginDocument << std::endl;
					// std::cout << "End Document: " << endDocument << std::endl;
					vector < size_t > spansOccurances( heuristics.size( ) );
					vector < size_t > next( size );

					// Find the rarest word for this document
					size_t minCount = wordCount[ documentNumber ][ 0 ];
					//std::cout << "Word count: " << minCount << " for index: 0" << std::endl;
					size_t minIndex = 0;
					for ( size_t index = 1;  index < size;  ++index )
						{
						//std::cout << "Word count: " << wordCount[ documentNumber ][ index ] << " for index: " << index << std::endl;
						if ( wordCount[ documentNumber ][ index ] < minCount )
							{
							minCount = wordCount[ documentNumber ][ index ];
							minIndex = index;
							}
						}
					size_t rarest = minIndex;

					//std::cout << "For document " << documentNumber << " the rarest index is " << rarest << std::endl;

					for ( size_t index = 0;  index < size;  ++index )
						{
						// If the current value we're looking at is past the end document there are no occurances
						// of the word in this document. Therefore, we do not increment the ISR until we know that
						// we're in the document that uses the word.
						current[ index ] = isrs[ index ]->seek( beginDocument );
						if ( current[ index ] < endDocument )
							{
							next[ index ] = isrs[ index ]->next( );
							}
						else
							{
							next[ index ] = current[ index ];
							}
						}

					// Should be setup such that next is one past current.

					dex::vector < size_t > closestLocations( size );
					while ( current[ rarest ] < endDocument )
						{
						// std::cout << "Iteration: " << current[ rarest ] << "\n";
						closestLocations[ rarest ] = current[ rarest ];
						for ( size_t index = 0;  index < size;  ++index )
							{
							// std::cout << "index: " << index << std::endl;
							// std::cout << "current: " << current[ index ] << std::endl;
							// std::cout << "next: " << next[ index ] << std::endl;
							size_t desiredPosition;
							if ( current[ rarest ] + index < rarest )
								desiredPosition = 0;
							else
								desiredPosition = current[ rarest ] + index - rarest;

							if ( index != rarest )
								{
								// Position our ISRs such that current is less than our desired position and next is greater than our
								// desired position.
								while ( next[ index ] < endDocument && next[ index ] + rarest < current[ rarest ] + index )
									{
									current[ index ] = next[ index ];
									next[ index ] = isrs[ index ]->next( );
									// std::cout << "\t\t\t..." << current[ index ] << "\n";
									}


								// Take the value that is closest to our desired position for this span.
								size_t closest;
								// Check to see if either current or next are not in the document
								if ( current[ index ] < beginDocument || next[ index ] >= endDocument )
									{
									// If current is out of bounds but next is in bounds, set closest to next
									if ( current[ index ] < beginDocument && next[ index ] < endDocument )
										{
										closest = next[ index ];
										// std::cout << "\t\tChoose " << closest << " between " << current[ index ]
										// 		<< " and " << next[ index ] << "\n";
										}
									// If current is in bounds but next is out of bounds, set closest to current
									if ( current[ index ] >= beginDocument && next[ index ] >= endDocument )
										{
										closest = current[ index ];
										// std::cout << "\t\tChoose " << closest << " between " << current[ index ]
										// 		<< " and " << next[ index ] << "\n";
										}
									// If both are out of bounds, just set closest to next, it'll be caught later.
									if ( current[ index ] < beginDocument && next[ index ] >= endDocument )
										{
										closest = next[ index ];
										// std::cout << "\t\tChoose " << closest << " between " << current[ index ]
										// 		<< " and " << next[ index ] << "\n";
										}
									}
								else
									{
									// If both next and current are in the document
									if ( desiredPosition + desiredPosition < next[ index ] + current[ index ] )
										{
										closest = current[ index ];
										// std::cout << "\t\tChoose " << closest << " between " << current[ index ]
										// 		<< " and " << next[ index ] << "\n";
										}
									else
										{
										closest = next[ index ];
										// std::cout << "\t\tChoose " << closest << " between " << current[ index ]
										// 		<< " and " << next[ index ] << "\n";
										}
									}

								closestLocations[ index ] = closest;
								}
							}
						size_t min = endDocument;
						size_t max = 0;
						for ( size_t index = 0;  index < size;  ++index )
							{
							if ( closestLocations[ index ] > max )
								{
								max = closestLocations[ index ];
								}
							if ( closestLocations[ index ] < min )
								{
								min = closestLocations[ index ];
								}
							}
						// If max is greater than or equal to end document, one of the words is not in our document.
						// Therefore the span should be npos.
						size_t span;
						if ( max >= endDocument || min < beginDocument )
							{
							span = dex::endOfDocumentISR::npos;
							}
						else
							{
							span = max - min + 1;
							}
						// std::cout << "\tSpan from " << min << " to " << max << " with length " << span << "\n";
						for ( size_t index = 0;  index < heuristics.size( );  ++index )
							{
							if ( span <= heuristics[ index ].first * size )
								{
								if ( spansOccurances[ index ] < maxNumSpans )
									{
									spansOccurances[ index ]++;
									}
								break;
								}
							}
						current[ rarest ] = next[ rarest ];
						if ( next[ rarest ] < endDocument )
							{
							next[ rarest ] = isrs[ rarest ]->next( );
							}
						}
					documentSpans.pushBack( spansOccurances );
					if ( chunk )
						{
						titles.pushBack( chunk->offsetsToEndOfDocumentMetadatas[ endDocument ].title );
						urls.pushBack( chunk->offsetsToEndOfDocumentMetadatas[ endDocument].url );
						}
					endDocument = matching->next( );
					ends->seek( endDocument );
					beginDocument = endDocument - ends->documentSize( );
					documentNumber++;
					}
				return documentSpans;
				}

			double getDynamicWordScore( const vector < size_t > &wordCount, const size_t documentLength,
					const vector < bool > &emphasized, const double emphasizedWeight, const double proportionCap,
					const double wordWeight )
				{
				double score = 0;
				for ( size_t index = 0;  index < wordCount.size( );  ++index )
					{
					if ( emphasized[ index ] )
						{
						score += wordCount[ index ] * emphasizedWeight;
						}
					else
						{
						score += wordCount[ index ];
						}
					}
				score /= double ( documentLength );
				if ( score > proportionCap )
					{
					score = proportionCap;
					}
				return score * wordWeight;
				}
			// bodyISRs, titleISRs, and emphasized should be in the order of the flattened query
			// ISRs will be shifted to the next position after the function ends, and begin after beginDocument
			// beginDocument and endDocument keep track of the document boundaries
			vector < double > getDynamicScores( vector < constraintSolver::ISR * > &bodyISRs,
					vector < constraintSolver::ISR * > &titleISRs, constraintSolver::ISR *matching,
					constraintSolver::endOfDocumentISR *ends, index::indexChunk *chunk, const vector < bool > &emphasized,
					vector < string > &titles, vector < string > &urls, bool printInfo = false )
				{
				vector < vector < size_t > > wordCount;
				titles.clear( );
				urls.clear( );
				vector < vector < size_t > > bodySpans = getDesiredSpans( bodyISRs, matching, ends, chunk, dynamicBodySpanHeuristics,
						maxNumBodySpans, wordCount, titles, urls );
				vector < double > bodySpanScores;

				for ( size_t i = 0;  i < bodySpans.size( );  ++i )
					{
					double bodySpanScore = 0;
					for ( size_t j = 0;  j < bodySpans[ i ].size( );  ++j )
						{
						bodySpanScore += bodySpans[ i ][ j ] * dynamicBodySpanHeuristics[ j ].second;
						}
					bodySpanScores.pushBack( bodySpanScore );
					if ( printInfo )
						{
						std::cout << "Index " << i << ", Body Span Score: " << bodySpanScore << std::endl;
						}
					}

				vector < double > dynamicWordScores;

				size_t beginDocument = 0;
				size_t endDocument = matching->seek( 0 );
				for ( size_t i = 0;  i < wordCount.size( );  ++i )
					{
					double dynamicWordScore = getDynamicWordScore( wordCount[ i ], endDocument - beginDocument, emphasized,
						emphasizedWeight, proportionCap, wordsWeight );
					dynamicWordScores.pushBack( dynamicWordScore );
					if ( printInfo )
						{
						std::cout << "Index: " << i << ", Dynamic Word Score: " << dynamicWordScore << std::endl;
						}
					beginDocument = endDocument;
					endDocument = matching->next( );
					}

				wordCount.clear( );
				vector < vector < size_t > > titleWordCount;
				// No need to get the titles and urls again, so just pass a nullptr.
				vector < vector < size_t > > titleSpans = getDesiredSpans( titleISRs, matching, ends, chunk, dynamicTitleSpanHeuristics,
						maxNumTitleSpans, wordCount, titles, urls );
				vector < double > titleSpanScores;

				for ( size_t i = 0;  i < titleSpans.size( );  ++i )
					{
					double titleSpanScore = 0;
					for ( size_t j = 0;  j < titleSpans[ i ].size( );  ++j )
						{
						titleSpanScore += titleSpans[ i ][ j ] * dynamicTitleSpanHeuristics[ j ].second;
						}
					titleSpanScores.pushBack( titleSpanScore );
					if ( printInfo )
						{
						std::cout << "Index: " << i << ", Title Span Score: " << titleSpanScore << std::endl;
						}
					}


				vector < double > totalScores;
				for ( size_t i = 0;  i < dynamicWordScores.size( );  ++i )
					{
					totalScores.pushBack( bodySpanScores[ i ] + titleSpanScores[ i ] + dynamicWordScores[ i ] );
					}
				return totalScores;
				}

			// returns a pair. First is the scores. second is error code. if error code is -1, you messed up.
			pair < vector < double >, int > scoreDocuments( matchedDocuments *documents, constraintSolver::endOfDocumentISR *ends,
					vector < string > &titles, vector < string > &urls, bool printinfo = false )
				{
				// if the query is bad...

				if ( !documents->matchingDocumentISR )
					{
					vector < double > scores;
					return { scores, -1 };
					}
				vector < constraintSolver::ISR * > bodyISRs;
				vector < constraintSolver::ISR * > titleISRs;
				for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
					{
					bodyISRs.pushBack( new index::indexChunk::indexStreamReader( documents->chunk, documents->flattenedQuery[ i ] ) );
					}
				for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
					{
					bodyISRs.pushBack( new index::indexChunk::indexStreamReader( documents->chunk, '#' + documents->flattenedQuery[ i ] ) );
					}
				vector < double > totalScores = getDynamicScores( bodyISRs, titleISRs,
						documents->matchingDocumentISR, ends, documents->chunk, documents->emphasizedWords, titles, urls, printinfo );
				for ( size_t i = 0;  i < titles.size( );  ++i )
					{
					totalScores[ i ] += getStaticScore( titles[ i ], urls[ i ], printinfo );
					}
				for ( size_t i = 0;  i < bodyISRs.size( );  ++i )
					{
					if ( bodyISRs[ i ] )
						delete bodyISRs[ i ];
					}
				for ( size_t i = 0;  i < titleISRs.size( );  ++i )
					{
					if ( titleISRs[ i ] )
						delete titleISRs[ i ];
					}
				return { totalScores, 0 };
				}

			// second is error, if -1 there was a bad query.
			pair < dex::vector < dex::searchResult >, int > getTopN( size_t n, dex::string query, bool printInfo = false )
				{
				dex::vector < dex::searchResult > results;
				results.reserve( n );
				// pass query to query compiler
				// pass compiled query to constraintSolver

				dex::vector < dex::matchedDocuments * > documents;
				dex::vector < double > scores;
				vector < string > titles;
				vector < Url > urls;

				dex::vector < dex::queryRequest > requests;
				requests.resize( chunkPointers.size( ) );

				pthread_t workerThreads [ chunkPointers.size( ) ];
				for ( size_t index = 0;  index < chunkPointers.size( );  ++index )
					{
					requests[ index ].query = query;
					requests[ index ].chunkPointer = chunkPointers[ index ];
					pthread_create( &workerThreads[ index ], nullptr, parseAndScore, ( void * ) &requests[ index ] );
					}

				for ( size_t index = 0;  index < chunkPointers.size( );  ++index )
					{
					void *returnValue;
					pthread_join( workerThreads[ index ], &returnValue );
					dex::matchedDocuments *returnDocuments = ( dex::matchedDocuments * ) returnValue;
					std::cout << "main matchingISR: " << returnDocuments->matchingDocumentISR << std::endl;
					documents.pushBack( returnDocuments );
					}

				for ( size_t index = 0;  index < documents.size( );  ++index )
					{
					// need to get the end ISR from the chunk
					if ( !documents[ index ] )
						{
						return { results, -1 };
						}
					dex::index::indexChunk::endOfDocumentIndexStreamReader *eodisr =
							new dex::index::indexChunk::endOfDocumentIndexStreamReader( documents[ index ]->chunk, "" );
					vector < string > currentTitles;
					vector < string > currentUrls;
					for ( int i = 0;  i < 10; ++i )
						std::cout << " Matched document found that ends at: " << documents[ index ]->matchingDocumentISR->next( ) << std::endl;
					documents[ index ]->matchingDocumentISR->seek( 0 );
					pair < vector < double >, int > currentScoresPair = scoreDocuments( documents[ index ], eodisr,
					currentTitles, currentUrls, printInfo );
					vector < double > currentScores = currentScoresPair.first;
					if ( currentScoresPair.second == -1 )
						{
						return { results, -1 };
						}
					// Now that we're done with these ISRs we delete them
					if ( documents[ index ] )
						{
						if ( documents[ index ]->matchingDocumentISR )
							delete documents[ index ]->matchingDocumentISR;
						delete documents[ index ];
						}
					if ( eodisr )
						delete eodisr;



					// We will be receiving the things in matched documents!
					if ( currentScores.size( ) != currentTitles.size( ) ||
							currentScores.size( ) != currentUrls.size( ) )
						{
						std::cerr << "Sizes of urls, titles, and scores don't match." << std::endl;
						throw dex::invalidArgumentException( );
						}
					for ( size_t j = 0;  j < currentScores.size( );  ++j )
						{
						std::cout << "Title: " << currentTitles[ j ] << std::endl;
						std::cout << "Url: " << currentUrls[ j ] << std::endl;
						std::cout << "Score: " << currentScores[ j ] << std::endl;
						scores.pushBack( currentScores[ j ] );
						titles.pushBack( currentTitles[ j ] );
						urls.pushBack( currentUrls[ j ] );
						}
					}

				documentInfo **topN, *p;
				dex::vector< dex::searchResult > topDocuments;

				topN = findTopN( scores, n );

				for ( size_t index = 0;  index < n && ( p = topN[ index ] );  index++ )
					{
					topDocuments.pushBack( { urls[ p->documentIndex ], titles[ p->documentIndex ] } );
					if ( printInfo )
						{
						std::cout << p->score << "\t" << urls[ p->documentIndex ].completeUrl( ) << "\t"
								<< titles[ p->documentIndex ] << "\n";
						}
					}
				return { results, 0 };
				}

		};
	}
#endif
