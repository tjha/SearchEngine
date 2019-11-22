// parser.hpp
// Provides functions to parse HTML content and deliver
// 
// 2019-11-22:  Added include guards, merged with changes by combsc that convert
//              links to vector of url types instead of string, modified
//              handling of relative links to always append '/' correctly: tjha
// 2019-11-21:  Modified BreakAnchors and RemovePunctuation to separate
//              words between punctuations except dashes: tjha
// 2019-11-20:  Modified constructor to take in format <url>\n<htnl_content>
//              Modified GetLinks function to correctly handle relative links:
//              tjha
// 2019-11-20:  Fixed cstdef to cstddef, modified size-t to std::size_t: tjha
// 2019-11-19:  Changed BreakAnchors, Getlinks; made removePunctuation: medhak
// 2019-11-11:  Implemented ParseTag function to aid in recursive parsing: tjha
// 2019-11-06:  Fixed whitespace errors, changed to paired to <size_t vector <size_t>>, : medhak
// 2019-11-05:  Implemented return functions, fixed errors in breakanchor.
// 2019-11-04:  Created parser class, some basic functions - constructor, BreakAnchor. Merged GetAnchorText and 
//              GetLinks into GetLinks - makes it easier to build anchorText vector : medhak
// 2019-11-04:  Added GetAnchorText function to get link anchor text: tjha
// 2019-11-04:  Fixed document style to match style guide: tjha
// 2019-10-26:  Created get_links function to get urls from basic html:
//              medhak, tjha

#ifndef DEX_HTML_PARSER
#define DEX_HTML_PARSER

#include "algorithm.hpp"
#include "basicString.hpp"
#include "exception.hpp"
#include "url.hpp"
#include "utility.hpp"
#include "vector.hpp"

#include <cstddef>

//#include <iostream>
//using std::cout;
//using std::endl;

namespace dex
{
   struct anchorPos
         {
         std::size_t linkInd;
         std::size_t startPos;
         std::size_t endPos;
         };

   class HTMLparser
   {
   private:
      dex::string htmlFile;
      dex::string pageLink;
      dex::vector< dex::Url > links;
      dex::vector< string > words;
      //dex::vector< string > relativeLinks;
      dex::vector< anchorPos > anchorText;

      void GetLinks( );

      struct Positions
         {
         std::size_t start;
         std::size_t end;
         };

      Positions ParseTag( Positions &pos, string &startTag, string &endTag )
         {
         Positions newPos;
         newPos.start = htmlFile.find( startTag.cStr( ), pos.start, pos.end - pos.start );
         if ( newPos.start == string::npos )
            {
            // incorporate some form of error logging
            std::cerr << "Error caught in ParseTag\n";
            throw dex::outOfRangeException();
            }
         newPos.end = htmlFile.find(endTag.cStr(), pos.start, pos.end - pos.start);
         if ( newPos.end == string::npos )
            {
            std::cerr << "Error caught in ParseTag\n";
            // incorporate some form of error logging
            throw dex::outOfRangeException();
            }
         newPos.end += endTag.length();
         return newPos;
         }

   public:
      HTMLparser( );
      HTMLparser( dex::string& html );
      string removePunctuation( string word );
      // static vector < string > BreakAnchorsOG ( const string anchor );
      void BreakAnchors ( string& anchor );
      // void GetAnchorText( );
      vector < dex::Url > ReturnLinks ( );
      // vector < dex::pair <size_t, size_t > > ReturnAnchorText ( );
      vector < anchorPos > ReturnAnchorText ( );
      vector < string > ReturnWords ( );
      
   };

   HTMLparser::HTMLparser( )
      {
      htmlFile = "";
      pageLink = "";
      }

   HTMLparser::HTMLparser( dex::string& html )
      {
      std::size_t linkEnd = html.findFirstOf( '\n' );
      pageLink = html.substr( 0, linkEnd );
      if ( pageLink.back( ) == '/' )
         {
         pageLink.popBack( );
         }
      htmlFile = html.substr( linkEnd + 1, html.length( ) - linkEnd - 1 );
      GetLinks( );
      }

   vector < dex::Url > HTMLparser::ReturnLinks ( )
      {
      return links;
      }
   
   vector < string > HTMLparser::ReturnWords ( )
      {
      return words;
      }
  
   vector < anchorPos > HTMLparser::ReturnAnchorText ( )
      {
      return anchorText;
      }
   
