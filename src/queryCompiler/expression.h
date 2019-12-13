/*
 * expression.h
 *
 * Class declarations for expressions
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <stdint.h>
#include <vector>

/**
 * Just a plain old expression
 */
class Expression
   {
   public:
      virtual ~Expression( );
      virtual int64_t Eval( ) const = 0;
      virtual ISR* getIndexISR( string word );
   };
// class Expression

/**
 * <Neg> ::= '-' <AddSub>
 */
class Not: public Expression
   {
   protected:
      Expression *value;
   public:
      Neg( Expression *value );
      ~Neg( );
      int64_t Eval( ) const override;
      ISR* getIndexISR( string word );
   };
// class AddSub

/**
 * <AddSub> ::= <MultDiv> { ‘+’ <MultDiv> }
 */
class Or: public Expression
   {
   protected:
      std::vector < Expression * > terms;
      std::vector < bool > positive;
      friend class Parser;
   public:
      Or( );
      ~Or( );
      int64_t Eval( ) const override;
		ISR* getIndexISR( string word );
   };
// class AddSub

/**
 * <MultDiv> ::= <FactorExpression> { ‘*’ <FactorExpression> }
 */
class MultDiv: public Expression
   {
   protected:
      std::vector < Expression * > terms;
      std::vector < bool > multiply;
      friend class Parser;
   public:
      MultDiv( );
      ~MultDiv( );
      int64_t Eval( ) const override;
   };
// class MultDiv

/**
 * A number
 */
class Number: public Expression
   {
   protected:
      int64_t value;
   public:
      Number( int64_t value );
      int64_t Eval( ) const override;
   };
// class Number

#endif /* EXPRESSION_H_ */
