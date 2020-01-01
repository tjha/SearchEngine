// constraintSolverTests.cpp
//
// Tests for the ISRs define in constraintSolver/constraintSolver.hpp. Addionally define a small testing ISR class.
//
// 2019-12-12: Wrote tests for andISR: jasina, medhak
// 2019-12-11: File created: jasina, medhak

#include <cstddef>
#include "testingISRs.hpp"
#include "catch.hpp"
#include "constraintSolver/constraintSolver.hpp"
#include "utils/vector.hpp"

TEST_CASE( "testISRs" )
	{
	SECTION( "next" )
		{
		testingEndOfDocumentISR *isrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
		testingISR isr( locationsToDeltas( dex::vector < size_t >( { 0, 4, 5, 9, 10, 11 } ) ), isrEnd );

		REQUIRE( isr.next( ) == 0 );
		REQUIRE( isr.get( ) == 0 );
		REQUIRE( isr.next( ) == 4 );
		REQUIRE( isr.get( ) == 4 );
		REQUIRE( isr.next( ) == 5 );
		REQUIRE( isr.get( ) == 5 );
		REQUIRE( isr.next( ) == 9 );
		REQUIRE( isr.get( ) == 9 );
		REQUIRE( isr.next( ) == 10 );
		REQUIRE( isr.get( ) == 10 );
		REQUIRE( isr.next( ) == 11 );
		REQUIRE( isr.get( ) == 11 );
		REQUIRE( isr.next( ) == testingISR::npos );
		REQUIRE( isr.get( ) == testingISR::npos );
		}

	SECTION( "nextDocument" )
		{
		testingEndOfDocumentISR *isrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
		testingISR isr( locationsToDeltas( dex::vector < size_t >( { 0, 4, 5, 9, 10, 11 } ) ), isrEnd );

		REQUIRE( isr.nextDocument( ) == 0 );
		REQUIRE( isr.get( ) == 0 );
		REQUIRE( isr.nextDocument( ) == 4 );
		REQUIRE( isr.get( ) == 4 );
		REQUIRE( isr.nextDocument( ) == 9 );
		REQUIRE( isr.get( ) == 9 );
		REQUIRE( isr.nextDocument( ) == testingISR::npos );
		REQUIRE( isr.get( ) == testingISR::npos );
		REQUIRE( isr.nextDocument( ) == testingISR::npos );
		REQUIRE( isr.get( ) == testingISR::npos );
		}

	SECTION( "overlapping nextDocument" )
		{
		testingEndOfDocumentISR *isrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
		testingISR isr( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ), isrEnd );

		REQUIRE( isr.nextDocument( ) == 3 );
		REQUIRE( isr.get( ) == 3 );
		REQUIRE( isr.nextDocument( ) == 7 );
		REQUIRE( isr.get( ) == 7 );
		REQUIRE( isr.nextDocument( ) == 8 );
		REQUIRE( isr.get( ) == 8 );
		REQUIRE( isr.nextDocument( ) == 12 );
		REQUIRE( isr.get( ) == 12 );
		REQUIRE( isr.nextDocument( ) == 13 );
		REQUIRE( isr.get( ) == 13 );
		REQUIRE( isr.nextDocument( ) == 14 );
		REQUIRE( isr.get( ) == 14 );
		REQUIRE( isr.nextDocument( ) == 16 );
		REQUIRE( isr.get( ) == 16 );
		REQUIRE( isr.nextDocument( ) == testingISR::npos );
		REQUIRE( isr.get( ) == testingISR::npos );
		REQUIRE( isr.nextDocument( ) == testingISR::npos );
		REQUIRE( isr.get( ) == testingISR::npos );
		}
	}

