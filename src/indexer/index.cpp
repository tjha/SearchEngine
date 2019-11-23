// index.hpp
// Indexer.
//
// 2019-11-21: File created

#include <cstddef>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "index.hpp"
#include "../utils/basicString.hpp"
#include "../utils/unorderedMap.hpp"
#include "../utils/unorderedSet.hpp"
#include "../utils/utf.hpp"
#include "../utils/utility.hpp"
#include "../utils/vector.hpp"

// postsChunk

dex::index::indexChunk::postsChunk::postsChunk( size_t previousPostsChunkOffset = 0 ) :
	previousPostsChunkOffset( previousPostsChunkOffset ), nextPostsChunkOffset( 0 ),
	currentPostOffset( 0 ) { }

bool dex::index::indexChunk::postsChunk::append( size_t delta )
	{
	// We're "full" if we can't insert a "widest" UTF-8 character
	if ( currentPostOffset + 8 > postsChunkSize )
		return false;

	// TODO: This should use something like longLongToUTF to take care of the "big delta" case
	dex::vector < byte > utf8Delta = dex::utf::encoder < size_t >( )( delta );
	for ( byte b : utf8Delta )
		posts[ currentPostOffset++ ] = b;

	return true;
	}

// postMetadata

dex::index::indexChunk::postsMetadata::postsMetadata( size_t chunkOffset = 0, const byte typeOfToken = BODY ) :
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

dex::index::indexChunk::indexChunk( int fileDescriptor, bool initialize = true )
	{
	postsChunkCount = static_cast < size_t * >( mmap( nullptr, sizeof( size_t ),
			PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, 0 ) );

	location = static_cast < size_t * >( mmap( nullptr, sizeof( size_t ),
			PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, sizeof( size_t ) ) );

	encodedURLsToOffsets = static_cast < byte * >( mmap( nullptr, urlsToOffsetsMemorySize,
			PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, urlsToOffsetsMemoryOffset ) );

	urlsToOffsets = dex::utf::decoder < dex::unorderedMap < dex::string, size_t > >( )
			( encodedURLsToOffsets );

	encodedOffsetsToPostMetadatas = static_cast < byte * >( mmap( nullptr, offsetsToPostMetadatasMemorySize,
			PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, offsetsToPostMetadatasMemoryOffset ) );

	offsetsToPostMetadatas = dex::utf::decoder < dex::unorderedMap < size_t, endOfDocumentMetadataType > >( )
			( encodedOffsetsToPostMetadatas );

	encodedDictionary = static_cast < byte * >( mmap( nullptr, dictionaryMemorySize,
			PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, dictionaryOffset) );

	dictionary = dex::utf::decoder < dex::unorderedMap < dex::string, size_t > >( )( encodedDictionary );

	postsMetadataArray = static_cast < postsMetadata * >( mmap( nullptr, postsMetadataArrayMemorySize,
			PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, postsMetadataArrayMemoryOffset ) );

	postsChunkArray = static_cast < postsChunk * >( mmap( nullptr, postsChunkArrayMemorySize,
			PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, postsChunkArrayMemoryOffset ) );

	if ( initialize )
		{
		*postsChunkCount = 0;
		*location = 0;

		// TODO: Add end of document metadata
		// postsMetadataArray[ 0 ] = postsMetadata( 0, postsMetadata::END_OF_DOCUMENT,  )
		}
	}

dex::index::indexChunk::~indexChunk( )
	{
	dex::utf::encoder < dex::unorderedMap < dex::string, size_t > >( )
			( urlsToOffsets, encodedURLsToOffsets );
	dex::utf::encoder < dex::unorderedMap < size_t, endOfDocumentMetadataType > >( )
			( offsetsToPostMetadatas, encodedOffsetsToPostMetadatas );
	dex::utf::encoder < dex::unorderedMap < dex::string, size_t > >( )( dictionary, encodedDictionary );
	}

bool dex::index::indexChunk::addDocument( const dex::string &url, const dex::vector < dex::string > &title,
		const dex::vector < dex::string > &body )
	{
	size_t documentOffset = location;
	if ( !append( body.cbegin( ), body.cend( ) ) || !append( title.cbegin( ), title.cend( ), "#" ) )
		return false;

	urlsToOffsets[ url ] = documentOffset;

	// TODO: Maybe make a function to count the number of unique words in a bunch of vectors?
	dex::unorderedSet < const dex::string > uniqueWords;
	for ( const dex::string &word : body )
		uniqueWords.insert( dex::porterStemmer( word ) );
	for ( const dex::string &word : title )
		uniqueWords.insert( dex::porterStemmer( word ) );

	offsetsToPostMetadatas[ documentOffset ] = endOfDocumentMetadataType
		{
		title.size( ) + body.size( ),
		uniqueWords.size( ),
		url,
		title,
		1 // TODO: how do we update this?
		};
	}