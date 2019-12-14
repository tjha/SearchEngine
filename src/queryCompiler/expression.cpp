/*
 * expression.cpp
 *
 * Class implementations for expression functionality
 */

#include <cstddef>
#include "constraintSolver.hpp"
#include "expression.hpp"

// using namespace std;


Expression::~Expression( ) { }

NotExpression::NotExpression( Expression *value ) : value( value ) { }

NotExpression::~NotExpression( )
	{
	delete value;
	}

dex::constraintSolver::ISR *NotExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR;
	//Get that somehow ;;
	dex::constraintSolver::ISR * temp = value->eval( );
	return &dex::constraintSolver::notISR( temp, endDocISR );
	}

OrExpression::OrExpression( ) { }

OrExpression::~OrExpression( )
	{
	for ( dex::constraintSolver::ISR  *expression : terms )
		delete expression;
	}

dex::constraintSolver::ISR * OrExpression::eval( ) const
	{
	// dex::vector < dex::constraintSolver::ISR * > isrs;
	dex::constraintSolver::endOfDocumentISR *endDocISR;
	//somehow get the endofDocISRs
	dex::vector < dex::constraintSolver::ISR * > isrs;
	for ( size_t i =0; i < terms.size(); i++)
		{
		isrs.pushBack( terms[ i ]->eval() );
		}
	return new dex::constraintSolver::orISR( isrs, endOfDocISR );
	}

AndExpression::AndExpression( ) { }

AndExpression::~AndExpression( )
	{
	for ( Expression *expression : terms )
		delete expression;
	}

dex::constraintSolver::ISR * AndExpression::eval( ) const
	{
	dex::constraintSolver::endOfDocumentISR *endDocISR;
	dex::vector < dex::constraintSolver::ISR * > isrs;
	for ( size_t i =0; i < terms.size(); i++)
		{
		isrs.pushBack( terms[ i ]->eval( ) );
		}
	dex::constraintSolver::andISR retIsr( isrs, endOfDocISR );
	return &retIsr;
	}


	
