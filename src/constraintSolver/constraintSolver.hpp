// constraintSolver.hpp
//
// Defines ISR interface and some specializations.
//
// 2019-12-11: File created: jasina, medhak

#ifndef DEX_CONSTRAINT_SOLVER
#define DEX_CONSTRAINT_SOLVER

#include <cstddef>
#include "../utils/vector.hpp"

namespace dex
	{
	namespace constraintSolver
		{
		// A virtual interface for index stream readers.
		class ISR
			{
			public:
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

		class andISR : ISR
			{
			private:
				// A vector of pointers to the ISRs we want to "and" together.
				vector < ISR * > factors;

			public:
				andISR( vector < ISR * > factors ) : factors( factors ) { }
			};

		class orISR : ISR
			{
			private:
				// A vector of pointers to the ISRs we want to "or" together.
				vector < ISR * > summands;

			public:
				orISR( vector < ISR * > summands ) : summands( summands ) { }
			};

		class notISR : ISR
			{
			private :
				// ISR pointer to ISR we don't want.
				ISR * neg;

			public:
				notISR ( ISR *neg ) : neg( neg ) { }
			};

		class phraseISR : ISR
			{
			private:
				// A vector of pointers to the ISRs we want to find a phrase of.
				vector < ISR * > words;

			public:
				phraseISR( vector < ISR * > words ) : words( words ) { }
			};
		}
	}

#endif