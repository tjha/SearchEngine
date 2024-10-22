// constraintSolver.cpp
//
// Defines ISR interface and some specializations.
//
// 2019-12-12: Implement all ISRs: jasina, medhak
// 2019-12-11: File created: jasina, medhak

#include <cstddef>
#include "constraintSolver.hpp"
#include "utils/vector.hpp"

#include <iostream>

size_t argMin( const dex::vector< size_t > &v )
	{
	size_t minIndex = 0;
	for ( size_t index = 1;  index < v.size( );  ++index )
		if ( v[ index ] < v[ minIndex ] )
			minIndex = index;

	return minIndex;
	}

size_t argMax( const dex::vector< size_t > &v )
	{
	size_t maxIndex = 0;
	for ( size_t index = 1;  index < v.size( );  ++index )
		if ( v[ index ] > v[ maxIndex ] )
			maxIndex = index;

	return maxIndex;
	}


const size_t dex::constraintSolver::ISR::npos = static_cast< size_t >( -1 );


bool dex::constraintSolver::andISR::align( )
	{
	while ( true )
		{
		endOfDocLocation = endOfDocISR->seek( locations[ argMax( locations ) ] );
		if ( endOfDocLocation == npos )
			return false;

		size_t minIndex = argMin( locations );

		size_t startOfDocLocation = endOfDocLocation - endOfDocISR->documentSize( ) + 1;
		if ( locations[ minIndex ] >= startOfDocLocation )
			return true;

		for ( size_t index = 0;  index < factors.size( );  ++index )
			{
			if ( locations[ index ] < startOfDocLocation )
				locations[ index ] = factors[ index ]->seek( startOfDocLocation );
			if ( locations[ index ] == npos )
				return false;
			}
		}
	}

dex::constraintSolver::andISR::andISR( dex::vector< dex::constraintSolver::ISR * > factors,
		endOfDocumentISR *endOfDocISR ) : factors( factors ), endOfDocISR( endOfDocISR ), toGet( 0 )
	{
	locations.resize( factors.size( ) );
	for ( size_t index = 1;  index < factors.size( );  ++index )
		locations[ index ] = factors[ index ]->next( );

	endOfDocLocation = endOfDocISR->next( );
	}

dex::constraintSolver::andISR::~andISR( )
	{
	for ( size_t index = 0;  index != factors.size( );  ++index )
		delete factors[ index ];
	delete endOfDocISR;
	}

size_t dex::constraintSolver::andISR::seek( size_t target )
	{
	size_t max = 0;
	for ( size_t index = 0;  index < locations.size( );  ++index )
		{
		locations[ index ] = factors[ index ]->seek( target );
		if ( locations[ index ] < max )
			max = locations[index];
		}
	endOfDocLocation = endOfDocISR->seek( max );
	if ( align( ) )
		return toGet = endOfDocLocation;
	return toGet = npos;
	}

size_t dex::constraintSolver::andISR::next( )
	{
	return toGet = nextDocument( );
	}

size_t dex::constraintSolver::andISR::nextDocument( )
	{
	locations[ 0 ] = factors[ 0 ]->nextDocument( );
	if ( align( ) )
		return toGet = endOfDocLocation;
	return toGet = npos;
	}

size_t dex::constraintSolver::andISR::get( ) const
	{
	return toGet;
	}


dex::constraintSolver::orISR::orISR( dex::vector< dex::constraintSolver::ISR * > summands,
		endOfDocumentISR *endOfDocISR )
				: summands( summands ), endOfDocISR( endOfDocISR ), endOfDocLocation( 0 ), toGet( 0 )
	{
	locations.resize( summands.size( ) );
	for ( size_t index = 0;  index < summands.size( );  ++index )
		locations[ index ] = summands[ index ]->next( );
	}

dex::constraintSolver::orISR::~orISR( )
	{
	for ( size_t index = 0;  index != summands.size( );  ++index )
		delete summands[ index ];
	delete endOfDocISR;
	}

size_t dex::constraintSolver::orISR::seek( size_t target )
	{
	for ( size_t index = 0;  index < locations.size( );  ++index )
		locations[ index ] = summands[ index ]->seek( target );

	return toGet = endOfDocLocation = endOfDocISR->seek( locations[ argMin( locations ) ] );
	}

