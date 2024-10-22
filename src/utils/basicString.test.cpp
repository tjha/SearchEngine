// basicStringTests.cpp
// Testing for the basicString class
//
// 2019-11-23: Add toLower: combsc
// 2019-11-20: Add replaceWhitespace and toString tests: combsc
// 2019-11-13: Add stripWhitespace tests: combsc
// 2019-10-27: Revamp swap tests: jasina
// 2019-10-20: Revamp assignment, write substring tests, combine (append, insert, erase, replace) into one, write hash
//             tests: combsc, jasina, lougheem
// 2019-10-17: Update file to contain full test suite. Dates of tests are noted below: combsc, jasina, lougheem
//   2019-10-14: Test replace, swap, popback: combsc, lougheem
//   2019-10-13: Test insert, erase, replace, find: jasina, lougheem
//   2019-10-8: Tested assign, append: combsc
//   2019-10-7: Tested Constructor, Element Access, Assignment, Capacity, Compare: combsc, jasina
// 2019-09-25: Created File: combsc

#include "catch.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"

using dex::outOfRangeException;
using dex::string;

// Need to do this since Catch 2 doesn't like string::npos inside of REQUIRE statements
auto npos = string::npos;

TEST_CASE( "constructors and element access work", "[string]" )
	{
	string str0;

	REQUIRE( str0 == "" );

	char other [4];
	other[0] = 'a';
	other[1] = 'b';
	other[2] = 'c';
	other[3] = '\0';
	string str1( other );

	REQUIRE( str1[0] == 'a' );
	REQUIRE( str1[1] == 'b' );
	REQUIRE( str1[2] == 'c' );
	REQUIRE( str1.at( 0 ) == 'a' );

	REQUIRE_THROWS_AS( str1.at( 4 ), dex::outOfRangeException );
	REQUIRE_THROWS_AS( str1.at( -1 ), dex::outOfRangeException );

	string str2(other, 2);
	REQUIRE( str2 == "ab" );
	string str3( str1 );
	REQUIRE( str3 == "abc" );

	string str4( str1, 1, 2 );
	REQUIRE( str4 == "bc" );


	string str5( 5, 'a' );
	REQUIRE( str5 == "aaaaa" );

	string str6( str3.cbegin( ), str3.cend( ) );
	REQUIRE( str6 == "abc" );
	string str7( str3.cbegin( ) + 1, str3.cend( ) - 1 );
	REQUIRE( str7 == "b" );
	string str8( "abcde" );
	}

TEST_CASE( "assignment works", "[string]" )
	{
	SECTION( "operator= works" )
		{
		string substance( "Substance" );
		string affection( "Affection" );
		affection = substance;
		substance = "True Substance";

		REQUIRE( affection == "Substance" );
		REQUIRE( substance == "True Substance" );

		string t( "a" );
		t = 'c';
		REQUIRE( t == "c");
		REQUIRE( t.size( ) == 1 );
		}

	SECTION( "assign works" )
		{
		string substance( "Substance" );
		string affection( "Affection" );
		affection.assign( substance );
		substance.assign( "True Substance" );

		REQUIRE( affection == "Substance" );
		REQUIRE( substance == "True Substance" );

		affection.assign(substance, 1, 4);
		REQUIRE( affection == "rue " );
		substance.assign( "True Substance stuff", 14);
		REQUIRE( substance == "True Substance" );

		affection.assign( substance.cbegin( ) + 1, substance.cend( ) - 1 );
		REQUIRE( affection == "rue Substanc" );

		affection.assign( 10, 'a' );
		REQUIRE( affection == "aaaaaaaaaa" );
		}
	}

TEST_CASE( "substring works", "[string]" )
	{
	string potato( "potato" );
	assert( potato.substr(0, 6) == potato );
	assert( potato.substr(0, 3) == "pot" );
	assert( potato.substr(4, 2) == "to" );
	assert( potato.substr(2, 3) == "tat" );
	assert( potato.substr(0, 0) == "" );
	assert( potato.substr(3, 0) == "" );
	assert( potato.substr(5, 0) == "" );
	}

