
#include "../utils/basicString.hpp"
#include "../utils/unorderedSet.hpp"
// 2019-10-31: Init commit, added query and fragment: combsc

namespace dex
	{
	class Url
		{
		public:
			string completeUrl;
			string service, host, port, path, query, fragment;

			Url( const char *url )
				{
				// Assumes url points to static text but
				// does not check.

				completeUrl = url;
				int endservice = completeUrl.find( "://");

				// If no service is specified, assume http
				int beginHost;
				if ( endservice == -1 )
					{
					service = "http";
					beginHost = 0;
					}
				else
					{
					service = completeUrl.substr( 0, endservice );
					beginHost = endservice + 3;
					}
					

				int endHost = completeUrl.findFirstOf( "/:", beginHost );
				// If there is no path or port, the end of the host is the end of the string.
				if ( endHost == -1 )
					endHost = completeUrl.size( );
				host = completeUrl.substr( beginHost, endHost - beginHost );

				// Now we check to see if the port is specified
				int beginPort = endHost + 1;
				int endPort = endHost;
				if ( completeUrl[ endHost ] == ':' )
					{
					endPort = completeUrl.find( "/", beginPort );
					// If there is no path, the end of the port is the end of the string.
					if ( endPort == -1 )
						endPort = completeUrl.size( );
					port = completeUrl.substr( beginPort, endPort - beginPort );
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
				if ( beginPath > int( completeUrl.size( ) ) )
					{
					endPath = int( completeUrl.size( ) );
					path = "/";
					}
				else
					{
					endPath = completeUrl.findFirstOf( "?#", beginPath );
					if ( endPath == -1 )
						endPath = int( completeUrl.size( ) );
					path = completeUrl.substr( beginPath, endPath - beginPath );
					}
				// If queries exist
				int beginQuery = completeUrl.find( "?", endPath );
				int endQuery = completeUrl.find( "#", endPath );
				if ( endQuery == -1 )
					endQuery = int( completeUrl.size( ) );
				// If a query exists
				if ( beginQuery != -1 )
					{
					query = completeUrl.substr( beginQuery, endQuery - beginQuery );
					}
				// If a fragment exists ( the end of the query is NOT the end of the Url )
				if ( endQuery < int( completeUrl.size( ) ) )
					{
					int beginFragment = endQuery + 1;
					int endFragment = int( completeUrl.size( ) );
					fragment = completeUrl.substr( beginFragment, endFragment - beginFragment );
					}
				}
		};
	}