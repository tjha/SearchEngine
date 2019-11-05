// parser.hpp
//
// Provides functions to parse HTML content and deliver 
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


using dex::string;
using dex::vector;
using dex::exception;


namespace dex
{
   struct paired
         {
         size_t first;
         size_t second;
         };

   class HTMLparser
   {
      private:
      string htmlFile;
      vector < string > links;
      vector < string > words;
      // Doing this as pair of < size_t, size_t >. The other option is pair of <size_t, vector <size_t> >.
      
      // vector< dex::pair< size_t, size_t > > anchorText; 
      vector< paired > anchorText; 
      public:
      HTMLparser (){ htmlFile = "";}

      HTMLparser( string html )
      {
         htmlFile = html; 
      }

      void GetLinks( );
      vector <string> BreakAnchors ( string anchor );
      void GetAnchorText( );
      vector < string > ReturnLinks ( );
      // vector < dex::pair <size_t, size_t > > ReturnAnchorText ( );
      vector < dex::paired > ReturnAnchorText ( );
      vector < string > ReturnWords ( );
      // void GetWords( );
   };

   vector < string > HTMLparser::ReturnLinks ( )
      {
      return links;
      }

   vector < string > HTMLparser::ReturnWords ( )
      {
      return words;
      }

   // vector < dex::pair < size_t, size_t > > HTMLparser::ReturnAnchorText ( )
   vector < dex::paired > HTMLparser::ReturnAnchorText ( )
      {
      return anchorText;
      }

   // breaks anchor string into individual words and returns them to add to words.
   vector < string > HTMLparser::BreakAnchors ( string anchor )
      {
      string word;
      vector < string > output;
      size_t pos_whitespace = anchor.find(" ");
      size_t pos_start = 0;
      while( pos_whitespace != string::npos )
         {
         word = anchor.substr(pos_start, pos_whitespace - pos_start + 1);
         if (word != " " && word != "\n" && word != "\t" && word!= ""){
         output.pushBack( word );
         }
         pos_start = pos_whitespace + 1;
         pos_whitespace = anchor.find( " ", pos_start );
         if ( pos_whitespace == string::npos )
            {
            pos_whitespace = anchor.find( "\n", pos_start );
            }
         }
      if ( pos_start == 0 )
         {
         output.pushBack( anchor );
         }
      else if ( pos_start != (anchor.length() -1) )
         {
         word = anchor.substr(pos_start, anchor.length()- pos_start + 1);
         output.pushBack( word );
         }
      return output;
      }   


   // Maybe we can use continue's to avoid the nested loops? Needs to be tested 
   void HTMLparser::GetLinks( )
      {
      size_t posOpenTag = 0, posCloseTag = 0;
      string url;
      string anchor;

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
                           // cout << url << "\n";
                           links.pushBack( url );
                           size_t link_ind = links.size();

                           //finding anchor text - - i think this should just be one function.
                           posOpenTag = htmlFile.find ( "<", posCloseTag );
                           if ( posOpenTag != string::npos )
                              {
                              //can we go over this logic later?
                              if ( posOpenTag < htmlFile.length() - 2
                                    && htmlFile[ posOpenTag + 1 ] == '/'
                                    && htmlFile[ posOpenTag + 2 ] == 'a')
                                 {
                                 anchor = htmlFile.substr( posCloseTag + 1, posOpenTag - posCloseTag - 1 );
                                 vector< string > wordsInAnchor;
                                 wordsInAnchor = BreakAnchors( anchor );
                                 for( size_t i = 0; i < wordsInAnchor.size(); i++)
                                    {
                                    words.pushBack( wordsInAnchor[i] );
                                    //pushing back the link index and the word index. Can't think of a better way rn..
                                    // anchorText.pushBack( dex::pair( link_ind, words.size() ) );
                                    dex::paired temp;
                                    temp.second = words.size() -1 ;
                                    temp.first = link_ind;
                                    anchorText.pushBack( temp );
                                    }
                                 
                                 }
                              }
                           }

                        
                        }
                     
                     }
                  }
               }
            }
         posOpenTag = htmlFile.find( "<", posCloseTag );   
         }
      }   



   // don't think we need a separate function for this -- added it to GetLinks. Kept this version here in case we want
   // to revert. 
   void HTMLparser:: GetAnchorText ( )
      {
      string anchor;
      vector < string > OGAnchorText; //name changed to avoid confusion with pvt member variable.
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
                              OGAnchorText.pushBack( anchor );
                              }
                           }
                        }
                     }
                  }
               }
            }
         posOpenTag = htmlFile.find( "<", posCloseTag );   
         }
      }
};