TEST_CASE( "capacity works", "[string]" )
	{
	string thicc( "thicc" );
	REQUIRE( thicc.size( ) == 5 );
	REQUIRE( thicc.length( ) == 5 );
	REQUIRE( thicc.maxSize( ) == npos );

	thicc.reserve( 100 );
	REQUIRE( thicc.capacity( ) == 100 );
	thicc.shrinkToFit( );
	REQUIRE( thicc.capacity( ) == thicc.size( ) );
	thicc.resize( 3 );
	REQUIRE( thicc == "thi" );
	thicc.resize( 5, 'c' );
	REQUIRE( thicc == "thicc" );
	thicc.clear( );
	REQUIRE( thicc.size( ) == 0 );
	REQUIRE( thicc.empty( ) );
	}

TEST_CASE( "modifiers works", "[string]" )
	{
	SECTION( "append works" )
		{
		string a( "To Be " );
		string b( "Or Not To Be" );

		a.append( b );
		REQUIRE( a == "To Be Or Not To Be" );

		a = "To Be";
		b = "";
		a.append( b );
		REQUIRE( a == "To Be" );

		a = "abcde";
		b = "fghi";
		a.append( b, 1, 2);
		REQUIRE( a == "abcdegh" );

		a = "abc";
		a.append( "de" );
		REQUIRE( a == "abcde" );
		a.append( "fghi" , 2 );
		REQUIRE( a == "abcdefg" );
		a.append( 5, 'z' );
		REQUIRE( a == "abcdefgzzzzz" );

		a = "cat";
		b = "potato";
		a.append( b.cbegin( ), b.cend( ) );
		REQUIRE( a == "catpotato" );
		}

	SECTION( "operator+= works" )
		{
		string c( "con" );
		c += "cat";
		REQUIRE( c == "concat" );
		string other( "en" );
		c += other;
		REQUIRE( c == "concaten" );
		c += 'a';
		REQUIRE( c == "concatena" );
		string other2( "Propitiation" );
		c.append(other2, 8, 4);
		REQUIRE( c == "concatenation" );
		}

	SECTION( "insertion works" )
		{
		string az = "az";
		string to = " to ";
		az.insert( az.begin() + 1, to.begin(), to.end() );
		REQUIRE( az == "a to z" );

		string digits = "0123456789";
		string chars = az;
		chars.insert( 0, digits );
		REQUIRE( chars == "0123456789a to z" );

		chars = az;
		chars.insert( 2, digits, 2, 3 );
		REQUIRE( chars == "a 234to z" );

		chars = az;
		chars.insert( 5, "stuff to ");
		REQUIRE( chars == "a to stuff to z");

		chars = az;
		chars.insert( 5, "stuff to ", 9 );
		REQUIRE( chars == "a to stuff to z");

		chars = az;
		chars.insert( 6, 11, 'z' );
		REQUIRE( chars == "a to zzzzzzzzzzzz" );

		chars = az;
		chars.insert( chars.cend( ), 'w' );
		REQUIRE( chars == "a to zw" );
		}

	SECTION( "erase works" )
		{
		string digits = "0123456789";
		digits.erase( 7, 1 );
		REQUIRE( digits == "012345689" );

		digits = "0123456789";
		digits.erase( digits.cbegin( ) );
		REQUIRE( digits == "" );

		digits = "0123456789";
		digits.erase( digits.cbegin( ) + 1);
		REQUIRE( digits == "0" );

		digits = "0123456789";
		digits.erase( digits.cbegin( ), digits.cbegin( ) + 3 );
		REQUIRE( digits == "3456789" );

		digits.erase( digits.cbegin( ), digits.cend( ) );
		REQUIRE( digits == "" );
		}

	SECTION( "replace works" )
		{
		string digits = "0123456789";

		string emptyString = "";
		emptyString.replace( 0, 0, digits );
		REQUIRE( emptyString == digits );

		string letters = "abc";
		digits.replace( 1, 3, letters );
		REQUIRE( digits == "0abc456789" );

		digits = "0123456789";
		digits.replace( digits.cbegin( ) + 1, digits.cbegin( ) + 4, letters );
		REQUIRE( digits == "0abc456789");

		emptyString = "";
		digits = "0123456789";
		digits.replace( 0, 1, emptyString );
		REQUIRE( digits == "123456789" );

		emptyString.replace( emptyString.cbegin( ), emptyString.cend( ), digits );
		REQUIRE( emptyString == digits );

		digits = "0123456789";
		digits.replace( 1, 3, letters, 1, 2 );
		REQUIRE( digits == "0bc456789" );

		digits = "0123456789";
		digits.replace( 1, 3, "abc" );
		REQUIRE( digits == "0abc456789" );

		digits = "0123456789";
		digits.replace( digits.cbegin( ) + 1, digits.cbegin( ) + 4, "abc");
		REQUIRE( digits == "0abc456789" );

		digits = "0123456789";
		digits.replace( 1, 3, "abc", 2 );
		REQUIRE( digits == "0ab456789" );

		digits = "0123456789";
		digits.replace( 1, 3, 5, 'x' );
		REQUIRE( digits == "0xxxxx456789" );

		digits = "0123456789";
		digits.replace( digits.cbegin( ) + 1, digits.cbegin( ) + 4, 5, 'x' );
		REQUIRE( digits == "0xxxxx456789" );
		}

	SECTION( "popback works" )
		{
		string a( "popback" );
		a.popBack( );
		REQUIRE( a == "popbac" );
		REQUIRE( a.size( ) == 6 );

		string b( "b" );
		b.popBack( );
		REQUIRE( b == "" );
		REQUIRE( b.size( ) == 0 );
		}
	}

