
#include "../utils/basicString.hpp"
#include "../utils/unorderedSet.hpp"
// 2019-10-31: Copy constructor for robots: jhirsh
// 2019-10-31: Init commit, added query and fragment: combsc

namespace dex
	{
	class Url
		{
		public:
			string service, host, port, path, query, fragment;

			Url( const char *url )
				{
				// Assumes url points to static text but
				// does not check.

				string totalUrl = url;
				int endservice = totalUrl.find( "://");

				// If no service is specified, assume http
				int beginHost;
				if ( endservice == -1 )
					{
					service = "http";
					beginHost = 0;
					}
				else
					{
					service = totalUrl.substr( 0, endservice );
					beginHost = endservice + 3;
					}
					

				int endHost = totalUrl.findFirstOf( "/:", beginHost );
				// If there is no path or port, the end of the host is the end of the string.
				if ( endHost == -1 )
					endHost = totalUrl.size( );
				host = totalUrl.substr( beginHost, endHost - beginHost );

				// Now we check to see if the port is specified
				int beginPort = endHost + 1;
				int endPort = endHost;
				if ( totalUrl[ endHost ] == ':' )
					{
					endPort = totalUrl.find( "/", beginPort );
					// If there is no path, the end of the port is the end of the string.
					if ( endPort == -1 )
						endPort = totalUrl.size( );
					port = totalUrl.substr( beginPort, endPort - beginPort );
					}
				else
					{
					if ( service == "http" )
						port = "80";
					else
						{
						if ( service == "https" )
							port = "443";
						else
							port = "";
						}
					}
				int beginPath = endPort + 1;
				int endPath;
				// Check to see if there are any queries or fragments in our path.
				if ( beginPath > int( totalUrl.size( ) ) )
					{
					endPath = int( totalUrl.size( ) );
					path = "/";
					}
				else
					{
					endPath = totalUrl.findFirstOf( "?#", beginPath );
					if ( endPath == -1 )
						endPath = int( totalUrl.size( ) );
					path = totalUrl.substr( beginPath, endPath - beginPath );
					}
				// If queries exist
				int beginQuery = totalUrl.find( "?", endPath );
				int endQuery = totalUrl.find( "#", endPath );
				if ( endQuery == -1 )
					endQuery = int( totalUrl.size( ) );
				// If a query exists
				if ( beginQuery != -1 )
					{
					query = totalUrl.substr( beginQuery, endQuery - beginQuery );
					}
				// If a fragment exists ( the end of the query is NOT the end of the Url )
				if ( endQuery < int( totalUrl.size( ) ) )
					{
					int beginFragment = endQuery + 1;
					int endFragment = int( totalUrl.size( ) );
					fragment = totalUrl.substr( beginFragment, endFragment - beginFragment );
					}
				}

			Url( const Url &other )
            {
            service = other.service;
            host = other.host;
            port = other.port;
            path = other.path;
            query = other.query;
            fragment = other.fragment;
            }
			
			string completeUrl( )
				{
				string completeUrl = service + "://" + host;
				if ( port != "" && port != "443" && port != "80" )
					{
					completeUrl += ":" + port;
					}
				if ( path != "/" )
					{
					completeUrl += path;
					}
				if ( query != "" )
					{
					completeUrl += "?" + query;
					}
				if ( fragment != "" )
					{
					completeUrl += "#" + fragment;
					}
				return completeUrl;
				}
		};
	}