TEST_CASE( "andISR" )
	{
	SECTION( "seek" )
		{
		testingEndOfDocumentISR *isrEndA =
				new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndB =
				new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndC =
				new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndAnd =
				new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );

		testingISR *isrA = new testingISR( locationsToDeltas(
				dex::vector < size_t >( { 9, 27, 45, 66, 75 } ) ), isrEndA );
		testingISR *isrB = new testingISR( locationsToDeltas(
				dex::vector < size_t >( { 1, 3, 4, 12, 48, 65, 77 } ) ), isrEndB );
		testingISR *isrC = new testingISR( locationsToDeltas( dex::vector < size_t >( { 7, 49, 67 } ) ), isrEndC );

		dex::constraintSolver::andISR isrAnd(
				dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB, isrC } ), isrEndAnd);

		REQUIRE( isrAnd.seek( 10 ) == 50 );
		REQUIRE( isrAnd.get( ) == 50 );
		REQUIRE( isrAnd.seek( 62 ) == 70 );
		REQUIRE( isrAnd.get( ) == 70 );
		REQUIRE( isrAnd.seek( 71 ) == dex::constraintSolver::ISR::npos );
		REQUIRE( isrAnd.get( ) == dex::constraintSolver::ISR::npos );
		}

	SECTION( "next/nextDocument" )
		{
		SECTION( "All in first doc" )
			{
			testingEndOfDocumentISR *isrEndA =
					new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) ) ;
			testingEndOfDocumentISR *isrEndB =
					new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
			testingEndOfDocumentISR *isrEndC =
					new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
			testingEndOfDocumentISR *isrEndAnd =
					new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );

			testingISR *isrA = new testingISR(
						locationsToDeltas( dex::vector < size_t >( { 0, 4, 5, 9, 10, 11 } ) ), isrEndA );
			testingISR *isrB = new testingISR( locationsToDeltas( dex::vector < size_t >( { 1, 7, 16 } ) ), isrEndB );
			testingISR *isrC = new testingISR( locationsToDeltas( dex::vector < size_t >( { 2 } ) ), isrEndC );

			dex::constraintSolver::andISR isrAnd( dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB, isrC } ), isrEndAnd );

			REQUIRE( isrAnd.next( ) == 3 );
			REQUIRE( isrAnd.get( ) == 3 );
			REQUIRE( isrAnd.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.get( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.get( ) == dex::constraintSolver::ISR::npos );
			}

		SECTION( "A few matches" )
			{
			testingEndOfDocumentISR *isrEndA =
					new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 35, 81, 126, 139, 165 } ) ) );
			testingEndOfDocumentISR *isrEndB =
					new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 35, 81, 126, 139, 165 } ) ) );
			testingEndOfDocumentISR *isrEndAnd =
					new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 35, 81, 126, 139, 165 } ) ) );

			testingISR *isrA =
					new testingISR( locationsToDeltas( dex::vector < size_t >( { 17, 19, 70, 140, 145 } ) ), isrEndA );
			testingISR *isrB =
					new testingISR( locationsToDeltas( dex::vector < size_t >( { 49, 53, 101, 131, 141 } ) ), isrEndB );
			dex::constraintSolver::andISR isrAnd(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB } ), isrEndAnd );

			REQUIRE( isrAnd.next( ) == 81 );
			REQUIRE( isrAnd.get( ) == 81 );
			REQUIRE( isrAnd.next( ) == 165 );
			REQUIRE( isrAnd.get( ) == 165 );
			REQUIRE( isrAnd.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.get( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.get( ) == dex::constraintSolver::ISR::npos );
			}

		SECTION( "No matches" )
			{
			testingEndOfDocumentISR *isrEndA =
					new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndB =
					new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndAnd =
					new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );

			testingISR *isrA =
					new testingISR( locationsToDeltas( dex::vector < size_t >( { 10, 20, 29, 43 } ) ), isrEndA );
			testingISR *isrB =
					new testingISR( locationsToDeltas( dex::vector < size_t >( { 33, 58 } ) ), isrEndB );

			dex::constraintSolver::andISR isrAnd(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB } ), isrEndAnd );

			REQUIRE( isrAnd.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.get( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.get( ) == dex::constraintSolver::ISR::npos );
			}

		SECTION( "Consecutive matches" )
			{
			testingEndOfDocumentISR *isrEndA =
					new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndB =
					new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndAnd =
					new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );

			testingISR *isrA =
					new testingISR( locationsToDeltas( dex::vector < size_t >( { 10, 21, 29, 43, 52, 53 } ) ), isrEndA );
			testingISR *isrB =
					new testingISR( locationsToDeltas( dex::vector < size_t >( { 22, 24, 44, 58 } ) ), isrEndB );

			dex::constraintSolver::andISR isrAnd( dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB } ), isrEndAnd );

			REQUIRE( isrAnd.next( ) == 30 );
			REQUIRE( isrAnd.get( ) == 30 );
			REQUIRE( isrAnd.next( ) == 50 );
			REQUIRE( isrAnd.get( ) == 50 );
			REQUIRE( isrAnd.next( ) == 60 );
			REQUIRE( isrAnd.get( ) == 60 );
			REQUIRE( isrAnd.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.get( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrAnd.get( ) == dex::constraintSolver::ISR::npos );
			}
		}
	}