TEST_CASE( "find works", "[string]" )
	{
	string abracadabra = "abracadabra", abra = "abra", bra = "bra", cab = "cab", xyz = "xyz", empty = "";

	REQUIRE( abracadabra.find( abra ) == 0 );
	REQUIRE( abracadabra.find( abra, 1 ) == 7 );
	REQUIRE( abracadabra.find( abra, 6 ) == 7 );
	REQUIRE( abracadabra.find( abra, 7 ) == 7 );
	REQUIRE( abracadabra.find( abra, 8 ) == npos );
	REQUIRE( abracadabra.find( bra ) == 1 );
	REQUIRE( abracadabra.find( cab ) == npos );
	REQUIRE( abracadabra.find( empty ) == 0 );

	REQUIRE( abracadabra.find( "abra" ) == 0 );
	REQUIRE( abracadabra.find( "abra", 1 ) == 7 );
	REQUIRE( abracadabra.find( "abra", 6 ) == 7 );
	REQUIRE( abracadabra.find( "abra", 7 ) == 7 );
	REQUIRE( abracadabra.find( "abra", 8 ) == npos );
	REQUIRE( abracadabra.find( "bra" ) == 1 );
	REQUIRE( abracadabra.find( "cab" ) == npos );
	REQUIRE( abracadabra.find( "" ) == 0 );

	REQUIRE( abracadabra.find( "abra", 0, 0 ) == 0 );
	REQUIRE( abracadabra.find( "abra", 0, 3 ) == 0 );
	REQUIRE( abracadabra.find( "abra", 0, 4 ) == 0 );
	REQUIRE( abracadabra.find( "abra", 1, 0 ) == 1 );
	REQUIRE( abracadabra.find( "abra", 1, 1 ) == 3 );
	REQUIRE( abracadabra.find( "abra", 8, 4 ) == npos );
	REQUIRE( abracadabra.find( "abra", 8, 1 ) == 10 );
	REQUIRE( abracadabra.find( "cab", 0, 2 ) == 4 );
	REQUIRE( abracadabra.find( "cab", 3, 2 ) == 4 );
	REQUIRE( abracadabra.find( "cab", 4, 2 ) == 4 );
	REQUIRE( abracadabra.find( "cab", 5, 2 ) == npos );

	REQUIRE( abracadabra.find( 'a' ) == 0 );
	REQUIRE( abracadabra.find( 'a', 1 ) == 3 );
	REQUIRE( abracadabra.find( 'b' ) == 1 );
	REQUIRE( abracadabra.find( 'd' ) == 6 );
	REQUIRE( abracadabra.find( 'd', 5 ) == 6 );
	REQUIRE( abracadabra.find( 'd', 6 ) == 6 );
	REQUIRE( abracadabra.find( 'd', 7 ) == npos );

	REQUIRE( abracadabra.rfind( abra ) == 7 );
	REQUIRE( abracadabra.rfind( abra, 2 ) == 0 );
	REQUIRE( abracadabra.rfind( abra, 3 ) == 0 );
	REQUIRE( abracadabra.rfind( abra, 6 ) == 0 );
	REQUIRE( abracadabra.rfind( abra, 7 ) == 7 );
	REQUIRE( abracadabra.rfind( abra, 8 ) == 7 );
	REQUIRE( abracadabra.rfind( bra ) == 8 );
	REQUIRE( abracadabra.rfind( bra, 0 ) == npos );
	REQUIRE( abracadabra.rfind( bra, 1 ) == 1 );
	REQUIRE( abracadabra.rfind( cab ) == npos );
	REQUIRE( abracadabra.rfind( empty ) == 11 );

	REQUIRE( abracadabra.rfind( "abra" ) == 7 );
	REQUIRE( abracadabra.rfind( "abra", 2 ) == 0 );
	REQUIRE( abracadabra.rfind( "abra", 3 ) == 0 );
	REQUIRE( abracadabra.rfind( "abra", 6 ) == 0 );
	REQUIRE( abracadabra.rfind( "abra", 7 ) == 7 );
	REQUIRE( abracadabra.rfind( "abra", 8 ) == 7 );
	REQUIRE( abracadabra.rfind( "bra" ) == 8 );
	REQUIRE( abracadabra.rfind( "bra", 0 ) == npos );
	REQUIRE( abracadabra.rfind( "bra", 1 ) == 1 );
	REQUIRE( abracadabra.rfind( "cab" ) == npos );
	REQUIRE( abracadabra.rfind( "" ) == 11 );

	REQUIRE( abracadabra.rfind( "abra", 0, 4 ) == 0 );
	REQUIRE( abracadabra.rfind( "abra", 0, 0 ) == 0 );
	REQUIRE( abracadabra.rfind( "abra", 11, 4 ) == 7 );
	REQUIRE( abracadabra.rfind( "abra", 11, 0 ) == 11 );
	REQUIRE( abracadabra.rfind( "abra", npos, 2 ) == 7 );
	REQUIRE( abracadabra.rfind( "abra", 1, 2 ) == 0 );
	REQUIRE( abracadabra.rfind( "abra", 6, 2 ) == 0 );
	REQUIRE( abracadabra.rfind( "abra", 7, 2 ) == 7 );
	REQUIRE( abracadabra.rfind( "abra", 8, 2 ) == 7 );
	REQUIRE( abracadabra.rfind( "cab", npos, 2 ) == 4 );
	REQUIRE( abracadabra.rfind( "cab", 3, 2 ) == npos );

	REQUIRE( abracadabra.rfind( 'a', 10 ) == 10 );
	REQUIRE( abracadabra.rfind( 'r', 10 ) == 9 );
	REQUIRE( abracadabra.rfind( 'r', 9 ) == 9 );
	REQUIRE( abracadabra.rfind( 'r', 8 ) == 2 );
	REQUIRE( abracadabra.rfind( 'r', 1 ) == npos );

	REQUIRE( abracadabra.findFirstOf( cab ) == 0 );
	REQUIRE( abracadabra.findFirstOf( cab, 1 ) == 1 );
	REQUIRE( abracadabra.findFirstOf( cab, 2 ) == 3 );
	REQUIRE( abracadabra.findFirstOf( xyz ) == npos );

	REQUIRE( abracadabra.findFirstOf( "cab" ) == 0 );
	REQUIRE( abracadabra.findFirstOf( "cab", 0, 1 ) == 4 );
	REQUIRE( abracadabra.findFirstOf( "cab", 0, 2 ) == 0 );
	REQUIRE( abracadabra.findFirstOf( "cab", 1 ) == 1 );
	REQUIRE( abracadabra.findFirstOf( "cab", 1, 1 ) == 4 );
	REQUIRE( abracadabra.findFirstOf( "cab", 1, 2 ) == 3 );
	REQUIRE( abracadabra.findFirstOf( "xyz" ) == npos );
	REQUIRE( abracadabra.findFirstOf( "xyz", 1, 1 ) == npos );
	REQUIRE( abracadabra.findFirstOf( "xyz", 1, 0 ) == npos );
	REQUIRE( abracadabra.findFirstOf( "" ) == npos );

	REQUIRE( abracadabra.findFirstOf( 'a' ) == 0 );
	REQUIRE( abracadabra.findFirstOf( 'a', 1 ) == 3 );
	REQUIRE( abracadabra.findFirstOf( 'x' ) == npos );

	REQUIRE( abracadabra.findLastOf( cab ) == 10 );
	REQUIRE( abracadabra.findLastOf( cab, 1 ) == 1 );
	REQUIRE( abracadabra.findLastOf( xyz ) == npos );
	REQUIRE( abracadabra.findLastOf( xyz, 1 ) == npos );
	REQUIRE( abracadabra.findLastOf( empty ) == npos );

	REQUIRE( abracadabra.findLastOf( "cab" ) == 10 );
	REQUIRE( abracadabra.findLastOf( "cab", 1 ) == 1 );
	REQUIRE( abracadabra.findLastOf( "cab", 9, 1 ) == 4 );
	REQUIRE( abracadabra.findLastOf( "cab", npos, 2 ) == 10 );
	REQUIRE( abracadabra.findLastOf( "xyz" ) == npos );
	REQUIRE( abracadabra.findLastOf( "xyz", 7 ) == npos );
	REQUIRE( abracadabra.findLastOf( "xyz", 10, 1 ) == npos );
	REQUIRE( abracadabra.findLastOf( "xyz", 10, 2 ) == npos );
	REQUIRE( abracadabra.findLastOf( "" ) == npos );

	REQUIRE( abracadabra.findLastOf( 'a' ) == 10 );
	REQUIRE( abracadabra.findLastOf( 'a', 8 ) == 7 );
	REQUIRE( abracadabra.findLastOf( 'x' ) == npos );

	REQUIRE( abracadabra.findFirstNotOf( cab ) == 2);
	REQUIRE( abracadabra.findFirstNotOf( cab, 1 ) == 2);
	REQUIRE( abracadabra.findFirstNotOf( cab, 2 ) == 2);
	REQUIRE( abracadabra.findFirstNotOf( cab, 3 ) == 6);
	REQUIRE( abracadabra.findFirstNotOf( xyz ) == 0 );
	REQUIRE( abracadabra.findFirstNotOf( xyz, 1 ) == 1 );
	REQUIRE( abracadabra.findFirstNotOf( empty ) == 0 );

	REQUIRE( abracadabra.findFirstNotOf( "cab" ) == 2);
	REQUIRE( abracadabra.findFirstNotOf( "cab", 1 ) == 2);
	REQUIRE( abracadabra.findFirstNotOf( "cab", 1, 1 ) == 1);
	REQUIRE( abracadabra.findFirstNotOf( "cab", 1, 0 ) == 1);
	REQUIRE( abracadabra.findFirstNotOf( "cab", 2 ) == 2);
	REQUIRE( abracadabra.findFirstNotOf( "cab", 3 ) == 6);
	REQUIRE( abracadabra.findFirstNotOf( "cab", 3, 2 ) == 6);
	REQUIRE( abracadabra.findFirstNotOf( "xyz" ) == 0 );
	REQUIRE( abracadabra.findFirstNotOf( "xyz", 1 ) == 1 );
	REQUIRE( abracadabra.findFirstNotOf( "" ) == 0 );

	REQUIRE( abracadabra.findFirstNotOf( 'a' ) == 1 );
	REQUIRE( abracadabra.findFirstNotOf( 'v' ) == 0 );
	REQUIRE( abracadabra.findFirstNotOf( 'a', 1 ) == 1 );
	REQUIRE( abracadabra.findFirstNotOf( 'v', 1 ) == 1 );

	REQUIRE( abracadabra.findLastNotOf( cab ) == 9 );
	REQUIRE( abracadabra.findLastNotOf( cab, 1 ) == npos );
	REQUIRE( abracadabra.findLastNotOf( cab, 5 ) == 2 );
	REQUIRE( abracadabra.findLastNotOf( cab, 6 ) == 6 );
	REQUIRE( abracadabra.findLastNotOf( cab, 7 ) == 6 );
	REQUIRE( abracadabra.findLastNotOf( empty ) == 10 );

	REQUIRE( abracadabra.findLastNotOf( "cab" ) == 9 );
	REQUIRE( abracadabra.findLastNotOf( "cab", 1 ) == npos );
	REQUIRE( abracadabra.findLastNotOf( "cab", 1, 2 ) == 1 );
	REQUIRE( abracadabra.findLastNotOf( "cab", 1, 1 ) == 1 );
	REQUIRE( abracadabra.findLastNotOf( "cab", 5 ) == 2 );
	REQUIRE( abracadabra.findLastNotOf( "cab", 6 ) == 6 );
	REQUIRE( abracadabra.findLastNotOf( "cab", 7 ) == 6 );
	REQUIRE( abracadabra.findLastNotOf( "" ) == 10 );

	REQUIRE( abracadabra.findLastNotOf( 'a' ) == 9 );
	REQUIRE( abracadabra.findLastNotOf( 'v' ) == 10 );
	REQUIRE( abracadabra.findLastNotOf( 'a', 5 ) == 4 );
	REQUIRE( abracadabra.findLastNotOf( 'v', 5 ) == 5 );
	}


