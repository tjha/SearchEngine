// index.hpp
// Indexer.
//
// 2019-11-21: ????????
// 2019-11-13: Define most of index chunk internals: jasina, lougheem
// 2019-11-10: Outline of main classes and functions created: jasina, lougheem
// 2019-11-03: File created

#ifndef DEX_INDEX
#define DEX_INDEX

#include <cstddef>
#include "../utils/basicString.hpp"
#include "../utils/unorderedMap.hpp"
#include "../utils/utf.hpp"
#include "../utils/utility.hpp"
#include "../utils/vector.hpp"

namespace dex
	{
	class index
		{
		private:
			// TODO: Master index.

			class indexChunk
				{
				private:
					typedef unsigned char byte;

					template < class T, class InputIt >
					friend class dex::utf::decoder;

					template < class T, class InputIt >
					friend class dex::utf::encoder;

					class postsChunk
						{
						private:
							// Posting list is size 2^16 for now.
							static const size_t postsChunkSize = 1 << 16;
							byte posts[ postsChunkSize ];

							// These are 0 if they do not "point" to anything meaningful.
							size_t previousPostsChunkOffset;
							size_t nextPostsChunkOffset;

							// Keep track of where we should append the next post.
							size_t currentPostOffset;
						public:
							postsChunk( size_t previousPostsChunkOffset = 0 );

							bool append( size_t delta );
						};

					class postsMetadata
						{
						public:
							// Types of tokens.
							enum : byte { END_OF_DOCUMENT, ANCHOR_WORD, URL, TITLE, BODY };

						private:
							friend class indexChunk;

							// Common header.
							size_t occurenceCount;
							size_t documentCount;

							byte postType;

							// First 32 bits of each long long form the seek offset in posting. The last 32 bits are actual
							// location of that post. We use a long long since it is (practically) guaranteed to be 64 bits.
							static const size_t synchronizationPointCount = 1 << 8;
							unsigned long long synchronizationPoints[ synchronizationPointCount ];

							// Offsets from the beginning of the postsChunkArray to let us access the chunks we want.
							size_t firstPostsChunkOffset;
							size_t lastPostsChunkOffset;

							// Keep track of the index of the last inserted word so that we can calculate the next delta.
							size_t lastPostIndex;

						public:
							postsMetadata( size_t chunkOffset = 0, const byte typeOfToken = BODY );

							bool append( size_t location, postsChunk *postsChunkArray,
									postsMetadata *endOfDocumentPostsMetadata );
						};

					struct endOfDocumentMetadataType
						{
						size_t documentLength;
						size_t numberUniqueWords;
						dex::string url;
						dex::string title;
						unsigned numberIncomingLinks;

						template < class T, class InputIt >
						friend class dex::utf::decoder;

						template < class T, class InputIt >
						friend class dex::utf::encoder;
						};

					// These consts can be adjusted if necessary.
					static const size_t maxURLCount = 1L << 25;
					static const size_t maxURLLength = 1L << 10;
					static const size_t maxWordLength = 64;
					static const size_t postsChunkArraySize = 1LL << 40;
					static const size_t postsMetadataArraySize = 1L << 30;

					static const size_t urlsToOffsetsMemorySize = 7 + maxURLCount * ( 7 + maxURLLength );
					static const size_t offsetsToURLsMemorySize = urlsToOffsetsMemorySize;
					static const size_t dictionaryMemorySize = 7 + ( 7 + maxWordLength + 7 ) * postsMetadataArraySize;
					static const size_t postsMetadataArrayMemorySize = postsMetadataArraySize * sizeof( postsMetadata );
					static const size_t postsChunkArrayMemorySize = postsChunkArraySize * sizeof( postsChunk );

					static const size_t urlsToOffsetsMemoryOffset = 200;
					static const size_t offsetsToURLsMemoryOffset = urlsToOffsetsMemoryOffset + urlsToOffsetsMemorySize;
					static const size_t dictionaryOffset = offsetsToURLsMemoryOffset + offsetsToURLsMemorySize;
					static const size_t postsMetadataArrayMemoryOffset = dictionaryOffset + dictionaryMemorySize;
					static const size_t postsChunkArrayMemoryOffset =
							postsMetadataArrayMemoryOffset + postsChunkArrayMemorySize;

					// How many spots are filled in the postsChunkArray.
					size_t *postsChunkCount;

					// Number of tokens in the index.
					size_t *location;

					// Use urls.size( ) to get how many documents there are in the index.
					// The following two maps are effectively inverses of each other.
					dex::unorderedMap < dex::string, size_t > urlsToOffsets;
					dex::unorderedMap < size_t, endOfDocumentMetadataType > offsetsToPostMetadatas;
					byte *encodedURLsToOffsets;
					byte *encodedOffsetsToURLs;

					// Use dictionary.size( ) to get "postsMetadataCount" (cf. postsChunkCount).
					dex::unorderedMap < dex::string, size_t > dictionary;
					byte *encodedDictionary;

					postsMetadata *postsMetadataArray;
					postsChunk *postsChunkArray;

				public:
					indexChunk( int fileDescriptor, bool initialize = true );
					~indexChunk( );

					// InputIt should dereference to a string.
					// Note: This has to be defined in the header due to the templating.
					template < class InputIt >
					bool append( InputIt first, InputIt last )
						{
						// Need to keep track of our old state in case the appendation fails.
						dex::unorderedMap < dex::string, size_t > newWords;
						size_t newLocation = *location;

						for ( ;  first != last;  ++first, ++newLocation )
							{
							postsMetadata *wordMetadata = nullptr;
							if ( !dictionary.count( *first ) && !newWords.count( *first ) )
								{
								// TODO: maybe there is a way to not have to do the postsChunk check?
								if ( dictionary.size( ) == postsMetadataArraySize || *postsChunkCount == postsChunkArraySize )
									return false;

								// Add a new postsMetaData.
								// TODO: make this sensitive to non-BODY types
								wordMetadata = &postsMetadataArray[ dictionary.size( ) + newWords.size( ) ]
								*wordMetadata = postsMetadata( *postsChunkCount, postsMetadata::BODY, nullptr );

								// Add a new postsChunk
								postsChunkArray[ *postsChunkCount++ ] = postsChunk( 0 );

								newWords[ *first ] = dictionary.size( ) + newWords.size( );
								}
							else
								wordMetadata = &postsMetadataArray[ dictionary[ *first ] ]

							// Note: this loop executes its body at most once, unless things have gone impossibly, horribly,
							// terribly wrong somehow.
							while ( !wordMetadata.append( *location, ostsChunkArray, postsMetadataArray ) )
								{
								if ( *postsChunkCount == postsChunkArraySize )
									return false;

								postsChunkArray[ wordMetadata.lastPostsChunkOffset ].nextPostsChunkOffset = postChunkCount;
								postsChunkArray[ *postsChunkCount ] = postsChunk( wordMetadata.lastPostsChunkOffset );
								wordMetadata.lastPostsChunkOffset = postChunkCount++;
								}
							}

						// Copy over newWords into dict.
						for ( const dex::unorderedMap::constIterator &it : newWords )
							dictionary.insert( *it );

						*location = newLocation;

						return true;
						}
				};

		public:
			// TODO: public interface.
		};

	namespace utf
		{
		template < class InputIt >
		class decoder < dex::index::indexChunk::endOfDocumentMetadataType, InputIt >
			{
			public:
				dex::index::indexChunk::endOfDocumentMetadataType operator( )
						( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt *localAdvancedEncoding = &encoding;
					size_t documentLength = decoder < size_t, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );
					size_t numberUniqueWords = decoder < size_t, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );
					dex::string url = decoder < dex::string, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );
					dex::string title = decoder < dex::string, InputIt >( )( *localAdvancedEncoding, localAdvancedEncoding );
					size_t numberIncomingLinks = decoder < unsigned, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return dex::index::indexChunk::endOfDocumentMetadataType
						{ documentLength, numberUniqueWords, url, title, numberIncomingLinks };
					}
			};

		class encoder < dex::index::indexChunk::endOfDocumentMetadataType >
			{
			public:
				template < class InputIt >
				InputIt operator( )( const dex::index::indexChunk::endOfDocumentMetadataType &data, InputIt it ) const
					{
					it = encoder < size_t >( )( data.documentLength, it );
					it = encoder < size_t >( )( data.numberUniqueWords, it );
					it = encoder < dex::string >( )( data.url, it );
					it = encoder < dex::string >( )( data.title, it );
					it = encoder < unsigned >( )( data.numberIncomingLinks, it );
					return it;
					}

				dex::vector < unsigned char > operator( )
						( const dex::index::indexChunk::endOfDocumentMetadataType &data) const
					{
					dex::vector < unsigned char > encodedData = encoder < size_t >( )( data.documentLength );
					dex::vector < unsigned char > encodedDataNext = encoder < size_t >( )( data.numberUniqueWords );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					encodedDataNext =  encoder < dex::string >( )( data.url );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					encodedDataNext =  encoder < dex::string >( )( data.title );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					encodedDataNext =  encoder < unsigned >( )( data.numberIncomingLinks );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					return encodedData;
					}
			};
		}
	}


#endif
