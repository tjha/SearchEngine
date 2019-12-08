// index.hpp
// Indexer.
//
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
#include "../utils/unorderedMap.hpp"
#include "../utils/unorderedSet.hpp"
#include "../utils/utf.hpp"
#include "../utils/utility.hpp"
#include "../utils/vector.hpp"

#include <iostream>

// postsChunk
dex::index::indexChunk::postsChunk::postsChunk( size_t previousPostsChunkOffset ) :
	previousPostsChunkOffset( previousPostsChunkOffset ), nextPostsChunkOffset( 0 ),
	currentPostOffset( 0 ) { }

bool dex::index::indexChunk::postsChunk::append( size_t delta )
	{
	// We're "full" if we can't insert a "widest" UTF-8 character
	if ( currentPostOffset + 8 > postsChunkSize )
		return false;

	// TODO: This maybe should use something like longLongToUTF to take care of the "big delta" case
	// TODO: Use the better encode fuction here
	dex::vector < byte > utf8Delta = dex::utf::encoder < size_t >( )( delta );
	for ( byte b : utf8Delta )
		posts[ currentPostOffset++ ] = b;

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
	bool successful = postsChunkArray[ lastPostsChunkOffset ].append( delta );

	if ( successful )
		{
		// If this occurence is in a currently "unseen" document
		if ( lastPostIndex < endOfDocumentPostsMetadata->lastPostIndex )
			// Then increase the count of documents we've seen
			++documentCount;

		++occurenceCount;
		lastPostIndex = location;
		}

	return successful;
	}

// indexChunk
dex::index::indexChunk::indexChunk( int fileDescriptor, bool initialize )
	{
	// TOOO: Add some sort of magic number

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
		*postsChunkCount = 0;
		*location = 0;

		postsChunkArray[ 0 ] = postsChunk( );
		postsMetadataArray[ 0 ] = postsMetadata( 0, postsMetadata::END_OF_DOCUMENT );
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

bool dex::index::indexChunk::addDocument( const dex::string &url, const dex::vector < dex::string > &anchorText,
		const dex::vector < dex::string > &title, const dex::string &titleString,
		const dex::vector < dex::string > &body )
	{
	size_t documentOffset = *location;
	if ( !append( body.cbegin( ), body.cend( ) ) || !append( title.cbegin( ), title.cend( ), "#" )
			|| !append( anchorText.cbegin( ), anchorText.cend( ), "@" ) )
		return false;

	urlsToOffsets[ url ] = documentOffset;

	// TODO: Maybe make a function to count the number of unique words in a bunch of vectors?
	dex::unorderedSet < const dex::string > uniqueWords;
	for ( dex::vector < dex::string >::constIterator it = body.cbegin( );  it != body.cend( );  ++it )
		uniqueWords.insert( dex::porterStemmer::stem( *it ) );
	for ( dex::vector < dex::string >::constIterator it = title.cbegin( );  it != title.cend( );  ++it )
		uniqueWords.insert( dex::porterStemmer::stem( *it ) );

	offsetsToEndOfDocumentMetadatas[ documentOffset ] = endOfDocumentMetadataType
		{
		title.size( ) + body.size( ),
		uniqueWords.size( ),
		url,
		titleString,
		1 // TODO: how do we update this?
		};

	return true;
	}

dex::index::indexChunk::indexStreamReader::indexStreamReader( dex::string word, indexChunk *indexChunk )
	{
	indexChunkum = indexChunk;
	postsMetadatum = indexChunkum->postsMetadataArray + indexChunkum->dictionary[ word ];
	postsChunkum = indexChunkum->postsChunkArray + postsMetadatum->firstPostsChunkOffset;
	post = postsChunkum->posts;
	}

unsigned char *dex::index::indexChunk::indexStreamReader::next( )
	{
	if ( postsMetadatum->occurenceCount == 0 )
		int x = 2; // TODO: fix this placeholder

	if ( !post )
		{
		postsChunkum = indexChunkum->postsChunkArray + postsMetadatum->firstPostsChunkOffset;
		post = postsChunkum->posts;
		}
	else
		{
		// Assume valid posts and all...
		// 	post is a pointer to an encoded something. Just decode this whatever using the decode functions
		size_t delta = dex::utf::decodeSafe( post );
		post += dex::utf::encoder < size_t >( )( delta ).size( ); // TODO: create ecoder length function
		return post;
		}
	}

