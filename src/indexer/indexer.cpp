// indexer.cpp
// Indexer.
//
// 2019-12-09: Fix bugs in addDocument and append: lougheem, jasina
// 2019-12-08: IndexStreamReader constructor and next: lougheem
// 2019-11-21: File created

#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "indexer/indexer.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/unorderedMap.hpp"
#include "utils/unorderedSet.hpp"
#include "utils/utf.hpp"
#include "utils/utility.hpp"
#include "utils/vector.hpp"


// postsChunk
dex::index::indexChunk::postsChunk::postsChunk( ) : nextPostsChunkOffset( 0 ), currentPostOffset( 0 )
	{
	posts[ 0 ] = dex::utf::sentinel;
	}

bool dex::index::indexChunk::postsChunk::append( uint32_t delta )
	{
	// We're "full" if we can't insert a "widest" UTF-8 character (estimated 8 characters, to be safe) and a sentinel.
	if ( currentPostOffset + 9 >= postsChunkSize )
		return false;

	currentPostOffset = dex::utf::encoder< uint32_t >( )( delta, posts + currentPostOffset ) - posts;
	posts[ currentPostOffset ] = dex::utf::sentinel;

	return true;
	}

// postMetadata
dex::index::indexChunk::postsMetadata::postsMetadata( uint32_t chunkOffset ) :
		occurenceCount( 0 ), documentCount( 0 ), firstPostsChunkOffset( chunkOffset ),
		lastPostsChunkOffset( chunkOffset ), lastLocation( 0 ), synchronizationPoints( ) { }

bool dex::index::indexChunk::postsMetadata::append( uint32_t location, postsChunk *postsChunkArray )
	{
	uint32_t delta = location - lastLocation;
	uint32_t originalPostOffset = postsChunkArray[ lastPostsChunkOffset ].currentPostOffset;
	bool successful = postsChunkArray[ lastPostsChunkOffset ].append( delta );

	if ( successful )
		{
		// The first 11 bits of the 31-bit location determine our synchronization point. We only update the table if we
		// haven't been "this high" before.
		for ( synchronizationPoint *syncPoint = synchronizationPoints + ( location >> 20 );
				syncPoint >= synchronizationPoints && ~syncPoint->inverseLocation == syncPoint->npos;  --syncPoint )
			*syncPoint = synchronizationPoint
				{
				lastPostsChunkOffset,
				originalPostOffset,
				~lastLocation
				};

		lastLocation = location;
		}

	return successful;
	}

// indexChunk
dex::index::indexChunk::indexChunk( int fileDescriptor, bool initialize )
	{
	if ( fileDescriptor == -1 )
		throw dex::exception( );

	if ( initialize )
		{
		int result = lseek( fileDescriptor, fileSize - 1, SEEK_SET );
		if ( result == -1 )
			throw dex::exception( );
		result = write( fileDescriptor, "", 1 );
		if ( result == -1 )
			throw dex::exception( );
		}

	filePointer = mmap( nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0 );

	if ( filePointer == MAP_FAILED )
		throw dex::exception( );

	postsChunkCount = reinterpret_cast< uint32_t * >( filePointer );
	location = postsChunkCount + 1;
	maxLocation = postsChunkCount + 2;

	encodedURLsToOffsets = reinterpret_cast< byte * >( filePointer ) + urlsToOffsetsMemoryOffset;

	encodedOffsetsToEndOfDocumentMetadatas = reinterpret_cast< byte * >( filePointer )
			+ offsetsToEndOfDocumentMetadatasMemoryOffset;

	encodedDictionary = reinterpret_cast< byte * >( filePointer ) + dictionaryMemoryOffset;

	postsMetadataArray = reinterpret_cast< postsMetadata * >(
			reinterpret_cast< byte * >( filePointer ) + postsMetadataArrayMemoryOffset );

	postsChunkArray = reinterpret_cast< postsChunk * >(
			reinterpret_cast< byte * >( filePointer ) + postsChunkArrayMemoryOffset );

	if ( initialize )
		{
		*postsChunkCount = 1;
		*location = 1;
		*maxLocation = 1;

		postsChunkArray[ 0 ] = postsChunk( );
		postsMetadataArray[ 0 ] = postsMetadata( 0 );
		dictionary[ "" ] = 0;

		offsetsToEndOfDocumentMetadatas[ 0 ] = endOfDocumentMetadataType { 1, 0, "", "" };
		postsMetadataArray[ 0 ].append( 0, postsChunkArray );
		}
	else
		{
		urlsToOffsets = dex::utf::decoder< dex::unorderedMap< dex::string, uint32_t > >( )( encodedURLsToOffsets );
		offsetsToEndOfDocumentMetadatas = dex::utf::decoder< dex::unorderedMap< uint32_t, endOfDocumentMetadataType > >( )
				( encodedOffsetsToEndOfDocumentMetadatas );
		dictionary = dex::utf::decoder< dex::unorderedMap< dex::string, uint32_t > >( )( encodedDictionary );
		}
	}

dex::index::indexChunk::~indexChunk( )
	{
	dex::utf::encoder< dex::unorderedMap< dex::string, uint32_t > >( )( urlsToOffsets, encodedURLsToOffsets );
	dex::utf::encoder< dex::unorderedMap< uint32_t, endOfDocumentMetadataType > >( )
			( offsetsToEndOfDocumentMetadatas, encodedOffsetsToEndOfDocumentMetadatas );
	dex::utf::encoder< dex::unorderedMap< dex::string, uint32_t > >( )( dictionary, encodedDictionary );

	msync( filePointer, fileSize, MS_SYNC );
	munmap( filePointer, fileSize );
	}

