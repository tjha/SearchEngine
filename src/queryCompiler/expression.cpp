/*
 * expression.cpp
 *
 * Class implementations for expression functionality
 */

#include <cstddef>
#include "constraintSolver.hpp"
#include "expression.hpp"

using namespace std;

Expression::~Expression( ) { }

NotExpression::NotExpression( Expression *value ) : value( value ) { }

NotExpression::~NotExpression( )
	{
	delete value;
	}

dex::constraintSolver::ISR *NotExpression::eval( ) const
	{
	return dex::constraintSolver::notISR( value->eval( ), getENDOFDOCISRTOPASSIN );
	}

OrExpression::OrExpression( ) { }

OrExpression::~OrExpression( )
	{
	for ( Expression *expression : terms )
		delete expression;
	}

dex::constraintSolver::ISR * OrExpression::eval( ) const
	{
	int64_t sum = 0;
	vector < dex::constraintSolver::orISR * > ISRs;
	for ( size_t i = 0;  i != terms.size( );  ++i )
		if ( positive[ i ] )
			sum += terms[ i ]->Eval( );
		else
			sum -= terms[i]->Eval( );
	return sum;
	}

AndExpression::AndExpression( ) { }

AndExpression::~AndExpression( )
	{
	for ( Expression *expression : terms )
		delete expression;
	}

dex::constraintSolver::ISR * AndExpression::eval( ) const
	{
	int64_t product = 1;
	for ( size_t i = 0;  i != terms.size( );  ++i )
		if ( multiply[ i ] )
			product *= terms[ i ]->Eval( );
		else
			product /= terms[ i ]->Eval( );
	return product;
	}


	{
	return value;
	}
