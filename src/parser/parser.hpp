// parser.hpp
//
// Provides functions to parse HTML content and deliver 
//
// 2019-11-04:  Added GetAnchorText function to get link anchor text: tjha
// 2019-11-04:  Fixed document style to match style guide: tjha
// 2019-10-26:  Created get_links function to get urls from basic html:
//              medhak, tjha

#include "basicString.hpp"
#include "vector.hpp"
#include "algorithm.hpp"
#include "exception.hpp"

using dex::string;
using dex::vector;
using dex::exception;


vector< string > GetLinks( string htmlFile )
   {
   size_t posOpenTag = 0, posCloseTag = 0;
   string url;
   vector < string > urlList;
  
   while ( posOpenTag != string::npos )
      {
      posCloseTag = htmlFile.find( ">", posOpenTag );
      if ( htmlFile[ posOpenTag + 1 ] == '!')
         {
         posCloseTag = htmlFile.find( "-->", posOpenTag );
         posCloseTag += 2;
         }
      else
         {
         if ( posCloseTag != string::npos )
            {
            size_t posHref = htmlFile.find( "href", posOpenTag );
            if ( posHref < posCloseTag && posHref != string::npos )
               {
               if ( htmlFile.find("a", posOpenTag) < posHref )
                  {
                  size_t posTemp1 = htmlFile.find( "=", posHref );
                  if ( posTemp1 != string::npos )
                     {
                     url = htmlFile.substr( posTemp1 + 1, posCloseTag-posTemp1-1 );               
                     if ( url[0] == '\"' )
                        {
                        url = url.substr( 1, url.length()-2 );
                        urlList.pushBack( url );
                        }
                     }
                  
                  }
               }
            }
         }
      posOpenTag = htmlFile.find( "<", posCloseTag );   
      }
   return urlList;
   }

vector< string > GetAnchorText ( string htmlFile )
   {
   vector< string > anchorText;
   string anchor;
   size_t posOpenTag = 0, posCloseTag = 0;
    
   while ( posOpenTag != string::npos )
      {
      posCloseTag = htmlFile.find( ">", posOpenTag );
      if ( htmlFile[ posOpenTag + 1 ] == '!')
         {
         posCloseTag = htmlFile.find( "-->", posOpenTag );
         posCloseTag += 2;
         }
      else
         {
         if ( posCloseTag != string::npos )
            {
            size_t posHref = htmlFile.find( "href", posOpenTag );
            if ( posHref < posCloseTag && posHref != string::npos )
               {
               if ( htmlFile.find("a", posOpenTag) < posHref )
                  {
                  size_t posTemp1 = htmlFile.find( "=", posHref );
                  if ( posTemp1 != string::npos )
                     {
                     posOpenTag = htmlFile.find ( "<", posCloseTag );
                     if ( posOpenTag != string::npos )
                        {
                        if ( posOpenTag < htmlFile.length() - 2
                              && htmlFile[ posOpenTag + 1 ] == '/'
                              && htmlFile[ posOpenTag + 2 ] == 'a')
                           {
                           anchor = htmlFile.substr( posCloseTag + 1, posOpenTag - posCloseTag - 1 );
                           anchorText.pushBack( anchor );
                           }
                        }
                     }
                  }
               }
            }
         }
      posOpenTag = htmlFile.find( "<", posCloseTag );   
      }

   return anchorText;
   }
   
