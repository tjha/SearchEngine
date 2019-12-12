// constraintSolver.cpp
//
// Defines ISR interface and some specializations.
//
// 2019-12-11: File created: jasina, medhak

#include <cstddef>
#include "constraintSolver.hpp"
#include "../indexer/index.hpp"
#include "../utils/vector.hpp"

size_t argMin( const dex::vector < size_t > &v )
	{
	size_t minIndex = 0;
	for ( size_t index = 1;  index < v.size( );  ++index )
		if ( v[ index ] < v[ minIndex ] )
			minIndex = index;

	return minIndex;
	}

size_t argMax( const dex::vector < size_t > &v )
	{
	size_t maxIndex = 0;
	for ( size_t index = 1;  index < v.size( );  ++index )
		if ( v[ index ] > v[ maxIndex ] )
			maxIndex = index;

	return maxIndex;
	}


bool dex::constraintSolver::andISR::align( )
	{
	while ( true )
		{
		size_t minIndex = argMin( locations );

		size_t startOfDocLocation = endOfDocLocation - endOfDocISR->documentSize( );
		if ( locations[ minIndex ] >= startOfDocLocation )
			return true;

		locations[ minIndex ] = factors[ minIndex ]->seek( startOfDocLocation );
		if ( locations[ minIndex ] == npos )
			return false;
		}
	}

dex::constraintSolver::andISR::andISR( dex::vector < dex::constraintSolver::ISR * > factors,
		dex::index::indexChunk::indexStreamReader *endOfDocISR ) : factors( factors ), endOfDocISR( endOfDocISR )
	{
	locations.resize( factors.size( ) );
	for ( size_t index = 1;  index < factors.size( );  ++index )
		locations[ index ] = factors[ index ]->next( );

	endOfDocLocation = endOfDocISR->next( );
	}

size_t dex::constraintSolver::andISR::seek( size_t target )
	{
	size_t max = 0;
	for ( size_t index = 0;  index < locations.size();  ++index )
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
	return nextDocument( );
	}

size_t dex::constraintSolver::andISR::nextDocument( )
	{
	locations[ 0 ] = factors[ 0 ]->nextDocument( );
	if ( align( ) )
		return locations[ 0 ];
	return npos;
	}


dex::constraintSolver::orISR::orISR( dex::vector < dex::constraintSolver::ISR * > summands,
		dex::index::indexChunk::indexStreamReader *endOfDocISR ) : summands( summands ), endOfDocISR( endOfDocISR )
	{
	locations.reserve( summands.size( ) );
	for ( size_t index = 1;  index < summands.size( );  ++index )
		locations[ index ] = summands[ index ]->next( );

	endOfDocLocation = endOfDocISR->next( );
	}

size_t dex::constraintSolver::orISR::seek( size_t target )
	{
	for ( size_t index = 0;  index < locations.size( );  ++index )
		locations[ index ] = summands[ index ]->seek( target );

	return locations[ argMin( locations ) ];
	}

size_t dex::constraintSolver::orISR::next(  )
	{
	// size_t minIndex = argMin( locations );
	// locations[ minIndex ] = summands[ minIndex ]->nextDocument( );
	// return locations[ argMin( locations ) ];
	return nextDocument( );
	}

size_t dex::constraintSolver::orISR::nextDocument( )
	{
	size_t minIndex = argMin( locations );
	endOfDocLocation = endOfDocISR->seek( locations[ minIndex ] );
	while ( locations[ minIndex ] < endOfDocLocation )
		{
		locations[ minIndex ] = summands[ minIndex ]->nextDocument( );
		minIndex = argMin( locations );
		}
	return locations[ minIndex ];
	}


dex::constraintSolver::notISR::notISR( dex::constraintSolver::ISR *neg,
		dex::index::indexChunk::indexStreamReader *endOfDocISR ) : neg( neg ), endOfDocISR( endOfDocISR )
	{
	endOfDocLocation = 0;
	location = neg->next( );
	}

size_t dex::constraintSolver::notISR::seek( size_t target )
	{
	endOfDocLocation = endOfDocISR->seek( target );
	size_t startOfDoc = endOfDocLocation - endOfDocISR->documentSize( );
	location = neg->seek( startOfDoc );
	if( endOfDocLocation < location )
		return endOfDocLocation;

	return nextDocument( );
	}

size_t dex::constraintSolver::notISR::next( )
	{
	return nextDocument( );
	}

size_t dex::constraintSolver::notISR::nextDocument( )
	{
	while ( ( endOfDocLocation = endOfDocISR->next( ) ) > location )
		location = neg->seek( endOfDocLocation );
	return endOfDocLocation;
	}


bool dex::constraintSolver::phraseISR::align( )
	{
	while ( true )
		{
		bool inOrder = true;
		for( size_t index = 1;  index < locations.size( );  ++index )
			if( locations[ index ] - locations[ index - 1 ] != 1 )
				{
				inOrder = false;
				break;
				}

		if( inOrder )
			return true;

		size_t maxLocation = locations[ argMax( locations ) ];

		if ( maxLocation == npos )
			return false;

		size_t rightmostPossibleOffset = maxLocation - words.size( );
		for ( size_t index = 0;  index < locations.size( );  ++index )
			if ( locations[ index ] < rightmostPossibleOffset )
				locations[ index ] = words[ index ]->seek( rightmostPossibleOffset );
		}
	}

dex::constraintSolver::phraseISR::phraseISR( vector < dex::constraintSolver::ISR * > words ) : words( words )
	{
	locations.resize( words.size( ) );
	for ( size_t index = 1;  index < words.size( );  ++index )
		locations[ index ] = words[ index ]->next( );
	}

size_t dex::constraintSolver::phraseISR::seek( size_t target )
	{
	size_t max = 0;
	for ( size_t index = 0;  index < locations.size();  ++index )
		{
		locations[ index ] = words[ index ]->seek( target );
		if ( locations[ index ] < max )
			max = locations[index];
		}

	if ( align( ) )
		return locations[ argMin( locations ) ];
	return npos;
	}

size_t dex::constraintSolver::phraseISR::next(  )
	{
	return nextDocument( );
	}

size_t dex::constraintSolver::phraseISR::nextDocument( )
	{
	locations[ 0 ] = words[ 0 ]->nextDocument( );
	if ( align( ) )
		return locations[ 0 ];
	return npos;
	}
