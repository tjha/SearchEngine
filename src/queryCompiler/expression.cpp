/*
 * expression.cpp
 *
 * Class implementations for expression functionality
 */

#include <cstddef>
#include <vector>
#include "expression.h"

using namespace std;

Expression::~Expression( ) { }

Not::Not( Expression *value ) : value( value ) { }

Neg::~Not( )
   {
   delete value;
   }

int64_t Not::Eval( ) const
   {
   return -value->Eval( );
   }

Or::Or( ) { }

Or::~Or( )
   {
   for ( Expression *expression : terms )
      delete expression;
   }

int64_t Or::Eval( ) const
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

MultDiv::MultDiv( ) { }

MultDiv::~MultDiv( )
   {
   for ( Expression *expression : terms )
      delete expression;
   }

int64_t MultDiv::Eval( ) const
   {
   int64_t product = 1;
   for ( size_t i = 0;  i != terms.size( );  ++i )
      if ( multiply[ i ] )
         product *= terms[ i ]->Eval( );
      else
         product /= terms[ i ]->Eval( );
   return product;
   }

Number::Number( int64_t value ) : value( value ) { }

int64_t Number::Eval( ) const
   {
   return value;
   }
