// parser.hpp
// Provides functions to parse HTML content and deliver
//
// 2019-11-26:  Eliminated unnecesssary code duplication in fixDots, and
// 			    addressed several edge cases: tjha
// 2019-11-23:  Fixed bugs in GetLinks function to avoid over-geralization of
//              the locations of 'a' in relation to 'href': tjha
// 2019-11-22:  Added include guards, merged with changes by combsc that convert
//              links to vector of url types instead of string, modified
//              handling of relative links to always append '/' correctly: tjha
// 2019-11-21:  Modified BreakAnchors and RemovePunctuation to separate
//              words between punctuations except dashes: tjha
//              Updated to use URLs instead of strings
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

#include <iostream>

namespace dex
{
   struct anchorPos
         {
         std::size_t linkInd;
         std::size_t startPos;
         std::size_t endPos;
         };
   struct Positions
         {
         std::size_t start;
         std::size_t end;
         };

   class HTMLparser
   {

   private:
      dex::string htmlFile;
      dex::string pageLink;
      dex::vector< dex::Url > links;
      dex::vector< string > words;
      std::size_t lenAnchors;
      //dex::vector< string > relativeLinks;
      dex::vector< anchorPos > anchorText;


      void GetLinks( );
      vector < Positions > findScripts( Positions body );
      void GetWords( );
      bool inAvoid(size_t pos, vector <Positions> avoidThis);

