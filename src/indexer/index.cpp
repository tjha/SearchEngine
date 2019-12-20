// index.cpp
// Indexer.
//
// 2019-12-09: Fix bugs in addDocument and append: lougheem, jasina
// 2019-12-08: IndexStreamReader constructor and next: lougheem
// 2019-11-21: File created

#include <cstddef>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "index.hpp"
#include "basicString.hpp"
#include "exception.hpp"
#include "unorderedMap.hpp"
#include "unorderedSet.hpp"
#include "utf.hpp"
#include "utility.hpp"
#include "vector.hpp"

// TODO: remove this
#include <iostream>

// postsChunk
dex::index::indexChunk::postsChunk::postsChunk( ) : nextPostsChunkOffset( 0 ), currentPostOffset( 0 )
	{
	posts[ 0 ] = dex::utf::sentinel;
	}

bool dex::index::indexChunk::postsChunk::append( size_t delta )
	{
	// We're "full" if we can't insert a "widest" UTF-8 character. Write a sentinel instead.
	if ( currentPostOffset + 8 > postsChunkSize )
		{
		posts[ currentPostOffset ] = dex::utf::sentinel;
		return false;
		}

	currentPostOffset = dex::utf::encoder < size_t >( )( delta, posts + currentPostOffset ) - posts;
	posts[ currentPostOffset ] = dex::utf::sentinel;

	return true;
	}

// postMetadata
dex::index::indexChunk::postsMetadata::postsMetadata( size_t chunkOffset ) :
		occurenceCount( 0 ), documentCount( 0 ), firstPostsChunkOffset( chunkOffset ),
		lastPostsChunkOffset( chunkOffset ), lastLocation( 0 ), synchronizationPoints( )
	{
	// std::cout << "new lastPostsChunkOffset: " << lastPostsChunkOffset << "\t in constructor\n";
	}

bool dex::index::indexChunk::postsMetadata::append( size_t location, postsChunk *postsChunkArray )
	{
	size_t delta = location - lastLocation;
	if ( lastPostsChunkOffset > ( 1L << 19 ) - 1 )
		{
		std::cout << "not good! lastPostsChunkOffset: " << lastPostsChunkOffset << "\n";
		// exit(1);
		}
	// std::cout << "postsChunkArray[ 1L << 20 - 1 ].currentPostOffset" << postsChunkArray[ ( 1L << 20 ) - 1 ].currentPostOffset << "\n";
	size_t originalPostOffset = postsChunkArray[ lastPostsChunkOffset ].currentPostOffset;
	bool successful = postsChunkArray[ lastPostsChunkOffset ].append( delta );

	if ( successful )
		{
		lastLocation = location;

		// The first 8 bits of our location determine our synchronization point. We only update the table if we haven't
		// been "this high" before.
		for ( synchronizationPoint *syncPoint = synchronizationPoints + ( location >> ( 8 * sizeof( location ) - 8 ) );
				syncPoint >= synchronizationPoints && ~syncPoint->inverseLocation == syncPoint->npos;  --syncPoint )
			{
			syncPoint->postsChunkArrayOffset = lastPostsChunkOffset;
			syncPoint->postsChunkOffset = originalPostOffset;
			syncPoint->inverseLocation = ~location;
			}
		}

	return successful;
	}

// indexChunk
dex::index::indexChunk::indexChunk( int fileDescriptor, bool initialize )
	{
	// TOOO: Add some sort of magic number

	// TODO: Throw exceptions so we know that this failed
	if ( fileDescriptor == -1 )
		throw dex::exception( );

	std::cout << "Creating new indexChunk yo\n";
	if ( initialize )
		{
		std::cout << "\tfrom scratch\n";
		int result = lseek( fileDescriptor, fileSize - 1, SEEK_SET );
		if ( result == -1 )
			throw dex::exception( );
		result = write( fileDescriptor, "", 1 );
		if ( result == -1 )
			throw dex::exception( );
		}
	else
		{
		std::cout << "\tfrom existing indexChunk\n";
		}

	filePointer = mmap( nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0 );

	if ( filePointer == MAP_FAILED )
		throw dex::exception( );

	postsChunkCount = reinterpret_cast < size_t * >( filePointer );
	location = postsChunkCount + 1;
	maxLocation = postsChunkCount + 2;

	encodedURLsToOffsets = reinterpret_cast < byte * >( filePointer ) + urlsToOffsetsMemoryOffset;

	encodedOffsetsToEndOfDocumentMetadatas = reinterpret_cast < byte * >( filePointer )
			+ offsetsToEndOfDocumentMetadatasMemoryOffset;

	encodedDictionary = reinterpret_cast < byte * >( filePointer ) + dictionaryOffset;

	postsMetadataArray = reinterpret_cast < postsMetadata * >(
			reinterpret_cast < byte * >( filePointer ) + postsMetadataArrayMemoryOffset );

	postsChunkArray = reinterpret_cast < postsChunk * >(
			reinterpret_cast < byte * >( filePointer ) + postsChunkArrayMemoryOffset );

	if ( initialize )
		{
		*postsChunkCount = 1;
		*location = 0;
		*maxLocation = 0;

		postsChunkArray[ 0 ] = postsChunk( );
		postsMetadataArray[ 0 ] = postsMetadata( 0 );
		dictionary[ "" ] = 0;
		std::cout << "\tlocation: " << *location << "\n";
		}
	else
		{
		std::cout << "reading indexChunk from file\n";
		std::cout << "filepointer: " << filePointer << "\n";
		std::cout << "postsChunkCount: " << postsChunkCount << "\n*postsChunkCount" << *postsChunkCount << "\n";
		urlsToOffsets = dex::utf::decoder < dex::unorderedMap < dex::string, size_t > >( )( encodedURLsToOffsets );
		offsetsToEndOfDocumentMetadatas = dex::utf::decoder < dex::unorderedMap < size_t, endOfDocumentMetadataType > >( )
				( encodedOffsetsToEndOfDocumentMetadatas );
		dictionary = dex::utf::decoder < dex::unorderedMap < dex::string, size_t > >( )( encodedDictionary );
		std::cout << "number of unqiue words: " << dictionary.size( ) << "\n";
		std::cout << "number of URLs: " << urlsToOffsets.size( ) << "\n";
		}
	}

