// rankerObjects.hpp
// objects used for the ranker are defined here
//
// 2019-12-11: Init Commit: combsc

#ifndef RANKER_OBJECTS_HPP
#define RANKER_OBJECTS_HPP

#include "constraintSolver.hpp"
#include "vector.hpp"
#include "url.hpp"

// // This is what our cosntraintSolver ISR interface looks like. See constraintSolver.hpp
// namespace dex
// 	{
// 	namespace constraintSolver
// 		{
// 		// A virtual interface for index stream readers.
// 		class ISR
// 			{
// 			public:
// 				static const size_t npos;

// 				// Jump this ISR to the first instance of our pattern that is at or after target. Return the location of the
// 				// instance, or -1 if there is none.
// 				virtual size_t seek( size_t target ) = 0;

// 				// Jump ISR to next location of our pattern. Return location of instance or -1 if there is none.
// 				virtual size_t next( ) = 0;

// 				// Jump this ISR to the first location of our pattern that is in the next document. Return the location of the
// 				// instance, or -1 if there is none.
// 				// Note that this requires some way of accessing document boundaries.
// 				virtual size_t nextDocument( ) = 0;
// 			};

// 		class endOfDocumentISR : public dex::constraintSolver::ISR
// 			{
// 			public:
// 				virtual size_t seek( size_t target ) = 0;
// 				virtual size_t next( ) = 0;
// 				virtual size_t nextDocument( ) = 0;
// 				virtual size_t documentSize( ) = 0;
// 			};
// 		}
// 	}

namespace dex
	{
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

	struct chunk
		{
		// Constraint solver needs to make sure to not return pornographic results
		// All four vectors should be in the order of the flattened query
		dex::vector < dex::constraintSolver::ISR > titleISRs;
		dex::vector < dex::constraintSolver::ISR > bodyISRs;
		dex::vector < dex::constraintSolver::endOfDocumentISR > documentISRs;
		dex::vector < bool > emphasizedWords;
		dex::string title;
		dex::Url url;
		};

	class indexChunkObject
		{
		// Whatever matt and stephen put in here
		public:
			indexChunkObject( dex::string filename );
		};

	struct queryRequest
		{
		dex::string query;
		dex::indexChunkObject *chunkPointer;
		};

	struct searchResult
		{
		dex::Url url;
		dex::string title;
		};

	void *getMatchingDocuments( void *args )
		{
		dex::queryRequest queryRequest = *( ( dex::queryRequest * ) args );
		// Also need to pass list of index chunk pointers
		/*we expect this to be a function that we pass a query that we get from the get request
		This function runs the query compiler and constraint solver and returns
		a vector of documents that match the query given.*/
		vector < dex::chunk > someVec;
		return nullptr;
		}
	}

#endif