bool dex::index::indexChunk::addDocument( const dex::string &url, const dex::vector< dex::string > &title,
		const dex::string &titleString, const dex::vector< dex::string > &body )
	{
	// Ignore documents that have long URLs or titles
	if ( url.size( ) > maxURLLength || titleString.size( ) > maxTitleLength )
		return true;

	// Ensure we never add too many docs
	if ( urlsToOffsets.size( ) >= maxURLCount )
		return false;

	dex::unorderedMap< dex::string, uint32_t > postsMetadataChanges( 2 * ( body.size( ) + title.size( ) ) );

	uint32_t documentOffset = *location;
	if ( !append( body.cbegin( ), body.cend( ), postsMetadataChanges ) )
		return false;
	++( *location );
	if ( !append( title.cbegin( ), title.cend( ), postsMetadataChanges, "#" ) )
		return false;

	*maxLocation = *location;

	urlsToOffsets[ url ] = documentOffset;

	// Update the count of how many documents each word appears in.
	for ( dex::unorderedMap< dex::string, uint32_t >::constIterator it = postsMetadataChanges.cbegin( );
			it != postsMetadataChanges.cend( );  ++it )
		{
		postsMetadata &datum = postsMetadataArray[ dictionary[ it->first ] ];
		++datum.documentCount;
		datum.occurenceCount += it->second;
		}

	offsetsToEndOfDocumentMetadatas[ *location ] = endOfDocumentMetadataType
		{
		static_cast< uint32_t >( title.size( ) + 1 + body.size( ) + 1 ),
		static_cast< uint32_t >( postsMetadataChanges.size( ) ),
		url,
		titleString,
		};

	// Add end of document
	postsMetadataArray[ 0 ].append( *location, postsChunkArray );
	++postsMetadataArray[ 0 ].occurenceCount;
	++( *location );

	return true;
	}

void dex::index::indexChunk::printDictionary( )
	{
	for ( auto it = dictionary.cbegin( );  it != dictionary.cend( );  ++it )
		std::cout << it->first << std::endl;
	}

dex::index::indexChunk::indexStreamReader::indexStreamReader(
		indexChunk *chunk, dex::string word, dex::string decorator ) :
		indexChunkum( chunk ), absoluteLocation( 0 ), word( word )
	{
	dex::string stemmedWord = decorator + dex::porterStemmer::stem( word );
	if ( !chunk->dictionary.count( stemmedWord ) )
		{
		postsMetadatum = nullptr;
		postsChunkum = nullptr;
		post = nullptr;
		begun = false;
		return;
		}
	postsMetadatum = chunk->postsMetadataArray + chunk->dictionary[ stemmedWord ];
	postsChunkum = chunk->postsChunkArray + postsMetadatum->firstPostsChunkOffset;
	post = postsChunkum->posts;
	begun = false;
	}

size_t dex::index::indexChunk::indexStreamReader::seek( size_t target )
	{
	if ( !postsMetadatum )
		return absoluteLocation = npos;

	if ( target > *( indexChunkum->maxLocation ) )
		return absoluteLocation = npos;

	postsMetadata::synchronizationPoint *syncPoint
			= postsMetadatum->synchronizationPoints + ( target >> 20 );

	if ( ~syncPoint->inverseLocation == syncPoint->npos )
		return absoluteLocation = npos;

	// Jump to the point the synchronization table tells us to.
	if ( ~syncPoint->inverseLocation != absoluteLocation )
		{
		postsChunkum = indexChunkum->postsChunkArray + syncPoint->postsChunkArrayOffset;
		post = postsChunkum->posts + syncPoint->postsChunkOffset;
		absoluteLocation = ~syncPoint->inverseLocation;
		}

	// Keep scanning until we find the first place not before our target. We'll return -1 if we fail to reach it.
	do
		if ( next( ) == npos )
			return absoluteLocation = npos;
	while ( absoluteLocation < target );

	return absoluteLocation;
	}

size_t dex::index::indexChunk::indexStreamReader::next( )
	{
	if ( !postsMetadatum || postsMetadatum->occurenceCount == 0 || absoluteLocation > *( indexChunkum->maxLocation )
			|| ( dex::utf::isSentinel( post ) && !postsChunkum->nextPostsChunkOffset ) )
		return absoluteLocation = npos;

	if ( dex::utf::isSentinel( post ) )
		// This operation returns a "good" postsChunk because we only add a new chunk if we have data to add (i.e. we
		// are now no longer pointing to a sentinel).
		{
		postsChunkum = indexChunkum->postsChunkArray + postsChunkum->nextPostsChunkOffset;
		post = postsChunkum->posts;
		}

	// Post is a pointer to a valid encoded size_t.
	return absoluteLocation += dex::utf::decoder< uint32_t >( )( post, &post );
	}

size_t dex::index::indexChunk::indexStreamReader::nextDocument( )
	{
	if ( !postsMetadatum )
		return absoluteLocation = npos;
	dex::index::indexChunk::indexStreamReader endOfDocumentISR( indexChunkum, "" );
	size_t endOfDocumentLocation = endOfDocumentISR.seek( absoluteLocation );
	if ( endOfDocumentLocation == npos )
		return absoluteLocation = npos;

	return seek( endOfDocumentLocation + 1 );
	}

size_t dex::index::indexChunk::indexStreamReader::get( ) const
	{
	return absoluteLocation;
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
	return next( );
	}

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::get( ) const
	{
	return dex::index::indexChunk::indexStreamReader::get( );
	}

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::documentSize( ) const
	{
	if ( indexChunkum->offsetsToEndOfDocumentMetadatas.count( absoluteLocation ) )
		return indexChunkum->offsetsToEndOfDocumentMetadatas[ absoluteLocation ].documentLength;
	return dex::index::indexChunk::indexStreamReader::npos;
	}
