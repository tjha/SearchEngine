// testingISRs.hpp
//
// 2019-12-15: File created: jasina

#ifndef DEX_TESTING_ISRS
#define DEX_TESTING_ISRS

#include <cstddef>
#include "constraintSolver/constraintSolver.hpp"
#include "utils/vector.hpp"

namespace dex
	{
	namespace constraintSolver
		{
		class testingEndOfDocumentISR : public dex::constraintSolver::endOfDocumentISR
			{
			private:
				friend class testingISR;
				dex::vector< size_t > deltas;

				size_t index;
				size_t location;

				size_t toGet;

			public:
				testingEndOfDocumentISR( const dex::vector<size_t> &deltas )
						: deltas( deltas ), index( 0 ), location( 0 ), toGet( 0 ) { }

				size_t seek( size_t target )
					{
					index = 0;
					location = 0;

					do
						location += deltas[ index++ ];
					while ( index < deltas.size( ) && location < target );

					if ( location < target )
						return toGet = npos;

					return toGet = location;
					}

				size_t next( )
					{
					if ( index >= deltas.size( ) )
						return toGet = npos;

					location += deltas[ index++ ];
					return toGet = location;
					}

				size_t nextDocument( )
					{
					return toGet = next( );
					}

				size_t get( ) const
					{
					return toGet;
					}

				size_t documentSize( ) const
					{
					if ( index == 0 || index > deltas.size( ) )
						return npos;

					if ( index == 1 )
						return deltas[ 0 ] + 1;
					return deltas[ index - 1 ];
					}
			};

		class testingISR : public dex::constraintSolver::ISR
			{
			private:
				dex::vector< size_t > deltas;
				testingEndOfDocumentISR *endOfDocumentISR;

				size_t index;
				size_t location;

				size_t toGet;

			public:
				testingISR( const dex::vector<size_t> &deltas, testingEndOfDocumentISR *endOfDocumentISR )
						: deltas( deltas ), endOfDocumentISR( endOfDocumentISR ), index( 0 ), location( 0 ), toGet( 0 ) { }

				~testingISR( )
					{
					delete endOfDocumentISR;
					}

				size_t seek( size_t target )
					{
					index = 0;
					location = 0;

					do
						location += deltas[ index++ ];
					while ( index < deltas.size( ) && location < target );

					if ( location < target )
						return toGet = npos;

					return toGet = location;
					}

				size_t next( )
					{
					if ( index >= deltas.size( ) )
						return toGet = npos;

					location += deltas[ index++ ];
					return toGet = location;
					}

				size_t nextDocument( )
					{
					if ( index == 0 )
						return toGet = next( );

					if ( index >= deltas.size( ) )
						return toGet = npos;

					endOfDocumentISR->seek( location );
					if ( seek( endOfDocumentISR->location ) == npos )
						return toGet = npos;

					if ( location == endOfDocumentISR->location )
						return toGet = next( );

					return toGet = location;
					}

				size_t get( ) const
					{
					return toGet;
					}
			};

		dex::vector< size_t > locationsToDeltas( const dex::vector< size_t > &locations )
			{
			dex::vector< size_t> deltas;

			if ( locations.empty( ) )
				return deltas;

			deltas.resize( locations.size( ) );

			deltas[ 0 ] = locations[ 0 ];
			for ( size_t index = 1;  index != locations.size( );  ++index )
				deltas[ index ] = locations[ index ] - locations[ index - 1 ];

			return deltas;
			}
		}
	}

#endif
