#include <iostream>
#include <time.h>
#include <string.h>
#include <stdio.h>

using namespace std;

namespace dex
   {
   struct RobotTxt
      {
      public:
      RobotTxt( char* domain, int crawlDelay, int allowedVisitTime );

      // File stream input, output
      friend ostream &operator<<( ostream& out, RobotTxt &obj );
      friend istream &operator>>( istream& in, RobotTxt &rhs );

      // Call each time you HTTP request a website
      void interact( );

      // Checks for if you can perform HTTP request
      bool check();

      // working on / test functions
      bool yes();
      char * substringMe( char* ptr );

      private:
      char* domain;
      int crawlDelay;
      int allowedVisitTime;
      time_t lastTimeVisited; 

      };

   RobotTxt::RobotTxt ( char* dom, int del = 10, int time = 30 )
      {
      domain = dom;
      crawlDelay = del;
      allowedVisitTime = time;
      interact( );
      }

   void RobotTxt::interact( )
      {

      lastTimeVisited = time( 0 );
      }

   bool RobotTxt::check ( )
      {
      time_t currentTime = time( 0 );
      return ( difftime( currentTime, lastTimeVisited ) > crawlDelay ); 

      // WHAT ARE THE OTHER CHECKS WE NEED TO MAKE?
      // append to this
      }

   ostream & operator<<( ostream &out, RobotTxt &obj ) 
      {
      return out << "Domain:\t\t" << obj.domain << "\n" 
                 << "Crawl-Delay:\t" << obj.crawlDelay << "\n" 
                 << "Visit-Time:\t" << obj.allowedVisitTime << "\n"
                 << "Last-Visit:\t" << ctime( &obj.lastTimeVisited ) << "\r\n";
      }

   char* RobotTxt::substringMe( char* ptr )
      {
      char * end = strstr( ptr, "\n" );
      int length = end - ptr; 

      char * dest = new char[ length ];
      int index = 0;
      while ( ptr != end ) 
         {
         dest[ index ] = *ptr;
         ++ptr; 
         }
      return dest;
      }

   istream & operator >>( istream &in, RobotTxt &obj )
      {
      char * domain, delay, visitTime, lastVisit, end;
      in >> domain >> delay >> visitTime >> lastVisit >> end;
      char * str = "ello\n";

      /*
      bool odd = yes(); // don't know why i'm getting undeclared identifier here or next line
      */
      char * ess = substringMe( str ); //strstr( domain, "Domain\t\t" + 1 ) ); 
      
      
      obj.domain = str;
      obj.crawlDelay = 0;
      obj.allowedVisitTime = 0;
      obj.lastTimeVisited = time( 0 );

      return in;
      }

   };
