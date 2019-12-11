// ranker.hpp
// This ranks the stuff

// 2019-12-10: Implement topN: lougheem
// 2019-12-10: Implement dynamic ranking + pthread stuff: combsc
// 2019-12-09: getDesiredSpans: lougheem
// 2019-12-09: Implementing static title ranking: combsc
// 2019-12-09: Init Commit: combsc + lougheem

#ifndef RANKER_HPP
#define RANKER_HPP

#include "vector.hpp"
#include "topN.hpp"
#include "../spinarak/url.hpp"
#include <pthread.h>

namespace dex
	{

	class indexChunkObject
		{
		// Whatever matt and stephen put in here
		};

	struct queryRequest
		{
		dex::string query;
		dex::indexChunkObject *chunkPointer;
		};

	void *getMatchingDocuments( void *args );
			/*{
			dex::queryRequest queryRequest = *( ( dex::queryRequest * ) args );
			
			we expect this to be a function that we pass a query that we get from the get request
			This function runs the query compiler and constraint solver and returns
			a vector of documents that match the query given. 
			vector < dex::document > someVec;
			return ( void * ) &someVec;
			}*/

	struct searchResult
		{
		dex::Url url;
		dex::string title;
		};

	class ISR
		{
		
		private:
			dex::vector < unsigned > internal;
			dex::string word;
			unsigned pos;

		public:
			const static unsigned npos = unsigned ( -1 );
			ISR( )
				{
				}
			ISR( dex::string word, dex::vector < unsigned > vecIn )
				{
				this->word = word;
				internal = vecIn;
				pos = 0;
				}
			unsigned first( )
				{
				return internal[ 0 ];
				}
			unsigned next( )
				{
				if ( pos == internal.size( ) )
					{
					return npos;
					}
				unsigned toRet = internal[ pos ];
				pos++;
				return toRet;
				}
			unsigned last( )
				{
				return internal.back( );
				}
			unsigned getPos( )
				{
				return pos;
				}
			dex::string getWord( )
				{
				return word;
				}
		};

		struct document
			{
			// Constraint solver needs to make sure to not return pornographic results
			// All four vectors should be in the order of the flattened query
			dex::vector < dex::ISR > titleISRs;
			dex::vector < dex::ISR > bodyISRs;
			dex::vector < bool > emphasizedWords;
			dex::string title;
			dex::Url url;
			unsigned rarestWordIndex;
			unsigned documentBodyLength;
			};
	
	class ranker
		{
		private:
			// pair of < maximum length of title, score awarded to title >
			dex::vector < dex::pair < unsigned, double > > staticTitleWeights;
			// score weighting for URL
			double staticUrlWeight;
			// pair of < inclusive upper bound on span range, score weighting of that range >
			dex::vector < dex::pair < unsigned, double > > dynamicSpanHeuristics;
			dex::vector < dex::indexChunkObject * > chunkPointers;

		public:
			ranker( dex::vector < dex::pair < unsigned, double > > titleWeights, double urlWeight, 
					dex::vector < pair < unsigned, double > > spanHeuristics, dex::vector < dex::indexChunkObject * > someChunks )
				{
				staticTitleWeights = titleWeights;
				staticUrlWeight = urlWeight;
				dynamicSpanHeuristics = spanHeuristics;
				chunkPointers = someChunks;
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

			double getStaticScore( dex::document doc )
				{
				double score = staticScoreTitle( doc.title );
				score += staticScoreUrl( doc.url );
				return score;
				}

			double getDynamicWordScore( vector < unsigned > wordCount, unsigned documentLength, vector < bool > emphasized, 
					double emphasizedWeight )
				{

				return 0;
				}
			
			// Heuristics is a vector contianing the lengths of the spans you're looking for in the document
			// Emphasized is a vector containing whether or not the word at that index was emphasized
			// Vector of ISRs should be arranged such that the words of the ISRs line up with the order of the query
			// rarest should be the index of the ISR of the rarest word in the query
			// { 1, 2, 4, 8 }
			vector < unsigned > getDesiredSpans( vector < ISR > isrs, unsigned rarest, vector < bool > emphasized )
				{
				vector < pair < unsigned, double > > heuristics = dynamicSpanHeuristics;
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
				while ( current[ rarest ] != ISR::npos )
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
							while ( next[ index ] != ISR::npos && next[ index ] + rarest < current[ rarest ] + index )
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
								if ( next[ index ] != ISR::npos && 
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
					unsigned min = ISR::npos;
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
						if ( span <= heuristics[ index ].second * size )
							{
							spansOccurances[ index ]++;
							break;
							}
						}
					current[ rarest ] = next[ rarest ];
					next[ rarest ] = isrs[ rarest ].next( );
					}
				return spansOccurances;
				}

			double getDynamicScore( dex::document doc)
				{
				vector < unsigned > bodySpans = getDesiredSpans( doc.bodyISRs, doc.rarestWordIndex, doc.emphasizedWords );
				vector < unsigned > titleSpans = getDesiredSpans( doc.titleISRs, doc.rarestWordIndex, doc.emphasizedWords );

				return 100;
				}

			dex::vector < dex::searchResult > getTopN( size_t n, dex::string query )
				{
				dex::vector < dex::searchResult > results;
				results.reserve( n );
				// pass query to query compiler
				// pass compiled query to constraintSolver
				
				dex::vector < dex::document > documents;
				dex::vector < double > scores;

				pthread_t workerThreads [ chunkPointers.size( ) ];
				for ( size_t index = 0;  index < chunkPointers.size( );  ++index )
					{
					dex::queryRequest request;
					request.query = query;
					request.chunkPointer = chunkPointers[ index ];
					pthread_create( &workerThreads[ index ], nullptr, getMatchingDocuments, ( void * ) &request );
					}

				for ( size_t index = 0;  index < chunkPointers.size( );  ++index )
					{
					void *returnValue;
					pthread_join( workerThreads[ index ], &returnValue );
					vector < dex::document > returnDocuments = *( vector < dex::document > * ) returnValue;
					documents.reserve( documents.size( ) + returnDocuments.size( ) );
					for ( dex::vector< dex::document >::constIterator newDocumentIt = returnDocuments.cbegin( );
							newDocumentIt != returnDocuments.cend( );  documents.pushBack( *( newDocumentIt++ ) ) );
					}
				// loop this over all index chunks
					// Constraint Solver returns a vector of sets of ISRs?
					// Each document contains a vector of ISRs that correspond to the words in the query
					// Each index worker has its own constraint solver.
					
				for ( size_t index = 0;  index < documents.size( );  ++index )
					{
					double score = getDynamicScore( documents[ index ] ) + getStaticScore( documents[ index ] );
					scores.pushBack( score );
					}

				documentInfo **topN, *p;
				dex::vector< dex::document > topDocuments;

				topN = findTopN( scores, n );

				for ( int index = 0;  index < n && ( p = topN[ index ] );  index++ )
					{
					topDocuments.pushBack( documents[ p->documentIndex ] );
					std::cout << p->score << "\t" << documents[ p->documentIndex ].title << "\t"
							<< documents[ p->documentIndex ].url.completeUrl << "\n";
					}
				return results;
				}
			
		};
	}
#endif