TEST_CASE( "orISR" )
	{
	SECTION( "seek" )
		{
		testingEndOfDocumentISR *isrEndA =
				new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndB =
				new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndC =
				new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndOr =
				new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );

		testingISR *isrA =
				new testingISR( locationsToDeltas( dex::vector < size_t >( { 9, 27, 45, 66, 75 } ) ), isrEndA );
		testingISR *isrB =
				new testingISR( locationsToDeltas( dex::vector < size_t >( { 1, 3, 4, 12, 48, 65, 77 } ) ), isrEndB );
		testingISR *isrC =
				new testingISR( locationsToDeltas( dex::vector < size_t >( { 7, 49, 67 } ) ), isrEndC );

		dex::constraintSolver::orISR isrOr(
				dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB, isrC } ), isrEndOr );

		REQUIRE( isrOr.seek( 10 ) == 20 );
		REQUIRE( isrOr.get( ) == 20 );
		REQUIRE( isrOr.seek( 30 ) == 50 );
		REQUIRE( isrOr.get( ) == 50 );
		REQUIRE( isrOr.seek( 40 ) == 50 );
		REQUIRE( isrOr.get( ) == 50 );
		REQUIRE( isrOr.seek( 71 ) == 80 );
		REQUIRE( isrOr.get( ) == 80 );
		REQUIRE( isrOr.seek( 78 ) == dex::constraintSolver::ISR::npos );
		REQUIRE( isrOr.get( ) == dex::constraintSolver::ISR::npos );
		}

	SECTION( "next/nextDocument" )
		{
		SECTION( "All in first doc" )
			{
			testingEndOfDocumentISR *isrEndA =
					new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 9, 12, 13, 14, 16 } ) ) );
			testingEndOfDocumentISR *isrEndB =
					new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 9, 12, 13, 14, 16 } ) ) );
			testingEndOfDocumentISR *isrEndC =
					new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 9, 12, 13, 14, 16 } ) ) );
			testingEndOfDocumentISR *isrEndOR =
					new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 9, 12, 13, 14, 16 } ) ) );

			testingISR *isrA =
					new testingISR( locationsToDeltas( dex::vector < size_t >( { 0, 4, 5, 8, 10, 11 } ) ), isrEndA );
			testingISR *isrB =
					new testingISR( locationsToDeltas( dex::vector < size_t >( { 1, 6, 15 } ) ), isrEndB );
			testingISR *isrC =
					new testingISR( locationsToDeltas( dex::vector < size_t >( { 2 } ) ), isrEndC );

			dex::constraintSolver::orISR isrOR(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB, isrC } ), isrEndOR );

			REQUIRE( isrOR.next( ) == 3 );
			REQUIRE( isrOR.get( ) == 3 );
			REQUIRE( isrOR.next( ) == 7 );
			REQUIRE( isrOR.get( ) == 7 );
			REQUIRE( isrOR.next( ) == 9 );
			REQUIRE( isrOR.get( ) == 9 );
			REQUIRE( isrOR.next( ) == 12 );
			REQUIRE( isrOR.get( ) == 12 );
			REQUIRE( isrOR.next( ) == 16 );
			REQUIRE( isrOR.get( ) == 16 );
			REQUIRE( isrOR.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrOR.get( ) == dex::constraintSolver::ISR::npos );
			}

		SECTION( "A few matches" )
			{
			testingEndOfDocumentISR *isrEndA
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 35, 81, 126, 139, 165 } ) ) );
			testingEndOfDocumentISR *isrEndB
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 35, 81, 126, 139, 165 } ) ) );
			testingEndOfDocumentISR *isrEndOr
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 35, 81, 126, 139, 165 } ) ) );

			testingISR *isrA = new testingISR(
					locationsToDeltas( dex::vector < size_t >( { 17, 19, 70, 140, 145 } ) ), isrEndA );
			testingISR *isrB = new testingISR(
					locationsToDeltas( dex::vector < size_t >( { 49, 53, 131, 141 } ) ), isrEndB );

			dex::constraintSolver::orISR isrOr(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB } ), isrEndOr );

			REQUIRE( isrOr.next( ) == 35 );
			REQUIRE( isrOr.get( ) == 35 );
			REQUIRE( isrOr.next( ) == 81 );
			REQUIRE( isrOr.get( ) == 81 );
			REQUIRE( isrOr.next( ) == 139 );
			REQUIRE( isrOr.get( ) == 139 );
			REQUIRE( isrOr.next( ) == 165 );
			REQUIRE( isrOr.get( ) == 165 );
			REQUIRE( isrOr.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrOr.get( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrOr.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrOr.get( ) == dex::constraintSolver::ISR::npos );
			}

		SECTION( "No matches" )
			{
			testingEndOfDocumentISR *isrEndA
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndB
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndOr
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );

			testingISR *isrA
				= new testingISR( locationsToDeltas( dex::vector < size_t >( { 9, 19, 29, 43 } ) ), isrEndA );
			testingISR *isrB = new testingISR( locationsToDeltas( dex::vector < size_t >( { 23, 58 } ) ), isrEndB );

			dex::constraintSolver::orISR isrOr(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB } ), isrEndOr );

			REQUIRE( isrOr.next( ) == 10 );
			REQUIRE( isrOr.get( ) == 10 );
			REQUIRE( isrOr.next( ) == 20 );
			REQUIRE( isrOr.get( ) == 20 );
			REQUIRE( isrOr.next( ) == 30 );
			REQUIRE( isrOr.get( ) == 30 );
			REQUIRE( isrOr.next( ) == 50 );
			REQUIRE( isrOr.get( ) == 50 );
			REQUIRE( isrOr.next( ) == 60 );
			REQUIRE( isrOr.get( ) == 60 );
			REQUIRE( isrOr.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrOr.get( ) == dex::constraintSolver::ISR::npos );
			}

		SECTION( "Consecutive matches" )
			{
			testingEndOfDocumentISR *isrEndA
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndB
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndOR
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );

			testingISR *isrA
					= new testingISR( locationsToDeltas( dex::vector < size_t >( { 9, 21, 29, 43, 52, 53 } ) ), isrEndA );
			testingISR *isrB
					= new testingISR( locationsToDeltas( dex::vector < size_t >( { 13, 22, 24, 44, 58 } ) ), isrEndB );

			dex::constraintSolver::orISR isrOr(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB } ), isrEndOR );

			REQUIRE( isrOr.next( ) == 10 );
			REQUIRE( isrOr.get( ) == 10 );
			REQUIRE( isrOr.next( ) == 20 );
			REQUIRE( isrOr.get( ) == 20 );
			REQUIRE( isrOr.seek( 29 ) == 30 );
			REQUIRE( isrOr.get( ) == 30 );
			REQUIRE( isrOr.seek( 45 ) == 60 );
			REQUIRE( isrOr.get( ) == 60 );
			REQUIRE( isrOr.next( ) == 60 );
			REQUIRE( isrOr.get( ) == 60 );
			REQUIRE( isrOr.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrOr.get( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrOr.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrOr.get( ) == dex::constraintSolver::ISR::npos );
			}
		}
	}

