// constraintSolver.hpp
//
// Defines ISR interface and some specializations.
//
// 2019-12-11: File created: jasina, medhak

#ifndef DEX_CONSTRAINT_SOLVER
#define DEX_CONSTRAINT_SOLVER

#include <cstddef>
#include "../indexer/index.hpp"
#include "../utils/vector.hpp"

namespace dex
	{
	namespace constraintSolver
		{
		// A virtual interface for index stream readers.
		class ISR
			{
			public:
				static const size_t npos = static_cast < size_t >( -1 );

				// Jump this ISR to the first instance of our pattern that is at or after target. Return the location of the
				// instance, or -1 if there is none.
				virtual size_t seek( size_t target ) = 0;

				// Jump ISR to next location of our pattern. Return location of instance or -1 if there is none.
				virtual size_t next( ) = 0;

				// Jump this ISR to the first location of our pattern that is in the next document. Return the location of the
				// instance, or -1 if there is none.
				// Note that this requires some way of accessing document boundaries.
				virtual size_t nextDocument( ) = 0;
			};

		class andISR : dex::constraintSolver::ISR
			{
			private:
				// A vector of pointers to the ISRs we want to "and" together.
				dex::vector < dex::constraintSolver::ISR * > factors;
				dex::index::indexChunk::indexStreamReader *endOfDocISR;

				dex::vector < size_t > locations;
				size_t endOfDocLocation;

				// Move all factors so they are in the same document. Return true when there are still things to read.
				bool align( );

			public:
				andISR( dex::vector < dex::constraintSolver::ISR * > factors,
						dex::index::indexChunk::indexStreamReader *endOfDocISR );
				virtual size_t seek( size_t target );
				virtual size_t next( );
				virtual size_t nextDocument( );
			};

		class orISR : dex::constraintSolver::ISR
			{
			private:
				// A vector of pointers to the ISRs we want to "or" together.
				dex::vector < dex::constraintSolver::ISR * > summands;
				dex::index::indexChunk::indexStreamReader *endOfDocISR;

				dex::vector < size_t > locations;
				size_t endOfDocLocation;

			public:
				orISR( dex::vector < dex::constraintSolver::ISR * > summands,
						dex::index::indexChunk::indexStreamReader *endOfDocISR );
				virtual size_t seek( size_t target );
				virtual size_t next( );
				virtual size_t nextDocument( );
			};

		class notISR : dex::constraintSolver::ISR
			{
			private :
				// ISR pointer to ISR we don't want.
				dex::constraintSolver::ISR *neg;
				dex::index::indexChunk::indexStreamReader *endOfDocISR;

				size_t location;
				size_t endOfDocLocation;

			public:
				notISR ( dex::constraintSolver::ISR *neg,
					dex::index::indexChunk::indexStreamReader *endOfDocISR );
				virtual size_t seek( size_t target );
				virtual size_t next( );
				virtual size_t nextDocument( );
			};

		class phraseISR : dex::constraintSolver::ISR
			{
			private:
				// A vector of pointers to the ISRs we want to find a phrase of.
				dex::vector < dex::constraintSolver::ISR * > words;

				dex::vector < size_t > locations;

				// Move all word ISRs so that they in order next to each other in a phrase. Return true when there are still
				// things to read.
				bool align( );

			public:
				phraseISR( dex::vector < dex::constraintSolver::ISR * > words );
				virtual size_t seek( size_t target );
				virtual size_t next( );
				virtual size_t nextDocument( );
			};
		}
	}

#endif
