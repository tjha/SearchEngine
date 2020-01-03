// 2019-12-10: Cache Complete Url, add subdomain and domain: combsc
// 2019-12-02: Added const to respective functions
// 2019-11-27: Fixed completeUrl if no path is given but ?# are: combsc
// 2019-11-24: Changed completeUrl: combsc
// 2019-11-21: Fixed fragment/query bug: combsc
// 2019-11-20: Add =operator, hash: combsc
// 2019-11-10: made parts of url private, have to call get and set
//             to ensure we don't mess up the url in the future: combsc
// 2019-10-31: Copy constructor for robots: jhirsh
// 2019-10-31: Init commit, added query and fragment: combsc

#ifndef DEX_URL_HPP
#define DEX_URL_HPP

#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/functional.hpp"
#include "utils/unorderedSet.hpp"

namespace dex
	{
	class Url
		{
		private:
			// Service must ONLY be the service, no :// allowed.
			string service;
			string subdomain;
			string domain;
			// Host does not include '/' at the end. Host should be in
			// the form of www.someSite.domain
			string host;
			// port does NOT include : nor /, it should ONLY be the numbers.
			string port;
			// path ALWAYS begins with '/'. Port should NOT end with
			// '/', since this is a URL and therefore an endpoint.
			string path;

			string query;
			string fragment;
			string complete;
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
				else
					endQuery++;
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

				// split the host into subdomain and domain
				size_t dotLocation = host.find( '.' );
				if ( dotLocation == string::npos )
					{
					subdomain = "";
					domain = host;
					}
				else
					{
					subdomain = host.substr( 0, dotLocation );
					domain = host.substr( dotLocation + 1, host.size( ) - dotLocation - 1 );
					}

				computeFullUrl( );
				}

			Url( string url )	: Url( url.cStr( ) )
				{
				}

			Url( const Url &other )
				{
				service = other.service;
				subdomain = other.subdomain;
				domain = other.domain;
				host = other.host;
				port = other.port;
				path = other.path;
				query = other.query;
				fragment = other.fragment;
				complete = other.complete;
				}

			Url operator=( const Url &other )
				{
				service = other.service;
				subdomain = other.subdomain;
				domain = other.domain;
				host = other.host;
				port = other.port;
				path = other.path;
				query = other.query;
				fragment = other.fragment;
				complete = other.complete;
				return *this;
				}

			string computeFullUrl( )
				{
				string completeUrl = service + "://" + host;
				if ( port != "" && port != "443" && port != "80" )
					{
					completeUrl += ":" + port;
					}
				completeUrl += path;
				completeUrl += query;
				completeUrl += fragment;
				complete = completeUrl;
				return completeUrl;
				}

			// TODO only compute this when you need to which should be
			// 	at url creation and when you set Url
			string completeUrl( ) const
				{
				return complete;
				}

			string getService( ) const
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
				computeFullUrl( );
				}

			string getHost( ) const
				{
				return host;
				}
			void setHost( const string &h )
				{
				host = h;
				computeFullUrl( );
				}

			string getSubdomain( ) const
				{
				return subdomain;
				}

			string getDomain( ) const
				{
				return domain;
				}

			string getPort( ) const
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
				computeFullUrl( );
				}

			string getPath( ) const
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
				computeFullUrl( );
				}

			string getQuery( ) const
				{
				return query;
				}

			void setQuery( const string &q )
				{
				query = q;
				computeFullUrl( );
				}

			string getFragment( ) const
				{
				return fragment;
				}

			void setFragment( const string &f )
				{
				fragment = f;
				computeFullUrl( );
				}

		};

	inline bool operator==( const dex::Url &lhs, const dex::Url &rhs )
		{
		return lhs.completeUrl( ) == rhs.completeUrl( );
		}
	template< >
	struct hash< dex::Url >
		{
		unsigned long operator()( const dex::Url &url ) const
			{
			return dex::hash< dex::string >{} ( url.completeUrl( ) );
			}
		};
	}
#endif
