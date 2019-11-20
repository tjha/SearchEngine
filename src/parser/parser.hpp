// parser.hpp
// Provides functions to parse HTML content and deliver
//
// 2019-11-19:  Changed BreakAnchors, Getlinks; made removePunctuation : medhak
// 2019-11-11:  Implemented ParseTag function to aid in recursive parsing: tjha
// 2019-11-06:  Fixed whitespace errors, changed to paired to <size_t vector <size_t>>, : medhak
// 2019-11-05:  Implemented return functions, fixed errors in breakanchor.
// 2019-11-04:  Created parser class, some basic functions - constructor, BreakAnchor. Merged GetAnchorText and 
//              GetLinks into GetLinks - makes it easier to build anchorText vector : medhak
// 2019-11-04:  Added GetAnchorText function to get link anchor text: tjha
// 2019-11-04:  Fixed document style to match style guide: tjha
// 2019-10-26:  Created get_links function to get urls from basic html:
//              medhak, tjha


#include "basicString.hpp"
#include "vector.hpp"
#include "algorithm.hpp"
#include "exception.hpp"
#include "utility.hpp"
#include <cstdef> 


namespace dex
{
   struct anchorPos
         {
         size_t linkInd;
         size_t startPos;
         size_t endPos;
         };

   class HTMLparser
   {
   private:
      string htmlFile;
      vector < string > links;
      vector < string > words;
      vector < string > relativeLinks;
      vector< anchorPos > anchorText;

      void GetLinks( );

      struct Positions
         {
         size_t start;
         size_t end;
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
      HTMLparser( string &html );
      string removePunctuation( string word );
      // static vector < string > BreakAnchorsOG ( const string anchor );
      vector < string > BreakAnchors ( string anchor );
      // void GetAnchorText( );
      vector < string > ReturnLinks ( );
      // vector < dex::pair <size_t, size_t > > ReturnAnchorText ( );
      vector < anchorPos > ReturnAnchorText ( );
      vector < string > ReturnWords ( );
      
   };

   HTMLparser::HTMLparser( )
      {
      htmlFile = ""; 
      }

   HTMLparser::HTMLparser( string &html )
      {
      htmlFile = html;
      GetLinks();
      }

   vector < string > HTMLparser::ReturnLinks ( )
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
   
   vector < string > HTMLparser::BreakAnchors ( string anchor )
      {
      static const char WHITESPACE [ ] = { ' ', '\t', '\n', '\r' };
      size_t indexNotOf = anchor.findFirstNotOf( WHITESPACE, 0, 4 ), indexOf = 0, start = indexNotOf;
      vector < string > output;
      string word;
      while ( indexNotOf != string::npos && indexOf != string::npos )
         {
         indexOf = anchor.findFirstOf( WHITESPACE, indexNotOf, 4 );
         if ( indexOf != string::npos )
            {
            word = anchor.substr( indexNotOf, indexOf - indexNotOf + 1 );
            word = removePunctuation( word );
            output.pushBack( word );
            indexNotOf = anchor.findFirstNotOf( WHITESPACE, indexOf, 4 );
            }
         else
            {
            word = anchor.substr( indexNotOf, anchor.length( ) - indexNotOf + 1 );
            word = removePunctuation( word );
            output.pushBack( word );
            indexNotOf = anchor.findFirstNotOf( WHITESPACE, indexNotOf + 1, 4 );
            
            }
         }
      if( indexNotOf == start )
         {
         output.pushBack( anchor );
         }
      return output;  
      }


   string HTMLparser::removePunctuation( string word )
      {
      static const char DELIMITERS [ ] = { '\n', '\t', '\r', ' ', ',', '.', '?', '>', '<', '!', '[', ']',
                                           '{', '}', '|', '\\', '-', '_', '=', '+', ')', '(', '*', '&', 
                                           '^', '%', '$', '#', '@', '~', '`', '\'', '\'', ';', ':', '/' };
      for( size_t i = 0; i < 36; i++ )
         {
         size_t ind = word.find( DELIMITERS[ i ] );
         while( ind != string::npos )
            {
            word = word.replace( ind , 1, "" );
            ind = word.find( DELIMITERS[ i ], ind );
            }
         } 
      return word;      
      }


