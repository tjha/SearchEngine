#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <tls.h>
#include "robots.hpp"
#include <iostream>
#include <string.h>

using std::cout;
using std::endl;

class ParsedUrl
   {
   public:
      const char *CompleteUrl;
      char *Service, *Host, *Port, *Path;

      ParsedUrl( const char *url )
         {
         // Assumes url points to static text but
         // does not check.

         CompleteUrl = url;

         pathBuffer = new char[ strlen( url ) + 1 ];
         const char *f;
         char *t;
         for ( t = pathBuffer, f = url; *t++ = *f++; )
            ;

         Service = pathBuffer;

         const char Colon = ':', Slash = '/';
         char *p;
         for ( p = pathBuffer; *p && *p != Colon; p++ )
            ;

         if ( *p )
            {
            // Mark the end of the Service.
            *p++ = 0;

            if ( *p == Slash )
               p++;
            if ( *p == Slash )
               p++;

            Host = p;

            for ( ; *p && *p != Slash && *p != Colon; p++ )
               ;

            if ( *p == Colon )
               {
               // Port specified.  Skip over the colon and
               // the port number.
               *p++ = 0;
               Port = +p;
               for ( ; *p && *p != Slash; p++ )
                  ;
               }
            else
               Port = p;

            if ( *p )
               // Mark the end of the Host and Port.
               *p++ = 0;

            // Whatever remains is the Path.
            Path = p;
            }
         else
            Host = Path = p;
         }

      ~ParsedUrl( )
         {
         delete[ ] pathBuffer;
         }

   private:
      char *pathBuffer;
   };

int main ( int argc, char ** argv) {
   // check if there is a domain to get
   if ( argc != 2 ) {
      cerr << "Usage: " << argv[ 0 ] << " url\n";
      return 1;
   }

   ParsedUrl url( argv[1] );

   tls_init( );
   tls_config * config = tls_config_new( );

   // Get the host address.
   /* struct addrinfo *address, hints;
   memset( &hints, 0, sizeof( hints ) );
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   //int getaddrResult = getaddrinfo( url.Host, "443", 
                                    &hints, &address );

   if ( getaddrResult == 1 ) 
      {
      cerr << "Could not resolve DNS\n";
      exit( 1 );
      }*/

   tls *ctx = tls_client( );
   tls_configure( ctx, config );

   cout << url.Host << "\n";
   cout << url.Port << "\n";
   int connectRes = tls_connect( ctx, url.Host, *url.Port ? url.Port : "443" );

   if ( connectRes == -1 )
      {
      cerr << "Could not connect to Host\n";
      exit( 1 );
      }

   std::string getMessage = "GET /" 
      + (std::string)url.Path 
      + " HTTP/1.1\r\nHost: "
      + (std::string)url.Host
      + "\r\nUser-Agent: LinuxGetUrl/2.0 jhirshey@umich.edu (Linux)\r\n"
      + "Accept: */*\r\n"
      + "Accept-Encoding: identity\r\n"
      //+ "Content-Length: 0\r\n"
      + "Connection: close\r\n\r\n";
   
   tls_write( ctx, getMessage.c_str( ), getMessage.length( ) );

   int status;
   char *newPath;
   char buffer[ 4096 ];
   int bytes;
   bool filteredHeader = false;
   int count = 0;
   //bytes = tls_read( ctx, buffer, sizeof( buffer ) );
   while ( ( bytes = tls_read( ctx, buffer, sizeof( buffer ) ) ) > 0 )
      {
      if ( filteredHeader)          {
         write( 1, buffer, bytes );
         }
      else 
         {
         if ( strstr( buffer, "\r\n\r\n" ) != nullptr )
            {
            // parse the http response code
            char statusCode[3];
            statusCode[0] = buffer[ 9 ];
            statusCode[1] = buffer[ 10 ];
            statusCode[2] = buffer[ 11 ];
            status = atoi( statusCode );

            // replace the url.Path if site moved permanently
            if ( status == 301 )
               {
               char *location = strstr( buffer, "Location: " ) + 10;
               char *endPath = location;
               while ( *endPath != '\\' )
                  {
                  ++endPath;
                  }
               memcpy( newPath, location, 
                       ( endPath - location ) * sizeof( char ) );

               std::cout << "Rerouted, adding to frontier " << newPath << "\n";
               return 0;
               }
            else 
               {
               std::cout << status << std::endl;
               char *headerEnd = strstr( buffer, "\r\n\r\n" );
               size_t restOfMessage = bytes - (headerEnd - buffer);
               write( 1, headerEnd, restOfMessage );
               filteredHeader = true;
               }
            }
         }
      }

   if ( !filteredHeader && bytes == 0 )
      {
      cerr << "No response from TLS_READ of:\n" << getMessage;
      exit(1);
      }

   tls_close( ctx );
   tls_free( ctx );
   return 0;
}

//dex::RobotTxt hello = dex::RobotTxt( url, 3 );
   /*
   cout << hello;
   cout << hello.canVisitDomain( ) << endl;
   sleep(2);
   cout << hello.canVisitDomain( ) << endl;
   sleep(2);
   cout << hello.canVisitDomain( ) << endl;
   hello.updateLastVisited( );
   cout << hello;
   */
