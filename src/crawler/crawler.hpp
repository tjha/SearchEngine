// 2019-12-10: Improve URL object: combsc
// 2019-12-04: Kill connection if page is greater than 5 MB or if recv takes longer than 5 seconds: combsc
// 2019-11-30: Make robotsMap threadsafe: combsc
// 2019-11-26: Differentiate between html and non-html when crawling: combsc
// 2019-11-23: Differentiate between path disallow and timeout: combsc
// 2019-11-21: increased robustness to failure: combsc, jhirsh
// 2019-11-18: removed isError function, receive input correctly, let robots expire: combsc
// 2019-11-13: fixed isError function, generalized User-agent: combsc
// 2019-11-12: http/sConect functions are now connectPage. Concatenate results
//					of requests to one dex::string, then have crawlUrl handle the result:
//					jhirsh
// 2019-11-10: Follow robots redirects until you find the url
// 2019-11-04: Added more error codes, added file for RobotsTxt content to be
//             printed to, added better error handling: combsc
// 2019-11-03: Fixed crawler politeness bugs, added ability to toggle politeness
//             in crawlUrl for testing purposes, fixed logic in crawlUrl function
//             regarding using the map of robots: combsc
// 2019-10-31: Crawler looks for robot objects in unorderedMap, asks server for
//             one if it isn't found: jhirsh
// 2019-10-31: Crawler now has all functions static since it's a stateless
//             object, added error enumerations: combsc
// 2019-10-30: Added everything into dex namespace and created a crawler object
//             with a cleaner interface for using: combsc
//             Made improvements suggested, removed uses of strstr, reduced code
//             duplication, don't destroy the buffer when returning the header.
// 2019-10-29: Turned ParsedUrl into Url, uses dex::strings instead of char*: combsc
// 2019-10-28: Cleaned up file, got working with HTTP and HTTPS: combsc
// 2019-10-20: Init Commit: Jonas

#ifndef DEX_CRAWLER_HPP
#define DEX_CRAWLER_HPP

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <tls.h>
#include <sys/time.h>
#include <string.h>
#include "crawler/robotsMap.hpp"
#include "utils/exception.hpp"
#include "utils/file.hpp"
#include "utils/url.hpp"
#include <iostream>


