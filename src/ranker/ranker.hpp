// ranker.hpp
// this ranks the stuff
//
// 2020-01-01: Add kendallsTau function, scoring functions: combsc
// 2019-12-31: Refactored code: jasina + combsc
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
				double maxBodySpanScore;
				double maxTitleSpanScore;
				double emphasizedWordWeight;
				double maxBagOfWordsScore;
				double wordsWeight;

			public:
				dynamicRanker( double maxBodySpanScore, double maxTitleSpanScore, double emphasizedWordWeight,
						double maxBagOfWordsScore,  double wordsWeight );

				// Returns the word count of all the documents in the index chunk for the ISRs passed.
				// also returns the titles and urls corresponding to each document in documentTitles and documentUrls
				dex::vector < dex::vector < size_t > > getDocumentInfo( 
						dex::vector < constraintSolver::ISR * > &isrs,
						constraintSolver::ISR *matching,
						constraintSolver::endOfDocumentISR *ends,
						dex::index::indexChunk *chunk,
						dex::vector < dex::string > &documentTitles,
						dex::vector < dex::Url > &documentUrls ) const;

				// For a given ordering returns the kendallTau coefficient
				// a correct ordering would look like this:
				// { 0, 1, 2, 3 }
				// the worst ording would look like this:
				// { 3, 2, 1, 0 }
				double kendallsTau( const dex::vector < size_t > &ordering ) const;

				// Score the span passed in
				double scoreBodySpan( size_t queryLength, size_t spanLength, double tau ) const;
				double scoreTitleSpan( size_t queryLength, size_t spanLength, double tau ) const;

				// Get the spans that occur in each document for the ISRs passed in
				// pair corresponds to span size found along with the kendall tau coefficient associated
				// with the ordering of the span.
				dex::vector < dex::vector < dex::pair < size_t, double > > > getDesiredSpans(
						// Vector of ISRs should be arranged such that the words of the ISRs line up with the order of the query
						vector < constraintSolver::ISR * > &isrs,
						constraintSolver::ISR *matching,
						constraintSolver::endOfDocumentISR *ends,
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

					// Maximum number of spans we measure for body ISRs
					const double maxBodySpanScore = 500,

					// Maximum number of spans we measure for title ISRs
					const double maxTitleSpanScore = 500,

					// double for the weighting applied to emphasized words in bag of words scoring
					const double emphasizedWordWeight = 3.0,

					const double maxBagOfWordsScore = 150,

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
