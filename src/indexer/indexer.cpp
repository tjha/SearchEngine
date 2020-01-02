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
#include "indexer/indexer.hpp"
#include "utils/basicString.hpp"
#include "utils/exception.hpp"
#include "utils/unorderedMap.hpp"
#include "utils/unorderedSet.hpp"
#include "utils/utf.hpp"
#include "utils/utility.hpp"
#include "utils/vector.hpp"

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
	}

bool dex::index::indexChunk::postsMetadata::append( size_t location, postsChunk *postsChunkArray )
	{
	size_t delta = location - lastLocation;
	if ( lastPostsChunkOffset > ( 1L << 19 ) - 1 )
		{
		std::cout << "not good! lastPostsChunkOffset: " << lastPostsChunkOffset << "\n";
		// exit(1);
		}
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
	if ( !append( body.cbegin( ), body.cend( ), postsMetadataChanges ) )
		return false;
	++location;
	if ( !append( title.cbegin( ), title.cend( ), postsMetadataChanges, "#" ) )
		return false;

	*maxLocation = *location;

	urlsToOffsets[ url ] = documentOffset;

	// TODO: This can be made more efficient by creating an auxillary vector of words we want to insert. At the same
	// time, we would populate uniqueWords
	dex::unorderedSet < const dex::string > uniqueWords( 2 * ( body.size( ) + title.size( ) ) );
	for ( dex::vector < dex::string >::constIterator it = body.cbegin( );  it != body.cend( );  ++it )
		uniqueWords.insert( dex::porterStemmer::stem( *it ) );
	for ( dex::vector < dex::string >::constIterator it = title.cbegin( );  it != title.cend( );  ++it )
		uniqueWords.insert( dex::porterStemmer::stem( "#" + *it ) );

	// Update the count of how many documents each word appears in.
	for ( dex::unorderedSet < const dex::string >::constIterator uniqueWord = uniqueWords.cbegin( );
			uniqueWord != uniqueWords.cend( );  ++postsMetadataArray[ dictionary[ *( uniqueWord++ ) ] ].documentCount )
		{
		postsMetadataArray[ dictionary[ *uniqueWord ] ].occurenceCount += postsMetadataChanges[ *uniqueWord ];
		}

	offsetsToEndOfDocumentMetadatas[ documentOffset ] = endOfDocumentMetadataType
		{
		title.size( ) + body.size( ),
		uniqueWords.size( ),
		url,
		titleString,
		};

	postsMetadataArray[ 0 ].append( ( *location )++, postsChunkArray );
	++postsMetadataArray[ 0 ].occurenceCount;

	return true;
	}

void dex::index::indexChunk::printDictionary( )
	{
	for ( auto it = dictionary.cbegin( );  it != dictionary.cend( );  ++it )
		{
		std::cout << it->first << std::endl;
		}
	}

dex::index::indexChunk::indexStreamReader::indexStreamReader( indexChunk *chunk, dex::string word ) :
		indexChunkum( chunk ), absoluteLocation( 0 ), toGet( npos )
	{
	if ( !chunk->dictionary.count( dex::porterStemmer::stem( word ) ) )
		{
		postsMetadatum = nullptr;
		postsChunkum = nullptr;
		post = nullptr;
		begun = false;
		return;
		}
	postsMetadatum = chunk->postsMetadataArray + chunk->dictionary[ dex::porterStemmer::stem( word ) ];
	postsChunkum = chunk->postsChunkArray + postsMetadatum->firstPostsChunkOffset;
	post = postsChunkum->posts;
	begun = false;
	}

size_t dex::index::indexChunk::indexStreamReader::seek( size_t target )
	{
	if ( !postsMetadatum )
		return ( npos );

	if ( target == 0 )
		begun = false;

	postsMetadata::synchronizationPoint *syncPoint
			= postsMetadatum->synchronizationPoints + ( target >> ( 8 * sizeof( target ) - 8 ) );

	// TODO: Maybe remove?
	// if ( target < absoluteLocation )
		// throw dex::invalidArgumentException( );

	if ( target > *( indexChunkum->maxLocation ) )
		return toGet = npos;

	if ( ~syncPoint->inverseLocation == syncPoint->npos )
		return toGet = npos;

	// Jump to the point the synchronization table tells us to.
	if ( ~syncPoint->inverseLocation != absoluteLocation )
		{
		postsChunkum = indexChunkum->postsChunkArray + syncPoint->postsChunkArrayOffset;
		post = postsChunkum->posts + syncPoint->postsChunkOffset;
		absoluteLocation = ~syncPoint->inverseLocation;
		}
	postsChunkum = indexChunkum->postsChunkArray + postsMetadatum->firstPostsChunkOffset;
	post = postsChunkum->posts;
	absoluteLocation = 0;

	// Keep scanning until we find the first place not before our target. We'll return -1 if we fail to reach it.
	while ( absoluteLocation < target || !begun )
		{
		begun = true;
		if ( next( ) == npos )
			return toGet = npos;
		}

	return toGet = absoluteLocation;
	}

size_t dex::index::indexChunk::indexStreamReader::next( )
	{

	if ( !postsMetadatum )
		return ( npos );

	if ( postsMetadatum->occurenceCount == 0 || absoluteLocation >= *( indexChunkum->maxLocation )
			|| ( dex::utf::isSentinel( post ) && !postsChunkum->nextPostsChunkOffset ) )
		return toGet = npos;

	if ( dex::utf::isSentinel( post ) )
		// This operation returns a "good" postsChunk because we only add a new chunk if we have data to add (i.e. we
		// are now no longer pointing to a sentinel).
		{
		postsChunkum = indexChunkum->postsChunkArray + postsChunkum->nextPostsChunkOffset;
		post = postsChunkum->posts;
		}

	// Post is a pointer to a valid encoded size_t.
	absoluteLocation += dex::utf::decoder < size_t >( )( post, &post );

	return toGet = absoluteLocation;
	}

size_t dex::index::indexChunk::indexStreamReader::nextDocument( )
	{
	if ( !postsMetadatum )
		return ( npos );
	dex::index::indexChunk::indexStreamReader endOfDocumentISR( indexChunkum, "" );
	size_t endOfDocumentLocation = endOfDocumentISR.seek( absoluteLocation );
	if ( endOfDocumentLocation == npos )
		return toGet = npos;
	return toGet = seek( endOfDocumentLocation );
	}

size_t dex::index::indexChunk::indexStreamReader::get( )
	{
	return toGet;
	}

dex::index::indexChunk::endOfDocumentIndexStreamReader::endOfDocumentIndexStreamReader( indexChunk *chunk, dex::string )
		: dex::index::indexChunk::indexStreamReader( chunk, "" ), toGet( this->npos ) { }

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::seek( size_t target )
	{
	return toGet = dex::index::indexChunk::indexStreamReader::seek( target );
	}

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::next( )
	{
	return toGet = dex::index::indexChunk::indexStreamReader::next( );
	}

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::nextDocument( )
	{
	return toGet = dex::index::indexChunk::indexStreamReader::nextDocument( );
	}

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::documentSize( )
	{
	if ( indexChunkum->offsetsToEndOfDocumentMetadatas.count( absoluteLocation ) )
		return indexChunkum->offsetsToEndOfDocumentMetadatas[ absoluteLocation ].documentLength;
	return dex::index::indexChunk::indexStreamReader::npos;
	}

size_t dex::index::indexChunk::endOfDocumentIndexStreamReader::get( )
	{
	return toGet;
	}