dex::index::indexChunk::~indexChunk( )
	{
	std::cout << "urlsToOffsets.size( ) == " << urlsToOffsets.size( ) << std::endl;
	std::cout << "offsetsToEndOfDocumentMetadatas.size( ) == " << offsetsToEndOfDocumentMetadatas.size( ) << std::endl;
	dex::utf::encoder < dex::unorderedMap < dex::string, size_t > >( )( urlsToOffsets, encodedURLsToOffsets );
	dex::utf::encoder < dex::unorderedMap < size_t, endOfDocumentMetadataType > >( )
			( offsetsToEndOfDocumentMetadatas, encodedOffsetsToEndOfDocumentMetadatas );
	dex::utf::encoder < dex::unorderedMap < dex::string, size_t > >( )( dictionary, encodedDictionary );

	std::cout << "closing indexChunk!\n" << "\tnumberOfDocuments: " << urlsToOffsets.size( ) << "\n\tpostsChunkCount: " << *postsChunkCount << "\n\tdictionary.size( ): " << dictionary.size( ) << "\n";

	msync( filePointer, fileSize, MS_SYNC );
	munmap( filePointer, fileSize );
	}

bool dex::index::indexChunk::addDocument( const dex::string &url, const dex::vector < dex::string > &title,
		const dex::string &titleString, const dex::vector < dex::string > &body )
	{
	if ( url.size( ) > maxURLLength || titleString.size( ) > maxTitleLength )
		return true;

	dex::unorderedMap < dex::string, size_t > postsMetadataChanges;

	size_t documentOffset = *location;
	if ( !append( body.cbegin( ), body.cend( ), postsMetadataChanges )
			|| !append( title.cbegin( ), title.cend( ), postsMetadataChanges, "#" ) )
		return false;

	*maxLocation = *location;

	urlsToOffsets[ url ] = documentOffset;

	// TODO: This can be made more efficient by creating an auxillary vector of words we want to insert. At the same
	// time, we would populate uniqueWords
	dex::unorderedSet < const dex::string > uniqueWords( 2 * ( body.size( ) + title.size( ) ) );
	for ( dex::vector < dex::string >::constIterator it = body.cbegin( );  it != body.cend( );  ++it )
		uniqueWords.insert( dex::porterStemmer::stem( *it ) );
	for ( dex::vector < dex::string >::constIterator it = title.cbegin( );  it != title.cend( );  ++it )
		uniqueWords.insert( dex::porterStemmer::stem( *it ) );

	// Update the count of how many documents each word appears in.
	for ( dex::unorderedSet < const dex::string >::constIterator uniqueWord = uniqueWords.cbegin( );
			uniqueWord != uniqueWords.cend( );  ++postsMetadataArray[ dictionary[ *( uniqueWord++ ) ] ].documentCount )
		postsMetadataArray[ dictionary[ *uniqueWord ] ].occurenceCount += postsMetadataChanges[ *uniqueWord ];

	offsetsToEndOfDocumentMetadatas[ documentOffset ] = endOfDocumentMetadataType
		{
		title.size( ) + body.size( ),
		uniqueWords.size( ),
		url,
		titleString,
		};

	postsMetadataArray[ 0 ].append( ( *location )++, postsChunkArray );

	return true;
	}

dex::index::indexChunk::indexStreamReader::indexStreamReader( indexChunk *chunk, dex::string word ) :
		indexChunkum( chunk ), absoluteLocation( 0 )
	{
	postsMetadatum = chunk->postsMetadataArray + chunk->dictionary[ dex::porterStemmer::stem( word ) ];
	postsChunkum = chunk->postsChunkArray + postsMetadatum->firstPostsChunkOffset;
	post = postsChunkum->posts;
	}

