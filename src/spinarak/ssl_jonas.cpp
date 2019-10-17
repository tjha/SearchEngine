#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <stdio.h>

#include <iostream>

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

void PrintAddress( const sockaddr_in *s, const size_t saLength )
   {
   const struct in_addr *ip = &s->sin_addr;
   uint32_t a = ntohl( ip->s_addr );

   std::cout << "Host address length = " << saLength << " bytes\n";
   std::cout << "Family = " << s->sin_family <<
      ", port = " << ntohs( s->sin_port ) <<
      ", address = " << ( a >> 24 ) << '.' << 
         ( ( a >> 16 ) & 0xff ) << '.' <<
         ( ( a >> 8 ) & 0xff ) << '.' << 
         ( a & 0xff ) << "\n";
   }

int main( int argc, char **argv )
   {

   if ( argc != 2 )
      {
      std::cerr << "Usage: " << argv[ 0 ] << " url" << std::endl;
      return 1;
      }

   // Parse the URL
   ParsedUrl url( argv[ 1 ] );

   // Get the host address.
   struct addrinfo *address, hints;
   memset( &hints, 0, sizeof( hints ) );
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   int getaddrResult = getaddrinfo( url.Host, *url.Port ? url.Port : "443", 
                                    &hints, &address );

   if ( getaddrResult == 1 ) 
      {
      std::cerr << "Could not resolve DNS\n";
      exit(1);
      }
                
   //PrintAddress( ( sockaddr_in * )address->ai_addr, 
   //              sizeof( struct sockaddr ) );

   // Create a TCP/IP socket.

   int s = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
   assert( s != -1 );

   // Connect the socket to the host address.

   int connectResult = connect( s, address->ai_addr,
         sizeof( struct sockaddr ) );
   assert( connectResult == 0 );
   
   SSL_library_init( );
   SSL_CTX *ctx = SSL_CTX_new( DTLS_method( ) );

   int socketFD = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

   SSL *ssl = SSL_new( ctx );
   SSL_set_fd( ssl, socketFD );
   SSL_connect( ssl );


   int status = 1;
   while ( status != 200 ) 
      {
      // Send a GET message.
      std::string getMessage = "GET /" 
         + (std::string)url.Path 
         + " HTTP/1.1\r\nHost: "
         + (std::string)url.Host
         + "\r\nUser-Agent: LinuxGetUrl/2.0 jhirshey@umich.edu (Linux)\r\n"
         + "Accept: */*\r\n"
         + "Accept-Encoding: identity\r\n"
         //+ "Content-Length: 0\r\n"
         + "Connection: close\r\n\r\n";
      
      send( s, getMessage.c_str(), getMessage.length( ), 0);

      // Read from the socket until there's no more data, copying it to
      // stdout.
      char buffer[ 1024 ];
      int bytes;
      bool filteredHeader = false;
      while ( ( bytes = recv( s, buffer, sizeof( buffer ), 0 ) ) > 0 )
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
                  memcpy( url.Path, location, 
                          ( endPath - location ) * sizeof( char ) );
                  std::cout << "urlPath " << url.Path << std::endl;
                  // strncpy( location, url.Path, 
                  //         ( endPath - location ) * sizeof( char ) );
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
      }



   // Close the socket and free the address info structure.

   SSL_shutdown( ssl );
   SSL_free( ssl );
   close( socketFD );
   close( s );
   SSL_CTX_free( ctx );
   freeaddrinfo( address );
   }
