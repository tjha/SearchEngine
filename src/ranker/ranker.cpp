// ranker.cpp
// This ranks the stuff

// 2020-01-01: Got code working using get( ), made function for getting documentInfo
//             moved maxNumSpans to dynamicScores instead of getDesiredSpans, add kendallTau,
//             add scoreBodySpan and scoreTitleSpan: combsc
// 2019-12-31: Refactored code: jasina + combsc
// 2019-12-29: Updated the parser interface, fixed documentSize bug: combsc
// 2019-12-28: Remove bad debug statements, fixed parseQueryScoreDocuments, moved topN out of ranker: combsc
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

#include <cstddef>
#include <pthread.h>
#include "constraintSolver/constraintSolver.hpp"
#include "indexer/index.hpp"
#include "queryCompiler/parser.hpp"
#include "ranker/ranker.hpp"
#include "ranker/topN.hpp"
#include "utils/utility.hpp"
#include "utils/vector.hpp"
#include "utils/url.hpp"
#include <iostream>


dex::ranker::staticRanker::staticRanker( dex::vector < dex::pair < size_t, double > > titleWeights, double urlWeight ) :
	titleWeights( titleWeights ), urlWeight( urlWeight ) { }

double dex::ranker::staticRanker::staticScoreUrl( const dex::Url &url ) const
	{
	double score = 0;
	dex::string host = url.getHost( );

	// Promote good url openers ( www, en )
	dex::vector < dex::string > blds = { "www.", "en." };
	for ( size_t i = 0;  i < blds.size( );  ++i )
		{
		const dex::string &bld = blds[ i ];
		if ( host.substr( 0, bld.size( ) ) == bld )
			{
			score += 8;
			host = host.substr( host.size( ) - bld.size( ), bld.size( ) );
			break;
			}
		}

	// Promote good top level domains ( .com, .org, .gov, .edu )
	dex::vector < dex::string > tlds = { ".com", ".org", ".gov", ".edu" };
	for ( size_t i = 0;  i < tlds.size( );  ++i )
		{
		if ( host == tlds[ i ] )
			{
			score += 10;
			break;
			}
		}

	// Promote short URLs
	// max of this should not give more than a good TLD
	// completeUrls of size 45 or lower get maximum score ( of 9 ).
	// decay after that
	dex::string completeUrl = url.completeUrl( );
	size_t urlSize = dex::max( static_cast < size_t >( 45 ), completeUrl.size( ) );
	score += 9.0 * 45 / urlSize;

	// Promote SSL
	if ( url.getService( ) == "https" )
		score += 8;

	// Promote not a ton of /'s
	// take off points for every / you have over 6 ( take off 3 for the slashes in every url )
	size_t numSlashes = 0;
	for ( size_t i = 0;  i < completeUrl.size( );  ++i )
		if ( completeUrl[ i ] == '/' )
			++numSlashes;
	if ( numSlashes > 6 )
		score -= ( numSlashes - 6 ) * 2;

	// Promote no queries
	if ( !url.getQuery( ).empty( ) )
		score -= 5;

	return urlWeight * score;
	}

double dex::ranker::staticRanker::staticScoreTitle( const dex::string &title ) const
	{
	size_t size = title.size( );

	for ( auto it = titleWeights.cbegin( );  it != titleWeights.cend( );  ++it )
		if ( size <= it->first )
			return it->second;

	return 0;
	}

double dex::ranker::staticRanker::getStaticScore( const dex::string &title, const dex::Url &url, bool printInfo ) const
	{
	double titleScore = staticScoreTitle( title );
	double urlScore = staticScoreUrl( url );

	if ( printInfo )
		{
		std::cout << "Static Score Title: " << titleScore << std::endl;
		std::cout << "Static Score Url: " << urlScore << std::endl;
		}

	return titleScore + urlScore;
	}


