// robots.hpp
// class for respecting robots protocol

// 2019-10-17: File created: Jonas, Chris

// commit : changed char*s to strings, added allowed paths, changed name of interact to be
// more clear, moved default values to static variables in public.

#ifndef ROBOTS_HPP
#define ROBOTS_HPP

#include <iostream>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unordered_set>

using namespace std;

namespace dex
   {
   // using this link to understand the protocol: 
   // https://www.promptcloud.com/blog/how-to-read-and-respect-robots-file/
   struct RobotTxt
      {
      private:
         string domain;
         // Time we have to wait to hit domain again in seconds
         int crawlDelay;
         // To be used if there is a specified time when crawling is disallowed
         int visitTimeHourStart;
         int visitTimeMinuteStart;
         int visitTimeHourEnd;
         int visitTimeMinuteEnd;

         // Last time this domain was visited, time that we're allowed to visit again
         time_t lastTimeVisited;
         time_t allowedVisitTime;

         // Paths that are allowed. If "/"" is in this set, all paths are allowed.
         unordered_set < string > allowedPaths;

      public:
         static const unsigned defaultDelay = 10;

         RobotTxt( string domain, unsigned crawlDelay);
         

         // File stream input, output
         friend ostream &operator<<( ostream& out, RobotTxt &obj );
         friend istream &operator>>( istream& in, RobotTxt &rhs );

         // Call each time you HTTP request a website
         void updateLastVisited( );

         // Update the allowed paths for the domain
         void updatePathsAllowed( unordered_set < string > );
         void updatePathsAllowed( string );

         // Checks for if you can perform HTTP request
         bool canVisitDomain( string path );

         // working on / test functions
         bool yes();
         char * substringMe( char* ptr );

      

      };

   RobotTxt::RobotTxt ( string dom, unsigned del = defaultDelay)
      {
      domain = dom;
      crawlDelay = del;
      // QUESTION: Should we default allow all paths? Or no? Honestly probably not. Default should be
      // no paths allowed IMO, we should have to set the paths that ARE allowed.
      // allowedPaths.insert("/");
      updateLastVisited( );
      }

   void RobotTxt::updateLastVisited( )
      {
      lastTimeVisited = time( 0 );
      allowedVisitTime = lastTimeVisited + crawlDelay;
      }
      
   void RobotTxt::updatePathsAllowed( unordered_set < string > allowed )
      {
      allowedPaths = allowed;
      }
   
    void RobotTxt::updatePathsAllowed( string path )
      {
      allowedPaths.insert( path );
      }
   
   bool RobotTxt::canVisitDomain ( string path = "/" )
      {
      
      if ( allowedPaths.find( path ) == allowedPaths.end( ) && allowedPaths.find( "/" ) == allowedPaths.end( ) )
         {
         return false;
         }
      return time( 0 ) > allowedVisitTime; 
      }

   ostream & operator<<( ostream &out, RobotTxt &obj ) 
      {
      return out << "Domain:\t\t\t" << obj.domain << "\n" 
                 << "Crawl-Delay:\t\t" << obj.crawlDelay << "\n" 
                 << "Allowed-Visit-Time:\t" << obj.allowedVisitTime << "\n"
                 << "Last-Visit:\t\t" << ctime( &obj.lastTimeVisited ) << "\r\n";
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
      string domain, delay, visitTime, lastVisit, end;
      in >> domain >> delay >> visitTime >> lastVisit >> end;

      
      // bool odd = yes(); // don't know why i'm getting undeclared identifier here or next line
      // char * ess = substringMe( str ); //strstr( domain, "Domain\t\t" + 1 ) ); 
      
      
      obj.domain = domain;
      obj.crawlDelay = 0;
      obj.allowedVisitTime = 0;
      obj.lastTimeVisited = time( 0 );

      return in;
      }

   };

#endif