TEST_CASE( "phraseISR" )
	{
	SECTION( "seek" )
		{
		testingEndOfDocumentISR *isrEndA
				= new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndB
				= new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndC
				= new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndPhrase
				= new testingEndOfDocumentISR(
						locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );

		testingISR *isrA = new testingISR(
				locationsToDeltas( dex::vector < size_t >( { 9, 13, 27, 45, 66, 75 } ) ), isrEndA );
		testingISR *isrB = new testingISR(
				locationsToDeltas( dex::vector < size_t >( { 1, 3, 4, 11, 14, 46, 65, 77 } ) ), isrEndB );
		testingISR *isrC = new testingISR(
				locationsToDeltas( dex::vector < size_t >( { 7, 12, 15, 47, 67 } ) ), isrEndC );

		dex::constraintSolver::phraseISR isrPhrase(
				dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB, isrC } ), isrEndPhrase );

		REQUIRE( isrPhrase.seek( 10 ) == 20 );
		REQUIRE( isrPhrase.get( ) == 20 );
		REQUIRE( isrPhrase.seek( 41 ) == 50 );
		REQUIRE( isrPhrase.get( ) == 50 );
		REQUIRE( isrPhrase.seek( 60 ) == dex::constraintSolver::ISR::npos ); // or should this be 80?
		REQUIRE( isrPhrase.get( ) == dex::constraintSolver::ISR::npos ); // or should this be 80?
		}

	SECTION( "next/nextDocument" )
		{
		SECTION( "All in first doc" )
			{
			testingEndOfDocumentISR *isrEndA
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 9, 12, 13, 14, 16 } ) ) );
			testingEndOfDocumentISR *isrEndB
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 9, 12, 13, 14, 16 } ) ) );
			testingEndOfDocumentISR *isrEndC
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 9, 12, 13, 14, 16 } ) ) );
			testingEndOfDocumentISR *isrEndPhrase
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 3, 7, 9, 12, 13, 14, 16 } ) ) );

			testingISR *isrA = new testingISR(
					locationsToDeltas( dex::vector < size_t >( { 0, 4, 5, 8, 10, 11 } ) ), isrEndA );
			testingISR *isrB = new testingISR( locationsToDeltas( dex::vector < size_t >( { 1, 6, 15 } ) ), isrEndB );
			testingISR *isrC = new testingISR( locationsToDeltas( dex::vector < size_t >( { 2 } ) ), isrEndC );

			dex::constraintSolver::phraseISR isrPhrase(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB, isrC } ), isrEndPhrase );

			REQUIRE( isrPhrase.next( ) == 3 );
			REQUIRE( isrPhrase.get( ) == 3 );
			REQUIRE( isrPhrase.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrPhrase.get( ) == dex::constraintSolver::ISR::npos );
			}

		SECTION( "A few matches" )
			{
			testingEndOfDocumentISR *isrEndA
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 35, 81, 126, 139, 165 } ) ) );
			testingEndOfDocumentISR *isrEndB
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 35, 81, 126, 139, 165 } ) ) );
			testingEndOfDocumentISR *isrEndPhrase
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 35, 81, 126, 139, 165 } ) ) );

			testingISR *isrA
					= new testingISR( locationsToDeltas( dex::vector < size_t >( { 17, 19, 70, 140, 145 } ) ), isrEndA );
			testingISR *isrB
					= new testingISR( locationsToDeltas( dex::vector < size_t >( { 18, 49, 53, 131, 141 } ) ), isrEndB );

			dex::constraintSolver::phraseISR isrPhrase(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB } ), isrEndPhrase );

			REQUIRE( isrPhrase.next( ) == 35 );
			REQUIRE( isrPhrase.get( ) == 35 );
			REQUIRE( isrPhrase.next( ) == 165 );
			REQUIRE( isrPhrase.get( ) == 165 );
			REQUIRE( isrPhrase.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrPhrase.get( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrPhrase.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrPhrase.get( ) == dex::constraintSolver::ISR::npos );
			}

		SECTION( "No matches" )
			{
			testingEndOfDocumentISR *isrEndA
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndB
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndPhrase
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );

			testingISR *isrA = new testingISR(
					locationsToDeltas( dex::vector < size_t >( { 9, 19, 24, 43 } ) ), isrEndA );
			testingISR *isrB = new testingISR( locationsToDeltas( dex::vector < size_t >( { 23, 45, 58 } ) ), isrEndB );

			dex::constraintSolver::phraseISR isrPhrase(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB } ), isrEndPhrase );

			REQUIRE( isrPhrase.next( ) == dex::constraintSolver::ISR::npos );
			}

		SECTION( "Consecutive matches" )
			{
			testingEndOfDocumentISR *isrEndA
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndB
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );
			testingEndOfDocumentISR *isrEndPhrase
					= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60 } ) ) );

			testingISR *isrA = new testingISR(
					locationsToDeltas( dex::vector < size_t >( { 11, 13, 21, 29, 43, 52, 54 } ) ), isrEndA );
			testingISR *isrB = new testingISR(
					locationsToDeltas( dex::vector < size_t >( { 12, 14, 22, 24, 44, 53, 58 } ) ), isrEndB );

			dex::constraintSolver::phraseISR isrPhrase(
					dex::vector < dex::constraintSolver::ISR * >( { isrA, isrB } ), isrEndPhrase );

			REQUIRE( isrPhrase.next( ) == 20 );
			REQUIRE( isrPhrase.get( ) == 20 );
			REQUIRE( isrPhrase.next( ) == 30 );
			REQUIRE( isrPhrase.get( ) == 30 );
			REQUIRE( isrPhrase.next( ) == 50 );
			REQUIRE( isrPhrase.get( ) == 50 );
			REQUIRE( isrPhrase.seek( 45 ) == 60 );
			REQUIRE( isrPhrase.get( ) == 60 );
			REQUIRE( isrPhrase.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrPhrase.get( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrPhrase.seek( 55 ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrPhrase.get( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrPhrase.next( ) == dex::constraintSolver::ISR::npos );
			REQUIRE( isrPhrase.get( ) == dex::constraintSolver::ISR::npos );
			}
		}
	}