dex::ranker::dynamicRanker::dynamicRanker( double maxBodySpanScore, double maxTitleSpanScore,
		double emphasizedWordWeight, double maxBagOfWordsScore,  double wordsWeight ) :
				maxBodySpanScore( maxBodySpanScore ), maxTitleSpanScore( maxTitleSpanScore ),
				emphasizedWordWeight( emphasizedWordWeight ), maxBagOfWordsScore( maxBagOfWordsScore ), wordsWeight( wordsWeight ) { }

dex::ranker::ranker::ranker(
		const dex::vector < dex::pair < size_t, double > > &staticTitleWeights,
		const double staticUrlWeight,
		const double maxBodySpanScore,
		const double maxTitleSpanScore,
		const double emphasizedWordWeight,
		const double maxBagOfWordsScore,
		const double wordWeight
		) :
				rankerStatic( staticTitleWeights, staticUrlWeight ),
				rankerDynamic( maxBodySpanScore, maxTitleSpanScore, emphasizedWordWeight,
						maxBagOfWordsScore, wordWeight ) { }

dex::vector < dex::vector < size_t > > dex::ranker::dynamicRanker::getDocumentInfo( 
		dex::vector < constraintSolver::ISR * > &isrs,
		constraintSolver::ISR *matching,
		constraintSolver::endOfDocumentISR *ends,
		dex::index::indexChunk *chunk,
		dex::vector < dex::string > &documentTitles,
		dex::vector < dex::Url > &documentUrls ) const
	{
	documentTitles.clear( );
	documentUrls.clear( );
	size_t isrCount = isrs.size( );
	dex::vector < dex::vector < size_t > > wordCount;
	size_t endDocument;
	size_t beginDocument;
	endDocument = matching->seek( 0 );
	ends->seek( endDocument );
	while ( endDocument != dex::constraintSolver::ISR::npos )
		{
		beginDocument = endDocument - ends->documentSize( ) + 1;
		dex::vector < size_t > currentWordCount( isrCount, 0 );
		for ( size_t isrIndex = 0;  isrIndex < isrCount;  ++isrIndex )
			{
			// Avoid accidental backwards seeks
			if ( isrs[ isrIndex ]->get( ) < beginDocument || isrs[ isrIndex ]->get( ) == dex::constraintSolver::ISR::npos )
				isrs[ isrIndex ]->seek( beginDocument );

			// Count the occurances for the word in this document
			while ( isrs[ isrIndex ]->get( ) < endDocument )
				{
				++currentWordCount[ isrIndex ];
				isrs[ isrIndex ]->next( );
				}
			}
		wordCount.pushBack( currentWordCount );
		if ( chunk )
			{
			documentTitles.pushBack( chunk->offsetsToEndOfDocumentMetadatas[ endDocument ].title );
			documentUrls.pushBack( chunk->offsetsToEndOfDocumentMetadatas[ endDocument].url );
			}
		endDocument = matching->next( );
		ends->seek( endDocument );
		}
	return wordCount;
	}

double dex::ranker::dynamicRanker::kendallsTau( const dex::vector < size_t > &ordering ) const
	{
	double numOrderedPairs = 0;
	double numUnorderedPairs = 0;
	for ( size_t left = 0;  left < ordering.size( ) - 1;  ++left )
		for ( size_t right = left + 1;  right < ordering.size( );  ++right )
			{
			if ( ordering[ left ] < ordering[ right ] )
				numOrderedPairs++;
			else
				numUnorderedPairs++;
			}
	return ( numOrderedPairs - numUnorderedPairs ) / ( numOrderedPairs + numUnorderedPairs);
	}

double dex::ranker::dynamicRanker::scoreBodySpan( size_t queryLength, size_t spanLength, double tau) const
	{
	double tauWeight = tau + 2;
	double spanProportion = spanLength / queryLength;

	// The higher the tauWeight, the higher the score
	// The shorter our span, the higher the score
	// very rudamentary scoring function, really just for compiling right now.
	return tauWeight / spanProportion;
	}

