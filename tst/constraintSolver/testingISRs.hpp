// testingISRs.hpp
//
// 2019-12-15: File created: jasina

#include <cstddef>
#include "../../src/constraintSolver/constraintSolver.hpp"
#include "../../src/utils/vector.hpp"

class testingEndOfDocumentISR : public dex::constraintSolver::endOfDocumentISR
	{
	private:
		friend class testingISR;
		dex::vector < size_t > deltas;

		size_t index;
		size_t location;

	public:
		testingEndOfDocumentISR( const dex::vector <size_t> &deltas )
				: deltas( deltas ), index( -1 ), location( 0 ) { }

		size_t seek( size_t target )
			{
			if ( index == static_cast < size_t >( -1 ) )
				next( );

			while ( index < deltas.size( ) && location < target )
				if ( index < deltas.size( ) - 1 )
					location += deltas[ ++index ];
				else
					++index;

			if ( index >= deltas.size( ) )
				return npos;

			return location;
			}

		size_t next( )
			{
			if ( index != static_cast < size_t >( -1 ) && index >= deltas.size( ) - 1 )
				return npos;
			location += deltas[ ++index ];
			return location;
			}

		size_t nextDocument( )
			{
			return next( );
			}

		size_t documentSize( )
			{
			if ( index == static_cast < size_t >( -1 ) || index >= deltas.size( ) )
				return npos;

			return deltas[ index ];
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
				: deltas( deltas ), endOfDocumentISR( endOfDocumentISR ), index( -1 ), location( 0 ) { }

		~testingISR( )
			{
			delete endOfDocumentISR;
			}

		size_t seek( size_t target )
			{
			if ( index == static_cast < size_t >( -1 ) )
				next( );

			while ( index < deltas.size( ) && location < target )
				if ( index < deltas.size( ) - 1 )
					location += deltas[ ++index ];
				else
					++index;
			if ( index >= deltas.size( ) )
				return npos;

			endOfDocumentISR->seek( location );
			return location;
			}

		size_t next( )
			{
			if ( index != static_cast < size_t >( -1 ) && index >= deltas.size( ) - 1 )
				return npos;
			location += deltas[ ++index ];
			return location;
			}

		size_t nextDocument( )
			{
			seek( endOfDocumentISR->location );
			if ( index >= deltas.size( ) )
				return npos;
			endOfDocumentISR->seek( location );
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
