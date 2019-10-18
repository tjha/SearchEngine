#include <tls.h>
#include <unistd.h>
#include <iostream>
#include "robots.hpp"
#include <string.h>

using std::cout;
using std::endl;

int main () {
   std::string url = "https://domain.com";
   dex::RobotTxt hello = dex::RobotTxt( url, 3 );
   cout << hello;
   cout << hello.canVisitDomain( ) << endl;
   sleep(2);
   cout << hello.canVisitDomain( ) << endl;
   sleep(2);
   cout << hello.canVisitDomain( ) << endl;
   hello.updateLastVisited( );
   cout << hello;

}
