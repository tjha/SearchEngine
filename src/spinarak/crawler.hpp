#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <tls.h>
#include "robots.hpp"
#include "../utils/exception.hpp"
#include "../utils/unorderedMap.hpp"
#include "../utils/file.hpp"
#include "url.hpp"
#include <iostream>

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
// 2019-10-29: Turned ParsedUrl into Url, uses strings instead of char*: combsc
// 2019-10-28: Cleaned up file, got working with HTTP and HTTPS: combsc
// 2019-10-20: Init Commit: Jonas

using dex::string;
using dex::RobotTxt;
using dex::writeToFile;

namespace dex
	{
	enum crawlerError
		{
		resolveDnsError = -1,
		connectionError = -2,
		sendingError = -3,
		noResponseError = -4,
		politenessError = -5,
		noLocationError = -6,
		noHeaderEndError = -7,
		lastError = -8
		};
	
	class crawler
		{
		// This is a class that is stateless, so every function within is static.
		// We're keeping it a class so that we can make the interface clear when
		// you're using it.
		private:

			static bool isError( const int &in )
				{
				return in >= politenessError && in <= lastError;
				}
			static string makeGetMessage( const string &path, const string &host )
				{
				return "GET /" 
					+ path
					+ " HTTP/1.1\r\nHost: "
					+ host
					+ "\r\nUser-Agent: LinuxGetUrl/2.0 jhirshey@umich.edu (Linux)\r\n"
					+ "Accept: */*\r\n"
					+ "Accept-Encoding: identity\r\n"
					+ "Connection: close\r\n\r\n";
				}

			// filteredHeader is how we tell if we are in the header of the response, or in the body
			//		If we're in the response we're looking for specific fields, otherwise we're printing
			// res is how we return any information back to the callee. If we get a redirect for example,
			//		the URL we're being redirected to will be contained within res.
			static int receive( const char *buffer, int bytes, bool &filteredHeader, string &res, int fileToWrite )
				{
				if ( filteredHeader )
					{
					write( fileToWrite , buffer, bytes );
					}
				else 
					{
					write( fileToWrite , buffer, bytes );
					string toSearch = buffer;
					int location = toSearch.find( "HTTP/1.1 " );
					if ( location != -1 )
						{
						char statusCode[ 4 ] = { buffer[ location + 9 ], buffer[ location + 10 ], 
								buffer[ location + 11 ], '\0' };
						int status = ( statusCode[ 0 ] - '0' ) * 100 +
								( statusCode[ 1 ] - '0' ) * 10 +
								( statusCode[ 2 ] - '0' );
						// If the status code starts with 2, it's a valid response
						if ( statusCode[ 0 ] == '2' )
							{
							int headerEnd = toSearch.find( "\r\n\r\n" );
							// What should we do if we can't find the header end? Honestly don't know...
							if ( headerEnd == -1 )
								{
								res = "Can't find the header end";
								return noHeaderEndError;
								}
							size_t restOfMessage = bytes - headerEnd;
							const char* startOfMessage = buffer + headerEnd;
							write( fileToWrite, startOfMessage, restOfMessage );
							filteredHeader = true;
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
									res = "Can't find the location in the header";
									return noLocationError;
									}
								const char *beginRedirect = buffer + locationStart + 10;
								const char *endRedirect = beginRedirect;
								for ( ; *endRedirect != '\r' ;  ++endRedirect );
								res = string( beginRedirect, endRedirect );
								return status;
								}
							// Otherwise we don't get a good response and need to return an error
							else
								{
								// Copy the HTTP response into res, then return the status code
								int headerEnd = toSearch.find( "\r\n\r\n" );
								res = string( buffer, buffer + headerEnd );
								return status;
								}
							}
						}
					}
				return 0;
				}

			static int httpConnect( Url url, string &res, int fileToWrite )
				{
				// Create a TCP/IP socket.
				struct addrinfo *address, hints;
				memset( &hints, 0, sizeof( hints ) );
				hints.ai_family = AF_INET;
				hints.ai_socktype = SOCK_STREAM;
				hints.ai_protocol = IPPROTO_TCP;

				int getaddrresult = getaddrinfo( url.host.cStr( ), url.port != "" ? url.port.cStr( ) : "80", &hints, &address );
				if ( getaddrresult == 1 ) 
					{
					res = "Could not resolve DNS\n";
					return resolveDnsError;
					}

				int socketFD = socket( address->ai_family, address->ai_socktype, address->ai_protocol );

				// Connect the socket to the host address.
				int connectRes = connect( socketFD, address->ai_addr, address->ai_addrlen);
				if ( connectRes == -1 )
					{
					res = "Could not connect to Host\n";
					return connectionError;
					}

				string getMessage = makeGetMessage( url.path, url.host );

				int error = send( socketFD, getMessage.cStr( ), getMessage.length( ), 0 );
				// Read from the socket until there's no more data, copying it to
				// stdout.
				if (error == -1)
					{
					res =  "Failure in sending\n";
					return sendingError;
					}

				char buffer[ 10240 ];
				int bytes;
				bool filteredHeader = false;
				while ( ( bytes = recv( socketFD, buffer, sizeof( buffer ), 0 ) ) > 0 )
					{
					int errorCode = receive( buffer, bytes, filteredHeader, res, fileToWrite );
					if ( errorCode != 0 )
						{
						return errorCode;
						}
					}

				if ( !filteredHeader && bytes == 0 )
					{
					res = "No response from TLS_READ of:\n" + getMessage;
					return noResponseError;
					}

				// Close the socket and free the address info structure.

				close( socketFD );
				freeaddrinfo( address );
				
				return 0;
				}

			static int httpsConnect( Url url, string &res, int fileToWrite )
				{
				// setup libressl stuff
				tls_init( );
				tls_config * config = tls_config_new( );  
				tls *ctx = tls_client( );
				tls_configure( ctx, config );

				// Connect to the host address
				int connectRes = tls_connect( ctx, url.host.cStr( ), url.port != "" ? url.port.cStr( ) : "443" );
				if ( connectRes == -1 )
					{
					res = "Could not connect to Host\n";
					return connectionError;
					}

				string getMessage = makeGetMessage( url.path, url.host );
				
				int error = tls_write( ctx, getMessage.cStr( ), getMessage.length( ) );
				if ( error == -1 )
					{
					res =  "Failure in sending\n";
					return sendingError;
					}
				
				char buffer[ 10240 ];
				int bytes;
				bool filteredHeader = false;
				while ( ( bytes = tls_read( ctx, buffer, sizeof( buffer ) ) ) > 0 )
					{
					int errorCode = receive( buffer, bytes, filteredHeader, res, fileToWrite );
					if ( errorCode != 0 )
						{
						return errorCode;
						}
					}

				if ( !filteredHeader && bytes == 0 )
					{
					res = "No response from TLS_READ of:\n" + getMessage;
					return noResponseError;
					}

				tls_close( ctx );
				tls_free( ctx );

				return 0;
				}

		
		public:
			// Function used for crawling URLs. bePolite should ALWAYS be on, only turned off for testing.
			static int crawlUrl( Url url, int contentFile, int robotFile, string &res, dex::unorderedMap < string, RobotTxt > &robots, bool bePolite = true )
				{
				if ( !bePolite )
					{
					std::cerr << "You are not being polite, you better be testing something" << std::endl;
					}
				else
					{
					RobotTxt robot;
					// Check to see if we have a robot object for the domain we're crawling
					if ( robots.count( url.host ) < 1 )
						{
						// visit robots.txt
						Url robotUrl( url );
						robotUrl.path = "robots.txt";

						int a;
						if ( robotUrl.service == "https" )
							{
							a = httpsConnect( robotUrl, res, robotFile );
							}
						else
							{
							a = httpConnect( robotUrl, res, robotFile );
							}

						// If our error code is 404, the path does not exist and we create a default robots.txt object
						if ( a == 404 )
							{
							// Create Default
							RobotTxt newRobot( url.host );
							robot = newRobot;
							}
						// If there was an error, we need to abort and return the error
						if ( a != 0 )
							{
							return a;
							}
						else
							{
							// Create new RobotsTxt
							string robotsTxtInformation = "";
							RobotTxt newRobot( url.host, robotsTxtInformation );
							robot = newRobot;
							}
						}
					else
						{
						robot = robots[ url.host ];
						}

					if ( !robot.canVisitPath( url.path ) )
						{
						res = "Cannot visit path due to robots object";
						return politenessError;
						}
					
					robot.updateLastVisited( );
					// Update the robot in our cache
					robots[ url.host ] = robot;
					}
				

				if ( url.service == "https" )
					{
					int a = httpsConnect( url, res, contentFile );
					return a;
					}
				else
					{
					int a = httpConnect( url, res, contentFile );
					return a;
					}
				}
		};
	}
