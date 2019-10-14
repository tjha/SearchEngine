// basicStringTests.cpp
// Test the dex::basicString library
// 2019-10-13: Test insert, erase, replace, find: jasina, lougheem
// 2019-10-8: Tested Assign, 
// 2019-10-7: Tested Constructor, Element Access, Assignment, Capacity, Compare: combsc, jasina

#include "basicString.hpp"
#include <iostream>
#include <cassert>

using dex::string;
using std::cout;
using std::endl;

void testConstructorElementAccess( );
void testAssignment( );
void testCapacity( );
void testCompare( );
void testAppend( );
void testInsert( );
void testErase( );
void testReplace( );
void testFind( );

int main( )
	{
	testConstructorElementAccess( );
	testAssignment( );
	testCapacity( );
	testCompare( );
	testAppend( );
	testInsert( );
	testErase( );
	testReplace( );
	testFind( );

	return 0;
	}

void testConstructorElementAccess( )
	{
	string str0;

	char other [4];
	other[0] = 'a';
	other[1] = 'b';
	other[2] = 'c';
	other[3] = '\0';
	string str1( other );
	
	assert( str1[0] == 'a' );
	assert( str1[1] == 'b' );
	assert( str1[2] == 'c' );
	assert( str1.at( 0 ) == 'a' );
	
	
	try
		{
		char a = str1.at( 4 );
		assert( false );
		cout << a << endl;
		}
	catch ( const dex::outOfRangeException& e ) { }
	
	try
		{
		char a = str1.at( -1 );
		assert( false );
		cout << a << endl;
		}
	catch ( const dex::outOfRangeException& e ) { }
	
	string str2(other, 2);
	assert( str2 == "ab" );
	string str3( str1 );
	assert( str3 == "abc" );
	
	string str4( str1, 1, 3 );
	assert( str4 == "bc" );

	
	string str5( 5, 'a' );
	assert( str5 == "aaaaa" );
	
	string str6( str3.cbegin( ), str3.cend( ) );
	assert( str6 == "abc" );
	string str7( str3.cbegin( ) + 1, str3.cend( ) - 1 );
	assert( str7 == "b" );
	string str8( "abcde" );

	cout << "Passed constructor and element access" << endl;
	}

void testAssignment( )
	{
	string substance( "Substance" );
	string affection( "Affection" );
	affection = substance;
	substance = "True Substance";

	assert(affection == "Substance" );
	assert(substance == "True Substance" );

	string t( "a" );
	t = 'c';
	assert( t == "c");
	assert( t.size( ) == 1 );

	cout << "Passed assignment" << endl;
	}

void testAppend( )
	{
	string c( "con" );
	assert( c == "con" );
	c += "cat";
	assert( c == "concat" );
	string other( "en" );
	c += other;
	assert( c == "concaten" );
	c += 'a';
	assert( c == "concatena" );
	string other2( "Propitiation" );
	c.append(other2, 8, 4);
	assert( c == "concatenation" );

	cout << "Passed append" << endl;
	}

void testCapacity( )
	{
	string thicc( "thicc" );
	assert( thicc.size( ) == 5 );
	assert( thicc.length( ) == 5 );
	assert( thicc.maxSize( ) == string::npos );
	
	thicc.reserve( 100 );
	assert( thicc.capacity( ) == 100 );
	thicc.shrinkToFit( );
	assert( thicc.capacity( ) == thicc.size( ) );
	thicc.resize( 3 );
	assert( thicc == "thi" );
	thicc.resize( 5, 'c' );
	assert( thicc == "thicc" );
	thicc.clear( );
	assert( thicc.size( ) == 0 );
	assert( thicc.empty( ) );

	cout << "Passed capacity" << endl;
	}

