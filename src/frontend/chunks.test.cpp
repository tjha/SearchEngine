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
#include <iostream>


TEST_CASE( "Sans QC" )
	{
	SECTION( "Match" )
		{
		dex::string filePath = "test_in.dex";
		int fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

		if ( fd == -1 )
			exit( 1 );

		// Create a scope here so that the index chunk calls its destructor
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

		REQUIRE( isrAnd.next( ) == 12 );
		REQUIRE( isrAnd.next( ) == isrAnd.npos );
		}
	}

TEST_CASE( "matching document generator 1")
	{
	dex::string filePath = "test_in.dex";
	int fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	if ( fd == -1 )
		exit( 1 );

	// Create a scope here so that the index chunk calls its destructor
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
	REQUIRE( andISR.next( ) == 3 );
	REQUIRE( andISR.next( ) == 6 );
	REQUIRE( andISR.next( ) == static_cast< size_t >( -1 ) );

	dex::index::indexChunk::indexStreamReader junkISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "junk" );
	REQUIRE( junkISR.next( ) == 2 );
	REQUIRE( junkISR.next( ) == 5 );
	REQUIRE( junkISR.next( ) == 7 );
	REQUIRE( junkISR.next( ) == static_cast< size_t >( -1 ) );

	dex::index::indexChunk::indexStreamReader someISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "some" );
	REQUIRE( someISR.next( ) == 1 );
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
		REQUIRE( next == 12 );
		REQUIRE( testdocs->matchingDocumentISR->next( ) == dex::constraintSolver::ISR::npos );
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
		REQUIRE( next == 12 );
		REQUIRE( testdocs->matchingDocumentISR->next( ) == dex::constraintSolver::ISR::npos );
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
		delete testdocs;
		}

	SECTION( "longer matching query" )
		{
		dex::index::indexChunk::indexStreamReader andISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "and" );
		REQUIRE( andISR.next( ) == 3 );
		REQUIRE( andISR.next( ) == 6 );
		REQUIRE( andISR.next( ) == static_cast< size_t >( -1 ) );

		dex::index::indexChunk::indexStreamReader junkISR = dex::index::indexChunk::indexStreamReader( chunkPointer, "junk" );
		REQUIRE( junkISR.next( ) == 2 );
		REQUIRE( junkISR.next( ) == 5 );
		REQUIRE( junkISR.next( ) == 7 );
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
		dex::queryCompiler::matchedDocumentsGenerator mdg = parser.parse( query );
		dex::queryCompiler::matchedDocuments *testdocs = mdg( chunkPointer );
		REQUIRE( testdocs );
		REQUIRE( testdocs->matchingDocumentISR );
		REQUIRE( testdocs->matchingDocumentISR->next( ) == 12 );
		REQUIRE( testdocs->matchingDocumentISR->next( ) == dex::constraintSolver::ISR::npos );
		delete testdocs;
		}
	}

