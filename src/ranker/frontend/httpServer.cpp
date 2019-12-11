#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <pthread.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <cassert>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h>

#include "basicString.hpp"

//  Multipurpose Internet Mail Extensions (MIME) types

struct MimetypeMap
   {
   const char *Extension,
      *Mimetype;
   };

const MimetypeMap MimeTable[ ] =
   {
   // List of some of the most common MIME types.
   // https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Complete_list_of_MIME_types
   ".aac",     "audio/aac",
   ".abw",     "application/x-abiword",
   ".arc",     "application/octet-stream",
   ".avi",     "video/x-msvideo",
   ".azw",     "application/vnd.amazon.ebook",
   ".bin",     "application/octet-stream",
   ".bz",      "application/x-bzip",
   ".bz2",     "application/x-bzip2",
   ".csh",     "application/x-csh",
   ".css",     "text/css",
   ".csv",     "text/csv",
   ".doc",     "application/msword",
   ".docx",    "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
   ".eot",     "application/vnd.ms-fontobject",
   ".epub",    "application/epub+zip",
   ".gif",     "image/gif",
   ".htm",     "text/html",
   ".html",    "text/html",
   ".ico",     "image/x-icon",
   ".ics",     "text/calendar",
   ".jar",     "application/java-archive",
   ".jpeg",    "image/jpeg",
   ".jpg",     "image/jpeg",
   ".js",      "application/javascript",
   ".json",    "application/json",
   ".mid",     "audio/midi",
   ".midi",    "audio/midi",
   ".mpeg",    "video/mpeg",
   ".mpkg",    "application/vnd.apple.installer+xml",
   ".odp",     "application/vnd.oasis.opendocument.presentation",
   ".ods",     "application/vnd.oasis.opendocument.spreadsheet",
   ".odt",     "application/vnd.oasis.opendocument.text",
   ".oga",     "audio/ogg",
   ".ogv",     "video/ogg",
   ".ogx",     "application/ogg",
   ".otf",     "font/otf",
   ".png",     "image/png",
   ".pdf",     "application/pdf",
   ".ppt",     "application/vnd.ms-powerpoint",
   ".pptx",    "application/vnd.openxmlformats-officedocument.presentationml.presentation",
   ".rar",     "application/x-rar-compressed",
   ".rtf",     "application/rtf",
   ".sh",      "application/x-sh",
   ".svg",     "image/svg+xml",
   ".swf",     "application/x-shockwave-flash",
   ".tar",     "application/x-tar",
   ".tif",     "image/tiff",
   ".tiff",    "image/tiff",
   ".ts",      "application/typescript",
   ".ttf",     "font/ttf",
   ".vsd",     "application/vnd.visio",
   ".wav",     "audio/x-wav",
   ".weba",    "audio/webm",
   ".webm",    "video/webm",
   ".webp",    "image/webp",
   ".woff",    "font/woff",
   ".woff2",   "font/woff2",
   ".xhtml",   "application/xhtml+xml",
   ".xls",     "application/vnd.ms-excel",
   ".xlsx",    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
   ".xml",     "application/xml",
   ".xul",     "application/vnd.mozilla.xul+xml",
   ".zip",     "application/zip",
   ".3gp",     "video/3gpp",
   ".3g2",     "video/3gpp2",
   ".7z",      "application/x-7z-compressed"
   };

const char *Mimetype( const dex::string filename )
   {
   // If a matching a extentsion is found return the corresponding MIME type.
   // Else, anything not matched is an "octet-stream", treated as an unknown binary, which can be downloaded.
   for ( size_t i = 0;  i < 66;  ++i )
      {
      const char *extension = MimeTable[ i ].Extension;
      if ( filename.rfind( extension ) == filename.size( ) - strlen( extension ) )
         return MimeTable[ i ].Mimetype;
      }
   return "application/octet-stream";
   }


// Use to determine Content Length
off_t FileSize( int f )
   {
   struct stat fileInfo;
   fstat( f, &fileInfo );
   return fileInfo.st_size;
   }

bool pathIsLegal( dex::string path )
   {
   return path.find( "/../" ) == dex::string::npos && ( path.size( ) < 3 || path.substr( path.size( ) - 3, 3) != "/.." ); // TODO ask stephen about path
   }