TEST_CASE( "comparisons work", "[string]" )
	{
	string cat( "cat" ), hat( "hat" ), cot( "cot" ), cab( "cab" ), cats( "cats" ), scat( "scat" ), scats( "scats" );
	REQUIRE( cat == cat );
	REQUIRE( !( cat != cat ) );
	REQUIRE( !( cat < cat ) );
	REQUIRE( !( cat > cat ) );
	REQUIRE( cat <= cat );
	REQUIRE( cat >= cat );

	REQUIRE( !( cat == hat ) );
	REQUIRE( cat != hat );
	REQUIRE( cat < hat );
	REQUIRE( !( cat > hat ) );
	REQUIRE( cat <= hat );
	REQUIRE( !( cat >= hat ) );
	REQUIRE( !( hat == cat ) );
	REQUIRE( hat != cat );
	REQUIRE( ! ( hat < cat ) );
	REQUIRE( hat > cat );
	REQUIRE( ! ( hat <= cat ) );
	REQUIRE( hat >= cat );

	REQUIRE( !( cat == cot ) );
	REQUIRE( cat != cot );
	REQUIRE( cat < cot );
	REQUIRE( !( cat > cot ) );
	REQUIRE( cat <= cot );
	REQUIRE( !( cat >= cot ) );
	REQUIRE( !( cot == cat ) );
	REQUIRE( cot != cat );
	REQUIRE( ! ( cot < cat ) );
	REQUIRE( cot > cat );
	REQUIRE( ! ( cot <= cat ) );
	REQUIRE( cot >= cat );

	REQUIRE( !( cab == cat ) );
	REQUIRE( cab != cat );
	REQUIRE( cab < cat );
	REQUIRE( !( cab > cat ) );
	REQUIRE( cab <= cat );
	REQUIRE( !( cab >= cat ) );
	REQUIRE( !( cat == cab ) );
	REQUIRE( cat != cab );
	REQUIRE( ! ( cat < cab ) );
	REQUIRE( cat > cab );
	REQUIRE( ! ( cat <= cab ) );
	REQUIRE( cat >= cab );

	REQUIRE( !( cat == cats ) );
	REQUIRE( cat != cats );
	REQUIRE( cat < cats );
	REQUIRE( !( cat > cats ) );
	REQUIRE( cat <= cats );
	REQUIRE( !( cat >= cats ) );
	REQUIRE( !( cats == cat ) );
	REQUIRE( cats != cat );
	REQUIRE( ! ( cats < cat ) );
	REQUIRE( cats > cat );
	REQUIRE( ! ( cats <= cat ) );
	REQUIRE( cats >= cat );

	REQUIRE( !( cat == scat ) );
	REQUIRE( cat != scat );
	REQUIRE( cat < scat );
	REQUIRE( !( cat > scat ) );
	REQUIRE( cat <= scat );
	REQUIRE( !( cat >= scat ) );
	REQUIRE( !( scat == cat ) );
	REQUIRE( scat != cat );
	REQUIRE( ! ( scat < cat ) );
	REQUIRE( scat > cat );
	REQUIRE( ! ( scat <= cat ) );
	REQUIRE( scat >= cat );

	REQUIRE( !( cat == scats ) );
	REQUIRE( cat != scats );
	REQUIRE( cat < scats );
	REQUIRE( !( cat > scats ) );
	REQUIRE( cat <= scats );
	REQUIRE( !( cat >= scats ) );
	REQUIRE( !( scats == cat ) );
	REQUIRE( scats != cat );
	REQUIRE( ! ( scats < cat ) );
	REQUIRE( scats > cat );
	REQUIRE( ! ( scats <= cat ) );
	REQUIRE( scats >= cat );

	REQUIRE( string( ) == string( ) );
	REQUIRE( string( ) < cat );
	REQUIRE( cat > string( ) );

	string cat2( cat );
	cat2.reserve( 100 );
	REQUIRE( cat == cat2 );

	REQUIRE( cat == "cat" );
	}