TEST_CASE( "notISR" )
	{
	SECTION( "next" )
		{
		testingEndOfDocumentISR *isrEndA
				= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndB
				= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndC
				= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndNotA
				= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndNotB
				= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );
		testingEndOfDocumentISR *isrEndNotC
				= new testingEndOfDocumentISR(
							locationsToDeltas( dex::vector < size_t >( { 10, 20, 30, 40, 50, 60, 70, 80 } ) ) );

		testingISR *isrA = new testingISR(
				locationsToDeltas( dex::vector < size_t >( { 9, 13, 27, 45, 66, 75 } ) ), isrEndA );
		testingISR *isrB = new testingISR(
				locationsToDeltas( dex::vector < size_t >( { 1, 3, 4, 11, 14, 46, 65, 77 } ) ), isrEndB );
		testingISR *isrC = new testingISR(
				locationsToDeltas( dex::vector < size_t >( { 7, 12, 15, 47, 67 } ) ), isrEndC );

		dex::constraintSolver::notISR isrNotA( isrA, isrEndNotA );
		dex::constraintSolver::notISR isrNotB( isrB, isrEndNotB );
		dex::constraintSolver::notISR isrNotC( isrC, isrEndNotC );

		REQUIRE( isrNotA.next( ) == 40 );
		REQUIRE( isrNotA.get( ) == 40 );
		REQUIRE( isrNotA.next( ) == 60 );
		REQUIRE( isrNotA.get( ) == 60 );
		REQUIRE( isrNotA.seek( 55 ) == 60 );
		REQUIRE( isrNotA.get( ) == 60 );
		REQUIRE( isrNotA.next( ) == dex::constraintSolver::ISR::npos );
		REQUIRE( isrNotA.get( ) == dex::constraintSolver::ISR::npos );

		REQUIRE( isrNotB.seek( 27 ) == 30 );
		REQUIRE( isrNotB.get( ) == 30 );
		REQUIRE( isrNotB.seek( 35 ) == 40 );
		REQUIRE( isrNotB.get( ) == 40 );
		REQUIRE( isrNotB.next( ) == 60 );
		REQUIRE( isrNotB.get( ) == 60 );
		REQUIRE( isrNotB.seek( 75 ) == dex::constraintSolver::ISR::npos );
		REQUIRE( isrNotB.get( ) == dex::constraintSolver::ISR::npos );
		REQUIRE( isrNotB.next( ) == dex::constraintSolver::ISR::npos );
		REQUIRE( isrNotB.get( ) == dex::constraintSolver::ISR::npos );
		}
	}

