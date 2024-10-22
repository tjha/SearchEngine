// parser.hpp
// Provides functions to parse HTML content and deliver
// 2019-12-04:	Changed anchorterxt return type; added title words. Added return functions for both.
// 2019-12-03:  Added GetWords,inAvoid,findScripts. Edited Parsetags.
// 2019-11-26:  Eliminated unnecesssary code duplication in fixDots, and
// 			    addressed several edge cases: tjha
// 2019-11-23:  Fixed bugs in GetLinks function to avoid over-geralization of
//              the locations of 'a' in relation to 'href': tjha
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
// 2019-11-06:  Fixed whitespace errors, changed to paired to <size_t vector<size_t>>, : medhak
// 2019-11-05:  Implemented return functions, fixed errors in breakanchor.
// 2019-11-04:  Created parser class, some basic functions - constructor, BreakAnchor. Merged GetAnchorText and
//              GetLinks into GetLinks - makes it easier to build anchorText vector : medhak
// 2019-11-04:  Added GetAnchorText function to get link anchor text: tjha
// 2019-11-04:  Fixed document style to match style guide: tjha
// 2019-10-26:  Created get_links function to get urls from basic html:
//              medhak, tjha

#ifndef DEX_HTML_PARSER
#define DEX_HTML_PARSER

