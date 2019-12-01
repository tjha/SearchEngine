
#ifndef DEX_URL_HPP
#define DEX_URL_HPP

#include "basicString.hpp"
#include "unorderedSet.hpp"
#include "exception.hpp"
#include "functional.hpp"
// 2019-11-27: Fixed completeUrl if no path is given but ?# are: combsc
// 2019-11-24: Changed completeUrl: combsc
// 2019-11-21: Fixed fragment/query bug: combsc
// 2019-11-20: Add =operator, hash: combsc
// 2019-11-10: made parts of url private, have to call get and set
//             to ensure we don't mess up the url in the future: combsc
// 2019-10-31: Copy constructor for robots: jhirsh
// 2019-10-31: Init commit, added query and fragment: combsc

namespace dex
	{
	class Url
		{
		private:
			// Service must ONLY be the service, no :// allowed.
			string service;
			// Host does not include '/' at the end. Host should be in
			// the form of www.someSite.domain
			string host;
			// port does NOT include : nor /, it should ONLY be the numbers.
			string port;
			// path ALWAYS begins with '/'. Port should NOT end with
			// '/', since this is a URL and therefore an endpoint.
			string path;
			// query should not include ?
			string query;
			// fragment should not include #
			string fragment;
		public:
			Url( )
				{
				}
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

				int endHost = totalUrl.findFirstOf( "?#/:", beginHost );
				// If there is no path or port, the end of the host is the end of the string.
				if ( endHost == -1 )
					endHost = totalUrl.size( );
				host = totalUrl.substr( beginHost, endHost - beginHost );

				// Now we check to see if the port is specified
				int beginPort = endHost + 1;
				int endPort = endHost;
				if ( totalUrl[ endHost ] == ':' )
					{
					endPort = totalUrl.findFirstOf( "/?#", beginPort );
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
				int beginPath = endPort;
				int endPath;
				// Check to see if there are any queries or fragments in our path.
				
				if ( beginPath >= int( totalUrl.size( ) ) )
					{
					endPath = int( totalUrl.size( ) );
					path = "/";
					}
				else
					{
					endPath = totalUrl.findFirstOf( "?#", beginPath );
					if ( endPath == -1 )
						endPath = int( totalUrl.size( ) );
					if ( endPath == beginPath )
						path = "/";
					else
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
					int beginFragment = endQuery;
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

			Url operator=( const Url &other )
				{
				service = other.service;
				host = other.host;
				port = other.port;
				path = other.path;
				query = other.query;
				fragment = other.fragment;
				return *this;
				}
			
			string completeUrl( ) const
				{
				string completeUrl = service + "://" + host;
				if ( port != "" && port != "443" && port != "80" )
					{
					completeUrl += ":" + port;
					}
				completeUrl += path;
				completeUrl += query;
				completeUrl += fragment;
				return completeUrl;
				}
			
			string getService( )
				{
				return service;
				}
			void setService( const string &s )
				{
				service = s;
				if ( service != "http" && service != "https" )
					{
					std::cerr << "We do not support service of the type " << service << "\n";
					throw invalidArgumentException( );
					}
				}

			string getHost( )
				{
				return host;
				}
			void setHost( const string &h )
				{
				host = h;
				}

			string getPort( )
				{
				return port;
				}
			
			void setPort( const string &p )
				{
				if ( p.empty() )
					{
					std::cerr << "Need to set port to a nonempty string";
					throw invalidArgumentException( );
					}
				if ( p.back( ) != ':' )
					{
					port = p;
					}
				else
					{
					port = p.substr( 0, p.size( ) - 1 );
					}
				}

			string getPath( )
				{
				return path;
				}

			void setPath( const string &p )
				{
				path = p;
				if ( path.empty( ) || path.front( ) != '/' )
					path.insert( 0, '/' );
				while ( path.size( ) > 1 && path.back( ) == '/' )
					path.popBack( );
				}

			string getQuery( )
				{
				return query;
				}

			void setQuery( const string &q )
				{
				query = q;
				}

			string getFragment( )
				{
				return fragment;
				}

			void setFragment( const string &f )
				{
				fragment = f;
				}
		
		};

	bool operator==( const dex::Url &lhs, const dex::Url &rhs )
		{
		return lhs.completeUrl( ) == rhs.completeUrl( );
		}
	template < >
	struct hash < dex::Url >
		{
		unsigned long operator()( const dex::Url &url ) const
			{
			return dex::hash< dex::string >{} ( url.completeUrl( ) );
			}
		};
	}
#endif
