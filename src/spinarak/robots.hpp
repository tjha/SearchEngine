// robots.hpp
// class for respecting robots protocol

// 2019-10-21: Improved definition of path: Chris
// 2019-10-17: File created: Jonas, Chris

#ifndef DEX_ROBOTS_HPP
#define DEX_ROBOTS_HPP

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
         // TODO: To be used if there is a specified time when crawling is disallowed
         int visitTimeHourStart;
         int visitTimeMinuteStart;
         int visitTimeHourEnd;
         int visitTimeMinuteEnd;

         // Last time this domain was visited, time that we're allowed to visit again
         time_t lastTimeVisited;
         time_t allowedVisitTime;

         // Paths that are disallowed. All extensions on the paths within this set are
         // also disallowed, unless explicitly allowed in allowedPaths.
         unordered_set < string > disallowedPaths;
         // Paths that are exceptions to disallowed paths above. All extensions on the paths
         // within this set are also allowed.
         unordered_set < string > allowedPaths;

         bool pathIsAllowed( string );
         string fixPath( const string &);
         

      public:
         static const unsigned defaultDelay = 10;

         RobotTxt( string domain, unsigned crawlDelay);
         

         // File stream input, output
         friend ostream &operator<<( ostream& out, RobotTxt &obj );
         friend istream &operator>>( istream& in, RobotTxt &rhs );

         // Call each time you HTTP request a website
         void updateLastVisited( );

         // Set the disallowed paths for the domain
         void setPathsDisallowed( unordered_set < string > );

         // Add paths to the disallowed paths for the domain
         void addPathsDisallowed( unordered_set < string > );
         void addPathsDisallowed( vector < string > );
         void addPathsDisallowed( string );

         // Set the allowed paths for the domain
         void setPathsAllowed( unordered_set < string > );

         // Add paths to the allowed paths for the domain
         void addPathsAllowed( unordered_set < string > );
         void addPathsAllowed( vector < string > );
         void addPathsAllowed( string );

         // Checks for if you can perform HTTP request
         bool canVisitPath( string path );
      };
   // A valid path has a '/' at the beginning and end.
   string RobotTxt::fixPath( const string &path )
      {
      string ret = path;
      if ( ret[ 0 ] != '/' )
         ret.insert( 0, "/" );
      if ( ret[ ret.size( ) - 1 ] != '/' )
         ret += "/";
      return ret;
      }
   bool RobotTxt::pathIsAllowed( string path )
      {
      path = fixPath( path );
      // if the path passed in is explicitly in disallowed paths, return false
      if ( disallowedPaths.find( path ) != disallowedPaths.end( ) )
         return false;
         
      // if the path passed in is explicitly in allowed paths, return true
      if ( allowedPaths.find( path ) != allowedPaths.end( ) )
         return true;

      // Parse the path to see if it is part of a disallowed path or allowed path
      size_t nextSlashLocation = path.find( "/" );
      bool ret = true;
      while ( nextSlashLocation != string::npos )
         {
         string toCheck = path.substr(0, nextSlashLocation + 1 );

         // If at any point our path is in disallowed paths, we know that if it's not explicitly
         // allowed we need to return false
         if ( disallowedPaths.find( toCheck ) != disallowedPaths.end( ) )
            ret = false;

         // If at any point our path is in allowed paths, we know that it's allowed and we can return true
         if ( allowedPaths.find( toCheck ) != allowedPaths.end( ) )
            return true;

         nextSlashLocation = path.find( "/", nextSlashLocation + 1 );
         }


      return ret;
      }

   RobotTxt::RobotTxt ( string dom, unsigned del = defaultDelay)
      {
      domain = dom;
      crawlDelay = del;
      updateLastVisited( );
      }

   void RobotTxt::updateLastVisited( )
      {
      lastTimeVisited = time( 0 );
      allowedVisitTime = lastTimeVisited + crawlDelay;
      }

   void RobotTxt::setPathsDisallowed( unordered_set < string > disallowed )
      {
      disallowedPaths.clear( );
      for ( auto it = disallowed.cbegin( );  it != disallowed.cend( );  ++it )
         disallowedPaths.insert( fixPath( *it ) );
      }
   
   void RobotTxt::addPathsDisallowed( unordered_set < string > disallowed )
      {
      for ( auto it = disallowed.cbegin( );  it != disallowed.cend( );  ++it )
         disallowedPaths.insert( fixPath( *it ) );
      }

   void RobotTxt::addPathsDisallowed( vector < string > disallowed )
      {
      for ( auto it = disallowed.cbegin( );  it != disallowed.cend( );  ++it )
         disallowedPaths.insert( fixPath( *it ) );
      }
   
    void RobotTxt::addPathsDisallowed( string path )
      {
      disallowedPaths.insert( fixPath( path ) );
      }

   void RobotTxt::setPathsAllowed( unordered_set < string > allowed )
      {
      allowedPaths.clear( );
      for ( auto it = allowed.cbegin( );  it != allowed.cend( );  ++it )
         allowedPaths.insert( fixPath( *it ) );
      }

   void RobotTxt::addPathsAllowed( unordered_set < string > allowed )
      {
      for ( auto it = allowed.cbegin( );  it != allowed.cend( );  ++it )
         allowedPaths.insert( fixPath( *it ) );
      }

   void RobotTxt::addPathsAllowed( vector < string > allowed)
      {
      for ( auto it = allowed.cbegin( );  it != allowed.cend( );  ++it )
         allowedPaths.insert( fixPath( *it ) );
      }
   
    void RobotTxt::addPathsAllowed( string path )
      {
      allowedPaths.insert( fixPath( path ) );
      }
   
   bool RobotTxt::canVisitPath ( string path )
      {
      path = fixPath( path );
      if ( !pathIsAllowed( path ) )
         return false;
      
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
   }

#endif