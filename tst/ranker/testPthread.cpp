// testing pthread architecture in ranker

// 2019-12-10: init commit, ran as expected: combsc
// NOT USED IN CODEBASE, JUST FOR TESTING

#include <pthread.h>
#include "basicString.hpp"
#include "vector.hpp"

struct filename
	{
	dex::string query;
	dex::string name;
	};

void *testFunction( void *args )
	{
	filename f = *( ( filename * ) args );
	dex::vector < dex::string > *toReturn = new dex::vector < dex::string > ( );
	toReturn->pushBack( f.query );
	toReturn->pushBack( f.name );
	return ( void * ) toReturn;
	}

int main( )
	{
	dex::string name = "success!";
	dex::string query = "query";
	filename f;
	f.name = name;
	f.query = query;
	pthread_t thread;
	pthread_create( &thread, nullptr, testFunction , ( void * ) &f );
	
	void *returnValue;
	pthread_join( thread, &returnValue );
	
	dex::vector < dex::string > *returned = ( dex::vector < dex::string > * ) returnValue;
	for ( unsigned i = 0;  i < returned->size( );  ++i )
		{
		std::cout << returned->at( i ) << std::endl;
		}
	delete returned;
	return 0;
	}