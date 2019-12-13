// masterRanker.cpp
// this runs the whole shebang

// 2019-12-10: Init Commit: combsc + lougheem

#include "ranker.hpp"

/*



struct document
	{
	// Constraint solver needs to make sure to not return pornographic results
	// vector of ISRs should be in the order of the flattened query
	dex::vector < dex::ISR > titleISRs;
	dex::vector < dex::ISR > bodyISRs;
	dex::string title;
	dex::Url url;
	unsigned rarestWordIndex;
	unsigned documentBodyLength;
	};

QUERY COMPILER + CONSTRAINT SOLVER TEAM IS IN CHARGE OF IMPLEMENTING THIS OBJECT
class worker
	{
	private:
		const char *filename;
		anything else you guys need
	public:
		worker( filename ){ }

		// Preferrably this should be multithreaded. If we get two search requests and call this function
		// on a worker twice we shouldn't have to wait for the first to finish for the second to begin. But
		// right now we're just trying to finish so if that's not reasonable to finish in 1-2 days then
		// don't do it.
		vector < document > getMatchingDocuments( string query )
			{
			we expect this to be a function that we pass a query that we get from the get request
			This function runs the query compiler and constraint solver and returns
			a vector of documents that match the query given.
			}
	}


RANKER TEAM IS IN CHARGE OF THIS STUFF:

vector < string > IndexChunkFileNames
unordered_set < workers>  IndexChunkWorkers
vector < double > rankerWeights;

We expect a folder
*/

int main( int argc, char *argv[ ] )
	{
	//Search home directory for index chunks and store into IndexChunkFileNames
	// for filename in IndexChunkFilenames
	// 	{
	// 	IndexChunkWorkers.insert( indexChunkWorkerConstructor( filename ) );
	// 	}

	// FRONT END TEAM IS RESPONSIBLE FOR MAKING THIS SERVER AND SERVING OUR VECTOR IN A NICE WAY
	
	//spins up front end linux server and waits on this for a get request
	//while ( 1 )
	//	{
	//	when we get a get request do:
	//		{
	//		string query = queryFromGetRequest;
	//		vector < document > docsReturned;
	//		ranker = rankerObject( rankerWeights );
	//		for worker in IndexChunkWorkers
	//			{
	// 			docsReturned.pushBack( worker.getMatchingDocuments( query );
	// 			}
	// 		// We then return the 10 highest scoring documents
	// 		serve( ranker.getTopN( docsReturned, 10 ) );
	// 		}
	// 	}
	// }