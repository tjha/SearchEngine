#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <tls.h>
#include "robots.hpp"
#include "../utils/exception.hpp"
#include "../utils/basicString.hpp"
#include <iostream>

// 2019-10-30: Added everything into dex namespace and created a crawler object
//             with a cleaner interface for using: combsc
// 2019-10-29: Turned ParsedUrl into Url, uses strings instead of char*: combsc
// 2019-10-28: Cleaned up file, got working with HTTP and HTTPS: combsc
// 2019-10-20: Init Commit: Jonas

using dex::string;

namespace dex
   {
   class Url
      {
      public:
         string CompleteUrl;
         string Service, Host, Port, Path;

         Url( const char *url )
            {
            // Assumes url points to static text but
            // does not check.

            CompleteUrl = url;
            int endService = CompleteUrl.find( "://");

            if ( endService == -1 )
               {
               std::cerr << "Couldn't find :// in " << CompleteUrl << std::endl;
               throw dex::invalidArgumentException( );
               }
            
            Service = CompleteUrl.substr( 0, endService );
            int beginHost = endService + 3;
            int endHost = CompleteUrl.findFirstOf( "/:", beginHost );
            // If there is no path or port, the end of the host is the end of the string.
            if ( endHost == -1 )
               endHost = CompleteUrl.size( );
            Host = CompleteUrl.substr( beginHost, endHost - beginHost );

            // Now we check to see if the port is specified
            int beginPort = endHost + 1;
            int endPort = endHost;
            if ( CompleteUrl[ endHost ] == ':' )
               {
               endPort = CompleteUrl.find( "/", beginPort );
               // If there is no path, the end of the port is the end of the string.
               if ( endPort == -1 )
                  endPort = CompleteUrl.size( );
               Port = CompleteUrl.substr( beginPort, endPort - beginPort );
               }
            else
               {
               Port = "";
               }
            int beginPath = endPort + 1;
            if ( beginPath > CompleteUrl.size( ) )
               {
               Path = "";
               }
            else
               {
               Path = CompleteUrl.substr( beginPath, CompleteUrl.size( ) - beginPath );
               }
            }
      };

   class crawler
      {
      private:


         int receive( char *buffer, int bytes, bool &filteredHeader, string &res, int fileToWrite )
            {
            if ( filteredHeader )
               {
               write( fileToWrite , buffer, bytes );
               }
            else 
               {
               char *p = strstr(buffer, "HTTP/1.1 ");
               if ( p )
                  {
                  char statusCode[4];
                  statusCode[ 0 ] = p[ 9 ];
                  statusCode[ 1 ] = p[ 10 ];
                  statusCode[ 2 ] = p[ 11 ];
                  statusCode[ 3 ] = '\0';
                  int status = ( statusCode[ 0 ] - '0' ) * 100 +
                        ( statusCode[ 1 ] - '0' ) * 10 +
                        ( statusCode[ 2 ] - '0' );
                  // If the status code starts with 2, it's a valid response
                  if ( statusCode[ 0 ] == '2' )
                     {
                     char *headerEnd = strstr( buffer, "\r\n\r\n" );
                     size_t restOfMessage = bytes - (headerEnd - buffer);
                     write( fileToWrite, headerEnd, restOfMessage );
                     filteredHeader = true;
                     }
                  // If the status code starts with a 3, it's a redirect
                  else if ( statusCode[ 0 ] == '3' )
                     {
                     // Copy the locaton into res and return the status code
                     char *location = strstr( buffer, "Location: " ) + 10;
                     char *endPath = location;
                     for ( int i = 0; *endPath != '\r' ;  ++i ){
                        endPath++;
                     }
                     endPath[1] = '\0';
                     res = location;
                     return status;
                     }
                  // Otherwise we don't get a good response and need to return an error
                  else
                     {
                     // Copy the HTTP response into res, then return the status code
                     char *headerEnd = strstr( buffer, "\r\n\r\n" );
                     headerEnd[0] = '\0';
                     res = p;
                     return status;
                     }
                  }
               }
            return 0;
            }

         int httpConnect( Url url, string &res, int fileToWrite )
            {
            res = "";
            // Create a TCP/IP socket.

            struct addrinfo *address, hints;
            memset( &hints, 0, sizeof( hints ) );
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            int getaddrresult = getaddrinfo( url.Host.cStr( ), url.Port != "" ? url.Port.cStr( ) : "80", &hints, &address );
            if ( getaddrresult == 1 ) 
               {
               res = "Could not resolve DNS\n";
               return -1;
               }

            int socketFD = socket( address->ai_family, address->ai_socktype, address->ai_protocol );

            // Connect the socket to the host address.
            int connectRes = connect( socketFD, address->ai_addr, address->ai_addrlen);
            if ( connectRes == -1 )
               {
               res = "Could not connect to Host\n";
               return -1;
               }

            string getMessage = "GET /" 
               + url.Path
               + " HTTP/1.1\r\nHost: "
               + url.Host
               + "\r\nUser-Agent: LinuxGetUrl/2.0 jhirshey@umich.edu (Linux)\r\n"
               + "Accept: */*\r\n"
               + "Accept-Encoding: identity\r\n"
               + "Connection: close\r\n\r\n";

            int err = send( socketFD, getMessage.cStr( ), getMessage.length( ), 0 );
            // Read from the socket until there's no more data, copying it to
            // stdout.
            if (err == -1)
               {
                  res =  "Failure in sending\n";
                  return -1;
               }

            char buffer[ 10240 ];
            int bytes;
            bool filteredHeader = false;
            while ( ( bytes = recv( socketFD, buffer, sizeof( buffer ), 0 ) ) > 0 )
               {
               int ret = receive( buffer, bytes, filteredHeader, res, fileToWrite );
               if ( ret != 0 )
                  {
                  return ret;
                  }
               }

            if ( !filteredHeader && bytes == 0 )
               {
               res = "No response from TLS_READ of:\n" + getMessage;
               return -1;
               }

            // Close the socket and free the address info structure.

            close( socketFD );
            freeaddrinfo( address );
            
            return 0;
            }

         int httpsConnect( Url url, string &res, int fileToWrite )
            {
            res = "";
            // setup libressl stuff
            tls_init( );
            tls_config * config = tls_config_new( );  
            tls *ctx = tls_client( );
            tls_configure( ctx, config );

            // Connect to the host address
            int connectRes = tls_connect( ctx, url.Host.cStr( ), url.Port != "" ? url.Port.cStr( ) : "443" );
            if ( connectRes == -1 )
               {
               res = "Could not connect to Host\n";
               return -1;
               }

            string getMessage = "GET /" 
               + url.Path
               + " HTTP/1.1\r\nHost: "
               + url.Host
               + "\r\nUser-Agent: LinuxGetUrl/2.0 jhirshey@umich.edu (Linux)\r\n"
               + "Accept: */*\r\n"
               + "Accept-Encoding: identity\r\n"
               + "Connection: close\r\n\r\n";
            
            tls_write( ctx, getMessage.cStr( ), getMessage.length( ) );

            int status;
            char *newPath;
            char buffer[ 10240 ];
            int bytes;
            bool filteredHeader = false;
            int count = 0;
            while ( ( bytes = tls_read( ctx, buffer, sizeof( buffer ) ) ) > 0 )
               {
               int ret = receive( buffer, bytes, filteredHeader, res, fileToWrite );
               if ( ret != 0 )
                  {
                  return ret;
                  }
               }

            if ( !filteredHeader && bytes == 0 )
               {
               res = "No response from TLS_READ of:\n" + getMessage;
               return -1;
               }

            tls_close( ctx );
            tls_free( ctx );

            return 0;
            }
      public:

         crawler( ) { }
         int crawlUrl( Url url, int fileToWrite, string &res )
            {
            if ( strstr( url.CompleteUrl.cStr( ), "https") )
               {
               int a = httpsConnect( url, res, fileToWrite );
               return a;
               }
            else
               {
               int a = httpConnect( url, res, fileToWrite );
               return a;
               }
            }

         int crawlUrl( string str, int fileToWrite, string &res )
            {
            return crawlUrl( Url( str.cStr( ) ), fileToWrite, res );
            }
      };
   }

int main ( int argc, char ** argv) {
   // check if there is a domain to get
   if ( argc != 2 ) {
      cerr << "Usage: " << argv[ 0 ] << " url\n";
      return 1;
   }

   int fileToWrite = 1;
   dex::Url url( argv[ 1 ] );
   dex::string res = "";
   dex::crawler spider;
   int err = spider.crawlUrl( url, fileToWrite, res);
   if ( err != 0 )
      {
      std::cout << err << std::endl;
      std::cout << res << std::endl;
      }
   
   return 0;
}