void *Talk( void *p )
   {
   size_t bufferSize = 10240;
   char buffer[ bufferSize ];
   int bytes;
   int socket = *( int * ) p;
   delete ( int * )p;

   dex::string request;
   bytes = recv( socket, buffer, bufferSize, 0 );
   request.append( buffer, buffer + bytes );

   if ( request.substr( 0, 3 ) != "GET" )
      return nullptr;

   size_t typeEnd = request.find( " /" ), pathEnd, protocolEnd = request.find( "\r\n" );
   pathEnd = request.find( ' ', typeEnd + 1 );
   if ( pathEnd >= protocolEnd )
      return nullptr;
   dex::string path( request.substr( typeEnd + 2, pathEnd - typeEnd - 2 ).cStr( ) );
   dex::string query;

   size_t r = path.find( "results.html");
   size_t q = path.find( "?query=");
   if ( r != dex::string::npos && q != dex::string::npos && r + 12 == q )
      {
      query = path.substr( q + 7, pathEnd - q - 7 );
      path = "results.html";
      }

   int file;
   if ( !pathIsLegal( path ) || ( file = open( path.cStr( ), O_RDONLY ) ) == -1 )
      {
      dex::string responseHeader = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
      send( socket, responseHeader.data( ), responseHeader.size( ), 0 );
      }


   // TODO: send query to query compiler
   
   // TODO: populate webpage with content


   struct stat fileInfo;
   fstat( file, &fileInfo );
   char *map = ( char * )mmap( nullptr, fileInfo.st_size, PROT_READ, MAP_PRIVATE, file, 0 );
   dex::string results;
   dex::string content;
   if ( path == "results.html" )
      {
      results = "Results 1 - 10 populated here";
      content = dex::string( map );
      content.insert( content.find( "query=" ) + 6, query );
      content.insert( content.find( "results=" ) + 8, results );
      }

   if ( map == MAP_FAILED )
      return nullptr;

   int totalSize = fileInfo.st_size + query.size( ) + results.size( );
   char buff[ sizeof(int) + 1 ];
   sprintf( buff, "%d", totalSize );
   dex::string responseHeader = "HTTP/1.1 200 OK\r\nContent-Length: "
      + dex::string( buff )
      + "\r\nConnection: close\r\nContent-Type: " + Mimetype( path ) + "\r\n\r\n";
   send( socket, responseHeader.data( ), responseHeader.size( ), 0 );


   if ( path == "results.html" )
      send( socket, content.data( ), content.size( ), 0 );
   else
      {
      for ( int i = 0;  i != fileInfo.st_size / bufferSize;  ++i )
         send( socket, map + i * bufferSize, bufferSize, 0 );

      send( socket, map + bufferSize * ( fileInfo.st_size / bufferSize ), fileInfo.st_size % bufferSize, 0 );
      }


   close( file );
   return nullptr;
   }



int main( int argc, char **argv )
   {
   if ( argc < 2 )
      {
      std::cerr << "Usage: server port" << std::endl;
      return 1;
      }

   int port = atoi( argv[ 1 ] );


   struct sockaddr_in listenAddress, talkAddress;
   socklen_t talkAddressLength;

   memset( &listenAddress, 0, sizeof( listenAddress ) );
   memset( &talkAddress, 0, sizeof( talkAddress ) );

   listenAddress.sin_family = AF_INET;
   listenAddress.sin_port = htons( port );
   listenAddress.sin_addr.s_addr = htonl( INADDR_ANY );

   int listenSockfd = socket( AF_INET, SOCK_STREAM, 0 );
   int talkSockfd;

   if ( listenSockfd == -1 )
      {
      std::cerr << "Could not create socket" << std::endl;
      return 1;
      }

   if ( bind( listenSockfd, ( struct sockaddr * )&listenAddress, sizeof( listenAddress ) ) == -1 )
      {
      std::cerr << "Could not bind socket" << std::endl;
      return 1;
      }

   if ( listen( listenSockfd, SOMAXCONN ) == -1 )
      {
		std::cerr << "Could not start listening" << std::endl;
      return 1;
      }

   while ( ( talkAddressLength = sizeof( talkAddress ),
         talkSockfd = accept( listenSockfd, ( struct sockaddr * )&talkAddress, &talkAddressLength ) )
         && talkSockfd != -1 )
      {
		pthread_t child;
		pthread_create( &child, nullptr, Talk, new int( talkSockfd ) );
		pthread_detach( child );
      }
   }
