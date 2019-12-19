
#include "basicString.hpp"
#include "catch.hpp"
#include "utf.hpp"
#include "utf.hpp"
#include "file.hpp"
#include "vector.hpp"
#include "url.hpp"

#include <iostream>

// Hamilton:
//
// After encoding: ee a4 9a
// 11101110 10100100 10011010
// 1110100100011010
// 59674
//
// Character counting
// Should be 59654

// Amazon:
// After encoding: e1 a4 80
// 11100001 10100100 10000000
// 1100100000000
// 6400

int saveHtml ( const dex::string &url, const dex::string &html, int fileDescriptor)
	{
	std::cerr << "URL: \"" << url << "\"; Size: " << url.size( ) << " + " << html.size( ) << std::endl;
	// allocate data to store
	unsigned char *toStore = new unsigned char[ url.size( ) + html.size( ) + 14 ];
	unsigned char *toStorePointer = toStore;
	dex::utf::encoder < dex::string > stringEncoder;
	toStorePointer = stringEncoder( url, toStorePointer );
	toStorePointer = stringEncoder( html, toStorePointer );
	int toReturn = write( fileDescriptor, toStore, toStorePointer - toStore );
	std::cerr << "Wrote " << ( toStorePointer - toStore ) << " bytes of data" << std::endl;
	delete[ ] toStore;
	if ( toReturn == -1 )
		{
		std::cout << "error writing html" << std::endl;
		}
	return toReturn;
	}

int main ( )
	{
	dex::vector< dex::string > urls;
	urls.pushBack( "hamiltoncshell.html");
	urls.pushBack( "amazon.html" );
	urls.pushBack( "cidermill.html" );
	urls.pushBack( "joel.html" );
	urls.pushBack( "hamiltoncshell.html");
	urls.pushBack( "joel.html" );

	dex::vector< dex::string > shortUrls;
	shortUrls.pushBack( "short1.txt" );
	shortUrls.pushBack( "short2.txt" );
	shortUrls.pushBack( "short3.txt" );

	const char *parserPath = "tst/parser/";
	dex::string filename = "encodedTest.txt";
	int fileDescriptor = open( filename.cStr( ), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
	if ( fileDescriptor == -1 )
		{
		std::cout << "Failed to open file" << std::endl;
		return 1;
		}

	for ( unsigned i = 0;  i < urls.size( );  ++i )
		{
		dex::string current = parserPath + urls[ i ];
		size_t filesize = 0;
		char *htmlFile = dex::readFromFileForIndex( current.cStr( ), filesize );
		dex::string currentFile( htmlFile, filesize );
		//std::cout << currentFile << std::endl;
		saveHtml( urls[ i ], currentFile, fileDescriptor );
		}

	dex::utf::decoder < dex::string > stringDecoder;
	unsigned char *savedHtml = reinterpret_cast< unsigned char * >( dex::readFromFile( filename.cStr( ), 0 ) );
	unsigned char *ptr = savedHtml;
	unsigned filesize = dex::getFileSize( filename.cStr( ) );
	std::cout << filesize << std::endl;
	while ( ptr - savedHtml < filesize )
		{
		// retrieve the saved url + html pair
		dex::Url url = dex::Url( stringDecoder( ptr, &ptr ).cStr( ) );
		dex::string html = stringDecoder( ptr, &ptr );
		std::cout << "<======URL: " << url.completeUrl( ) << "; SIZE: " << url.completeUrl( ).size( ) << " + " << html.size( ) << "\n " << html << "======>";
		}
	}





/*

	vector< string > expectedLink;
	expectedLink.pushBack(
		"https://web.eecs.umich.edu/~pmchen/subdir1/software1" );
	expectedLink.pushBack(
		"https://web.eecs.umich.edu/~pmchen/software2" );
	expectedLink.pushBack(
		"https://web.eecs.umich.edu/~pmchen/subdir1/subdir2/software3" );
	expectedLink.pushBack(
		"https://web.eecs.umich.edu/~pmchen/subdir1/subdir2/software3" );
	expectedLink.pushBack(
		"https://web.eecs.umich.edu/~pmchen/" );
	expectedLink.pushBack(
		"https://web.eecs.umich.edu/~pmchen/" );
	expectedLink.pushBack(
		"https://web.eecs.umich.edu/" ); */
