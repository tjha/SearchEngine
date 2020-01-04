// file for testing chunks
// Not going to use catch2 format yet.
#include "indexer/indexer.hpp"
#include "ranker/ranker.hpp"
#include "constraintSolver/constraintSolver.hpp"
#include "utils/basicString.hpp"
#include "utils/vector.hpp"
#include "utils/file.hpp"
#include "catch.hpp"
#include <pthread.h>


TEST_CASE( "Sans QC" )
	{
	SECTION( "Match" )
		{
		dex::string filePath = "test_in.dex";
		int fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );

			{
			dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );

			dex::string url = "hamiltoncshell.com";
			dex::vector< dex::string > title = { "hamilton", "c", "shell" };
			dex::string titleString = "Hamilton C Shell 2012";
			dex::vector< dex::string > body = { "some", "junk", "and", "more", "junk", "and", "junk" };

			REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
			}

		dex::index::indexChunk *chunkPointer = new dex::index::indexChunk( fd, false );

		close( fd );

		// dex::index::indexChunk::endOfDocumentIndexStreamReader *isrEndOrA =
		// 		new dex::index::indexChunk::endOfDocumentIndexStreamReader( chunkPointer, "" );
		// dex::index::indexChunk::endOfDocumentIndexStreamReader *isrEndOrB =
		// 		new dex::index::indexChunk::endOfDocumentIndexStreamReader( chunkPointer, "" );
		dex::index::indexChunk::endOfDocumentIndexStreamReader *isrEndAnd =
				new dex::index::indexChunk::endOfDocumentIndexStreamReader( chunkPointer, "" );

		dex::index::indexChunk::indexStreamReader *isrSome =
				new dex::index::indexChunk::indexStreamReader( chunkPointer, "some" );
		// dex::index::indexChunk::indexStreamReader *isrHashSome =
		// 		new dex::index::indexChunk::indexStreamReader( chunkPointer, "#some" );
		dex::index::indexChunk::indexStreamReader *isrJunk =
				new dex::index::indexChunk::indexStreamReader( chunkPointer, "junk" );
		// dex::index::indexChunk::indexStreamReader *isrHashJunk =
		// 		new dex::index::indexChunk::indexStreamReader( chunkPointer, "#junk" );

		// dex::constraintSolver::orISR *isrOrA =
		// 		new dex::constraintSolver::orISR( dex::vector< dex::constraintSolver::ISR * >( { isrSome, isrHashSome } ), isrEndOrA );

		// dex::constraintSolver::orISR *isrOrB =
		// 		new dex::constraintSolver::orISR( dex::vector< dex::constraintSolver::ISR * >( { isrJunk, isrHashJunk } ), isrEndOrB );

		dex::constraintSolver::andISR isrAnd( dex::vector< dex::constraintSolver::ISR * >( { isrSome, isrJunk } ), isrEndAnd );

		REQUIRE( isrAnd.next( ) == 11 );
		REQUIRE( isrAnd.next( ) == isrAnd.npos );
		}
	}