namespace dex
	{
	enum crawlerError
		{
		RESOLVE_DNS_ERROR = -1,
		CONNECTION_ERROR = -2,
		SENDING_ERROR = -3,
		NO_RESPONSE_ERROR = -4,
		POLITENESS_ERROR = -5,
		NO_LOCATION_ERROR = -6,
		NO_HEADER_END_ERROR = -7,
		SOCKET_CONNECTION_ERROR = -8,
		PROTOCOL_ERROR = -9,
		TLS_CONFIG_ERROR = -10,
		RESPONSE_ERROR = -11,
		DISALLOWED_ERROR = -12,
		NOT_HTML = -13,
		PUT_BACK_IN_FRONTIER = -14,
		LOCKING_ERROR = -15,
		RESPONSE_TOO_LARGE = -16
		};

	enum httpProtocol
		{
		HTTP = 0,
		HTTPS = 1
		};

	class crawler
		{
		private:
			const static size_t maxPageSize = 5000000;
			static dex::string makeGetMessage( const dex::string &path, const dex::string &host )
				{

				return "GET "
					+ path
					+ " HTTP/1.1\r\nHost: "
					+ host
					+ "\r\nUser-Agent: " + dex::RobotTxt::userAgent + "\r\n"
					+ "Accept: */*\r\n"
					+ "Accept-Encoding: identity\r\n"
					+ "Connection: close\r\n\r\n";
				}

			// filteredHeader is how we tell if we are in the header of the response, or in the body
			//		If we're in the response we're looking for specific fields, otherwise we're printing
			// res is how we return any information back to the callee. If we get a redirect for example,
			//		the URL we're being redirected to will be contained within res.
			static void receive( const char *buffer, unsigned bytes, dex::vector< char > &response )
				{
				for ( unsigned i = 0;  i < bytes;  ++i )
					response.pushBack( buffer[ i ] );
				}

			static int parseResponse( dex::vector< char > &response, dex::string &result, bool isRobot = false )
				{
				dex::string toSearch( response.cbegin( ), response.cend( ) );

				int endHeader;
				int startContent;
				int location = toSearch.find( "HTTP/1.1 " );
				if ( location != -1 )
					{
					int returnValue = 0;
					char statusCode[ 4 ] = { toSearch[ location + 9 ], toSearch[ location + 10 ],
							toSearch[ location + 11 ], '\0' };
					int status = ( statusCode[ 0 ] - '0' ) * 100 +
							( statusCode[ 1 ] - '0' ) * 10 +
							( statusCode[ 2 ] - '0' );
					// If the status code starts with 2, it's a valid response
					if ( statusCode[ 0 ] == '2' )
						{
						endHeader = toSearch.find( "\r\n\r\n" );
						if ( endHeader == -1 )
							{
							result = toSearch;
							return NO_HEADER_END_ERROR;
							}
						// If there is a Content-Type field in the header
						if ( !isRobot )
							{
							int contentType = toSearch.find( "Content-Type:" );
							if ( contentType < endHeader && contentType >= 0 )
								{
								int contentTypeStart = contentType + 14;
								int contentTypeEnd = toSearch.find( "\n", contentTypeStart );
								string content = toSearch.substr( contentTypeStart, contentTypeEnd - contentTypeStart );
								if ( content.find( "text/html" ) == dex::string::npos )
									{
									returnValue = NOT_HTML;
									}

								}
							}
						startContent = endHeader + 4;
						result = toSearch.substr( startContent, toSearch.size( ) - startContent );
						return returnValue;
						}
					// If the status code starts with a 3, it's a redirect
					else
						{
						if ( statusCode[ 0 ] == '3' )
							{
							// Copy the locaton into res and return the status code
							int locationStart = toSearch.find( "Location: " );
							if ( locationStart == -1 )
								{
								result = toSearch;
								return NO_LOCATION_ERROR;
								}
							auto beginRedirect = toSearch.begin( ) + locationStart + 10;
							auto endRedirect = beginRedirect;
							for ( ; *endRedirect != '\r' ;  ++endRedirect );
							result = dex::string( beginRedirect, endRedirect );
							return status;
							}
						// Otherwise we don't get a good response and need to return an error
						else
							{
							// Copy the HTTP response into res, then return the status code
							result = toSearch;
							return status;
							}
						}
					}
				return NO_RESPONSE_ERROR;
				}

			static int connectPage( Url url, dex::string &result, bool protocol, bool isRobot = false )
				{
				int connectResult = 0;
				struct addrinfo *address;
				int socketFD; // HTTP
				tls *ctx; // HTTPS
				// setup is different based on HTTP or HTTPS
				if ( protocol == HTTP )
					{
					// Create a TCP/IP socket.
					struct addrinfo hints;
					memset( &hints, 0, sizeof( hints ) );
					hints.ai_family = AF_INET;
					hints.ai_socktype = SOCK_STREAM;
					hints.ai_protocol = IPPROTO_TCP;

					int getaddrresult = getaddrinfo( url.getHost( ).cStr( ), !url.getPort( ).empty( ) ? url.getPort( ).cStr( ) : "80", &hints, &address );
					if ( getaddrresult != 0 )
						{
						result = "Could not resolve DNS\n";
						return RESOLVE_DNS_ERROR;
						}
					socketFD = socket( address->ai_family, address->ai_socktype, address->ai_protocol );
					if ( socketFD == -1 )
						{
						result = "Could not connect to socket\n";
						return SOCKET_CONNECTION_ERROR;
						}

					// set timeout on recv
					struct timeval tv;
					tv.tv_sec = 5;
					tv.tv_usec = 0;
					int option = setsockopt( socketFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv );
					if ( option == -1 )
						return SOCKET_CONNECTION_ERROR;
					// Connect the socket to the host address.
					connectResult = connect( socketFD, address->ai_addr, address->ai_addrlen);
					}
				else
					{
					if ( protocol != HTTPS )
						{
						result = "Disallowed protocol\n";
						return PROTOCOL_ERROR;
						}
					// setup libressl stuff
					tls_init( );
					tls_config * config = tls_config_new( );
					if ( !config )
						{
						return TLS_CONFIG_ERROR;
						}
					ctx = tls_client( );
					if ( !ctx )
						{
						return TLS_CONFIG_ERROR;
						}
					int err = tls_configure( ctx, config );
					if ( err == -1 )
						{
						return TLS_CONFIG_ERROR;
						}
					tls_config_free( config );
					const char * error = tls_config_error( config );
					if ( error != NULL )
						{
						result = error;
						return TLS_CONFIG_ERROR;
						}
					error = tls_error( ctx );
					if ( error != NULL )
						{
						result = error;
						return RESPONSE_ERROR;
						}
					// Connect to the host address
					connectResult = tls_connect( ctx, url.getHost( ).cStr( ), !url.getPort( ).empty( ) ? url.getPort( ).cStr( ) : "443" );
					error = tls_config_error( config );
					if ( error != NULL )
						{
						result = error;
						return TLS_CONFIG_ERROR;
						}
					error = tls_error( ctx );
					if ( error != NULL )
						{
						result = error;
						return RESPONSE_ERROR;
						}
					}

				if ( connectResult == -1 )
					{
					result = "Could not connect to " + url.getHost( ) + " on port " + url.getPort( ) + " using ";
					result += ( protocol == HTTP ? "http\n" : "https\n" );
					return CONNECTION_ERROR;
					}

				dex::string getMessage = makeGetMessage( url.getPath( ), url.getHost( ) );

				int sendGETResult = 0;
				if ( protocol == HTTP )
					{
					sendGETResult = send( socketFD, getMessage.cStr( ), getMessage.length( ), 0 );
					}
				else
					{
					sendGETResult = tls_write( ctx, getMessage.cStr( ), getMessage.length( ) );
					}

				if ( sendGETResult == -1 )
					{
					result =  "Failure in sending\n";
					return SENDING_ERROR;
					}

				char buffer[ 10240 ];
				int bytes;
				size_t totalBytes = 0;
				dex::vector< char > response;
				response.reserve( maxPageSize );
				if ( protocol == HTTP )
					{
					while ( ( bytes = recv( socketFD, buffer, sizeof( buffer ), 0 ) ) > 0 )
						{
						totalBytes += bytes;
						if ( totalBytes > maxPageSize )
							{
							return RESPONSE_TOO_LARGE;
							}
						receive( buffer, bytes, response );
						}


					// Close the socket and free the address info structure.
					close( socketFD );
					freeaddrinfo( address );
					}
				else
					{
					while ( ( bytes = tls_read( ctx, buffer, sizeof( buffer ) ) ) > 0 )
						{
						totalBytes += bytes;
						if ( totalBytes > maxPageSize )
							{
							return RESPONSE_TOO_LARGE;
							}
						receive( buffer, bytes, response );
						}


					// Close SSL connection
					tls_close( ctx );
					tls_free( ctx );
					}
				if ( bytes == -1 )
					{
					return RESPONSE_ERROR;
					}
				int errorCode = parseResponse( response, result, isRobot );
				return errorCode;
				}

		public:
			// Function used for crawling URLs. bePolite should ALWAYS be on, only turned off for testing.
			static int crawlUrl( const Url &url, dex::string &result, dex::robotsMap &robotsCache, bool bePolite = true )
				{
				int protocol = ( url.getService( ) == "https" ) ? HTTPS : HTTP;

				if ( !bePolite )
					{
					std::cerr << "You are not being polite, you better be testing something" << std::endl;
					}
				else
					{
					// Check to see if we have a robot object for the domain we're crawling

					int startResult = robotsCache.startRobotCreation( url.getDomain( ), url.getPath( ) );
					if ( startResult == -2 )
						{
						result = "Different thread making the robots object for " + url.getDomain( );
						return PUT_BACK_IN_FRONTIER;
						}
					if ( startResult == 1 )
						{
						result = "Not ready to visit " + url.getDomain( );
						return POLITENESS_ERROR;
						}
					if ( startResult == 2 )
						{
						result = "path " + url.getPath( ) + " is disallowed for domain " + url.getDomain( );
						return DISALLOWED_ERROR;
						}

					// Robots.txt does not exist for this domain yet
					if ( startResult == -1 )
						{
						// visit robots.txt until we no longer receieve a redirect or until we've
						// gone too long and we need to kill this redirect chain.
						Url robotUrl( url );
						robotUrl.setPath( "/robots.txt" );

						dex::string urlToVisit = robotUrl.completeUrl( );

						int errorCode = 300;
						int numRedirectsFollowed = 0;
						for ( ;  numRedirectsFollowed < 10 && errorCode / 100 == 3;  ++numRedirectsFollowed )
							{
							Url robotUrl( urlToVisit.cStr( ) );
							protocol = ( robotUrl.getService( ) == "http" ) ? HTTP : HTTPS;
							errorCode = connectPage( robotUrl, result, protocol, true );
							urlToVisit = result;
							}
						// If our error code is 404, the path does not exist and we create a default robots.txt object
						dex::RobotTxt robot;
						if ( errorCode >= 400 && errorCode < 500 )
							{
							// Create Default
							robot = dex::RobotTxt( url.getDomain( ) );
							}
						// If there was an error, we need to abort and return the error
						else
							{
							if ( errorCode != 0 )
								{
								int err = robotsCache.writeUnlock( url.getDomain( ) );
								if ( err != 0 )
									return dex::LOCKING_ERROR;
								return errorCode;
								}
							// Create new RobotsTxt
							if ( errorCode == 0 )
								{
								dex::string robotsTxtInformation = result;
								robot = dex::RobotTxt( url.getDomain( ), robotsTxtInformation );
								}
							}
						int pathResult = robot.visitPathResult( url.getPath( ) );
						robot.updateLastVisited( );
						int err = robotsCache.finishRobotCreation( url.getDomain( ), robot );
						if ( err != 0 )
							return dex::LOCKING_ERROR;
						if ( pathResult == 1 )
							{
							result = "Not ready to visit " + url.getDomain( );
							return POLITENESS_ERROR;
							}
						if ( pathResult == 2 )
							{
							result = "path " + url.getPath( ) + " is disallowed for domain " + url.getDomain( );
							return DISALLOWED_ERROR;
							}
						}
					}
				result = "";
				protocol = ( url.getService( ) == "http" ) ? HTTP : HTTPS;
				int errorCode = connectPage( url, result, protocol, false );
				return errorCode;
				}
			// Used for testing our connectPage function
			static int testConnect( Url url, dex::string &result, bool protocol )
				{
				return connectPage( url, result, protocol, false );
				}
		};
	}

#endif
