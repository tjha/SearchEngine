// rankerObjects.hpp
// objects used for the ranker are defined here
//
// 2019-12-11: Init Commit: combsc

#ifndef RANKER_OBJECTS_HPP
#define RANKER_OBJECTS_HPP

#include "vector.hpp"
#include "url.hpp"
namespace dex
	{
	class ISR
		{
		private:
			dex::vector < unsigned > internal;
			dex::string word;
			unsigned pos;

		public:
			const static unsigned npos = unsigned ( -1 );
			ISR( )
				{
				}
			ISR( dex::string word, dex::vector < unsigned > vecIn )
				{
				this->word = word;
				internal = vecIn;
				pos = 0;
				}
			unsigned first( )
				{
				return internal[ 0 ];
				}
			unsigned next( )
				{
				if ( pos == internal.size( ) )
					{
					return npos;
					}
				unsigned toRet = internal[ pos ];
				pos++;
				return toRet;
				}
			unsigned last( )
				{
				return internal.back( );
				}
			unsigned getPos( )
				{
				return pos;
				}
			dex::string getWord( )
				{
				return word;
				}
		};

	struct document
		{
		// Constraint solver needs to make sure to not return pornographic results
		// All four vectors should be in the order of the flattened query
		dex::vector < dex::ISR > titleISRs;
		dex::vector < dex::ISR > bodyISRs;
		dex::vector < bool > emphasizedWords;
		dex::string title;
		dex::Url url;
		unsigned rarestWordIndex;
		unsigned documentBodyLength;
		};
	
	class indexChunkObject
		{
		// Whatever matt and stephen put in here
		public:
			indexChunkObject( dex::string filename );
		};

	struct queryRequest
		{
		dex::string query;
		dex::indexChunkObject *chunkPointer;
		};

	struct searchResult
		{
		dex::Url url;
		dex::string title;
		};

	void *getMatchingDocuments( void *args )
		{
		dex::queryRequest queryRequest = *( ( dex::queryRequest * ) args );
		/*we expect this to be a function that we pass a query that we get from the get request
		This function runs the query compiler and constraint solver and returns
		a vector of documents that match the query given.*/
		vector < dex::document > someVec;
		return nullptr;
		}
	}

#endif