TEST_CASE( "matching document ISR")
	{
	dex::string filePath = "test_in.dex";
	int fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	if ( fd == -1 )
		exit( 1 );

		{
		dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );

		dex::string url = "hamiltoncshell.com";
		dex::vector< dex::string > title = { "hamilton", "c", "shell" };
		dex::string titleString = "Hamilton C Shell 2012";
		dex::vector< dex::string > body = { "some", "junk", "and", "more", "junk", "and", "junk" };

		/*
		Phrase: "some junk"
		And: some junk  (alternatively some & junk)
		Or: some | junk
		*/

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}

	dex::index::indexChunk *chunkPointer = new dex::index::indexChunk( fd, false );

	close( fd );

	dex::index::indexChunk::indexStreamReader andISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "and" );
	REQUIRE( andISR.next( ) == 2 );
	REQUIRE( andISR.next( ) == 5 );
	REQUIRE( andISR.next( ) == static_cast< size_t >( -1 ) );

	dex::index::indexChunk::indexStreamReader junkISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "junk" );
	REQUIRE( junkISR.next( ) == 1 );
	REQUIRE( junkISR.next( ) == 4 );
	REQUIRE( junkISR.next( ) == 6 );
	REQUIRE( junkISR.next( ) == static_cast< size_t >( -1 ) );

	dex::index::indexChunk::indexStreamReader someISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "some" );
	REQUIRE( someISR.next( ) == 0 );
	REQUIRE( someISR.next( ) == static_cast< size_t >( -1 ) );

	dex::index::indexChunk::indexStreamReader nonexistentISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "qubfiqebfoqbgoqbe" );
	REQUIRE( nonexistentISR.next( ) == static_cast< size_t >( -1 ) );

	SECTION( "check for query we expect to return results" )
		{
		dex::string query = "junk";
		dex::queryCompiler::parser parser;
		dex::queryCompiler::matchedDocumentsGenerator mdg = parser.parse( query );
		dex::queryCompiler::matchedDocuments *testdocs = mdg( chunkPointer );

		REQUIRE( testdocs );
		REQUIRE( testdocs->matchingDocumentISR );
		size_t next = testdocs->matchingDocumentISR->next( );
		REQUIRE( next == 11 );
		REQUIRE( testdocs->matchingDocumentISR->next( ) == dex::constraintSolver::ISR::npos );
		delete testdocs->matchingDocumentISR;
		delete testdocs;
		}

	SECTION( "check for complex query we expect to return results" )
		{
		dex::string query = "junk and";
		dex::queryCompiler::parser parser;
		dex::queryCompiler::matchedDocumentsGenerator mdg = parser.parse( query );
		dex::queryCompiler::matchedDocuments *testdocs = mdg( chunkPointer );

		REQUIRE( testdocs );
		REQUIRE( testdocs->matchingDocumentISR );
		size_t next = testdocs->matchingDocumentISR->next( );
		REQUIRE( next == 11 );
		REQUIRE( testdocs->matchingDocumentISR->next( ) == dex::constraintSolver::ISR::npos );
		delete testdocs->matchingDocumentISR;
		delete testdocs;
		}

	SECTION( "check for query we do not expect to return results" )
		{
		dex::string query = "qubfiqebfoqbgoqbe";
		dex::queryCompiler::parser parser;
		dex::queryCompiler::matchedDocumentsGenerator mdg = parser.parse( query );
		dex::queryCompiler::matchedDocuments *testdocs = mdg( chunkPointer );
		REQUIRE( testdocs );
		REQUIRE( testdocs->matchingDocumentISR );
		size_t next = testdocs->matchingDocumentISR->next( );
		REQUIRE( next == dex::constraintSolver::ISR::npos );
		delete testdocs->matchingDocumentISR;
		delete testdocs;
		}

	SECTION( "longer matching query" )
		{
		std::cout << "===================================================================" << std::endl;

		dex::index::indexChunk::indexStreamReader andISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "and" );
		REQUIRE( andISR.next( ) == 2 );
		REQUIRE( andISR.next( ) == 5 );
		REQUIRE( andISR.next( ) == static_cast< size_t >( -1 ) );

		dex::index::indexChunk::indexStreamReader junkISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "junk" );
		REQUIRE( junkISR.next( ) == 1 );
		REQUIRE( junkISR.next( ) == 4 );
		REQUIRE( junkISR.next( ) == 6 );
		REQUIRE( junkISR.next( ) == static_cast< size_t >( -1 ) );

		dex::index::indexChunk::indexStreamReader andBody = dex::index::indexChunk::indexStreamReader( chunkPointer, "and" );
		dex::index::indexChunk::indexStreamReader junkBody = dex::index::indexChunk::indexStreamReader( chunkPointer, "junk" );
		dex::index::indexChunk::indexStreamReader andTitle = dex::index::indexChunk::indexStreamReader( chunkPointer, "#and" );
		dex::index::indexChunk::indexStreamReader junkTitle = dex::index::indexChunk::indexStreamReader( chunkPointer, "#junk" );
		dex::index::indexChunk::endOfDocumentIndexStreamReader endOfDocISR( chunkPointer, "" );
		// orISR *andISRs = new orISR
		/*
		dex::constraintSolver::orISR *andISRs =
				new dex::constraintSolver::orISR( dex::vector< ISR * >(
						{ andBody, andTitle }, )
		*/

		dex::string query = "more junk";
		dex::queryCompiler::parser parser;
		std::cout << "about to parse query\n";
		dex::queryCompiler::matchedDocumentsGenerator mdg = parser.parse( query );
		std::cout << "about to get matchedDocuments\n";
		dex::queryCompiler::matchedDocuments *testdocs = mdg( chunkPointer );
		REQUIRE( testdocs );
		REQUIRE( testdocs->matchingDocumentISR );
		std::cout << "about to get query\n";
		std::cout << "mdg.getQuery( ): " << mdg.getQuery( ) << std::endl;
		REQUIRE( testdocs->matchingDocumentISR->next( ) == 11 );
		REQUIRE( testdocs->matchingDocumentISR->next( ) == dex::constraintSolver::ISR::npos );
		delete testdocs->matchingDocumentISR;
		delete testdocs;
		}
	}

