// index.hpp
// Indexer.
//
// 2019-12-09: Fix bugs in addDocument and append: lougheem, jasina
// 2019-12-08: IndexStreamReader constructor and next: lougheem
// 2019-11-21: File created

#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "index.hpp"
#include "../utils/basicString.hpp"
#include "../utils/exception.hpp"
#include "../utils/unorderedMap.hpp"
#include "../utils/unorderedSet.hpp"
#include "../utils/utf.hpp"
#include "../utils/utility.hpp"
#include "../utils/vector.hpp"

// TODO: remove this
#include <iostream>

// postsChunk
dex::index::indexChunk::postsChunk::postsChunk( size_t previousPostsChunkOffset ) :
	previousPostsChunkOffset( previousPostsChunkOffset ), nextPostsChunkOffset( 0 ),
	currentPostOffset( 0 ) { }

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
dex::index::indexChunk::postsMetadata::postsMetadata( size_t chunkOffset, const byte typeOfToken ) :
		occurenceCount( 0 ), documentCount( 0 ), postType( typeOfToken ),
		firstPostsChunkOffset( chunkOffset ), lastPostsChunkOffset( chunkOffset ),
		lastPostIndex( 0 )
		{
		std::memset( synchronizationPoints, 0, synchronizationPointCount * sizeof( unsigned long long ) );
		}

