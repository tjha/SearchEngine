// constraintSolver.cpp
//
// Defines ISR interface and some specializations.
//
// 2019-12-11: File created: jasina, medhak

#include <cstddef>
#include "constraintSolver.hpp"
#include "../indexer/index.hpp"
#include "../utils/vector.hpp"

dex::constraintSolver::andISR::andISR( dex::vector < dex::constraintSolver::ISR * > factors,
		dex::index::indexChunk::indexStreamReader *endOfDocISR ) : factors( factors ), endOfDocISR( endOfDocISR )
	{
	locations.resize( factors.size( ) );
	for ( size_t index = 1;  index < factors.size( );  ++index )
		locations[ index ] = factors[ index ]->next( );

	endOfDocLocation = endOfDocISR->next( );
	}

bool dex::constraintSolver::andISR::align( )
	{
	while ( true )
		{
		size_t minIndex = 0;
		for ( size_t index = 1;  index < locations.size( );  ++index )
			if ( locations[ index ] < locations[ minIndex ] )
				minIndex = index;

		size_t startOfDocLocation = endOfDocLocation - endOfDocISR->documentSize( );
		if ( locations[ minIndex ] >= startOfDocLocation )
			return true;

		locations[ minIndex ] = factors[ minIndex ]->seek( startOfDocLocation );
		if ( locations[ minIndex ] == npos )
			return false;
		}
	}

size_t dex::constraintSolver::andISR::seek( size_t target )
	{
	size_t max = 0;
	for ( size_t index = 0;  index != locations.size();  ++index )
		{
		locations[ index ] = factors[ index ]->seek( target );
		if ( locations[ index ] < max )
			max = locations[index];
		}
	endOfDocLocation = endOfDocISR->seek( max );
	if ( align( ) )
		return locations[ 0 ];
	return npos;
	}

// TODO: Do we really need this?
size_t dex::constraintSolver::andISR::next( )
	{
	return -1;
	}

size_t dex::constraintSolver::andISR::nextDocument( )
	{
	locations[ 0 ] = factors[ 0 ]->nextDocument( );
	if ( align( ) )
		return locations[ 0 ];
	return npos;
	}

dex::constraintSolver::orISR::orISR( dex::vector < dex::constraintSolver::ISR * > summands ) : summands( summands ) { }


size_t dex::constraintSolver::orISR::seek( size_t target )
	{
	}

size_t dex::constraintSolver::orISR::next(  )
	{
	}

size_t dex::constraintSolver::orISR::nextDocument( )
	{
	}

dex::constraintSolver::notISR::notISR( dex::constraintSolver::ISR *neg ) : neg( neg ) { }

size_t dex::constraintSolver::notISR::seek( size_t target )
	{
	}
size_t dex::constraintSolver::notISR::next( )
	{
	}
size_t dex::constraintSolver::notISR::nextDocument( )
	{
	}


dex::constraintSolver::phraseISR::phraseISR( vector < dex::constraintSolver::ISR * > words ) : words( words ) { }


size_t dex::constraintSolver::phraseISR::seek( size_t target )
	{
	}

size_t dex::constraintSolver::phraseISR::next(  )
	{
	}

size_t dex::constraintSolver::phraseISR::nextDocument( )
	{
	}