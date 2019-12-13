// rankerObjects.hpp
// objects used for the ranker are defined here
//
// 2019-12-11: Init Commit: combsc

#ifndef RANKER_OBJECTS_HPP
#define RANKER_OBJECTS_HPP

#include "constraintSolver.hpp"
#include "index.hpp"
#include "vector.hpp"
#include "url.hpp"

// This is what our cosntraintSolver ISR interface looks like. See constraintSolver.hpp
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
	class endOfDocumentISR
		{
		private:
			dex::vector < unsigned > internal;
			unsigned pos;
		public:
			endOfDocumentISR( )
				{
				}
			endOfDocumentISR( dex::vector < unsigned > vecIn )
				{
				internal = vecIn;
				pos = 0;
				}
			const static unsigned npos = unsigned ( -1 );
			size_t current( )
				{
				if ( pos == internal.size( ) )
					{
					return npos;
					}
				return internal[ npos ];
				}
			void reset( size_t target )
				{
				for ( unsigned index = 0;  index < internal.size( );  ++index )
					{
					if ( internal[ index ] >= target )
						{
						pos = index;
						return;
						}
					}
				pos = internal.size( );
				}
			size_t seek( size_t target )
				{
				for ( unsigned index = 0;  index < internal.size( );  ++index )
					{
					if ( internal[ index ] >= target )
						{
						pos = index;
						return next( );
						}
					}
				pos = internal.size( );
				return npos;
				}
			size_t next( )
				{
				if ( pos == internal.size( ) )
					{
					return npos;
					}
				size_t toReturn = internal[ pos ];
				pos++;

				return toReturn;
				}
			size_t nextDocument( )
				{
				return next( );
				}
			size_t documentSize( )
				{
				if ( pos == 0 )
					{
					return internal[ pos ];
					}
				return internal[ pos ] - internal[ pos ] - 1;
				}
		};
	
	class ISR
		{
		private:
			dex::vector < unsigned > internal;
			dex::string word;
			unsigned pos;
			dex::endOfDocumentISR ends;
		public:
			const static unsigned npos = unsigned ( -1 );
			ISR( )
				{
				}
			ISR( dex::string word, dex::vector < unsigned > vecIn, dex::endOfDocumentISR endsIn )
				{
				this->word = word;
				internal = vecIn;
				pos = 0;
				ends = endsIn;
				}

			
			void reset( size_t target )
				{
				for ( unsigned index = 0;  index < internal.size( );  ++index )
					{
					if ( internal[ index ] >= target )
						{
						pos = index;
						return;
						}
					}
				pos = internal.size( );
				}
			// Jump this ISR to the first instance of our pattern that is at or after target. Return the location of the
			// instance, or -1 if there is none.
			size_t seek( size_t target )
				{
				for ( unsigned index = 0;  index < internal.size( );  ++index )
					{
					if ( internal[ index ] >= target )
						{
						pos = index;
						return next( );
						}
					}
				pos = internal.size( );
				return npos;
				}

			// Jump ISR to next location of our pattern. Return location of instance or -1 if there is none.
			size_t next( )
				{
				if ( pos == internal.size( ) )
					{
					return npos;
					}
				size_t toReturn = internal[ pos ];
				pos++;

				return toReturn;
				}

			// Jump this ISR to the first location of our pattern that is in the next document. Return the location of the
			// instance, or -1 if there is none.
			// Note that this requires some way of accessing document boundaries.
			size_t nextDocument( )
				{
				if ( pos == internal.size( ) )
					{
					return npos;
					}
				ends.seek( internal[ pos ] );
				return seek( ends.next( ) );
				}
			// seek to the first instance in this document
			size_t beginDocument( )
				{
				if ( pos == internal.size( ) )
					{
					ends.seek( internal[ pos - 1 ] );
					}
				else
					{
					ends.seek( internal[ pos ] );
					}
				return seek( ends.current( ) - ends.documentSize( ) );
				}
			dex::string getWord( )
				{
				return word;
				}
		};
	

	struct matchedDocuments
		{
		// Constraint solver needs to make sure to not return pornographic results
		// All four vectors should be in the order of the flattened query
		dex::vector < dex::ISR > titleISRs;
		dex::vector < dex::ISR > bodyISRs;
		dex::ISR *matchingDocumentISR;
		// next of matchingDocumentISR returns the offset of the end document that you care about
		dex::index::indexChunk *chunk;

		
		// void example( ) {
		// 	size_t nextDocEndOffset = matchingDocumentISR->nextDocument( );
		// 	dex::string title = chunk->offsetsToEndOfDocumentMetadatas[ nextDocEndOffset ].title;
		// 	dex::string url = chunk->offsetsToEndOfDocumentMetadatas[ nextDocEndOffset ].url;
		// }

		// void example2( ) {
		// 	dex::index::indexChunk::endOfDocumentIndexStreamReader eodisr( chunk, "" );
		// }

		dex::vector < bool > emphasizedWords;
		dex::vector < dex::string > titles;
		dex::vector < dex::Url > urls;
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
		vector < dex::matchedDocuments > someVec;
		return nullptr;
		}
	}

#endif
