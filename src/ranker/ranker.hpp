// ranker.hpp
// this ranks the stuff
//
// 2012-12-31: Refactored code: jasina + combsc
// 2019-12-11: Init Commit: combsc

#ifndef DEX_RANKER_OBJECTS
#define DEX_RANKER_OBJECTS

#include "constraintSolver/constraintSolver.hpp"
#include "indexer/index.hpp"
#include "utils/url.hpp"
#include "utils/utility.hpp"
#include "utils/vector.hpp"

namespace dex
	{
	namespace ranker
		{
		// Forward declarations
		class staticRanker;
		class dynamicRanker;
		class ranker;

		struct matchedDocuments
			{
			dex::vector < dex::string > flattenedQuery;
			dex::constraintSolver::ISR *matchingDocumentISR;
			dex::index::indexChunk *chunk;
			dex::vector < bool > emphasizedWords;
			};

		struct searchResult
			{
			dex::Url url;
			dex::string title;
			double score;
			};

		struct queryRequest
			{
			dex::string query;
			dex::index::indexChunk *chunkPointer;
			dex::ranker::ranker *rankerPointer;
			bool printInfo;
			};

		class staticRanker
			{
			private:
				// pair of < maximum length of title, score awarded to title >
				dex::vector < dex::pair < size_t, double > > titleWeights;
				// score weighting for URL
				double urlWeight;

			public:
				staticRanker( dex::vector < dex::pair < size_t, double > > titleWeights, double urlWeight );

				double staticScoreUrl( const dex::Url &url ) const;
				double staticScoreTitle( const dex::string &title ) const;
				double getStaticScore( const dex::string &title, const dex::Url &url, bool printInfo = false ) const;
			};

		class dynamicRanker
			{
			private:
				// pair of < inclusive upper bound on span range, score weighting of that range >
				dex::vector < dex::pair < size_t, double > > bodySpanHeuristics;
				dex::vector < dex::pair < size_t, double > > titleSpanHeuristics;
				size_t maxNumBodySpans;
				size_t maxNumTitleSpans;
				double emphasizedWeight;
				double proportionCap;
				double wordsWeight;

			public:
				dynamicRanker( dex::vector < dex::pair < size_t, double > > bodySpanHeuristics,
					dex::vector < dex::pair < size_t, double > > titleSpanHeuristics, size_t maxNumBodySpans,
					size_t maxNumTitleSpans, double emphasizedWeight, double proportionCap,  double wordsWeight );

				// Returns the word count of all the documents in the index chunk for the ISRs passed.
				// also returns the titles and urls corresponding to each document in documentTitles and documentUrls
				dex::vector < dex::vector < size_t > > getDocumentInfo( 
						dex::vector < constraintSolver::ISR * > &isrs,
						constraintSolver::ISR *matching,
						constraintSolver::endOfDocumentISR *ends,
						dex::index::indexChunk *chunk,
						dex::vector < dex::string > &documentTitles,
						dex::vector < dex::Url > &documentUrls ) const;

				// Get the number of spans that occur in each document for the ISRs passed in
				vector < vector < size_t > > getDesiredSpans(
						// Vector of ISRs should be arranged such that the words of the ISRs line up with the order of the query
						vector < constraintSolver::ISR * > &isrs,
						constraintSolver::ISR *matching,
						constraintSolver::endOfDocumentISR *ends,
						// This determines the sizes of spans that this function finds
						const vector < size_t > &spanSizes,
						const vector < vector < size_t > > &wordCount ) const;

				// Returns bag of words scoring of the number of words in the given document
				double dynamicScoreWords(
						// Number of each query word for a given document
						const vector < size_t > &wordCount,
						const size_t documentLength,
						// Emphasized is a vector containing whether or not the word at that index was emphasized
						const vector < bool > &emphasized ) const;

				// getDynamicScores returns a vector of dynamic scores for the matching documents passed in.
				// titles and urls are more return variables that correspond to the matching documents passed in.
				vector < double > getDynamicScores( vector < constraintSolver::ISR * > &bodyISRs,
					vector < constraintSolver::ISR * > &titleISRs, constraintSolver::ISR *matching,
					constraintSolver::endOfDocumentISR *ends, index::indexChunk *chunk, const vector < bool > &emphasized,
					vector < dex::string > &titles, vector < dex::Url > &urls, bool printInfo = false ) const;
			};

		class ranker
			{
			public:
				staticRanker rankerStatic;
				dynamicRanker rankerDynamic;
				ranker(
					// pair of < maximum length of title, score awarded to title >
					const dex::vector < dex::pair < size_t, double > > &staticTitleWeights
							= dex::vector < dex::pair < size_t, double > >( { { 15, 50 }, { 25, 40 }, { 50, 20 } } ),

					// score weighting for URL
					const double staticUrlWeight = 1,

					// pair of < inclusive upper bound on body span range, score weighting of that range >
					const dex::vector < dex::pair < size_t, double > > &bodySpanHeuristics
							= dex::vector < dex::pair < size_t, double > >( { { 1, 50 }, { 3, 25 }, { 4, 15 }, { 5, 5 } } ),

					// pair of < inclusive upper bound on title span range, score weighting of that range >
					const dex::vector < dex::pair < size_t, double > > &titleSpanHeuristics
							= dex::vector < dex::pair < size_t, double > >( { { 1, 250 }, { 2, 100 } } ),

					// Maximum number of spans we measure for body ISRs
					const size_t maxBodySpans = 5,

					// Maximum number of spans we measure for title ISRs
					const size_t maxTitleSpans = 1,

					// double for the weighting applied to emphasized words in bag of words scoring
					const double emphasizedWeight = 3.0,

					// double for the maximum word occurance proportion measured for a document in bag
					// of words scoring, this should be quite low.
					const double proportionCap = 0.06,

					// Weighting for the bag of words scoring
					const double wordWeight = 1000
				);

				// titles and urls act as extra return values
				pair < vector < double >, int > scoreDocuments(
					matchedDocuments *documents,
					constraintSolver::endOfDocumentISR *ends,
					vector < dex::string > &titles,
					vector < dex::Url > &urls,
					bool printInfo = true ) const;
			};

		void *parseQueryScoreDocuments( void *args );
		dex::pair < dex::vector < dex::ranker::searchResult >, int > getTopN(
				size_t n, dex::string query, dex::ranker::ranker *rankerPointer,
				dex::vector < dex::index::indexChunk * > chunkPointers, bool printInfo = false );
		}
	}

#endif
