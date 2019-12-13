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

#include "../constraintSolver/constraintSolver.h"
#include "expression.h"
#include "parser.h"

Expression *Parser::FindPhrase( )
   {
   if ( stream.AllConsumed( ) )
      return nullptr;
   if ( stream.Match( '"' ) )
      {
      Expression *add = FindOR( );  // NOT SURE ABOUT THIS LINE
      if ( stream.Match( '"' ) )
         return add;
      if ( add )
         delete add;
      return nullptr;
      }
   return stream.ParseNumber( );
   }

Expression *Parser::FindFactor( )
   {
   if ( stream.AllConsumed( ) )
      return nullptr;
   if ( stream.Match( '(' ) )
      {
      Expression *add = FindOR( );
      if ( stream.Match( ')' ) )
         return add;
      if ( add )
         delete add;
      return nullptr;
      }
   return stream.ParseNumber( );
   }

Expression *Parser::FindNot( )
   {
   size_t count;
   for ( count = 0;  stream.Match( '~' );  ++count );

   if ( count % 2 == 0 )
      return FindPhrase( );
   else
      {
      Expression *factor = FindPhrase( );
      if ( factor )
         return new Neg( factor );
      return nullptr;
      }
   }

Expression *Parser::FindOR( )
   {
   Expression *left = FindAND( );
   if ( left )
      {
      AddSub *self = new AddSub( );
      self->terms.push_back( left );
      self->positive.push_back( true );

      bool termAdded = true;
      while ( termAdded )
         {
         if ( stream.Match( '|' ) )
            {
            left = FindAND( );
            if ( !left )
               return nullptr;
            self->terms.push_back( left );
            self->positive.push_back( true );
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
      MultDiv *self = new MultDiv( );
      self->terms.push_back( left );
      self->multiply.push_back( true );

      bool termAdded = true;
      while ( termAdded )
         {
         if ( stream.Match( '&' ) )
            {
            left = FindNot( );
            if ( !left )
               return nullptr;
            self->terms.push_back( left );
            self->multiply.push_back( true );
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

Parser::Parser( const std::string &in ) : stream( in ) { }