TEST_CASE( "swap works", "[string]" )
	{
	string a( "google" );
	string b( "microsoft" );

	a.swap( b );

	REQUIRE( a == "microsoft" );
	REQUIRE( a.size( ) == 9 );
	REQUIRE( b == "google" );
	REQUIRE( b.size( ) == 6 );

	dex::swap( a, b );
	REQUIRE( a == "google" );
	REQUIRE( a.size( ) == 6 );
	REQUIRE( b == "microsoft" );
	REQUIRE( b.size( ) == 9 );
	}

TEST_CASE( "hashing works", "[string]" )
	{
	// These are known to be collisions with the algorithm that we're using.
	string a = "costarring";
	string b = "liquid";
	dex::hash< string > hasher;
	REQUIRE( hasher( a ) == hasher( b ) );

	a = "declinate";
	b = "macallums";
	REQUIRE( hasher( a ) == hasher( b ) );

	// These should not be collisions.
	a = "potato";
	b = "tomato";
	REQUIRE( hasher( a ) != hasher( b ) );

	
	a = "dog";
	b = "dogs";
	REQUIRE( hasher( a ) != hasher( b ) );

	a = "";
	b = "Pokedex";
	REQUIRE( hasher( a ) != hasher( b ) );
	}

TEST_CASE( "+ operator", "[string]" )
	{
	
	string a = "concat";
	string b = a + "enate";
	REQUIRE( b == "concatenate");

	string c = "enate";
	string d = "concat" + c;
	REQUIRE( d == "concatenate" );

	string e = "concat";
	string f = "enate";
	string g = e + f;
	REQUIRE( g == "concatenate" );
	
	string h = "coolUserName";
	string i = h + 'X';
	REQUIRE( i == "coolUserNameX" );

	string j = 'X' + i;
	REQUIRE( j == "XcoolUserNameX" );
	}

