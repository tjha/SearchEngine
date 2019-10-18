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
	testCanVisitDomain( );
   
}

void testCanVisitDomain( )
	{
	std::string url = "https://domain.com";
   dex::RobotTxt hello = dex::RobotTxt( url, 1 );
	hello.updatePathsAllowed( "/" );
	assert( !hello.canVisitDomain( ) );
   sleep(2);
	assert( hello.canVisitDomain( ) );

	dex::RobotTxt henlo = dex::RobotTxt( url, 1 );
	assert( !henlo.canVisitDomain( ) );
   sleep(2);
	assert( !henlo.canVisitDomain( ) );
	}

void testUpdate( )
	{
	
	}
