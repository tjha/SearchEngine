#include <tls.h>
#include <unistd.h>
#include <iostream>
#include "robots.hpp"

using namespace std;
using namespace dex;

int main () {
   char *url = (char *)"https://domain.com";
   RobotTxt hello = RobotTxt( url );
   cout << hello;
   sleep(2);
   hello.interact();
   cout << hello;
}