TEST_CASE( "nested ISRs" )
	{
	SECTION( "OR with alignment" )
		{
		testingEndOfDocumentISR *isrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
		testingEndOfDocumentISR *isrOrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );

		testingISR *isr = new testingISR( locationsToDeltas( dex::vector < size_t >( { 7, 8, 13, 16 } ) ), isrEnd );
		dex::constraintSolver::orISR orISR( dex::vector < dex::constraintSolver::ISR * >{ isr }, isrOrEnd );

		REQUIRE( orISR.nextDocument( ) == 7 );
		REQUIRE( orISR.get( ) == 7 );
		REQUIRE( orISR.nextDocument( ) == 8 );
		REQUIRE( orISR.get( ) == 8 );
		REQUIRE( orISR.nextDocument( ) == 13 );
		REQUIRE( orISR.get( ) == 13 );
		REQUIRE( orISR.nextDocument( ) == 16 );
		REQUIRE( orISR.get( ) == 16 );
		REQUIRE( orISR.nextDocument( ) == testingISR::npos );
		REQUIRE( orISR.get( ) == testingISR::npos );
		REQUIRE( orISR.nextDocument( ) == testingISR::npos );
		REQUIRE( orISR.get( ) == testingISR::npos );
		}

	SECTION( "AND with alignment" )
		{
		testingEndOfDocumentISR *isrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
		testingEndOfDocumentISR *isrAndEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );

		testingISR *isr = new testingISR( locationsToDeltas( dex::vector < size_t >( { 7, 8, 13, 16 } ) ), isrEnd );
		dex::constraintSolver::andISR andISR( dex::vector < dex::constraintSolver::ISR * >{ isr }, isrAndEnd );

		REQUIRE( andISR.nextDocument( ) == 7 );
		REQUIRE( andISR.get( ) == 7 );
		REQUIRE( andISR.nextDocument( ) == 8 );
		REQUIRE( andISR.get( ) == 8 );
		REQUIRE( andISR.nextDocument( ) == 13 );
		REQUIRE( andISR.get( ) == 13 );
		REQUIRE( andISR.nextDocument( ) == 16 );
		REQUIRE( andISR.get( ) == 16 );
		REQUIRE( andISR.nextDocument( ) == testingISR::npos );
		REQUIRE( andISR.get( ) == testingISR::npos );
		REQUIRE( andISR.nextDocument( ) == testingISR::npos );
		REQUIRE( andISR.get( ) == testingISR::npos );
		}

	SECTION( "one word in OR in OR" )
		{
		testingEndOfDocumentISR *isrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
		testingEndOfDocumentISR *isrOrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
		testingEndOfDocumentISR *isrOrOrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );

		testingISR *isr = new testingISR( locationsToDeltas( dex::vector < size_t >( { 0, 4, 5, 9, 10, 11 } ) ), isrEnd );
		dex::constraintSolver::orISR *orISR = new dex::constraintSolver::orISR( dex::vector < dex::constraintSolver::ISR * >{ isr }, isrOrEnd );
		dex::constraintSolver::orISR orOrISR( dex::vector < dex::constraintSolver::ISR * >{ orISR }, isrOrOrEnd );

		REQUIRE( orOrISR.nextDocument( ) == 3 );
		REQUIRE( orOrISR.get( ) == 3 );
		REQUIRE( orOrISR.nextDocument( ) == 7 );
		REQUIRE( orOrISR.get( ) == 7 );
		REQUIRE( orOrISR.nextDocument( ) == 12 );
		REQUIRE( orOrISR.get( ) == 12 );
		REQUIRE( orOrISR.nextDocument( ) == testingISR::npos );
		REQUIRE( orOrISR.get( ) == testingISR::npos );
		REQUIRE( orOrISR.nextDocument( ) == testingISR::npos );
		REQUIRE( orOrISR.get( ) == testingISR::npos );
		}

	SECTION( "one word in AND in AND" )
		{
		testingEndOfDocumentISR *isrEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
		testingEndOfDocumentISR *isrAndEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );
		testingEndOfDocumentISR *isrAndAndEnd
				= new testingEndOfDocumentISR( locationsToDeltas( dex::vector < size_t >( { 3, 7, 8, 12, 13, 14, 16 } ) ) );

		testingISR *isr = new testingISR( locationsToDeltas( dex::vector < size_t >( { 0, 4, 5, 9, 10, 11 } ) ), isrEnd );
		dex::constraintSolver::andISR *orISR = new dex::constraintSolver::andISR( dex::vector < dex::constraintSolver::ISR * >{ isr }, isrAndEnd );
		dex::constraintSolver::andISR orOrISR( dex::vector < dex::constraintSolver::ISR * >{ orISR }, isrAndAndEnd );

		REQUIRE( orOrISR.nextDocument( ) == 3 );
		REQUIRE( orOrISR.get( ) == 3 );
		REQUIRE( orOrISR.nextDocument( ) == 7 );
		REQUIRE( orOrISR.get( ) == 7 );
		REQUIRE( orOrISR.nextDocument( ) == 12 );
		REQUIRE( orOrISR.get( ) == 12 );
		REQUIRE( orOrISR.nextDocument( ) == testingISR::npos );
		REQUIRE( orOrISR.get( ) == testingISR::npos );
		REQUIRE( orOrISR.nextDocument( ) == testingISR::npos );
		REQUIRE( orOrISR.get( ) == testingISR::npos );
		}
	}