double dex::ranker::dynamicRanker::scoreTitleSpan( size_t queryLength, size_t spanLength, double tau) const
	{
	double tauWeight = tau + 2;
	double spanProportion = spanLength / queryLength;

	// The higher the tauWeight, the higher the score
	// The shorter our span, the higher the score
	// very rudamentary scoring function, really just for compiling right now.
	return 10 * tauWeight / spanProportion;
	}


dex::vector < dex::vector < dex::pair < size_t, double > > > dex::ranker::dynamicRanker::getDesiredSpans(
		dex::vector < dex::constraintSolver::ISR * > &isrs,
		dex::constraintSolver::ISR *matching,
		dex::constraintSolver::endOfDocumentISR *ends,
		const dex::vector < dex::vector < size_t > > &wordCount ) const
	{
	dex::vector < dex::vector < dex::pair < size_t, double > > > documentSpans;
	size_t isrCount = isrs.size( );
	size_t endDocument;
	size_t beginDocument;
	dex::vector < size_t > previous( isrCount, 0 );

	endDocument = matching->seek( 0 );
	ends->seek( endDocument );
	size_t documentNumber = 0;

	while ( endDocument != dex::constraintSolver::ISR::npos )
		{
		beginDocument = endDocument - ends->documentSize( ) + 1;
		dex::vector < dex::pair < size_t, double > > spansOccurances;
		dex::vector < size_t > current( isrCount );

		// Find the rarest word for this document
		size_t rarest = 0;
		for ( size_t index = 1;  index < isrCount;  ++index )
			if ( wordCount[ documentNumber ][ index ] < wordCount[ documentNumber ][ rarest ] )
				rarest = index;

		for ( size_t index = 0;  index < isrCount;  ++index )
			{
			previous[ index ] = isrs[ index ]->seek( beginDocument );
			if ( index != rarest )
				isrs[ index ]->next( );
			}

		// Should be setup such that ISR is one past previous.
		dex::vector < size_t > closestLocations( isrCount );
		while ( isrs[ rarest ]->get( ) < endDocument )
			{
			closestLocations[ rarest ] = isrs[ rarest ]->get( );
			for ( size_t index = 0;  index < isrCount;  ++index )
				{
				size_t desiredPosition;
				if ( isrs[ rarest ]->get( ) + index < rarest )
					desiredPosition = 0;
				else
					desiredPosition = isrs[ rarest ]->get( ) + index - rarest;

				if ( index != rarest )
					{
					// Position our ISRs such that ISR is pointing at first value past rarest location
					while ( isrs[ index ]->get( ) < endDocument && isrs[ index ]->get( ) + rarest < isrs[ rarest ]->get( ) + index )
						{
						previous[ index ] = isrs[ index ]->get( );
						isrs[ index ]->next( );
						}


					// Take the value that is closest to our desired position for this span.
					size_t closest;
					// Check to see if either previous or ISR are not in the document
					if ( previous[ index ] < beginDocument || isrs[ index ]->get( ) >= endDocument )
						{
						// If previous is out of bounds but ISR is in bounds, set closest to ISR
						if ( previous[ index ] < beginDocument && isrs[ index ]->get( ) < endDocument )
							{
							closest = isrs[ index ]->get( );
							}
						// If previous is in bounds but ISR is out of bounds, set closest to previous
						if ( previous[ index ] >= beginDocument && isrs[ index ]->get( ) >= endDocument )
							{
							closest = previous[ index ];
							}
						// If both are out of bounds, just set closest to ISR, it'll be caught later.
						if ( previous[ index ] < beginDocument && isrs[ index ]->get( ) >= endDocument )
							{
							closest = isrs[ index ]->get( );
							}
						}
					else
						// When both ISR and previous are in the document
						{
						if ( desiredPosition + desiredPosition < isrs[ index ]->get( ) + previous[ index ] )
							{
							closest = previous[ index ];
							}
						else
							{
							closest = isrs[ index ]->get( );
							}
						}
					closestLocations[ index ] = closest;
					}
				}
			// closestLocations now contains the locations of all of our words in the span
			// Find the length of our span
			size_t min = endDocument;
			size_t max = 0;
			for ( size_t index = 0;  index < isrCount;  ++index )
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
			// If max >= end document or min < beginDoc, one of the words is not in our document.
			// Therefore the span should be npos.
			size_t span;
			double tau;
			if ( max >= endDocument || min < beginDocument )
				{
				span = dex::constraintSolver::ISR::npos;
				tau = -1;
				}
			else
				{
				span = max - min + 1;
				tau = kendallsTau( closestLocations );
				}
			spansOccurances.pushBack( dex::pair( span, tau ) );
			isrs[ rarest ]->next( );
			}
		documentSpans.pushBack( spansOccurances );
		endDocument = matching->next( );
		ends->seek( endDocument );
		documentNumber++;
		}
	return documentSpans;
	}