TEST_CASE( "matching document generator 2" )
	{
	dex::string filePath = "test_in.dex";
	int fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	REQUIRE( fd != -1 );
		// Scope to make sure we call the destructor
		{
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

	dex::queryCompiler::parser parser;
	dex::queryCompiler::matchedDocumentsGenerator mdg = parser.parse( "$short the $housing market" );
	dex::queryCompiler::matchedDocuments *documents = mdg( chunkPointer );
	documents->chunk = chunkPointer;

	SECTION( "Flattened Query" )
		{
		// Test all things returned from the matchedDocumentsGenerator
		REQUIRE( documents->flattenedQuery[ 0 ] == "short" );
		REQUIRE( documents->flattenedQuery[ 1 ] == "the" );
		REQUIRE( documents->flattenedQuery[ 2 ] == "housing" );
		REQUIRE( documents->flattenedQuery[ 3 ] == "market" );

		dex::vector< dex::constraintSolver::ISR * > bodyISRs;
		dex::vector< dex::constraintSolver::ISR * > titleISRs;
		for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
			bodyISRs.pushBack( new dex::index::indexChunk::indexStreamReader( documents->chunk, documents->flattenedQuery[ i ] ) );
		for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
			titleISRs.pushBack( new dex::index::indexChunk::indexStreamReader( documents->chunk, '#' + documents->flattenedQuery[ i ] ) );
		REQUIRE( bodyISRs[ 0 ]->next( ) == 1 );
		REQUIRE( bodyISRs[ 0 ]->next( ) == 13 );
		REQUIRE( bodyISRs[ 1 ]->next( ) == 2 );
		REQUIRE( bodyISRs[ 1 ]->next( ) == 14 );
		REQUIRE( bodyISRs[ 2 ]->next( ) == 3 );
		REQUIRE( bodyISRs[ 2 ]->next( ) == 19 );
		REQUIRE( bodyISRs[ 3 ]->next( ) == 4 );
		REQUIRE( bodyISRs[ 3 ]->next( ) == 20 );
		REQUIRE( titleISRs[ 0 ]->next( ) == 8 );
		REQUIRE( titleISRs[ 1 ]->next( ) == 9 );
		REQUIRE( titleISRs[ 1 ]->next( ) == 25 );
		REQUIRE( titleISRs[ 2 ]->next( ) == 10 );
		REQUIRE( titleISRs[ 2 ]->next( ) == 26 );
		REQUIRE( titleISRs[ 3 ]->next( ) == 11 );
		REQUIRE( titleISRs[ 3 ]->next( ) == 27 );

		for ( size_t i = 0;  i < documents->flattenedQuery.size( );  ++i )
			{
			delete bodyISRs[ i ];
			delete titleISRs[ i ];
			}
		}

	SECTION( "end isrs" )
		{
		dex::index::indexChunk::endOfDocumentIndexStreamReader *eodisr =
			new dex::index::indexChunk::endOfDocumentIndexStreamReader( documents->chunk, "" );
		REQUIRE( eodisr->next( ) == 0 );
		REQUIRE( eodisr->next( ) == 12 );
		REQUIRE( eodisr->next( ) == 28 );
		REQUIRE( eodisr->next( ) == dex::constraintSolver::ISR::npos );
		delete eodisr;

		dex::index::indexChunk::endOfDocumentIndexStreamReader *ends = dex::queryCompiler::getEndOfDocumentISR( documents->chunk );
		REQUIRE( ends->next( ) == 0 );
		REQUIRE( ends->next( ) == 12 );
		REQUIRE( ends->next( ) == 28 );
		REQUIRE( ends->next( ) == dex::constraintSolver::ISR::npos );
		delete ends;
		}

	SECTION( "matching ISR" )
		{
		dex::constraintSolver::ISR *matches = documents->matchingDocumentISR;
		REQUIRE( matches->next( ) == 12 );
		REQUIRE( matches->next( ) == 28 );
		REQUIRE( matches->next( ) == dex::constraintSolver::ISR::npos );
		}

	SECTION( "emphasized words" )
		{
		REQUIRE( documents->emphasizedWords[ 0 ] );
		REQUIRE( !documents->emphasizedWords[ 1 ] );
		REQUIRE( documents->emphasizedWords[ 2 ] );
		REQUIRE( !documents->emphasizedWords[ 3 ] );
		}

	close( fd );
	}

TEST_CASE( "ranker" )
	{
	// Looking for three types of documents
	// for each of the following queries
	// short the housing market
	// oakland athletics baseball
	// pokemon sword and shield
	dex::string filePath = "test_in0.dex";
	int fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	REQUIRE( fd != -1 );

	// Create a scope here so that the index chunk calls its destructor
	// Making two financial documents
		{
		dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );
		dex::string url = "www.robinhood.com/shortthehousingmarket";
		dex::vector< dex::string > title = { "learn", "to", "short", "the", "housing", "market" };
		dex::string titleString = "Learn to short the housing market";
		dex::vector< dex::string > body = { "learn", "to", "short", "the", "housing", "market", "using",
				"our", "ten", "step", "program", "here", "at", "robinhood", "short", "the", "housing", "market",
				"has", "never", "been", "easier", "using", "the", "robinhood", "techniques", "available", "get",
				"cold", "hard", "cash", "when", "you", "short", "the", "housing", "market", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		url = "goldmansachs.com/unrelated/garbo/garbo/stuff";
		title = { "buy", "bonds", "in", "the", "housing", "market" };
		titleString = "buy bonds in the housing market";
		body = { "we", "here", "at", "goldman", "sachs", "believe", "that", "the", "housing", "market", "is",
				"stronger", "than", "ever", "some", "claim", "that", "you", "should", "short", "the", "very",
				"strong", "housing", "market", "well", "we", "think", "that", "the", "housing", "market", "has",
				"never", "been", "stronger", "in", "fact", "at", "goldman", "we", "believe", "you",
				"should", "buy", "bonds", "in", "the", "market", "do", "not", "short", "the", "housing", "market",
				"please", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}

	dex::index::indexChunk *chunkPointer = new dex::index::indexChunk( fd, false );

	close( fd );

	SECTION( "Simple ranker internals tests" )
		{
		dex::ranker::ranker rankerObject;
		dex::ranker::scoreRequest request;
		request.chunkPointer = chunkPointer;
		pthread_mutex_t generatorLock = PTHREAD_MUTEX_INITIALIZER;
		request.generatorLockPointer = &generatorLock;
		dex::queryCompiler::parser parser;
		dex::queryCompiler::matchedDocumentsGenerator mdg = parser.parse( "short the housing market" );
		dex::queryCompiler::matchedDocuments *documents = mdg( chunkPointer );
		documents->chunk = chunkPointer;
		request.generatorPointer = &mdg;
		request.printInfo = false;
		request.rankerPointer = &rankerObject;


		void *returnValue = dex::ranker::findAndScoreDocuments( static_cast< void * > ( &request ) );
		dex::pair< dex::vector< dex::ranker::searchResult >, int > *returnedResults =
				static_cast< dex::pair< dex::vector< dex::ranker::searchResult >, int > * > ( returnValue );
		REQUIRE( returnedResults->second == 0 );
		REQUIRE( returnedResults->first.size( ) == 2 );
		REQUIRE( returnedResults->first[ 0 ].title == "Learn to short the housing market" );
		REQUIRE( returnedResults->first[ 0 ].url.completeUrl( ) == "http://www.robinhood.com/shortthehousingmarket" );
		REQUIRE( returnedResults->first[ 1 ].title == "buy bonds in the housing market" );
		REQUIRE( returnedResults->first[ 1 ].url.completeUrl( ) == "http://goldmansachs.com/unrelated/garbo/garbo/stuff" );
		
		}

	dex::vector< dex::index::indexChunk * > indexChunkPointers;
	indexChunkPointers.pushBack( chunkPointer );

	SECTION( "Run TopN with 1 index chunk" )
		{
		dex::ranker::ranker rankerObject;


		dex::pair< dex::vector< dex::ranker::searchResult >, int > results = dex::ranker::getTopN( 2, "short the housing market",
				&rankerObject, indexChunkPointers, false );
		REQUIRE( results.second == 0 );
		}

	// Making two pokemon articles
	filePath = "test_in1.dex";
	fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	REQUIRE( fd != -1 );

	// Create a scope here so that the index chunk calls its destructor
		{
		dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );
		dex::string url = "www.serebii.com/mewtwo";
		dex::vector< dex::string > title = { "find", "mewtwo", "in", "pokemon", "sword", "and", "shield" };
		dex::string titleString = "find mewtwo in pokemon sword and shield";
		dex::vector< dex::string > body = { "in", "the", "new", "game", "from", "nintendo", "pokemon", "sword",
				"and", "shield", "the", "pokemon", "mewtwo", "can", "be", "tricky", "to", "find", "fortunately",
				"the", "pokemon", "company", "has", "made", "sword", "and", "shield", "with", "mewtwo", "pokemon",
				"sword", "and", "shield", "pokemon", "mewtwo", "sword", "and", "shield", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		url = "gamefaqs.biz/mewtwo";
		title = { "pokemon", "mewtwo", "found", "in", "sword", "and", "shield" };;
		titleString = "pokemon mewtwo found in sword and shield";
		body = { "gamers", "rise", "up", "in", "pokemon", "weve", "found", "the", "pokemon", "mewtwo", "in", "the",
				"new", "game", "sword", "and", "shield", "i", "love", "swords", "and", "mewtwo", "pokemon", "and",
				"shield", "mewtwo", "mewtwo", "subscribe", "to", "gamefaqs", "for", "more", "informational", "forum",
				"posts", "like", "this", "one", "gamers", "rise", "up", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}

	chunkPointer = new dex::index::indexChunk( fd, false );
	indexChunkPointers.pushBack( chunkPointer );

	close( fd );

	// making two baseball articles

	filePath = "test_in2.dex";
	fd = open( filePath.cStr( ), O_RDWR | O_CREAT | O_TRUNC, 0777 );

	REQUIRE( fd != -1 );

	// Create a scope here so that the index chunk calls its destructor
	// Making two financial documents
		{
		dex::index::indexChunk initializingIndexChunk = dex::index::indexChunk( fd );
		dex::string url = "www.mlb.edu/moenyball";
		dex::vector< dex::string > title = { "oakland", "athletics", "baseball", "performance" };
		dex::string titleString = "oakland athletics baseball performance";
		dex::vector< dex::string > body = { "the", "oakland", "athletics", "baseball", "program",
				"has", "been", "looking", "great", "this", "year", "they", "used", "stats", "more", "than", "any",
				"other", "baseball", "program", "in", "the", "nation", "their", "athletics", "and", "baseball",
				"abilities", "are", "off", "the", "charts", "man", "i", "just", "love", "the", "oakland",
				"athletics", "baseball", "program", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );

		url = "gamers.rise.up/against/baseball";
		title = { "this", "is", "a", "really", "long", "title", "that", "really", "isnt", "very", "good", "and",
				"shouldnt", "be", "rewarded", "but", "at", "least", "its", "about", "the", "oakland", "athletics",
				"baseball", "team" };
		titleString = "this is a really long title that really isnt very good and shouldnt be rewarded but at least its about the oakland athletics baseball team";
		body = { "we", "really", "dont", "have", "much", "except", "gamers", "need", "to", "rise", "up", "against",
				"the", "oakland", "athletics", "baseball", "team", "they", "arent", "gamers", "oakland", "was", "a",
				"mistake", "and" };

		REQUIRE( initializingIndexChunk.addDocument( url, title, titleString, body ) );
		}

	chunkPointer = new dex::index::indexChunk( fd, false );
	indexChunkPointers.pushBack( chunkPointer );
	close( fd );

	SECTION( "Run TopN with 3 index chunks" )
		{
		dex::ranker::ranker rankerObject;

		REQUIRE( indexChunkPointers.size( ) == 3 );
		dex::pair< dex::vector< dex::ranker::searchResult >, int > results = dex::ranker::getTopN( 4, "and",
				&rankerObject, indexChunkPointers, false );
		REQUIRE( results.second == 0 );
		REQUIRE( results.first.size( ) == 4 );
		for ( size_t i = 0;  i < results.first.size( ) - 1;  ++i )
			REQUIRE( results.first[ i ].score.getTotalScore( ) >= results.first[ i + 1 ].score.getTotalScore( ) );
		}
	}