#include <cstddef>
#include "utils/algorithm.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/utility.hpp"
#include "utils/url.hpp"
#include "utils/vector.hpp"


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
		struct AncWord
			{
			string url;
			vector<string> text;
			};

	class HTMLparser
	{
	private:
		bool isAlpha( const char c )
			{
			return ( 'A' <= c && c <= 'Z' ) || ( 'a' <= c && c <= 'z' );
			}

	public:
		//dex::string htmlFile;
		dex::string pageLink;
		dex::vector< dex::Url > links;
		dex::vector< string > words;
		dex::vector< string > title;
		dex::vector< AncWord > anchors;
		std::size_t lenAnchors;
		//dex::vector< string > relativeLinks;
		dex::vector< anchorPos > anchorText;


		void GetLinks( dex::string &htmlFile, bool isIndex );
		vector< Positions > findScripts( Positions body, dex::string &htmlFile );
		void GetWords( dex::string &htmlFile );
		void GetTitle( dex::string &htmlFile );
		bool inAvoid(size_t pos, vector<Positions> avoidThis);

		vector<string> tagger (string temp)
			{
			vector<string> result;
			result.pushBack("</ " + temp + ">");
			result.pushBack("< /" + temp + ">");
			result.pushBack("< / " + temp + ">");
			result.pushBack("</ " + temp + " >");
			result.pushBack("< /" + temp + " >");
			result.pushBack("< / " + temp + " >");
			result.pushBack("</" + temp + " >");
			return result;
			}

		Positions ParseTag( Positions &pos, string &startTag, string &endTag, string &htmlFile )
			{
			Positions newPos;
			newPos.start = htmlFile.find( startTag.cStr( ), pos.start );
			if ( newPos.start == string::npos )
				{
				// incorporate some form of error logging
				string temp = startTag.substr(1, startTag.size() - 1);
				temp = "< " + temp;
				newPos.start = htmlFile.find( temp.cStr( ), pos.start );
				if ( newPos.start == string::npos )
					{
					newPos.start = string::npos;
					newPos.end = string::npos;
					return newPos;
					}
				}
			newPos.end = htmlFile.find(endTag.cStr(), newPos.start );
			if ( newPos.end == string::npos )
				{
				string temp = endTag.substr(2, endTag.size() - 3);
				vector<string> possibleVars = tagger(temp);
				size_t i = 0;
				while ( i < possibleVars.size( ) )
					{
					newPos.end = htmlFile.find(possibleVars[ i ].cStr(), newPos.start );
					if ( newPos.end != string::npos )
						{
						newPos.end += possibleVars[ i ].length();
						return newPos;
						}
					i++;
					}
				// if we still don't find the damn thing.
				throw dex::outOfRangeException();
				}
			newPos.end += endTag.length();
			return newPos;
			}

		HTMLparser( );
		HTMLparser( dex::Url &url, dex::string& html, bool isIndex );
		void removePunctuation( string &word );
		// static vector< string > BreakAnchorsOG ( const string anchor );
		void BreakAnchors ( string& anchor );
		// void GetAnchorText( );
		vector< dex::Url > ReturnLinks ( );
		// vector< dex::pair<size_t, size_t > > ReturnAnchorText ( );
		vector< AncWord > ReturnAnchorText ( );
		vector<string > ReturnTitle ( );
		size_t ReturnAnchorTextLength( );
		vector< string > ReturnWords ( );
		void fixDots (string &url);
		void changeToLowercase( string &word );
		void buildAnchors( );
		void buildTitle( );
	};

	void HTMLparser::buildAnchors( )
		{
		vector< string > temp;
		for( size_t i = 0; i < anchorText.size( ); i++ )
			{
			for ( size_t j = anchorText[ i ].startPos ; j < anchorText[ i ].endPos + 1 ; j++ )
				{
					temp.pushBack( words[ j ] );
				}
			AncWord push;
			push.url = (links[ anchorText[ i ].linkInd ]).completeUrl( ) ;
			push.text = temp;
			anchors.pushBack( push );
			temp.clear();
			}
		words.clear();
		}

	HTMLparser::HTMLparser( )
		{
		//htmlFile = "";
		pageLink = "";
		}

	HTMLparser::HTMLparser( dex::Url &url, dex::string& html, bool isIndex )
		{
      /*
		std::size_t linkEnd = html.findFirstOf( '\n' );
		pageLink = html.substr( 0, linkEnd );
      */

      pageLink = url.completeUrl( );
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
		//htmlFile = html.substr( linkEnd + 1, html.length( ) - linkEnd - 1 );
		//GetLinks( html, isIndex );
      		if ( isIndex )
         		{
		         lenAnchors = words.size();
		         buildAnchors();

         		GetTitle( html );
         		buildTitle();

         		GetWords( html );
         		}
     		else
			{
			GetLinks( html, isIndex );
			}
		}


	size_t HTMLparser::ReturnAnchorTextLength( )
		{
		return lenAnchors;
		}

	vector< dex::Url > HTMLparser::ReturnLinks ( )
		{
		return links;
		}
	vector< string > HTMLparser::ReturnWords ( )
		{
		return words;
		}

	vector< AncWord > HTMLparser::ReturnAnchorText ( )
		{
		return anchors;
		}

	vector< string > HTMLparser::ReturnTitle ( )
		{
		return title ;
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
				if( word.front( ) == '&' )
					{
					continue;
					}
				removePunctuation( word );
				// removePunctuation( word );
				std::size_t wordIdx = word.findFirstNotOf( ' ', 0 );
				std::size_t spaceIdx = word.find( ' ' );
				while ( spaceIdx != dex::string::npos && wordIdx != dex::string::npos )
					{
					if ( wordIdx < spaceIdx )
						{
						string processedWord = word.substr( wordIdx, spaceIdx - wordIdx );
						changeToLowercase( processedWord );
						// removePunctuation( processedWord );
						if( processedWord != "" )
							{
							words.pushBack( processedWord );
							}
						wordIdx = word.findFirstNotOf ( ' ', spaceIdx );
						}
					spaceIdx = word.find( ' ', spaceIdx + 1);
					}
				if ( wordIdx != dex::string::npos )
					{
					string processedWord = word.substr( wordIdx, word.length( ) - wordIdx );
					changeToLowercase( processedWord );
					// removePunctuation( processedWord );
					if( processedWord != "" )
						{
						words.pushBack( processedWord );
						}
					}

			}
		if( indexNotOf == start )
			{
			changeToLowercase( anchor );
			// removePunctuation( anchor );
			words.pushBack( anchor );
			}
		}

	void HTMLparser::changeToLowercase( string &word )
		{
		word = word.cStr();
		int alph = 0;
		for(size_t i = 0 ; i < word.size(); i++)
			{
			if ( word[i] > 64 && word[i] < 91 )
				{
				word[ i ] = word[ i ] + 32 ;
				alph = 1;
				}
			if ( word[i] > 96 && word[i] < 123 )
				{
				alph = 1;
				}
			}
		if ( alph == 0 )
			{
			word = "";
			}
		else
			{
			removePunctuation( word );
			}
		}

	void HTMLparser::removePunctuation( string &word )
		{
		if ( word.empty( ) )
			return;

		string strippedWord;
		strippedWord.reserve( word.size( ) );
		for ( size_t index = 0;  index != word.size( );  ++index )
			{
			if ( isAlpha( word[ index ] ) )
				strippedWord.pushBack( word[ index ] );
			else
				switch ( word[ index ] )
					{
					case '-':
					case '_':
						break;
					default:
						strippedWord.pushBack( ' ' );
						break;
					}
			}

		word.swap( strippedWord );
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


	void HTMLparser::GetLinks( dex::string &htmlFile, bool isIndex )
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
				if ( isIndex )
               {
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
				}
			posOpenTag = htmlFile.find( "<", posCloseTag );
			}
		}

	//OLD STUFF ENDS;