size_t dex::index::indexChunk::indexStreamReader::seek( size_t target )
	{
	std::cout << "seek(" << target << ")\tabsoluteLocation: " << absoluteLocation << "\n";
	std::cout << "\tfinding syncPoint at index: " << ( target >> ( 8 * sizeof( target ) - 8 ) ) << "\n";

	postsMetadata::synchronizationPoint *syncPoint
			= postsMetadatum->synchronizationPoints + ( target >> ( 8 * sizeof( target ) - 8 ) );


	// TODO: Maybe remove?
	// if ( target < absoluteLocation )
		// throw dex::invalidArgumentException( );

	if ( target > *( indexChunkum->maxLocation ) )
		return ( npos );

	if ( ~syncPoint->inverseLocation == syncPoint->npos )
		return ( npos );

	// Jump to the point the synchronization table tells us to.
	// if ( ~syncPoint->inverseLocation > absoluteLocation )
	if ( ~syncPoint->inverseLocation != absoluteLocation )
		{
		postsChunkum = indexChunkum->postsChunkArray + syncPoint->postsChunkArrayOffset;
		post = postsChunkum->posts + syncPoint->postsChunkOffset;
		absoluteLocation = ~syncPoint->inverseLocation;
		std::cout << "\tseek-ed to absolute location: " << absoluteLocation << "\n";
		}

	// Keep scanning until we find the first place not before our target. We'll return -1 if we fail to reach it.
	std::cout << "\tseek-ing for target: " << target << "\tabsoluteLocation: " << absoluteLocation << "\n";
	while ( absoluteLocation < target )
		{
		if ( next( ) == npos )
			return npos;
		std::cout << "\tseek-ing for target: " << target << "\tabsoluteLocation: " << absoluteLocation << "\n";
		}

	std::cout << "\tFound at absoluteLocation: " << absoluteLocation << "\n";

	return absoluteLocation;
	}

size_t dex::index::indexChunk::indexStreamReader::next( )
	{
	std::cout << "next( )\n";
	std::cout << "\tpost: " << *post << " " << *( post + 1 ) << "\n";
	std::cout << "\tabsoluteLocation: " << absoluteLocation << "\n";
	std::cout << "\tpostsMetadatum: " << postsMetadatum << "\n";
	std::cout << "\tpostsChunkum: " << postsChunkum << "\n";
	std::cout << "\tindexChunkum: " << indexChunkum << "\n";
	/*
	std::cout << '\t' << postsMetadatum->occurenceCount
			<< '\t' << absoluteLocation
			<< '\t' << *( indexChunkum->maxLocation )
			<< '\t' << dex::utf::isSentinel( post )
			<< '\t' << !postsChunkum->nextPostsChunkOffset << std::endl;
	*/
	if ( postsMetadatum->occurenceCount == 0 || absoluteLocation >= *( indexChunkum->maxLocation )
			|| ( dex::utf::isSentinel( post ) && !postsChunkum->nextPostsChunkOffset ) )
		return npos;

	if ( dex::utf::isSentinel( post ) )
		// This operation returns a "good" postsChunk because we only add a new chunk if we have data to add (i.e. we
		// are now no longer pointing to a sentinel).
		{
		postsChunkum = indexChunkum->postsChunkArray + postsChunkum->nextPostsChunkOffset;
		post = postsChunkum->posts;
		}

	// Post is a pointer to a valid encoded size_t.
	absoluteLocation += dex::utf::decoder < size_t >( )( post, &post );
	std::cout << "next found at absoluteLocation: " << absoluteLocation << "\n";
	return absoluteLocation;
	// return absoluteLocation += dex::utf::decoder < size_t >( )( post, &post );
	}

size_t dex::index::indexChunk::indexStreamReader::nextDocument( )
	{
	dex::index::indexChunk::indexStreamReader endOfDocumentISR( indexChunkum, "" );
	size_t endOfDocumentLocation = endOfDocumentISR.seek( absoluteLocation );
	if ( endOfDocumentLocation == npos )
		return npos;
	return seek( endOfDocumentLocation );
	}

dex::index::indexChunk::endOfDocumentIndexStreamReader::endOfDocumentIndexStreamReader( indexChunk *chunk, dex::string )
		: dex::index::indexChunk::indexStreamReader( chunk, "" ) { }

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::seek( size_t target )
	{
	return dex::index::indexChunk::indexStreamReader::seek( target );
	}

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::next( )
	{
	return dex::index::indexChunk::indexStreamReader::next( );
	}

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::nextDocument( )
	{
	return dex::index::indexChunk::indexStreamReader::nextDocument( );
	}

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::documentSize( )
	{
	if ( indexChunkum->offsetsToEndOfDocumentMetadatas.count( absoluteLocation ) )
		return indexChunkum->offsetsToEndOfDocumentMetadatas[ absoluteLocation ].documentLength;
	return dex::index::indexChunk::indexStreamReader::npos;
	}