bool dex::index::indexChunk::postsMetadata::append( size_t location, postsChunk *postsChunkArray,
		postsMetadata *endOfDocumentPostsMetadata )
	{
	size_t delta = location - lastPostIndex;
	size_t originalPostOffset = postsChunkArray[ lastPostsChunkOffset ].currentPostOffset;
	bool successful = postsChunkArray[ lastPostsChunkOffset ].append( delta );

	if ( successful )
		{
		// If this occurence is in a currently "unseen" document
		if ( lastPostIndex < endOfDocumentPostsMetadata->lastPostIndex )
			// Then increase the count of documents we've seen
			++documentCount;

		++occurenceCount;
		lastPostIndex = location;

		// The first 8 bits of our location determine our synchronization point. We only update the table if we haven't
		// been "this high" before.
		for ( synchronizationPoint *syncPoint = synchronizationPoints + ( location >> ( sizeof( location ) - 8 ) );
				syncPoint >= synchronizationPoints && !syncPoint->location;  --syncPoint )
			{
			syncPoint->postsChunkArrayOffset = lastPostsChunkOffset;
			syncPoint->postsChunkOffset = originalPostOffset;
			syncPoint->location = location;
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
		{
		close( fileDescriptor );
		return;
		}

	if ( initialize )
		{
		int result = lseek( fileDescriptor, fileSize - 1, SEEK_SET );
		if ( result == -1 )
			{
			close( fileDescriptor );
			return;
			}
		result = write( fileDescriptor, "", 1 );
		if ( result == -1 )
			{
			close( fileDescriptor );
			return;
			}
		}

	filePointer = mmap( nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0 );

	postsChunkCount = reinterpret_cast < size_t * >( filePointer );
	location = postsChunkCount + 1;

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

		postsChunkArray[ 0 ] = postsChunk( );
		postsMetadataArray[ 0 ] = postsMetadata( 0, postsMetadata::END_OF_DOCUMENT );
		dictionary[ "" ] = 0;
		}
	else
		{
			urlsToOffsets = dex::utf::decoder < dex::unorderedMap < dex::string, size_t > >( )
					( encodedURLsToOffsets );
			offsetsToEndOfDocumentMetadatas = dex::utf::decoder < dex::unorderedMap < size_t, endOfDocumentMetadataType > >( )
					( encodedOffsetsToEndOfDocumentMetadatas );
			dictionary = dex::utf::decoder < dex::unorderedMap < dex::string, size_t > >( )( encodedDictionary );
		}
	}

dex::index::indexChunk::~indexChunk( )
	{
	dex::utf::encoder < dex::unorderedMap < dex::string, size_t > >( )
			( urlsToOffsets, encodedURLsToOffsets );
	dex::utf::encoder < dex::unorderedMap < size_t, endOfDocumentMetadataType > >( )
			( offsetsToEndOfDocumentMetadatas, encodedOffsetsToEndOfDocumentMetadatas );
	dex::utf::encoder < dex::unorderedMap < dex::string, size_t > >( )( dictionary, encodedDictionary );

	msync( filePointer, fileSize, MS_SYNC );
	munmap( filePointer, fileSize );
	}

// TODO: remove (replace?) the anchorText argument
bool dex::index::indexChunk::addDocument( const dex::string &url, const dex::vector < dex::string > &anchorText,
		const dex::vector < dex::string > &title, const dex::string &titleString,
		const dex::vector < dex::string > &body )
	{
	if ( url.size( ) > maxURLLength || titleString.size( ) > maxTitleLength )
		throw dex::invalidArgumentException( );

	size_t documentOffset = *location;
	if ( !append( body.cbegin( ), body.cend( ) ) || !append( title.cbegin( ), title.cend( ), "#" )
			|| !append( anchorText.cbegin( ), anchorText.cend( ), "@" ) )
		return false;

	urlsToOffsets[ url ] = documentOffset;

	dex::unorderedSet < const dex::string > uniqueWords;
	for ( dex::vector < dex::string >::constIterator it = body.cbegin( );  it != body.cend( );  ++it )
		uniqueWords.insert( dex::porterStemmer::stem( *it ) );
	for ( dex::vector < dex::string >::constIterator it = title.cbegin( );  it != title.cend( );  ++it )
		uniqueWords.insert( dex::porterStemmer::stem( *it ) );

	// Update the count of how many documents each word appears in.
	for ( dex::unorderedSet < const dex::string >::constIterator uniqueWord = uniqueWords.cbegin( );
			uniqueWord != uniqueWords.cend( ); postsMetadataArray[ dictionary[ *( uniqueWord++ ) ] ].documentCount++ )

	offsetsToEndOfDocumentMetadatas[ documentOffset ] = endOfDocumentMetadataType
		{
		title.size( ) + body.size( ),
		uniqueWords.size( ),
		url,
		titleString,
		1 // TODO: how do we update this? Answer: it's really really hard :(
		};

	postsMetadataArray[ 0 ].append( ( *location )++, postsChunkArray, postsMetadataArray );

	return true;
	}

dex::index::indexChunk::indexStreamReader::indexStreamReader( indexChunk *indexChunk, dex::string word ) :
		indexChunkum( indexChunk ), absoluteLocation( 0 )
	{
	postsMetadatum = indexChunkum->postsMetadataArray + indexChunkum->dictionary[ word ];
	postsChunkum = indexChunkum->postsChunkArray + postsMetadatum->firstPostsChunkOffset;
	post = postsChunkum->posts;
	documentPost = indexChunkum->postsChunkArray[ 0 ].posts;
	}

size_t dex::index::indexChunk::indexStreamReader::seek( size_t target )
	{
	postsMetadata::synchronizationPoint *syncPoint
			= postsMetadatum->synchronizationPoints + ( target >> ( sizeof( target ) - 8 ) );

	// TODO: Maybe remove?
	if ( target < absoluteLocation )
		throw dex::invalidArgumentException( );

	if ( !syncPoint->location )
		return ( npos );

	// Jump to the point the synchronization table tells us to.
	if ( syncPoint->location > absoluteLocation )
		{
		postsChunkum = indexChunkum->postsChunkArray + syncPoint->postsChunkArrayOffset;
		post = postsChunkum->posts + syncPoint->postsChunkOffset;
		absoluteLocation = syncPoint->location;
		}

	// Keep scanning until we find the first place not before our target. We'll return -1 if we fail to reach it.
	while ( absoluteLocation < target )
		if ( next( ) == npos )
			return npos;

	return absoluteLocation;
	}

size_t dex::index::indexChunk::indexStreamReader::next( )
	{
	if ( postsMetadatum->occurenceCount == 0
			|| ( dex::utf::isSentinel( post ) && !postsChunkum->nextPostsChunkOffset ) )
		return npos;

	if ( dex::utf::isSentinel( post ) )
		// This operation returns a "good" postsChunk because we only add a new chunk if we have data to add (i.e. we
		// are now no longer pointing to a sentinel).
		postsChunkum = indexChunkum->postsChunkArray + postsChunkum->nextPostsChunkOffset;

	// Assume valid posts and all...
	// Post is a pointer to an encoded size_t.
	return absoluteLocation += dex::utf::decoder < size_t >( )( post, &post );
	}

size_t dex::index::indexChunk::indexStreamReader::nextDocument( )
	{
	dex::index::indexChunk::indexStreamReader endOfDocumentISR( indexChunkum );
	size_t endOfDocumentLocation = endOfDocumentISR.seek( absoluteLocation );
	if ( endOfDocumentLocation == npos )
		return npos;
	return seek( endOfDocumentLocation );
	}

size_t dex::index::indexChunk::indexStreamReader::documentSize( )
	{
	if ( indexChunkum->offsetsToEndOfDocumentMetadatas.count( absoluteLocation ) )
		return indexChunkum->offsetsToEndOfDocumentMetadatas[ absoluteLocation ].documentLength;
	return -1;
	}
