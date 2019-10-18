// robots.hpp
// class for respecting robots protocol

// 2019-10-17: File created: Jonas, Chris

#ifndef ROBOTS_HPP
#define ROBOTS_HPP

#include <iostream>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <vector>
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

         // Paths that are disallowed. If "/"" is in this set, all paths are disallowed.
         unordered_set < string > disallowedPaths;
         // Paths that are exceptions to disallowed paths above. 
         unordered_set < string > allowedPaths;
         

      public:
         static const unsigned defaultDelay = 10;

         RobotTxt( string domain, unsigned crawlDelay);
         

         // File stream input, output
         friend ostream &operator<<( ostream& out, RobotTxt &obj );
         friend istream &operator>>( istream& in, RobotTxt &rhs );

         // Call each time you HTTP request a website
         void updateLastVisited( );

         // Update the disallowed paths for the domain
         void setPathsDisallowed( unordered_set < string > );
         void addPathsDisallowed( unordered_set < string > );
         void addPathsDisallowed( vector < string > );
         void addPathsDisallowed( string );

         // Update the allowed paths for the domain
         void setPathsAllowed( unordered_set < string > );
         void addPathsAllowed( unordered_set < string > );
         void addPathsAllowed( vector < string > );
         void addPathsAllowed( string );

         // Checks for if you can perform HTTP request
         bool canVisitPath( string path );

         // working on / test functions
         bool yes();
         char * substringMe( char* ptr );

      

      };

   RobotTxt::RobotTxt ( string dom, unsigned del = defaultDelay)
      {
      domain = dom;
      crawlDelay = del;
      // QUESTION: Should we default allow all paths? That seems to be the assumption.
      updateLastVisited( );
      }

   void RobotTxt::updateLastVisited( )
      {
      lastTimeVisited = time( 0 );
      allowedVisitTime = lastTimeVisited + crawlDelay;
      }

   void RobotTxt::setPathsDisallowed( unordered_set < string > disallowed )
      {
      disallowedPaths = disallowed;
      }
   
   void RobotTxt::addPathsDisallowed( unordered_set < string > disallowed )
      {
      for ( auto it = disallowed.begin( );  it != disallowed.end( );  ++it )
         {
         disallowedPaths.insert( *it );
         }
      }

   void RobotTxt::addPathsDisallowed( vector < string > disallowed )
      {
      for ( int i = 0;  i < disallowed.size( );  ++i )
         {
         disallowedPaths.insert( disallowed[ i ] );
         }
      }
   
    void RobotTxt::addPathsDisallowed( string path )
      {
      disallowedPaths.insert( path );
      }

   void RobotTxt::setPathsAllowed( unordered_set < string > allowed )
      {
      allowedPaths = allowed;
      }

   void RobotTxt::addPathsAllowed( unordered_set < string > allowed )
      {
      for ( auto it = allowed.begin( );  it != allowed.end( );  ++it )
         {
         allowedPaths.insert( *it );
         }
      }

   void RobotTxt::addPathsAllowed( vector < string > allowed)
      {
      for ( int i = 0;  i < allowed.size( );  i++ )
         {
         allowedPaths.insert( allowed[ i ] );
         }
      }
   
    void RobotTxt::addPathsAllowed( string path )
      {
      allowedPaths.insert( path );
      }
   
   bool RobotTxt::canVisitPath ( string path = "/" )
      {
      
      if ( ( disallowedPaths.find( path ) != disallowedPaths.end( ) || 
            disallowedPaths.find( "/" ) != disallowedPaths.end( ) )
            && allowedPaths.find( path ) == allowedPaths.end( ) )
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
   };

   

#endif