      Positions ParseTag( Positions &pos, string &startTag, string &endTag )
         {
         Positions newPos;
         newPos.start = htmlFile.find( startTag.cStr( ), pos.start, pos.end - pos.start );
         if ( newPos.start == string::npos )
            {
            // incorporate some form of error logging
            // std::cout<< startTag <<" " << endTag <<"\n";
            newPos.start = string::npos;
            newPos.end = string::npos;
            return newPos;
           std::cerr << "Error caught in ParseTag\n";
            // throw dex::outOfRangeException();
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
      void removePunctuation( string &word );
      // static vector < string > BreakAnchorsOG ( const string anchor );
      void BreakAnchors ( string& anchor );
      // void GetAnchorText( );
      vector < dex::Url > ReturnLinks ( );
      // vector < dex::pair <size_t, size_t > > ReturnAnchorText ( );
      vector < anchorPos > ReturnAnchorText ( );
      vector < string > ReturnWords ( );
      void fixDots (string &url);


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

		std::size_t indexPos = pageLink.find( ".html", 0 );
		while ( indexPos != dex::string::npos && pageLink[ indexPos ] != '/' )
			{
			indexPos--;
			}

		if ( indexPos != dex::string::npos )
			{
			pageLink = pageLink.substr( 0, indexPos );
			}

      if ( pageLink.back( ) == '/' )
         {
         pageLink.popBack( );
         }
      htmlFile = html.substr( linkEnd + 1, html.length( ) - linkEnd - 1 );
      GetLinks( );

      lenAnchors = words.size();

      // GetWords();
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

            removePunctuation( word );
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


   void HTMLparser::removePunctuation( string &word )
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

      }

	std::size_t spaceDelimitedTargetPosition( dex::string target,
											  				dex::vector< dex::string >& query,
															dex::string& str)
		{
		std::size_t pos = str.find( query[ 0 ], 0 );
		std::size_t final_pos = ( query[ 0 ] == target ) ? pos : dex::string::npos;
      for ( std::size_t i = 1; i < query.size( ); i++ )
			{
			if ( pos == dex::string::npos )
				{
				return dex::string::npos;
				}
			std::size_t next = str.find( query[ i ], pos );
			if ( next == dex::string::npos )
				{
				return dex::string::npos;
				}
			pos += query[ i - 1 ].length( );
			while ( pos < next )
				{
				if ( str[ pos ] != ' ' )
					{
					return dex::string::npos;
					}
				pos++;
				}
			pos = next;
			if ( query[ i ] == target && final_pos == dex::string::npos )
				{
				final_pos = pos;
				}
			}
		return final_pos;
		}

	// remove numBack entries from end of path
	// If numBack is larger than number of path elements to remove, return '/'
	dex::string parsePath( unsigned int numBack, dex::string path )
		{

		std::size_t pos = path.findLastOf( '/' );
		if ( path.back( ) == '/' )
			{
			path.popBack( );
			pos = path.findLastOf( '/' );
			}

		while( numBack-- && pos != dex::string::npos )
			{
			path = path.substr( 0, pos );
			pos = path.findLastOf( '/' );
			}

		path = path + "/";
		return path;
		}

	// NOTE: This method currently will not handle multiple './' repeated (very
	// 		unlikely since such a relative link would be unnecessarily long
   void HTMLparser::fixDots( dex::string& url )
      {

      if ( url.empty( ) )
			{
			url = pageLink;
			return;
			}

		if( url.front( ) != '/' )
			{
			url.insert( 0, '/' );
			}

		if ( url.back( ) == '.' && url[ url.length( ) - 2 ] == '.' )
			{
			url.pushBack( '/' );
			}


		if( url[ 1 ] != '.' )
			{
			url = pageLink + url;
			return;
			}

		if ( url.length( ) >= 4 && url[ 2 ] == '/' )
			{
			url = pageLink + url.substr( 2, url.length( ) - 2 );
			return;
			}
		else if ( url.length( ) < 4 )
			{
			return;
			}

		if ( url[ 2 ] == '.' && url[ 3 ] == '/' )
			{
			size_t pos = 4;
			unsigned int numBack = 1;
			while ( pos + 2 < url.length( ) && url[ pos ] == '.'
						&& url[ pos + 1 ] == '.' && url[ pos + 2 ] == '/' )
				{
				numBack++;
				pos += 3;
				}

			dex::Url linkBase( pageLink.cStr( ) );
			std::size_t pathPos = pageLink.find( linkBase.getPath( ), 0 );


			dex::string newLinkBase =
				pageLink.substr( 0, pathPos )
				+ parsePath( numBack, linkBase.getPath( ) );

			if ( pos < url.length( ) )
				{
				url = newLinkBase + url.substr( pos, url.length( ) - pos );
				}
			else
				{
				url = newLinkBase;
				}

			}
		else
			{
			url = pageLink + url;
			}
		return;
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

				dex::string tagStr = htmlFile.substr( posOpenTag,
																  posCloseTag - posOpenTag + 1 );
				dex::vector< dex::string > query(2);
				query[ 0 ] = "<";
				query[ 1 ] = "a";

				std::size_t posA =
						spaceDelimitedTargetPosition( "a", query, tagStr );

				if ( posA == dex::string::npos )
					{
               posOpenTag = htmlFile.find( "<", posCloseTag );
               continue;
					}

				query = dex::vector<string>(2);
				query[ 0 ] = "href";
				query[ 1 ] = "=";

				std::size_t posEqual =
						spaceDelimitedTargetPosition( "=", query, tagStr );

            if ( posEqual == string::npos )
               {
               posOpenTag = htmlFile.find( "<", posCloseTag );
               continue;
               }
				posEqual = posOpenTag + posEqual;
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
               }
            std::size_t linkIndex = 0;
            // PushBack absolute url
            if ( url.find("https://", 0) != string::npos ||
                 url.find("http://", 0) != string::npos )
					{
               // url is already an absolute url
               links.pushBack( dex::Url( url.cStr( ) ) );
					}
				else
					{
               fixDots(url);
               links.pushBack( dex::Url( url.cStr( ) ) );
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

   //OLD STUFF ENDS;
// WORKING ON WORDS BELOW

/*
   vector < Positions > HTMLparser::findScripts( Positions body )
      {
      vector <Positions> avoider;
      string s = "<script>";
      string t = "</script>";
      Positions strt = ParseTag( body, s, t), temp;
      temp.end = body.end;
      while( strt.start < body.end && strt.start != string::npos )
         {
         if (strt.end == string::npos)
            {
            break;
            }
         avoider.pushBack( strt );
         temp.start = strt.end + 1;
         strt = ParseTag( temp, s, t );
         }
      temp.start = body.start;
      s = "<style>";
      t = "</style>";
      strt = ParseTag( body, s, t);
      while( strt.start < body.end && strt.start != string::npos )
         {
         if (strt.end == string::npos)
            {
            break;
            }
         avoider.pushBack( strt );
         temp.start = strt.end + 1;
         strt = ParseTag( temp, s, t );
         }
      return avoider;
      }


   void HTMLparser::GetWords( )
      {
      static const char WHITESPACE [ ] = { ' ', '\t', '\n', '\r' };
      Positions body, start;
      start.start = 0;
      start.end = htmlFile.length( ) - 1;
      string s, t;
      s = "<body>";
      t = "</body>";
      body = ParseTag( start, s, t );
      std::cout << "Body : " << body.start << "  " << body.end << "\n";
      vector < Positions > avoidThis;
      avoidThis = findScripts( body );
      std::cout << "Avoid This " << avoidThis.size() <<"\n";
      for(size_t i = 0; i < avoidThis.size(); i++)
         {
         std::cout << avoidThis[i].start << "  " << avoidThis[i].end << "\n";
         }
      std::cout << "Done!\n";
      string text;
      std::size_t posClose = htmlFile.find( '<', body.start );
      std::size_t posOpen = htmlFile.find( '>', posClose );
      while ( posClose < body.end )
         {
         if( posOpen >= body.end )
            {
            break;
            }
         if( inAvoid( posClose, avoidThis ) || inAvoid( posOpen, avoidThis ) )
            {
            posClose = htmlFile.find( '<', posClose + 1 );
            posOpen = htmlFile.find( '>', posClose );
            continue;
            }
         text = htmlFile.substr(posClose + 1, posOpen - posClose - 1);
         if( text.findFirstNotOf(WHITESPACE) == string::npos)
            {
            posClose = htmlFile.find( '<', posClose + 1 );
            posOpen = htmlFile.find( '>', posClose );
            continue;
            }
         BreakAnchors( text );
         }
      }

   bool HTMLparser::inAvoid(size_t pos, vector <Positions> avoidThis)
      {
      for(size_t i = 0; i< avoidThis.size(); i++)
         {
         if( pos >= avoidThis[i].start && pos <= avoidThis[i].end )
            {
            return true;
            }
         }
      return false;
      }
*/
};
#endif // DEX_HTML_PARSER
