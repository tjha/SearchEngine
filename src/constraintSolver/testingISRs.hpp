// testingISRs.hpp
//
// 2019-12-15: File created: jasina

#include <cstddef>
#include "constraintSolver/constraintSolver.hpp"
#include "utils/vector.hpp"

class testingEndOfDocumentISR : public dex::constraintSolver::endOfDocumentISR
	{
	private:
		friend class testingISR;
		dex::vector < size_t > deltas;

		size_t index;
		size_t location;

	public:
		testingEndOfDocumentISR( const dex::vector <size_t> &deltas )
				: deltas( deltas ), index( 0 ), location( 0 ) { }

		size_t seek( size_t target )
			{
			index = 0;
			location = 0;

			do
				location += deltas[ index++ ];
			while ( index < deltas.size( ) && location < target );

			if ( location < target )
				return npos;

			return location;
			}

		size_t next( )
			{
			if ( index >= deltas.size( ) )
				return npos;

			location += deltas[ index++ ];
			return location;
			}

		size_t nextDocument( )
			{
			return next( );
			}

		size_t documentSize( )
			{
			if ( index == 0 || index > deltas.size( ) )
				return npos;

			return deltas[ index - 1 ];
			}
	};

class testingISR : public dex::constraintSolver::ISR
	{
	private:
		dex::vector < size_t > deltas;
		testingEndOfDocumentISR *endOfDocumentISR;

		size_t index;
		size_t location;

	public:
		testingISR( const dex::vector <size_t> &deltas, testingEndOfDocumentISR *endOfDocumentISR )
				: deltas( deltas ), endOfDocumentISR( endOfDocumentISR ), index( 0 ), location( 0 ) { }

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
				return npos;

			return location;
			}

		size_t next( )
			{
			if ( index >= deltas.size( ) )
				return npos;

			location += deltas[ index++ ];
			return location;
			}

		size_t nextDocument( )
			{
			if ( index == 0 )
				return next( );

			if ( index >= deltas.size( ) )
				return npos;

			endOfDocumentISR->seek( location );
			if ( seek( endOfDocumentISR->location ) == npos )
				return npos;

			if ( location == endOfDocumentISR->location )
				return next( );

			return location;
			}
	};

dex::vector < size_t > locationsToDeltas( const dex::vector < size_t > &locations )
	{
	dex::vector < size_t> deltas;
	deltas.resize( locations.size( ) );

	deltas[ 0 ] = locations[ 0 ];
	for ( size_t index = 1;  index != locations.size( );  ++index )
		deltas[ index ] = locations[ index ] - locations[ index - 1 ];

	return deltas;
	}