void testCompare( )
	{
	string cat( "cat" ), hat( "hat" ), cot( "cot" ), cab( "cab" ), cats( "cats" ), scat( "scat" ), scats( "scats" );
	assert( cat == cat );
	assert( !( cat != cat ) );
	assert( !( cat < cat ) );
	assert( !( cat > cat ) );
	assert( cat <= cat );
	assert( cat >= cat );
	
	assert( !( cat == hat ) );
	assert( cat != hat );
	assert( cat < hat );
	assert( !( cat > hat ) );
	assert( cat <= hat );
	assert( !( cat >= hat ) );
	assert( !( hat == cat ) );
	assert( hat != cat );
	assert( ! ( hat < cat ) );
	assert( hat > cat );
	assert( ! ( hat <= cat ) );
	assert( hat >= cat );

	assert( !( cat == cot ) );
	assert( cat != cot );
	assert( cat < cot );
	assert( !( cat > cot ) );
	assert( cat <= cot );
	assert( !( cat >= cot ) );
	assert( !( cot == cat ) );
	assert( cot != cat );
	assert( ! ( cot < cat ) );
	assert( cot > cat );
	assert( ! ( cot <= cat ) );
	assert( cot >= cat );

	assert( !( cab == cat ) );
	assert( cab != cat );
	assert( cab < cat );
	assert( !( cab > cat ) );
	assert( cab <= cat );
	assert( !( cab >= cat ) );
	assert( !( cat == cab ) );
	assert( cat != cab );
	assert( ! ( cat < cab ) );
	assert( cat > cab );
	assert( ! ( cat <= cab ) );
	assert( cat >= cab );

	assert( !( cat == cats ) );
	assert( cat != cats );
	assert( cat < cats );
	assert( !( cat > cats ) );
	assert( cat <= cats );
	assert( !( cat >= cats ) );
	assert( !( cats == cat ) );
	assert( cats != cat );
	assert( ! ( cats < cat ) );
	assert( cats > cat );
	assert( ! ( cats <= cat ) );
	assert( cats >= cat );

	assert( !( cat == scat ) );
	assert( cat != scat );
	assert( cat < scat );
	assert( !( cat > scat ) );
	assert( cat <= scat );
	assert( !( cat >= scat ) );
	assert( !( scat == cat ) );
	assert( scat != cat );
	assert( ! ( scat < cat ) );
	assert( scat > cat );
	assert( ! ( scat <= cat ) );
	assert( scat >= cat );

	assert( !( cat == scats ) );
	assert( cat != scats );
	assert( cat < scats );
	assert( !( cat > scats ) );
	assert( cat <= scats );
	assert( !( cat >= scats ) );
	assert( !( scats == cat ) );
	assert( scats != cat );
	assert( ! ( scats < cat ) );
	assert( scats > cat );
	assert( ! ( scats <= cat ) );
	assert( scats >= cat );

	assert( string( ) == string( ) );
	assert( string( ) < cat );
	assert( cat > string( ) );

	string cat2( cat );
	cat2.reserve( 100 );
	assert( cat == cat2 );

	assert( cat == "cat" );

	cout << "Passed compare" << endl;
	}

void testInsert( )
	{
	string az = "az";
	string to = " to ";
	az.insert(az.begin() + 1, to.begin(), to.end());
	assert( az == "a to z" );

	string digits = "0123456789";
	string chars = az;
	chars.insert( 0, digits );
	assert( chars == "0123456789a to z" );

	chars = az;
	chars.insert( 2, digits, 2, 3 );
	assert( chars == "a 234to z" );

	chars = az;
	chars.insert( 5, "stuff to ");
	assert( chars == "a to stuff to z");

	chars = az;
	chars.insert( 5, "stuff to ", 9 );
	assert( chars == "a to stuff to z");

	chars = az;
	chars.insert( 6, 11, 'z' );
	assert( chars == "a to zzzzzzzzzzzz" );

	chars = az;
	chars.insert( chars.cend( ), 'w' );
	assert( chars == "a to zw" );

	cout << "Passed insert" << endl;
	}

void testErase( ) 
	{
	string digits = "0123456789";
	digits.erase( 7, 1 );
	assert( digits == "012345689" );

	digits = "0123456789";
	digits.erase( digits.cbegin( ) );
	assert( digits == "" );
	
	digits = "0123456789";
	digits.erase( digits.cbegin( ) + 1);
	assert( digits == "0" );

	digits = "0123456789";
	digits.erase( digits.cbegin( ), digits.cbegin( ) + 3 );
	assert( digits == "3456789" );

	digits.erase( digits.cbegin( ), digits.cend( ) );
	assert( digits == "" );

	cout << "Passed erase" << endl;
	}

