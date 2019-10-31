// robots.hpp
// class for respecting robots protocol

// 2019-10-31: Path sets defined size now, hash func, Copy constructors, 
//             operator=: jhirsh
// 2019-10-21: Improved definition of path: Chris
// 2019-10-17: File created: Jonas, Chris

#ifndef DEX_ROBOTS_HPP
#define DEX_ROBOTS_HPP

#include <iostream>
#include <time.h>
#include <stdio.h>
#include "../utils/basicString.hpp"
#include "../utils/vector.hpp"
#include "../utils/unorderedSet.hpp"
#include "../utils/algorithm.hpp"

using std::cerr;
using std::ostream;
using dex::unorderedSet;
using dex::vector;
using dex::string;


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
         //int visitTimeHourStart;
         //int visitTimeMinuteStart;
         //int visitTimeHourEnd;
         //int visitTimeMinuteEnd;

         // Last time this domain was visited, time that we're allowed to visit again
         time_t lastTimeVisited;
         time_t allowedVisitTime;

         // Paths that are disallowed. All extensions on the paths within this set are
         // also disallowed, unless explicitly allowed in allowedPaths.
         unorderedSet < string > disallowedPaths { 10 };
         // Paths that are exceptions to disallowed paths above. All extensions on the paths
         // within this set are also allowed.
         unorderedSet < string > allowedPaths { 10 };

         bool pathIsAllowed( string );
         string fixPath( const string &);
         

      public:
         static const unsigned defaultDelay = 10;

         RobotTxt( );
         RobotTxt( const RobotTxt &other );
         RobotTxt( const string &domain, unsigned crawlDelay);
         RobotTxt( const string &domain, const string &robotTxtFile );
         
         RobotTxt operator=( const RobotTxt &other );
         RobotTxt operator=( RobotTxt &&other );

         // File stream input, output
         friend ostream &operator<<( ostream& out, RobotTxt &obj );
         //friend istream &operator>>( istream& in, RobotTxt &rhs );

         // Call each time you HTTP request a website
         void updateLastVisited( );

         // Set the disallowed paths for the domain
         void setPathsDisallowed( const unorderedSet < string > & );

         // Add paths to the disallowed paths for the domain
         void addPathsDisallowed( const unorderedSet < string > & );
         void addPathsDisallowed( const vector < string > & );
         void addPathsDisallowed( const string & );

         // Set the allowed paths for the domain
         void setPathsAllowed( const unorderedSet < string > & );

         // Add paths to the allowed paths for the domain
         void addPathsAllowed( const unorderedSet < string > & );
         void addPathsAllowed( const vector < string > & );
         void addPathsAllowed( const string & );

         // Checks for if you can perform HTTP request
         bool canVisitPath( const string & );

         // need domain for hash func
         const string getDomain( ) const;
      };
   // A valid path has a '/' at the beginning and end.
   string RobotTxt::fixPath( const string &path )
      {
      string ret = path;
      if ( ret[ 0 ] != '/' )
         ret.insert( 0, "/" );
      if ( ret.back( ) != '/' )
         ret += "/";
      return ret;
      }
   bool RobotTxt::pathIsAllowed( string path )
      {
      path = fixPath( path );
      // if the path passed in is explicitly in disallowed paths, return false
      if ( disallowedPaths.count( path ) > 0 )
         return false;
         
      // if the path passed in is explicitly in allowed paths, return true
      if ( allowedPaths.count( path ) > 0 )
         return true;

      bool pathIsAllowed = true;
      // Parse the path to see if it is part of a disallowed path or allowed path
      for ( size_t nextSlashLocation = path.find( "/" );  nextSlashLocation != string::npos;
            nextSlashLocation = path.find( "/", nextSlashLocation + 1 ) )
         {
         string toCheck = path.substr(0, nextSlashLocation + 1 );

         // If at any point our path is in disallowed paths, we know that if it's not explicitly
         // allowed we need to return false
         if ( disallowedPaths.count( toCheck ) > 0 )
            pathIsAllowed = false;

         // If at any point our path is in allowed paths, we know that it's allowed and we can return true
         if ( allowedPaths.count( toCheck ) > 0 )
            return true;
         }

      return pathIsAllowed;
      }

   RobotTxt::RobotTxt ( )
      {
      }

   RobotTxt::RobotTxt ( const RobotTxt &other )
      {
      domain = other.domain;
      crawlDelay = other.crawlDelay;
      disallowedPaths = other.disallowedPaths;
      allowedPaths = other.allowedPaths;
      updateLastVisited( );
      }

   RobotTxt::RobotTxt ( const string &dom, unsigned del = defaultDelay)
         : domain( dom ), crawlDelay( del )
      {
      updateLastVisited( );
      }

   RobotTxt::RobotTxt ( const string &dom, const string &robotTxtFile )
      {
      // here is where the parsing of the actual file will take place
      std::cout << "Parsing Time" << std::endl;
      std::cout << robotTxtFile << std::endl;
      }

   RobotTxt RobotTxt::operator=( const RobotTxt &other )
      {
      RobotTxt otherCopy ( other );
      dex::swap( domain, otherCopy.domain );
      dex::swap( crawlDelay, otherCopy.crawlDelay );
      dex::swap( disallowedPaths, otherCopy.disallowedPaths );
      dex::swap( allowedPaths, otherCopy.allowedPaths );
      return *this;
      }

   RobotTxt RobotTxt::operator=( RobotTxt &&other )
      {
      dex::swap( domain, other.domain );
      dex::swap( crawlDelay, other.crawlDelay );
      dex::swap( disallowedPaths, other.disallowedPaths );
      dex::swap( allowedPaths, other.allowedPaths );
      return *this;
      }

   const string RobotTxt::getDomain ( ) const
      {
      return domain;
      }

   void RobotTxt::updateLastVisited( )
      {
      lastTimeVisited = time( nullptr );
      allowedVisitTime = lastTimeVisited + crawlDelay;
      }

   void RobotTxt::setPathsDisallowed( const unorderedSet < string > &disallowed )
      {
      disallowedPaths.clear( );
      addPathsDisallowed( disallowed );
      }
   
   void RobotTxt::addPathsDisallowed( const unorderedSet < string > &disallowed )
      {
      for ( auto it = disallowed.cbegin( );  it != disallowed.cend( );  ++it )
         disallowedPaths.insert( fixPath( *it ) );
      }

   void RobotTxt::addPathsDisallowed( const vector < string > &disallowed )
      {
      for ( auto it = disallowed.cbegin( );  it != disallowed.cend( );  ++it )
         disallowedPaths.insert( fixPath( *it ) );
      }
   
    void RobotTxt::addPathsDisallowed( const string &path )
      {
      disallowedPaths.insert( fixPath( path ) );
      }

   void RobotTxt::setPathsAllowed( const unorderedSet < string > &allowed )
      {
      allowedPaths.clear( );
      addPathsAllowed( allowed );
      }

   void RobotTxt::addPathsAllowed( const unorderedSet < string > &allowed )
      {
      for ( auto it = allowed.cbegin( );  it != allowed.cend( );  ++it )
         allowedPaths.insert( fixPath( *it ) );
      }

   void RobotTxt::addPathsAllowed( const vector < string > &allowed)
      {
      for ( auto it = allowed.cbegin( );  it != allowed.cend( );  ++it )
         allowedPaths.insert( fixPath( *it ) );
      }
   
    void RobotTxt::addPathsAllowed( const string &path )
      {
      allowedPaths.insert( fixPath( path ) );
      }
   
   bool RobotTxt::canVisitPath ( const string &path )
      {
      string fixedPath = fixPath( path );
      if ( !pathIsAllowed( fixedPath ) )
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

   template < class Key >
   struct hash;

   template< >
   struct hash< dex::RobotTxt >
      {
      unsigned long operator()( const dex::RobotTxt &robot ) const
         {
         return dex::hash< dex::string >{} ( robot.getDomain( ) );
         }
      };
   }

#endif