TEST_CASE( )
	{
	SECTION( "single word query on multiple documents" )
		{

		}
	}

TEST_CASE( "ranker" )
	{
	// Looking for three types of documents
	// for each of the following queries
	// short the housing market
	// oakland athletics baseball
	// pokemon sword and shield
	dex::string filePath = "test_in.dex";
	int fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	if ( fd == -1 )
		exit( 1 );


		{
		// dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );
		// dex::string url = "www.robinhood.com/shortthehousingmarket";
		// dex::vector< dex::string > title = { "learn", "to", "short", "the", "housing", "market" };
		// dex::string titleString = "Learn to short the housing market";
		// dex::vector< dex::string > body = { "learn", "to", "short", "the", "housing", "market", "using",
		// 		"our", "ten", "step", "program", "here", "at", "robinhood", "short", "the", "housing", "market",
		// 		"has", "never", "been", "easier", "using", "the", "robinhood", "techniques", "available", "get",
		// 		"cold", "hard", "cash", "when", "you", "short", "the", "housing", "market" };

		// REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		// url = "goldmansachs.com/unrelated/garbo/garbo/stuff";
		// title = { "buy", "bonds", "in", "the", "housing", "market" };
		// titleString = "buy bonds in the housing market";
		// body = { "we", "here", "at", "goldman", "sachs", "believe", "that", "the", "housing", "market", "is",
		// 		"stronger", "than", "ever", "some", "claim", "that", "you", "should", "short", "the", "very",
		// 		"strong", "housing", "market", "well", "we", "think", "that", "the", "housing", "market", "has",
		// 		"never", "been", "stronger", "in", "fact", "at", "goldman", "we", "believe", "you",
		// 		"should", "buy", "bonds", "in", "the", "market", "do", "not", "short", "the", "housing", "market",
		// 		"please" };

		// REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );
		dex::string url = "www.robinhood.com/shortthehousingmarket";
		dex::vector< dex::string > title = { "learn", "to", "short", "the", "housing", "market" };
		dex::string titleString = "Learn to short the housing market";
		dex::vector< dex::string > body = { "short", "the", "housing", "market" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		url = "goldmansachs.com/unrelated/garbo/garbo/stuff";
		title = { "buy", "bonds", "in", "the", "housing", "market" };
		titleString = "buy bonds in the housing market";
		body = { "short", "the", "not", "a", "a", "a", "housing", "market" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}

	dex::index::indexChunk *chunkPointer = new dex::index::indexChunk( fd, false );

	close( fd );

	SECTION( "Simple ranker internals tests" )
		{


		dex::ranker::ranker rankerObject;

		dex::vector< dex::index::indexChunk * > indexChunkPointers;
		indexChunkPointers.pushBack( chunkPointer );

		dex::ranker::scoreRequest request;
		request.chunkPointer = chunkPointer;
		pthread_mutex_t generatorLock = PTHREAD_MUTEX_INITIALIZER;
		request.generatorLockPointer = &generatorLock;
		dex::queryCompiler::parser parser;
		dex::queryCompiler::matchedDocumentsGenerator mdg = parser.parse( "short the housing market" );
		dex::queryCompiler::matchedDocuments *documents = mdg( chunkPointer );
		documents->chunk = chunkPointer;
		REQUIRE( documents->flattenedQuery[ 0 ] == "short" );
		REQUIRE( documents->flattenedQuery[ 1 ] == "the" );
		REQUIRE( documents->flattenedQuery[ 2 ] == "housing" );
		REQUIRE( documents->flattenedQuery[ 3 ] == "market" );

		dex::index::indexChunk::endOfDocumentIndexStreamReader *eodisr =
				new dex::index::indexChunk::endOfDocumentIndexStreamReader( documents->chunk, "" );
		REQUIRE( eodisr->next( ) == 11 );
		REQUIRE( eodisr->next( ) == 27 );
		REQUIRE( eodisr->next( ) == dex::constraintSolver::ISR::npos );
		delete eodisr;

		dex::index::indexChunk::endOfDocumentIndexStreamReader *ends = dex::queryCompiler::getEndOfDocumentISR( documents->chunk );
		REQUIRE( ends->next( ) == 11 );
		REQUIRE( ends->next( ) == 27 );
		REQUIRE( ends->next( ) == dex::constraintSolver::ISR::npos );
		delete ends;

		dex::constraintSolver::ISR *matches = documents->matchingDocumentISR;
		REQUIRE( matches->next( ) == 11 );
		REQUIRE( matches->next( ) == 27 );
		REQUIRE( matches->next( ) == dex::constraintSolver::ISR::npos );

		dex::vector< dex::constraintSolver::ISR * > bodyISRs;
		dex::vector< dex::constraintSolver::ISR * > titleISRs;
		for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
			bodyISRs.pushBack( new dex::index::indexChunk::indexStreamReader( documents->chunk, documents->flattenedQuery[ i ] ) );
		for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
			titleISRs.pushBack( new dex::index::indexChunk::indexStreamReader( documents->chunk, '#' + documents->flattenedQuery[ i ] ) );
		REQUIRE( bodyISRs[ 0 ]->next( ) == 0 );
		REQUIRE( bodyISRs[ 0 ]->next( ) == 12 );
		REQUIRE( bodyISRs[ 1 ]->next( ) == 1 );
		REQUIRE( bodyISRs[ 1 ]->next( ) == 13 );
		REQUIRE( bodyISRs[ 2 ]->next( ) == 2 );
		REQUIRE( bodyISRs[ 2 ]->next( ) == 18 );
		REQUIRE( bodyISRs[ 3 ]->next( ) == 3 );
		REQUIRE( bodyISRs[ 3 ]->next( ) == 19 );
		REQUIRE( titleISRs[ 0 ]->next( ) == 7 );
		REQUIRE( titleISRs[ 1 ]->next( ) == 8 );
		REQUIRE( titleISRs[ 1 ]->next( ) == 24 );
		REQUIRE( titleISRs[ 2 ]->next( ) == 9 );
		REQUIRE( titleISRs[ 2 ]->next( ) == 25 );
		REQUIRE( titleISRs[ 3 ]->next( ) == 10 );
		REQUIRE( titleISRs[ 3 ]->next( ) == 26 );


		request.generatorPointer = &mdg;
		request.printInfo = true;
		request.rankerPointer = &rankerObject;

		for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
			{
			delete bodyISRs[ i ];
			delete titleISRs[ i ];
			}


		//std::cout << documents->matchingDocumentISR->next( ) << " " << documents->matchingDocumentISR->next( ) << " " << documents->matchingDocumentISR->next( ) << " " << std::endl;

		// void *returnValue = dex::ranker::findAndScoreDocuments( static_cast< void * > ( &request ) );
		// dex::pair< dex::vector< dex::ranker::searchResult >, int > *returnedResults =
		// 		static_cast< dex::pair< dex::vector< dex::ranker::searchResult >, int > * > ( returnValue );
		// REQUIRE( returnedResults->second == 0 );
		// REQUIRE( returnedResults->first.size( ) == 2 );
		// for ( size_t document = 0;  document < returnedResults->first.size( );  ++document )
		// 	{
		// 	std::cout << returnedResults->first[ document ].url.completeUrl( ) << " " <<
		// 			returnedResults->first[ document ].score << std::endl;
		// 	}
		}
	}