void testReplace( )
	{
	string digits = "0123456789";

	string emptyString = "";
	emptyString.replace( 0, 0, digits );
	assert( emptyString == digits );

	string letters = "abc";
	digits.replace( 1, 3, letters );
	assert( digits == "0abc456789" );

	digits = "0123456789";
	digits.replace( digits.cbegin( ) + 1, digits.cbegin( ) + 4, letters );
	assert( digits == "0abc456789");

	emptyString = "";
	digits = "0123456789";
	digits.replace( 0, 1, emptyString );
	assert( digits == "123456789" );
	
	emptyString.replace( emptyString.cbegin( ), emptyString.cend( ), digits );
	assert( emptyString == digits );

	digits = "0123456789";
	digits.replace( 1, 3, letters, 1, 2 );
	assert( digits == "0bc456789" );

	digits = "0123456789";
	digits.replace( 1, 3, "abc" );
	assert( digits == "0abc456789" );


	cout << "Passed replace" << endl;	
	}

void testFind( )
	{
	string abracadabra = "abracadabra", abra = "abra", bra = "bra", cab = "cab", xyz = "xyz", empty = "";

	assert( abracadabra.find( abra ) == 0 );
	assert( abracadabra.find( abra, 1 ) == 7 );
	assert( abracadabra.find( abra, 6 ) == 7 );
	assert( abracadabra.find( abra, 7 ) == 7 );
	assert( abracadabra.find( abra, 8 ) == string::npos );
	assert( abracadabra.find( bra ) == 1 );
	assert( abracadabra.find( cab ) == string::npos );
	assert( abracadabra.find( empty ) == 0 );

	assert( abracadabra.find( "abra" ) == 0 );
	assert( abracadabra.find( "abra", 1 ) == 7 );
	assert( abracadabra.find( "abra", 6 ) == 7 );
	assert( abracadabra.find( "abra", 7 ) == 7 );
	assert( abracadabra.find( "abra", 8 ) == string::npos );
	assert( abracadabra.find( "bra" ) == 1 );
	assert( abracadabra.find( "cab" ) == string::npos );
	assert( abracadabra.find( "" ) == 0 );

	assert( abracadabra.find( "abra", 0, 0 ) == 0 );
	assert( abracadabra.find( "abra", 0, 3 ) == 0 );
	assert( abracadabra.find( "abra", 0, 4 ) == 0 );
	assert( abracadabra.find( "abra", 1, 0 ) == 1 );
	assert( abracadabra.find( "abra", 1, 1 ) == 3 );
	assert( abracadabra.find( "abra", 8, 4 ) == string::npos );
	assert( abracadabra.find( "abra", 8, 1 ) == 10 );
	assert( abracadabra.find( "cab", 0, 2 ) == 4 );
	assert( abracadabra.find( "cab", 3, 2 ) == 4 );
	assert( abracadabra.find( "cab", 4, 2 ) == 4 );
	assert( abracadabra.find( "cab", 5, 2 ) == string::npos );

	assert( abracadabra.find( 'a' ) == 0 );
	assert( abracadabra.find( 'a', 1 ) == 3 );
	assert( abracadabra.find( 'b' ) == 1 );
	assert( abracadabra.find( 'd' ) == 6 );
	assert( abracadabra.find( 'd', 5 ) == 6 );
	assert( abracadabra.find( 'd', 6 ) == 6 );
	assert( abracadabra.find( 'd', 7 ) == string::npos );

	assert( abracadabra.rfind( abra ) == 7 );
	assert( abracadabra.rfind( abra, 2 ) == 0 );
	assert( abracadabra.rfind( abra, 3 ) == 0 );
	assert( abracadabra.rfind( abra, 6 ) == 0 );
	assert( abracadabra.rfind( abra, 7 ) == 7 );
	assert( abracadabra.rfind( abra, 8 ) == 7 );
	assert( abracadabra.rfind( bra ) == 8 );
	assert( abracadabra.rfind( bra, 0 ) == string::npos );
	assert( abracadabra.rfind( bra, 1 ) == 1 );
	assert( abracadabra.rfind( cab ) == string::npos );
	assert( abracadabra.rfind( empty ) == 11 );

	assert( abracadabra.rfind( "abra" ) == 7 );
	assert( abracadabra.rfind( "abra", 2 ) == 0 );
	assert( abracadabra.rfind( "abra", 3 ) == 0 );
	assert( abracadabra.rfind( "abra", 6 ) == 0 );
	assert( abracadabra.rfind( "abra", 7 ) == 7 );
	assert( abracadabra.rfind( "abra", 8 ) == 7 );
	assert( abracadabra.rfind( "bra" ) == 8 );
	assert( abracadabra.rfind( "bra", 0 ) == string::npos );
	assert( abracadabra.rfind( "bra", 1 ) == 1 );
	assert( abracadabra.rfind( "cab" ) == string::npos );
	assert( abracadabra.rfind( "" ) == 11 );

	assert( abracadabra.rfind( "abra", 0, 4 ) == 0 );
	assert( abracadabra.rfind( "abra", 0, 0 ) == 0 );
	assert( abracadabra.rfind( "abra", 11, 4 ) == 7 );
	assert( abracadabra.rfind( "abra", 11, 0 ) == 11 );
	assert( abracadabra.rfind( "abra", string::npos, 2 ) == 7 );
	assert( abracadabra.rfind( "abra", 1, 2 ) == 0 );
	assert( abracadabra.rfind( "abra", 6, 2 ) == 0 );
	assert( abracadabra.rfind( "abra", 7, 2 ) == 7 );
	assert( abracadabra.rfind( "abra", 8, 2 ) == 7 );
	assert( abracadabra.rfind( "cab", string::npos, 2 ) == 4 );
	assert( abracadabra.rfind( "cab", 3, 2 ) == string::npos );

	assert( abracadabra.rfind( 'a', 10 ) == 10 );
	assert( abracadabra.rfind( 'r', 10 ) == 9 );
	assert( abracadabra.rfind( 'r', 9 ) == 9 );
	assert( abracadabra.rfind( 'r', 8 ) == 2 );
	assert( abracadabra.rfind( 'r', 1 ) == string::npos );

	assert( abracadabra.findFirstOf( cab ) == 0 );
	assert( abracadabra.findFirstOf( cab, 1 ) == 1 );
	assert( abracadabra.findFirstOf( cab, 2 ) == 3 );
	assert( abracadabra.findFirstOf( xyz ) == string::npos );

	assert( abracadabra.findFirstOf( "cab" ) == 0 );
	assert( abracadabra.findFirstOf( "cab", 0, 1 ) == 4 );
	assert( abracadabra.findFirstOf( "cab", 0, 2 ) == 0 );
	assert( abracadabra.findFirstOf( "cab", 1 ) == 1 );
	assert( abracadabra.findFirstOf( "cab", 1, 1 ) == 4 );
	assert( abracadabra.findFirstOf( "cab", 1, 2 ) == 3 );
	assert( abracadabra.findFirstOf( "xyz" ) == string::npos );
	assert( abracadabra.findFirstOf( "xyz", 1, 1 ) == string::npos );
	assert( abracadabra.findFirstOf( "xyz", 1, 0 ) == string::npos );
	assert( abracadabra.findFirstOf( "" ) == string::npos );

	assert( abracadabra.findFirstOf( 'a' ) == 0 );
	assert( abracadabra.findFirstOf( 'a', 1 ) == 3 );
	assert( abracadabra.findFirstOf( 'x' ) == string::npos );

	assert( abracadabra.findLastOf( cab ) == 10 );
	assert( abracadabra.findLastOf( cab, 1 ) == 1 );
	assert( abracadabra.findLastOf( xyz ) == string::npos );
	assert( abracadabra.findLastOf( xyz, 1 ) == string::npos );
	assert( abracadabra.findLastOf( empty ) == string::npos );

	assert( abracadabra.findLastOf( "cab" ) == 10 );
	assert( abracadabra.findLastOf( "cab", 1 ) == 1 );
	assert( abracadabra.findLastOf( "cab", 9, 1 ) == 4 );
	assert( abracadabra.findLastOf( "cab", string::npos, 2 ) == 10 );
	assert( abracadabra.findLastOf( "xyz" ) == string::npos );
	assert( abracadabra.findLastOf( "xyz", 7 ) == string::npos );
	assert( abracadabra.findLastOf( "xyz", 10, 1 ) == string::npos );
	assert( abracadabra.findLastOf( "xyz", 10, 2 ) == string::npos );
	assert( abracadabra.findLastOf( "" ) == string::npos );

	assert( abracadabra.findLastOf( 'a' ) == 10 );
	assert( abracadabra.findLastOf( 'a', 8 ) == 7 );
	assert( abracadabra.findLastOf( 'x' ) == string::npos );

	assert( abracadabra.findFirstNotOf( cab ) == 2);
	assert( abracadabra.findFirstNotOf( cab, 1 ) == 2);
	assert( abracadabra.findFirstNotOf( cab, 2 ) == 2);
	assert( abracadabra.findFirstNotOf( cab, 3 ) == 6);
	assert( abracadabra.findFirstNotOf( xyz ) == 0 );
	assert( abracadabra.findFirstNotOf( xyz, 1 ) == 1 );
	assert( abracadabra.findFirstNotOf( empty ) == 0 );

	assert( abracadabra.findFirstNotOf( "cab" ) == 2);
	assert( abracadabra.findFirstNotOf( "cab", 1 ) == 2);
	assert( abracadabra.findFirstNotOf( "cab", 1, 1 ) == 1);
	assert( abracadabra.findFirstNotOf( "cab", 1, 0 ) == 1);
	assert( abracadabra.findFirstNotOf( "cab", 2 ) == 2);
	assert( abracadabra.findFirstNotOf( "cab", 3 ) == 6);
	assert( abracadabra.findFirstNotOf( "cab", 3, 2 ) == 6);
	assert( abracadabra.findFirstNotOf( "xyz" ) == 0 );
	assert( abracadabra.findFirstNotOf( "xyz", 1 ) == 1 );
	assert( abracadabra.findFirstNotOf( "" ) == 0 );

	assert( abracadabra.findFirstNotOf( 'a' ) == 1 );
	assert( abracadabra.findFirstNotOf( 'v' ) == 0 );
	assert( abracadabra.findFirstNotOf( 'a', 1 ) == 1 );
	assert( abracadabra.findFirstNotOf( 'v', 1 ) == 1 );

	assert( abracadabra.findLastNotOf( cab ) == 9 );
	assert( abracadabra.findLastNotOf( cab, 1 ) == string::npos );
	assert( abracadabra.findLastNotOf( cab, 5 ) == 2 );
	assert( abracadabra.findLastNotOf( cab, 6 ) == 6 );
	assert( abracadabra.findLastNotOf( cab, 7 ) == 6 );
	assert( abracadabra.findLastNotOf( empty ) == 10 );

	assert( abracadabra.findLastNotOf( "cab" ) == 9 );
	assert( abracadabra.findLastNotOf( "cab", 1 ) == string::npos );
	assert( abracadabra.findLastNotOf( "cab", 1, 2 ) == 1 );
	assert( abracadabra.findLastNotOf( "cab", 1, 1 ) == 1 );
	assert( abracadabra.findLastNotOf( "cab", 5 ) == 2 );
	assert( abracadabra.findLastNotOf( "cab", 6 ) == 6 );
	assert( abracadabra.findLastNotOf( "cab", 7 ) == 6 );
	assert( abracadabra.findLastNotOf( "" ) == 10 );

	assert( abracadabra.findLastNotOf( 'a' ) == 9 );
	assert( abracadabra.findLastNotOf( 'v' ) == 10 );
	assert( abracadabra.findLastNotOf( 'a', 5 ) == 4 );
	assert( abracadabra.findLastNotOf( 'v', 5 ) == 5 );

	cout << "Passed find" << endl;
	}