double dex::ranker::dynamicRanker::scoreBagOfWords( const dex::vector < size_t > &wordCount,
		const size_t documentLength, const dex::vector < bool > &emphasized ) const
	{
	double score = 0;
	for ( size_t index = 0;  index < wordCount.size( );  ++index )
		{
		if ( emphasized[ index ] )
			{
			score += wordCount[ index ] * emphasizedWordWeight;
			}
		else
			{
			score += wordCount[ index ];
			}
		}
	score /= double ( documentLength );
	return dex::min( score * wordsWeight, maxBagOfWordsScore );
	}

dex::vector < double > dex::ranker::dynamicRanker::getDynamicScores( dex::vector < constraintSolver::ISR * > &bodyISRs,
		dex::vector < constraintSolver::ISR * > &titleISRs, dex::constraintSolver::ISR *matching,
		dex::constraintSolver::endOfDocumentISR *ends, dex::index::indexChunk *chunk,
		const dex::vector < bool > &emphasized, dex::vector < dex::string > &titles, dex::vector < dex::Url > &urls,
		bool printInfo ) const
	{
	titles.clear( );
	urls.clear( );
	dex::vector < dex::vector < size_t > > wordCount = getDocumentInfo( bodyISRs, matching, ends, chunk, titles, urls );

	dex::vector < dex::vector < dex::pair < size_t, double > > > bodySpans = getDesiredSpans( bodyISRs, matching, ends, 
			wordCount );
	dex::vector < double > bodySpanScores;

	for ( size_t i = 0;  i < bodySpans.size( );  ++i )
		{
		double bodySpanScore = 0;
		for ( size_t j = 0;  j < bodySpans[ i ].size( );  ++j )
			{
			bodySpanScore += scoreBodySpan( bodyISRs.size( ), bodySpans[ i ][ j ].first, bodySpans[ i ][ j ].second );
			}
		bodySpanScores.pushBack( dex::min( bodySpanScore, maxBodySpanScore ) );
		if ( printInfo )
			{
			std::cout << "Index " << i << ", Body Span Score: " << bodySpanScore << std::endl;
			}
		}

	dex::vector < double > dynamicWordScores;

	size_t beginDocument = 0;
	size_t endDocument = matching->seek( 0 );
	for ( size_t i = 0;  i < wordCount.size( );  ++i )
		{
		double dynamicWordScore = scoreBagOfWords( wordCount[ i ], endDocument - beginDocument, emphasized );
		dynamicWordScores.pushBack( dynamicWordScore );
		if ( printInfo )
			{
			std::cout << "Index: " << i << ", Bag of Words Score: " << dynamicWordScore << std::endl;
			}
		beginDocument = endDocument;
		endDocument = matching->next( );
		}

	wordCount.clear( );
	dex::vector < dex::vector < size_t > > titleWordCount;
	dex::vector < dex::vector < dex::pair < size_t, double > > > titleSpans = getDesiredSpans( titleISRs, matching, ends,
			wordCount );
	dex::vector < double > titleSpanScores;

	for ( size_t i = 0;  i < titleSpans.size( );  ++i )
		{
		double titleSpanScore = 0;
		for ( size_t j = 0;  j < titleSpans[ i ].size( );  ++j )
			{
			titleSpanScore += scoreTitleSpan( titleISRs.size( ), titleSpans[ i ][ j ].first, titleSpans[ i ][ j ].second );
			}
		titleSpanScores.pushBack( dex::min( titleSpanScore, maxTitleSpanScore ) );
		if ( printInfo )
			{
			std::cout << "Index: " << i << ", Title Span Score: " << titleSpanScore << std::endl;
			}
		}


	dex::vector < double > totalScores;
	for ( size_t i = 0;  i < dynamicWordScores.size( );  ++i )
		{
		totalScores.pushBack( bodySpanScores[ i ] + titleSpanScores[ i ] + dynamicWordScores[ i ] );
		}
	return totalScores;
	}

