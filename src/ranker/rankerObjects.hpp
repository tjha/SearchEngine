// rankerObjects.hpp
// objects used for the ranker are defined here
//
// 2019-12-11: Init Commit: combsc

#ifndef RANKER_OBJECTS_HPP
#define RANKER_OBJECTS_HPP

#include "constraintSolver/constraintSolver.hpp"
#include "crawler/url.hpp"
#include "indexer/index.hpp"
#include "utils/vector.hpp"

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
	class endOfDocumentISR : public constraintSolver::endOfDocumentISR
		{
		private:
			dex::vector < size_t > internal;
			size_t pos;
		public:
			endOfDocumentISR( )
				{
				}
			endOfDocumentISR( dex::vector < size_t > vecIn )
				{
				internal = vecIn;
				pos = 0;
				}
			endOfDocumentISR &operator =( const endOfDocumentISR other )
				{
				internal = other.internal;
				pos = other.pos;
				return *this;
				}
			const static size_t npos = size_t ( -1 );
			size_t seek( size_t target )
				{
				for ( size_t index = 0;  index < internal.size( );  ++index )
					{
					if ( internal[ index ] >= target )
						{
						pos = index;
						return internal[ pos ];
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
				return internal[ pos ] - internal[ pos - 1 ] - 1;
				}
		};

	class ISR : public constraintSolver::ISR
		{
		private:
			dex::vector < size_t > internal;
			dex::string word;
			size_t pos;
			dex::endOfDocumentISR ends;
		public:

			const static size_t npos = size_t ( -1 );
			ISR( )
				{
				}
			ISR( dex::string word, dex::vector < size_t > vecIn, dex::endOfDocumentISR endsIn )
				{
				this->word = word;
				internal = vecIn;
				pos = 0;
				ends = endsIn;
				}

			void reset( )
				{
				pos = 0;
				}
			// Jump this ISR to the first instance of our pattern that is at or after target. Return the location of the
			// instance, or -1 if there is none.
			size_t seek( size_t target )
				{
				for ( size_t index = 0;  index < internal.size( );  ++index )
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
					std::cout << "CALLED NEXT DOCUMENT, POS AT END OF INTERNAL" << std::endl;
					return npos;
					}
				ends.seek( internal[ pos ] );
				return seek( ends.next( ) );
				}
			dex::string getWord( )
				{
				return word;
				}
		};

	struct matchedDocuments
		{
		dex::vector < dex::string > flattenedQuery;
		dex::constraintSolver::ISR *matchingDocumentISR;
		dex::index::indexChunk *chunk;
		dex::vector < bool > emphasizedWords;
		};

	struct queryRequest
		{
		dex::string query;
		dex::index::indexChunk *chunkPointer;
		};

	struct searchResult
		{
		dex::Url url;
		dex::string title;
		};
	}

#endif