   // Maybe we can use continue's to avoid the nested loops? Needs to be tested 
   void HTMLparser::GetLinks( )
      {
      size_t posOpenTag = htmlFile.find( "<", 0 ), posCloseTag = 0;
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
            size_t posHref = htmlFile.find( "href", posOpenTag );
            if ( posHref >= posCloseTag || posHref == string::npos )
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            if ( htmlFile.find( "a", posOpenTag ) >= posHref ) 
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            size_t posEqual = htmlFile.find( "=", posHref );
            if ( posEqual == string::npos )
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            url = htmlFile.substr( posEqual + 1, posCloseTag-posEqual-1 );  
            size_t qPos = url.find( "\"" );
            if ( qPos == string::npos )
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );   
               continue;
               }
            else
               {
               url = url.substr( qPos+1, url.find( "\"", qPos+1 ) - qPos - 1 );
               }
            size_t linkIndex = 0;
            if ( url.front( ) == '.' || url.front( ) == '\\' )
               {
               relativeLinks.pushBack( url );
               linkIndex = relativeLinks.size( ) - 1;
               }
            else
               {
               links.pushBack( url );     
               linkIndex = links.size( ) - 1;
               }

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
               vector< string > wordsInAnchor;
               wordsInAnchor = BreakAnchors( anchor );
               anchorPos anchorIndex;
               anchorIndex.linkInd = linkIndex;
               words.pushBack( wordsInAnchor[ 0 ] );
               anchorIndex.startPos = words.size( )-1;
               anchorIndex.endPos = words.size( )-1 + wordsInAnchor.size( );
               for( size_t i = 1; i < wordsInAnchor.size( ); i++ )
                  {
                  words.pushBack( wordsInAnchor[ i ] );
                  }
               anchorText.pushBack( anchorIndex );
               }
            }
         posOpenTag = htmlFile.find( "<", posCloseTag );   
         }
      }   

   // DO NOT REVIEW ; NOT BEING USED ANYMORE 
   // - - - breaks anchor string into individual words and returns them to add to words.
   /*         
      vector < string > HTMLparser::BreakAnchorsOG ( const string anchor )
      {
       string word;
      vector < string > output;
      size_t pos_whitespace = anchor.find(" ");
      size_t pos_start = 0;
      if ( pos_whitespace != string::npos ){
         size_t nfind = anchor.find("\n", pos_start);
         size_t tfind = anchor.find("\t", pos_start);
         if ( nfind != string::npos && nfind < pos_whitespace)
               {
               pos_whitespace = nfind;
               }   
         if ( tfind != string::npos && tfind < pos_whitespace)
               {
               pos_whitespace = tfind;
               }
      }
      size_t nfind = 0, tfind = 0, sfind = 0;
      while( pos_whitespace != string::npos )
         {
         word = anchor.substr(pos_start, pos_whitespace - pos_start + 1);   
        
         if (word != " " && word != "\n" && word != "\t" && word!= ""){
               nfind = word.find("\n");
               tfind = word.find("\t");
               sfind = word.find(" ");
               if ( word.find("\n") != string::npos )
               {
                  // nfind = word.find("\n");
                  word = word.replace( nfind , 1, "" );
               }
               if ( tfind != string::npos )
               {
                  // tfind = word.find("\t");
                  word = word.replace( tfind, 1, "");
               }
               if ( sfind != string::npos )
               {
                  // sfind = word.find(" ");
                  word = word.replace( sfind, 1, "");
               }
               output.pushBack( word );
         }
         pos_start = pos_whitespace + 1;
         pos_whitespace = anchor.find( " ", pos_start );
         if ( pos_whitespace == string::npos )
            {
            pos_whitespace = anchor.find( "\n", pos_start );
            }
         else
            {
            size_t nfind = anchor.find("\n", pos_start);
            size_t tfind = anchor.find("\t", pos_start);
            if ( nfind != string::npos && nfind < pos_whitespace)
               {
               pos_whitespace = nfind;
               }   
            if ( tfind != string::npos && tfind < pos_whitespace)
               {
               pos_whitespace = tfind;
               }
            }
         }
        if ( pos_start == 0 )
         {
         output.pushBack( anchor );
         }
         else 
         {
         if ( pos_start != (anchor.length() -1) )
            {
            word = anchor.substr(pos_start, anchor.length()- pos_start + 1);
            output.pushBack( word );
            }
         }     
        return output;
       }   
       //nxt
   */
};

