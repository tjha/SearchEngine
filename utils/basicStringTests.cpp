// basicStringTests.cpp
// Test the dex::basicString library
// 2019-10-7: Tested Constructor, Element Access, Assignment, Capacity, Compare: combsc, jasina

#include "basicString.hpp"
#include <iostream>
#include <cassert>

typedef dex::basicString < char > string;
using std::cout;
using std::endl;

void testConstructorElementAccess( );
void testAssignment( );
void testCapacity( );
void testCompare( );

int main( )
	{
	
	testConstructorElementAccess( );
	testAssignment( );
	testCapacity( );
	testCompare( );

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
	}

void testAssignment( )
	{
	string substance( "Substance" );
	string affection( "Affection" );
	affection = substance;
	substance = "True Substance";

	assert(affection == "Substance" );
	assert(substance == "True Substance" );
	}

void testCapacity( )
	{
	string thicc( "thicc" );
	assert( thicc.size( ) == 5 );
	assert( thicc.length( ) == 5 );
	assert( thicc.max_size( ) == string::npos );
	
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
	}