   void HTMLparser::BreakAnchors ( string& anchor )
      {
      static const char WHITESPACE[ ] = { ' ', '\t', '\n', '\r' };
      std::size_t indexNotOf = anchor.findFirstNotOf( WHITESPACE, 0, 4 );
      std::size_t indexOf = 0, start = indexNotOf;

      string word;
      while ( indexNotOf != dex::string::npos && indexOf != dex::string::npos )
         {
         indexOf = anchor.findFirstOf( WHITESPACE, indexNotOf, 4 );
         if ( indexOf != dex::string::npos )
            {
            word = anchor.substr( indexNotOf, indexOf - indexNotOf );
            indexNotOf = anchor.findFirstNotOf( WHITESPACE, indexOf, 4 );
            }
         else
            {
            word = anchor.substr( indexNotOf, anchor.length( ) - indexNotOf );
            indexNotOf = anchor.findFirstNotOf( WHITESPACE, indexNotOf + 1, 4 );
            }

            word = removePunctuation( word );
            std::size_t wordIdx = word.findFirstNotOf( ' ', 0 );
            std::size_t spaceIdx = word.find( ' ' );
            while ( spaceIdx != dex::string::npos && wordIdx != dex::string::npos )
               {
               if ( wordIdx < spaceIdx )
                  { 
                  words.pushBack( word.substr( wordIdx, spaceIdx - wordIdx ) );
                  wordIdx = word.findFirstNotOf ( ' ', spaceIdx );
                  }
               spaceIdx = word.find( ' ', spaceIdx + 1);
               }
            if ( wordIdx != dex::string::npos )
               {
               words.pushBack( word.substr( wordIdx, word.length() - wordIdx ) );
               }
            
         }
      if( indexNotOf == start )
         {
         words.pushBack( anchor );
         }
      }


   string HTMLparser::removePunctuation( string word )
      {
      static const char DELIMITERS[ ] = { '\n', '\t', '\r', ',', '.', '?', '>', 
                                          '<', '!', '[', ']', '{', '}', '|', 
                                          '\\', '_', '=', '+', ')', '(', '*', 
                                          '&', '^', '%', '$', '#', '@', '~',
                                          '`', '\'', '\'', ';', ':', '/'};

      // TODO: Improve implementation by avoiding 36 loops over word for each
      //       delimiter in array
      for( std::size_t i = 0; i < 34; i++ )
         {
         std::size_t ind = word.find( DELIMITERS[ i ] );
         while( ind != string::npos )
            {
            word = word.replace( ind , 1, " " );
            ind = word.find( DELIMITERS[ i ], ind);
            }
         }

      std::size_t ind = word.find( '-' );
      while( ind != string::npos )
         {
         word = word.erase( ind , 1 );
         ind = word.find( '-', ind);
         }

      return word;      
      }


   void HTMLparser::GetLinks( )
      {
      std::size_t posOpenTag = htmlFile.find( "<", 0 ), posCloseTag = 0;
      string url;
      string anchor;

      while ( posOpenTag != string::npos )
         {
         posCloseTag = htmlFile.find( ">", posOpenTag );
         if ( htmlFile[ posOpenTag + 1 ] == '!' && htmlFile[ posOpenTag + 2 ] == '-' 
               && htmlFile[ posOpenTag + 3 ] == '-' )
            {
            posCloseTag = htmlFile.find( "-->", posOpenTag ) + 3;
            }
         else
            {
            if ( posCloseTag == string::npos )
               {
               posCloseTag = posOpenTag + 1;
               if( posCloseTag >= htmlFile.length( ) )
                  {
                  posOpenTag = string::npos;
                  }
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            std::size_t posA = htmlFile.find( "a", posOpenTag );
            if ( posA >= posCloseTag || posA == string::npos )
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            std::size_t posHref = htmlFile.find( "href", posA );
            if ( posHref >= posCloseTag || posHref == string::npos )
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            std::size_t posEqual = htmlFile.find( "=", posHref );
            if ( posEqual == string::npos )
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            url = htmlFile.substr( posEqual + 1, posCloseTag-posEqual-1 );  
            std::size_t qPos = url.find( "\"" );
            if ( qPos == string::npos )
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            else
               {
               url = url.substr( qPos + 1, url.find( "\"", qPos + 1 ) - qPos - 1 );
               //std::cout << url << std::endl;
               }
            std::size_t linkIndex = 0;
            // PushBack absolute url
            if ( url.find("https://", 0) != string::npos ||
                 url.find("http://", 0) != string::npos ) {
               // url is already an absolute url
               links.pushBack( dex::Url( url.cStr( ) ) );     
            } else {
               // url is a relative url
               dex::string newLink = pageLink;
               if ( url.front( ) == '/' )
                  {
                  newLink += url;
                  }
               else
                  {
                  newLink += '/' + url;
                  }
               links.pushBack( dex::Url( newLink.cStr( ) ) );
            }
            linkIndex = links.size( ) - 1;

            //finding anchor text - - i think this should just be one function.
            posOpenTag = htmlFile.find ( "<", posCloseTag );
            if ( posOpenTag == string::npos )
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            if ( posOpenTag < htmlFile.length( ) - 2
                  && htmlFile[ posOpenTag + 1 ] == '/'
                  && htmlFile[ posOpenTag + 2 ] == 'a' )
               {
               anchor = htmlFile.substr( posCloseTag + 1, posOpenTag - posCloseTag - 1 );
               
               anchorPos anchorIndex;
               anchorIndex.linkInd = linkIndex;
               anchorIndex.startPos = words.size( );
               BreakAnchors( anchor );
               anchorIndex.endPos = words.size( ) - 1;
               anchorText.pushBack( anchorIndex );
               }
            }
         posOpenTag = htmlFile.find( "<", posCloseTag );   
         }
      }   

};
#endif // DEX_HTML_PARSER