size_t dex::constraintSolver::orISR::next(  )
	{
	return toGet = nextDocument( );
	}

size_t dex::constraintSolver::orISR::nextDocument( )
	{
	size_t minIndex = argMin( locations );

	// We push all of the ISRs past the current endOfDoc location
	// Then return the new endOfDoc given our ISR locations

	while ( locations[ minIndex ] <= endOfDocLocation && locations[ minIndex ] != npos )
		{
		locations[ minIndex ] = summands[ minIndex ]->nextDocument( );
		minIndex = argMin( locations );
		}
	endOfDocLocation = endOfDocISR->seek( locations[ minIndex ] );
	return toGet = endOfDocLocation;
	}

size_t dex::constraintSolver::orISR::get( ) const
	{
	return toGet;
	}


dex::constraintSolver::notISR::notISR( dex::constraintSolver::ISR *neg, endOfDocumentISR *endOfDocISR )
		: neg( neg ), endOfDocISR( endOfDocISR ), toGet( 0 )
	{
	location = neg->next( );
	}

dex::constraintSolver::notISR::~notISR( )
	{
	delete neg;
	delete endOfDocISR;
	}

size_t dex::constraintSolver::notISR::seek( size_t target )
	{
	endOfDocLocation = endOfDocISR->seek( target );
	size_t startOfDoc = endOfDocLocation - endOfDocISR->documentSize( ) + 1;
	location = neg->seek( startOfDoc );
	if ( endOfDocLocation < location )
		return toGet = endOfDocLocation;

	return toGet = nextDocument( );
	}

size_t dex::constraintSolver::notISR::next( )
	{
	return toGet = nextDocument( );
	}

size_t dex::constraintSolver::notISR::nextDocument( )
	{
	while ( ( endOfDocLocation = endOfDocISR->next( ) ) > location )
		location = neg->seek( endOfDocLocation );
	return toGet = endOfDocLocation;
	}

size_t dex::constraintSolver::notISR::get( ) const
	{
	return toGet;
	}


bool dex::constraintSolver::phraseISR::align( )
	{
	while ( true )
		{
		bool inOrder = true;
		for ( size_t index = 1;  index < locations.size( );  ++index )
			if ( locations[ index ] - locations[ index - 1 ] != 1 )
				{
				inOrder = false;
				break;
				}

		if ( inOrder )
			return true;

		size_t maxIndex = argMax( locations );
		size_t maxLocation = locations[ maxIndex ];

		if ( maxLocation == npos )
			return false;

		for ( size_t index = 0;  index < locations.size( );  ++index )
			if ( locations[ index ] < maxLocation - maxIndex + index )
				locations[ index ] = words[ index ]->seek( maxLocation - maxIndex + index );
		}
	}

dex::constraintSolver::phraseISR::phraseISR( vector< dex::constraintSolver::ISR * > words,
		dex::constraintSolver::endOfDocumentISR *endOfDocISR ) : words( words ), endOfDocISR( endOfDocISR ), toGet( 0 )
	{
	locations.resize( words.size( ) );
	for ( size_t index = 1;  index < words.size( );  ++index )
		locations[ index ] = words[ index ]->next( );
	}

dex::constraintSolver::phraseISR::~phraseISR( )
	{
	for ( size_t index = 0;  index != words.size( );  ++index )
		delete words[ index ];
	delete endOfDocISR;
	}

size_t dex::constraintSolver::phraseISR::seek( size_t target )
	{
	size_t max = 0;
	for ( size_t index = 0;  index < locations.size( );  ++index )
		{
		locations[ index ] = words[ index ]->seek( target );
		if ( locations[ index ] < max )
			max = locations[index];
		}
	if ( align( ) )
		{
		size_t endOfDocLocation = endOfDocISR->seek( locations[ argMin( locations ) ] );
		return toGet = endOfDocLocation;
		}
	return toGet = npos;
	}

size_t dex::constraintSolver::phraseISR::next(  )
	{
	return toGet = nextDocument( );
	}

size_t dex::constraintSolver::phraseISR::nextDocument( )
	{
	locations[ 0 ] = words[ 0 ]->nextDocument( );
	if ( align( ) )
		return toGet = endOfDocISR->seek( locations[ 0 ] );
	return toGet = npos;
	}

size_t dex::constraintSolver::phraseISR::get( ) const
	{
	return toGet;
	}
