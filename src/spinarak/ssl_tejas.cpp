#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>

#include <iostream>
#include <string>
#include <sstream>
#include <assert.h>
#include <string.h>

using std::cout;
using std::endl;
using std::cerr;

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

std::string function ( const char* argv)
   {
   // Parse the URL
   ParsedUrl url( argv );

   // Get the host address.
   struct addrinfo* address, hints;
   memset( &hints, 0, sizeof( hints) );
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   int getaddrResult = getaddrinfo( url.Host, *url.Port ? url.Port : "443",
                                    &hints, &address);

   // Create a TCP/IP socket.
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if ( sockfd == -1 )
      {
      std::cerr << "Error creating socket" << std::endl;
      return "1";
      }
   
   //cout << address->ai_addr << endl;

   // Connect the socket to the host address.
   if ( connect( sockfd, address->ai_addr, sizeof( struct sockaddr ) ) != 0 )
      {
      std::cerr << "Error connecting to stream socket" << std::endl;
      return "1";
      }


   // Build an SSL layer and set it to read/write
   // to the socket we've connected.
   SSL_library_init( );
   SSL_CTX *ctx = SSL_CTX_new( SSLv23_method( ) );
   assert( ctx );
   SSL *ssl = SSL_new( ctx );
   assert( ssl );
   SSL_set_fd( ssl, sockfd );
   int sslConnectionResult = SSL_connect( ssl );
   if ( sslConnectionResult != 1) 
      {
      std::cerr << "Error during SSL_connect" << std::endl;
      return "1";
      }
   
   // Send a GET message.
   std::string request = "GET /" + std::string(url.Path) + "/ HTTP/1.1\r\nHost: "
   + std::string(url.Host) + "\r\nUser-Agent: LinuxGetSSL/2.0 tjha@umich.edu (Linux)"
+ "\r\nAccept: */*\r\nConnection: close\r\n\r\n";

   if ( SSL_write( ssl , request.c_str( ), request.length( )) !=  
        ( int ) request.length( ) )
      {
      std::cerr << "Error sending GET request" << std::endl;
      return "1";
      }

   // Read from the socket until there's no more data, copying it to
   // stdout.

   int bytes;
   char buffer[ 1024 ];
   char tempBuffer[ 21 ];
   std::string newURL = "";
   bool first = true;
   if ( ( bytes = SSL_read( ssl, buffer, sizeof( buffer )) ) > 0 )
      {
      char* p = strstr(buffer, "301 Moved Permanently");
      if (p)
         {
         p = strstr(buffer, "https://");
         char* end = strstr(buffer, "Content-Length");
         while (p + 2 != end)
            {
            newURL += *p;
            p++;
            }         
         close ( sockfd );
         freeaddrinfo ( address );
         return newURL;
         }
      p = strstr(buffer, "\r\n\r\n");
      p = p + 4;
      if(p)
        {
        bytes -= p - buffer;
        char* start = p;
        write(1, start, bytes);
        if (bytes > 0) first = false;
        }
      //write( 1, buffer, bytes);
      }

   char buffer2[ 1024 ];
   while ( ( bytes = SSL_read( ssl, buffer2, sizeof( buffer2 )) ) > 0 )
      {
      char* st = buffer2;
      if (first)
         {
         st = strstr(buffer2, "<");
         bytes -= st - buffer2;
         first = false;
         }
      char* end = strstr(buffer2, "</html>");
      //end += 7;
      if (end)
         {
         bytes = end - st + 8;
         write ( 1, st, bytes);
         break;
         }
      write( 1, st, bytes );
      }


   // Close the socket and free the address info structure.
   close( sockfd );
   freeaddrinfo ( address );

   return newURL;
   
   }

int main( int argc, char **argv )
   {

   if ( argc != 2 )
      {
      std::cerr << "Usage: " << argv[ 0 ] << " url" << std::endl;
      return 1;
      }
   std::string str = function(argv[1]);
   if ( str == "1" ) {
      return 1;
   } else if ( str != "") {
      function(str.c_str());
   }
   return 0;
   }
