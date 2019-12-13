/*
 * parser.cpp
 *
 * Implementation of parser.h
 *
 * See parser.h for a full BNF of the grammar to implement
 *
 * Lab3: You should implement the different Find( ) functions,
 * as well as any additional functions you declare in parser.h
 */

#include "../constraintSolver/constraintSolver.hpp"
#include "expression.hpp"
#include "parserQC.hpp"

// Expression *Parser::FindPhrase( )
//    {
//    if ( stream.AllConsumed( ) )
//       return nullptr;
//    if ( stream.Match( '"' ) )
//       {
//       Expression *phrase; // ParsePhrase : 
//       phrase->
//       if ( stream.Match( '"' ) )
//          return phrase;
//       if ( phrase )
//          delete phrase;
//       return nullptr;
//       }
//    return stream.ParsePhrase( );
//    }

// Expression *Parser::FindFactor( )
//    {
//    if ( stream.AllConsumed( ) )
//       return nullptr;
//    if ( stream.Match( '(' ) )
//       {
//       Expression *add = FindOR( );
//       if ( stream.Match( ')' ) )
//          return add;
//       if ( add )
//          delete add;
//       return nullptr;
//       }
//    return stream.ParsePhrase( );
//    }

Expression *Parser::FindNot( )
   {
   size_t count;
   for ( count = 0;  stream.Match( '~' );  ++count );

   if ( count % 2 == 0 )
      return FindPhrase( );
   else
      {
      Expression *factor;// = FindPhrase( );
      if ( factor )
         return new NotExpression( factor );
      return nullptr;
      }
   }

Expression *Parser::FindOR( )
   {
   Expression *left = FindAND( );
   if ( left )
      {
      OrExpression *self = new OrExpression( );
      self->terms.pushBack( left );

      bool termAdded = true;
      while ( termAdded )
         {
         if ( stream.Match( '|' ) )
            {
            left = FindAND( );
            if ( !left )
               return nullptr;
            self->terms.pushBack( left );
            }
         else
            {
               termAdded = false;
            }
         }
      return self;
      }
   return nullptr;
   }

Expression *Parser::FindAND( )
   {
   Expression *left = FindNot( );
   if ( left )
      {
      AndExpression *self = new AndExpression( );
      self->terms.pushBack( left );
      bool termAdded = true;
      while ( termAdded )
         {
         if ( stream.Match( '&' ) )
            {
            left = FindNot( );
            if ( !left )
               return nullptr;
            self->terms.pushBack( left );        
            }
         else
            {
               termAdded = false;
            }
         }
      return self;
      }
   return nullptr;
   }

Expression *Parser::Parse( )
   {
   Expression *root = FindOR( );
   if ( root )
      if ( stream.AllConsumed( ) )
         return root;
      delete root;
   return nullptr;
   }

Parser::Parser( const dex::string &in ) : stream( in ) { }
