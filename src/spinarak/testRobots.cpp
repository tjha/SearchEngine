// testRobots.cpp
// file for testing robots struct

// 2019-10-17: File created, combsc

#include <tls.h>
#include <unistd.h>
#include <iostream>
#include "robots.hpp"
#include <string.h>
#include <assert.h>

using std::cout;
using std::endl;

void testCanVisitDomain( );
void testUpdate( );

int main () {
	//testCanVisitDomain( );
	testUpdate( );
   
}

void testCanVisitDomain( )
	{
	std::string url = "https://domain.com";
   dex::RobotTxt hello = dex::RobotTxt( url, 1 );
	assert( !hello.canVisitPath( ) );
   sleep(2);
	assert( hello.canVisitPath( ) );

	dex::RobotTxt henlo = dex::RobotTxt( url, 1 );
	henlo.addPathsDisallowed( "/" );
	assert( !henlo.canVisitPath( ) );
   sleep(2);
	assert( !henlo.canVisitPath( ) );
	}

void testUpdate( )
	{
	std::string url = "https://domain.com";
   dex::RobotTxt hello = dex::RobotTxt( url, 0 );
	sleep(1);
	hello.addPathsDisallowed( "/" );
	hello.addPathsAllowed( "/secret/path/" );
	assert( !hello.canVisitPath( "/some/other/path/" ) );
	assert( !hello.canVisitPath( "/" ) );
	assert( hello.canVisitPath( "/secret/path/" ) );
	assert( hello.canVisitPath( "/secret/path/somefile/" ) );
	}