// WORKING ON WORDS BELOW


	vector< Positions > HTMLparser::findScripts( Positions body, string &htmlFile )
		{
		vector<Positions> avoider;
		string s = "<script";
		string t = "</script>";
		Positions strt = ParseTag( body, s, t, htmlFile ), temp;
		temp.end = body.end;
		while( strt.start < body.end && strt.start != string::npos )
			{
			if (strt.end == string::npos)
				{
				break;
				}
			avoider.pushBack(strt);
			temp.start = strt.end + 1;
			strt = ParseTag( temp, s, t, htmlFile );
			}
		temp.start = body.start;
		s = "<style";
		t = "</style>";
		strt = ParseTag( body, s, t, htmlFile );
		while( strt.start < body.end && strt.start != string::npos )
			{
			if (strt.end == string::npos)
				{
				break;
				}
			avoider.pushBack(strt);
			temp.start = strt.end + 1;
			strt = ParseTag( temp, s, t, htmlFile );
			}
		return avoider;
		}


	void HTMLparser::GetWords( dex::string &htmlFile )
		{
		static const char WHITESPACE [ ] = { ' ', '\t', '\n', '\r' };
		static const char ALPHABET [ ] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
														'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
														'w', 'x', 'y','z' };
		Positions body, start;
		start.start = 0;
		start.end = htmlFile.length( ) - 1;

		string s, t;
		s = "<body";
		t = "</body>";
		body = ParseTag( start, s, t, htmlFile );


		if( body.start == string::npos )
			{
			return;
			}

		vector< Positions > avoidThis;
		avoidThis = findScripts( body, htmlFile );

		string text;
		std::size_t posClose = htmlFile.find( '>', body.start + 6 );
		std::size_t posOpen = htmlFile.find( '<', posClose );

		while ( posClose < body.end )
			{
			if( posOpen >= body.end )
				{
				break;
				}

			if( inAvoid( posClose, avoidThis ) || inAvoid( posOpen, avoidThis ) )
				{
				posClose = htmlFile.find( '>', posOpen );
				posOpen = htmlFile.find( '<', posClose );
				continue;
				}

			text = htmlFile.substr(posClose + 1, posOpen - posClose - 1);

			if( text.findFirstNotOf(WHITESPACE) == string::npos)
				{
				if( text.find( ALPHABET ) != string::npos )
					{
					// Because manpages git(line 27) wasn't being caught.
					BreakAnchors( text );
					}
				posClose = htmlFile.find( '>', posOpen );
				posOpen = htmlFile.find( '<', posClose );
				continue;
				}

			BreakAnchors( text );

			posClose = htmlFile.find( '>', posOpen );
			posOpen = htmlFile.find( '<', posClose );
			}
		}

	bool HTMLparser::inAvoid(size_t pos, vector<Positions> avoidThis)
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



	void HTMLparser::GetTitle( dex::string &htmlFile )
		{
		static const char WHITESPACE [ ] = { ' ', '\t', '\n', '\r' };
		static const char ALPHABET [ ] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
														'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
														'w', 'x', 'y','z' };
		Positions title, start;
		start.start = 0;
		start.end = htmlFile.length( ) - 1;

		string s, t;
		s = "<title";
		t = "</title>";
		title = ParseTag( start, s, t, htmlFile );

		if( title.start == string::npos )
			{
			return;
			}
		string text;
		std::size_t posClose = htmlFile.find( '>', title.start );
		std::size_t posOpen = htmlFile.find( '<', posClose );

		while ( posClose < title.end )
			{
			if( posOpen >= title.end )
				{
				break;
				}

			text = htmlFile.substr(posClose + 1, posOpen - posClose - 1);

			if( text.findFirstNotOf(WHITESPACE) == string::npos)
				{
				if( text.find( ALPHABET ) != string::npos )
					{
					BreakAnchors( text );
					}
				posClose = htmlFile.find( '>', posOpen );
				posOpen = htmlFile.find( '<', posClose );
				continue;
				}

			BreakAnchors( text );

			posClose = htmlFile.find( '>', posOpen );
			posOpen = htmlFile.find( '<', posClose );
			}
		}

	void HTMLparser::buildTitle()
		{
		for(size_t i = 0; i < words.size(); i++ )
			{
			title.pushBack( words[ i ] );
			}
		words.clear();
		}


}
#endif // DEX_HTML_PARSER