// returns a pair. First is the scores. second is error code. if error code is -1, you messed up.
dex::pair < dex::vector < double >, int > dex::ranker::ranker::scoreDocuments( dex::ranker::matchedDocuments *documents,
		dex::constraintSolver::endOfDocumentISR *ends, dex::vector < dex::string > &titles, dex::vector < dex::Url > &urls,
		bool printInfo ) const
	{
	// if the query is bad return -1
	if ( !documents->matchingDocumentISR )
		{
		dex::vector < double > scores;
		return { scores, -1 };
		}
	dex::vector < dex::constraintSolver::ISR * > bodyISRs;
	dex::vector < dex::constraintSolver::ISR * > titleISRs;
	for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
		bodyISRs.pushBack( new dex::index::indexChunk::indexStreamReader( documents->chunk, documents->flattenedQuery[ i ] ) );
	for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
		bodyISRs.pushBack( new dex::index::indexChunk::indexStreamReader( documents->chunk, '#' + documents->flattenedQuery[ i ] ) );
	dex::vector < double > totalScores = rankerDynamic.getDynamicScores( bodyISRs, titleISRs,
			documents->matchingDocumentISR, ends, documents->chunk, documents->emphasizedWords, titles, urls, printInfo );
	for ( size_t i = 0;  i < titles.size( );  ++i )
		totalScores[ i ] += rankerStatic.getStaticScore( titles[ i ], urls[ i ], printInfo );

	for ( size_t i = 0;  i < bodyISRs.size( );  ++i )
		if ( bodyISRs[ i ] )
			delete bodyISRs[ i ];
	for ( size_t i = 0;  i < titleISRs.size( );  ++i )
		if ( titleISRs[ i ] )
			delete titleISRs[ i ];
	return { totalScores, 0 };
	}

