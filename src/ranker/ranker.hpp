// ranker.hpp
// This ranks the stuff

// 2019-12-11: Implement more dynamic ranking: combsc
// 2019-12-10: Implement topN: lougheem
// 2019-12-10: Implement dynamic ranking + pthread stuff: combsc
// 2019-12-09: getDesiredSpans: lougheem
// 2019-12-09: Implementing static title ranking: combsc
// 2019-12-09: Init Commit: combsc + lougheem

#ifndef RANKER_HPP
#define RANKER_HPP

#include "topN.hpp"
#include "rankerObjects.hpp"
#include "rankerConfig.hpp"
#include <pthread.h>

namespace dex
	{
	class ranker
		{
		private:
			// pair of < maximum length of title, score awarded to title >
			dex::vector < dex::pair < unsigned, double > > staticTitleWeights;
			// score weighting for URL
			double staticUrlWeight;
			// pair of < inclusive upper bound on span range, score weighting of that range >
			dex::vector < dex::pair < unsigned, double > > dynamicBodySpanHeuristics;
			dex::vector < dex::pair < unsigned, double > > dynamicTitleSpanHeuristics;
			unsigned maxNumBodySpans;
			unsigned maxNumTitleSpans;
			double emphasizedWeight;
			double proportionCap;
			dex::vector < dex::indexChunkObject * > chunkPointers;
			

		public:
			ranker( dex::vector < dex::pair < unsigned, double > > titleWeights, double urlWeight, 
					dex::vector < dex::pair < unsigned, double > > bodySpanHeuristics, 
					dex::vector < dex::pair < unsigned, double > > titleSpanHeuristics,
					double emphasizedWeightIn, double proportionCapIn,
					unsigned bodySpans, unsigned titleSpans, dex::vector < dex::indexChunkObject * > someChunks )
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
				}
			
			ranker ( dex::vector < dex::indexChunkObject * > someChunks )
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
				unsigned size = title.size( );
				
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

			// When given bodyISRs return the rarest word index. Also keep track of the word count for this
			// document.
			// Body ISRs are assumed to be pointing to the beginning of the document.
			// ISRs will seek back to the beginning after this function.
			unsigned getRarestWord( vector < ISR > &bodyISRs, unsigned beginDocument, unsigned endDocument,
					vector < unsigned > &wordCount )
				{
				for ( unsigned index = 0;  index < bodyISRs.size( );  ++index )
					{
					wordCount[ index ] = 0;
					unsigned result = bodyISRs[ index ].next( );
					while ( result < endDocument )
						{
						++wordCount[ index ];
						result = bodyISRs[ index ].next( );
						}
					bodyISRs[ index ].reset( beginDocument );
					}
				unsigned minCount = wordCount[ 0 ];
				unsigned minIndex = 0;
				for ( unsigned index = 1;  index < wordCount.size( );  ++index )
					{
					if ( wordCount[ index ] < minCount )
						{
						minCount = wordCount[ index ];
						minIndex = index;
						}
					}
				return minIndex;
				}

			// Heuristics is a vector contianing the lengths of the spans you're looking for in the document
			// Emphasized is a vector containing whether or not the word at that index was emphasized
			// Vector of ISRs should be arranged such that the words of the ISRs line up with the order of the query
			// rarest should be the index of the ISR of the rarest word in the query
			// { 1, 2, 4, 8 }
			// ISRs will not be moved back to the beginning
			vector < unsigned > getDesiredSpans( vector < ISR > &isrs, unsigned rarest, 
					const vector < pair < unsigned, double > > &heuristics, unsigned maxNumSpans,
					unsigned endDocument )
				{
				unsigned size = isrs.size( );
				vector < unsigned > spansOccurances( heuristics.size( ) );

				vector < unsigned > current( size );
				vector < unsigned > next( size );
				for ( unsigned index = 0;  index < size;  ++index )
					{
					std::cout << "Initializing " << isrs[ index ].getWord ( ) << "\n";
					current[ index ] = isrs[ index ].next( );
					std::cout << "\t" << current[ index ];
					next[ index ] = isrs[ index ].next( );
					std::cout << "\t" << next[ index ] << "\n";
					}

				dex::vector < unsigned > closestLocations( size );
				while ( current[ rarest ] < endDocument )
					{
					std::cout << "Iteration: " << current[ rarest ] << "\n";
					closestLocations[ rarest ] = current[ rarest ];
					for ( unsigned index = 0;  index < size;  ++index )
						{
						unsigned desiredPosition;
						if ( current[ rarest ] + index < rarest )
							desiredPosition = 0;
						else
							desiredPosition = current[ rarest ] + index - rarest;

						std::cout << "\t -" << isrs[ index ].getWord( ) << " starts at " << current[ index ] 
								<< " with desiredPosition of " << desiredPosition << "\n";
						if ( index != rarest )
							{
							// Position our ISRs such that current is less than our desired position and next is greater than our
							// desired position.
							// next[ index ] - current[ rarest ] < index - rarest
							while ( next[ index ] < endDocument && next[ index ] + rarest < current[ rarest ] + index )
								{
								current[ index ] = next[ index ];
								next[ index ] = isrs[ index ].next( );
								std::cout << "\t\t\t..." << current[ index ] << "\n";
								}

							// Take the value that is closest to our desired position for this span.
							unsigned closest;
							// if ( desiredPosition - current[ index ] < next[ index ] - desiredPosition )
							if ( desiredPosition + desiredPosition < next[ index ] + current[ index ] )
								{
								closest = current[ index ];
								std::cout << "\t\tChoose " << closest << " between " << current[ index ]
										<< " and " << next[ index ] << "\n";
								}
							else
								{
								// if ( desiredPosition - current[ index ] > next[ index ] - desiredPosition || index > rarest )
								if ( next[ index ] < endDocument && 
										( 2 * desiredPosition > next[ index ] + current[ index ]  || index > rarest ) )
									{
									closest = next[ index ];
									std::cout << "\t\tChoose " << closest << " between " << current[ index ]
											<< " and " << next[ index ] << "\n";
									}
								else
									{
									closest = current[ index ];
									std::cout << "\t\tChoose " << closest << " between " << current[ index ]
											<< " and " << next[ index ] << "\n";
									}
								}
							closestLocations[ index ] = closest;
							}
						}
					unsigned min = endDocument;
					unsigned max = 0;
					for ( unsigned index = 0;  index < size;  ++index )
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
					// What if min/max weren't updated??
					unsigned span = max - min + 1;
					std::cout << "\tSpan from " << min << " to " << max << " with length " << span << "\n";
					for ( unsigned index = 0;  index < heuristics.size( );  ++index )
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
					next[ rarest ] = isrs[ rarest ].next( );
					}
				for ( unsigned index = 0;  index < size;  ++index )
					{
					while ( next[ index ] < endDocument )
						{
						current[ index ] = next[ index ];
						next[ index ] = isrs[ index ].next( ); 
						}
					}
				return spansOccurances;
				}

			double getDynamicWordScore( const vector < unsigned > &wordCount, unsigned documentLength, vector < bool > emphasized, 
					double emphasizedWeight, double proportionCap )
				{
				double score = 0;
				for ( unsigned index = 0;  index < wordCount.size( );  ++index )
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
				score /= documentLength;
				if ( score > proportionCap )
					{
					score = proportionCap;
					}
				return proportionCap;
				}
			// bodyISRs, titleISRs, and emphasized should be in the order of the flattened query
			// ISRs will be shifted to the next position after the function ends, and begin after beginDocument
			// beginDocument and endDocument keep track of the document boundaries
			double getDynamicScore( vector < ISR > &bodyISRs, vector < ISR > &titleISRs,
			unsigned beginDocument, unsigned endDocument, vector < bool > emphasized, bool printInfo = false )
				{
				vector < unsigned > wordCount;
				unsigned rarestWord = getRarestWord( bodyISRs, beginDocument, endDocument, wordCount );
				vector < unsigned > bodySpans = getDesiredSpans( bodyISRs, rarestWord, 
						dynamicBodySpanHeuristics, maxNumBodySpans, endDocument );
				double bodySpanScore = 0;
				for ( unsigned index = 0;  index < bodySpans.size( );  ++index )
					{
					bodySpanScore += bodySpans[ index ] * dynamicBodySpanHeuristics[ index ].second;
					}
				if ( printInfo )
					{
					std::cout << "Body Span Score: " << bodySpanScore << std::endl;
					}
				vector < unsigned > titleWordCount;
				vector < unsigned > titleSpans = getDesiredSpans( titleISRs, rarestWord, 
						dynamicTitleSpanHeuristics, maxNumTitleSpans, endDocument );
				double titleSpanScore = 0;
				for ( unsigned index = 0;  index < titleSpans.size( );  ++index )
					{
					titleSpanScore += titleSpans[ index ] * dynamicTitleSpanHeuristics[ index ].second;
					}
				if ( printInfo )
					{
					std::cout << "Title Span Score: " << titleSpanScore << std::endl;
					}
				double dynamicWordScore = 0;
				unsigned docLength = beginDocument - endDocument;
				dynamicWordScore = getDynamicWordScore( wordCount, docLength, emphasized,
						emphasizedWeight, proportionCap );
				if ( printInfo )
					{
					std::cout << "Dynamic Word Score: " << dynamicWordScore << std::endl;
					}

				return bodySpanScore + titleSpanScore + dynamicWordScore;
				}
			
			// dex::vector < dex::searchResult > getTopN( size_t n, dex::string query )
			// 	{
			// 	dex::vector < dex::searchResult > results;
			// 	results.reserve( n );
			// 	// pass query to query compiler
			// 	// pass compiled query to constraintSolver
				
			// 	dex::vector < dex::matchedDocuments > documents;
			// 	dex::vector < double > scores;

			// 	pthread_t workerThreads [ chunkPointers.size( ) ];
			// 	for ( size_t index = 0;  index < chunkPointers.size( );  ++index )
			// 		{
			// 		dex::queryRequest request;
			// 		request.query = query;
			// 		request.chunkPointer = chunkPointers[ index ];
			// 		pthread_create( &workerThreads[ index ], nullptr, getMatchingDocuments, ( void * ) &request );
			// 		}

			// 	for ( size_t index = 0;  index < chunkPointers.size( );  ++index )
			// 		{
			// 		void *returnValue;
			// 		pthread_join( workerThreads[ index ], &returnValue );
			// 		vector < dex::document > returnDocuments = *( vector < dex::document > * ) returnValue;
			// 		documents.reserve( documents.size( ) + returnDocuments.size( ) );
			// 		for ( dex::vector< dex::document >::constIterator newDocumentIt = returnDocuments.cbegin( );
			// 				newDocumentIt != returnDocuments.cend( );  documents.pushBack( *( newDocumentIt++ ) ) );
			// 		}
			// 	// loop this over all index chunks
			// 		// Constraint Solver returns a vector of sets of ISRs?
			// 		// Each document contains a vector of ISRs that correspond to the words in the query
			// 		// Each index worker has its own constraint solver.
					
			// 	for ( size_t index = 0;  index < documents.size( );  ++index )
			// 		{
			// 		double score = getDynamicScore( documents[ index ] ) + getStaticScore( documents[ index ] );
			// 		scores.pushBack( score );
			// 		}

			// 	documentInfo **topN, *p;
			// 	dex::vector< dex::document > topDocuments;

			// 	topN = findTopN( scores, n );

			// 	for ( int index = 0;  index < n && ( p = topN[ index ] );  index++ )
			// 		{
			// 		topDocuments.pushBack( documents[ p->documentIndex ] );
			// 		std::cout << p->score << "\t" << documents[ p->documentIndex ].title << "\t"
			// 				<< documents[ p->documentIndex ].url.completeUrl( ) << "\n";
			// 		}
			// 	return results;
			// 	}
			
		};
	}
#endif