TEST_CASE( "strip whitespace", "[string]" )
	{
	string a = "                   to strip              ";
	REQUIRE( a.stripWhitespace( ) == "to strip" );
	a = "to strip";
	REQUIRE( a.stripWhitespace( ) == "to strip" );
	a = "   to\nstrip  \r\n ";
	REQUIRE( a.stripWhitespace( ) == "to\nstrip" );
	a = " to strip      out ";
	REQUIRE( a.stripWhitespace( ) == "to strip      out" );
	a = "";
	REQUIRE( a.stripWhitespace( ) == "" );
	a = "     \t\v\n\f\n\r\n ";
	REQUIRE( a.stripWhitespace( ) == "" );
	a = "v";
	REQUIRE( a.stripWhitespace( ) == "v" );
	a = "f";
	REQUIRE( a.stripWhitespace( ) == "f" );
	a = "n";
	REQUIRE( a.stripWhitespace( ) == "n" );
	a = "t";
	REQUIRE( a.stripWhitespace( ) == "t" );
	a = "v";
	REQUIRE( a.stripWhitespace( ) == "v" );
	}

TEST_CASE( "replace whitespace", "[string]" )
	{
	string a = "                   to strip              ";
	REQUIRE( a.replaceWhitespace( ) == "tostrip" );
	a = "vfnt \v\f\n\t";
	REQUIRE( a.replaceWhitespace( ) == "vfnt" );
	a = "";
	REQUIRE( a.replaceWhitespace( ) == "" );
	a = " \v\f\n\t";
	REQUIRE( a.replaceWhitespace( ) == "" );
	a = "potato tomato\nswag";
	REQUIRE( a.replaceWhitespace( "_" ) == "potato_tomato_swag" );
	}

TEST_CASE( "toString", "[string]" )
	{
	int i = 10;
	REQUIRE( dex::toString( i ) == "10" );
	i = -10;
	REQUIRE( dex::toString( i ) == "-10" );
	i = 987;
	REQUIRE( dex::toString( i ) == "987" );
	i = 901;
	REQUIRE( dex::toString( i ) == "901" );
	i = 1234567890;
	REQUIRE( dex::toString( i ) == "1234567890" );
	long j = -1234567890000;
	REQUIRE( dex::toString( j ) == "-1234567890000" );
	i = -10;
	REQUIRE( dex::toString( i ) == "-10" );
	i = 0;
	REQUIRE( dex::toString( i ) == "0" );
	}

TEST_CASE( "toLower", "[string]" )
	{
	string a = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890!@#$%^&*(),./;'[]<>?:{}-=_+";
	REQUIRE( dex::toLower( a ) == "qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnm1234567890!@#$%^&*(),./;'[]<>?:{}-=_+");
	}