void *dex::ranker::parseQueryScoreDocuments( void *args )
	{
		dex::ranker::queryRequest queryRequest = *( ( dex::ranker::queryRequest * ) args );
		dex::queryCompiler::parser parser;
		dex::queryCompiler::matchedDocumentsGenerator generator = parser.parse( queryRequest.query );
		dex::ranker::matchedDocuments *documents = generator( queryRequest.chunkPointer );
		dex::ranker::ranker *rankerPointer = queryRequest.rankerPointer;
		bool printInfo = queryRequest.printInfo;

		dex::pair < dex::vector < dex::ranker::searchResult >, int > *searchResults =
				new dex::pair < dex::vector < dex::ranker::searchResult >, int >( );
		// need to get the end ISR from the chunk
		if ( !documents )
			{
			*searchResults = { { }, -1 };
			return reinterpret_cast < void * >( searchResults );
			}
		dex::index::indexChunk::endOfDocumentIndexStreamReader *eodisr =
				new dex::index::indexChunk::endOfDocumentIndexStreamReader( documents->chunk, "" );
		dex::vector < dex::string > titles;
		dex::vector < dex::Url > urls;
		dex::pair < dex::vector < double >, int > scoresPair = rankerPointer->scoreDocuments( documents, eodisr,
		titles, urls, printInfo );
		if ( scoresPair.second == -1 )
			{
			*searchResults = { { }, -1 };
			return reinterpret_cast < void * >( searchResults );
			}
		dex::vector < double > scores = scoresPair.first;
		// Now that we're done with these ISRs we delete them
		if ( documents )
			{
			if ( documents->matchingDocumentISR )
				delete documents->matchingDocumentISR;
			delete documents;
			}
		if ( eodisr )
			delete eodisr;
		// We will be receiving the things in matched documents!
		if ( scores.size( ) != titles.size( ) || scores.size( ) != urls.size( ) )
			{
			std::cerr << "Sizes of urls, titles, and scores don't match." << std::endl;
			throw dex::invalidArgumentException( );
			}
		searchResults->second = 0;
		for ( size_t index = 0;  index < scores.size( );  ++index )
			{
			searchResults->first.pushBack( { urls[ index ], titles[ index ], scores[ index ] } );
			}

		return reinterpret_cast < void * >( searchResults );
	}

// second is error, if -1 there was a bad query.
dex::pair < dex::vector < dex::ranker::searchResult >, int > dex::ranker::getTopN( size_t n, dex::string query, dex::ranker::ranker *rankerPointer,
dex::vector < dex::index::indexChunk * > chunkPointers, bool printInfo )
	{
		dex::vector < dex::ranker::searchResult > results;
		results.reserve( n );
		dex::vector < double > scores;
		dex::vector < dex::ranker::queryRequest > requests;
		requests.resize( chunkPointers.size( ) );

		pthread_t *workerThreads = new pthread_t[ chunkPointers.size( ) ];
		for ( size_t index = 0;  index < chunkPointers.size( );  ++index )
			{
			requests[ index ].query = query;
			requests[ index ].chunkPointer = chunkPointers[ index ];
			requests[ index ].rankerPointer = rankerPointer;
			requests[ index ].printInfo = printInfo;
			pthread_create( &workerThreads[ index ], nullptr, dex::ranker::parseQueryScoreDocuments,
					( void * ) &requests[ index ] );
			}

		for ( size_t index = 0;  index < chunkPointers.size( );  ++index )
			{
			void *returnValue;
			pthread_join( workerThreads[ index ], &returnValue );
			dex::pair < dex::vector < dex::ranker::searchResult >, int > *returnedResults =
					( dex::pair < dex::vector < dex::ranker::searchResult >, int > * ) returnValue;
			// If the query results in an error, propogate up.
			if ( returnedResults->second == -1 )
				{
				delete [ ] workerThreads;
				return { { }, -1 };
				}
			// Otherwise, add the results from this index chunk to the results and scores vectors
			else
				for ( size_t index = 0;  index < returnedResults->first.size( );  ++index )
					{
					results.pushBack( returnedResults->first[ index ] );
					scores.pushBack( results[ index ].score );
					}
			}

		dex::documentInfo **topN, *p;
		dex::vector< dex::ranker::searchResult > topDocuments;

		topN = findTopN( scores, n );

		for ( size_t index = 0;  index < n && ( p = topN[ index ] );  index++ )
			{
			topDocuments.pushBack( results[ p->documentIndex ] );
			if ( printInfo )
				std::cout << results[ p->documentIndex ].score << "\t" << results[ p->documentIndex ].url.completeUrl( ) << "\t"
						<< results[ p->documentIndex ].title << "\n";
			}

		delete [ ] workerThreads;
		return { results, 0 };
	}
