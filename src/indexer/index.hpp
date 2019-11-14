// index.hpp
// MVP indexer
//
// 2019-11-13: Define most of index chunk internals: jasina, lougheem
// 2019-11-10: Outline of main classes and functions created: jasina, lougheem
// 2019-11-03: File created

#ifndef DEX_INDEX
#define DEX_INDEX

#include <cstddef>
#include <sys/mman.h>
#include <sys/stat.h>
#include "../utils/basicString.hpp"
#include "../utils/unorderedMap.hpp"
#include "../utils/utf.hpp"
#include "../utils/vector.hpp"

namespace dex
	{
	class index
		{
		private:
			// TODO: Master index

			class indexChunk
				{
				private:
					typedef unsigned char byte;

					class postsChunk
						{
						private:
							// Posting list is size 2^16 for now
							static const size_t postsChunkSize = 1 << 16;
							byte posts[ postsChunkSize ];

							// These are 0 if they do not "point" to anything meaningful
							size_t previousPostsChunkOffset;
							size_t nextPostsChunkOffset;

							// Keep track of where we should append the next post
							size_t currentPostOffset;
						public:
							postsChunk( size_t previousPostsChunkOffset = 0 ) :
									previousPostsChunkOffset( previousPostsChunkOffset ), nextPostsChunkOffset( 0 ),
									currentPostOffset( 0 ) { }

							bool append( size_t delta )
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
						};

					class postsMetadata
						{
						public:
							enum : byte { END_OF_DOCUMENT, ANCHOR_WORD, URL, TITLE, BODY }; // Types of tokens

							struct endOfDocumentMetadataType
								{
								size_t documentLength;
								char url[ 1 << 10 ];
								char title[ 1 << 10 ];

								size_t numberUniqueWords;

								unsigned numberIncomingLinks;
								};

						private:
							friend class indexChunk;

							// Common header
							size_t occurenceCount;
							size_t documentCount;

							byte postType;

							// Meta-data
							union metaDataType
								{
								endOfDocumentMetadataType endOfDocumentMetadata;
								};
							metaDataType metaData;

							// First 32 bits of each long long form the seek offset in posting. The last 32 bits are actual
							// location of that post. We use a long long since it is (practically) guaranteed to be 64 bits.
							static const size_t synchronizationPointCount = 1 << 8;
							unsigned long long synchronizationPoints[ synchronizationPointCount ];

							// Offsets from the beginning of the postsChunkArray to let us access the chunks we want
							size_t firstPostsChunkOffset;
							size_t lastPostsChunkOffset;

							// Keep track of the index of the last inserted word so that we can calculate the next delta
							size_t lastPostIndex;

						public:
							// TODO: Pass in offset numbers
							postsMetadata( size_t chunkOffset = 0, const byte typeOfToken = BODY,
									void *metaDataPtr = nullptr ) :
									occurenceCount( 0 ), documentCount( 0 ), postType( typeOfToken ),
									firstPostsChunkOffset( chunkOffset ), lastPostsChunkOffset( chunkOffset ),
									lastPostIndex( 0 )
								{
								if ( typeOfToken == END_OF_DOCUMENT )
									metaData.endOfDocumentMetadata = *static_cast < endOfDocumentMetadataType * >( metaDataPtr );
								}

							bool append( size_t location, postsChunk *postsChunkArray,
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
						};

					// These consts can be adjusted if necessary
					static const size_t maxURLCount = 1L << 25;
					static const size_t maxURLLength = 1L << 10;
					static const size_t maxWordLength = 64;
					static const size_t postsChunkArraySize = 1LL << 40;
					static const size_t postsMetadataArraySize = 1L << 30;

					static const size_t urlsMemorySize = 7 + maxURLCount * ( 7 + maxURLLength );
					static const size_t dictionaryMemorySize = 7 + ( 7 + maxWordLength + 7 ) * postsMetadataArraySize;
					static const size_t postsMetadataArrayMemorySize = postsMetadataArraySize * sizeof( postsMetadata );
					static const size_t postsChunkArrayMemorySize = postsChunkArraySize * sizeof( postsChunk );

					static const size_t urlsMemoryOffset = 200;
					static const size_t dictionaryOffset = urlsMemoryOffset + urlsMemorySize;
					static const size_t postsMetadataArrayMemoryOffset = dictionaryOffset + dictionaryMemorySize;
					static const size_t postsChunkArrayMemoryOffset =
							postsMetadataArrayMemoryOffset + postsChunkArrayMemorySize;

					// How many spots are filled in the postsChunkArray
					size_t *postsChunkCount;

					// Number of tokens in the index
					size_t *location;

					// Use urls.size( ) to get how many documents there are in the index
					dex::vector < dex::string > urls;
					byte *encodedURLs;

					//  Use dictionary.size( ) to get "postsMetadataCount" (cf. postsChunkCount)
					dex::unorderedMap < dex::string, size_t > dictionary;
					byte *encodedDictionary;

					postsMetadata *postsMetadataArray;
					postsChunk *postsChunkArray;

				public:
					indexChunk( int fileDescriptor, bool initialize = true )
						{
						postsChunkCount = static_cast < size_t * >( mmap( nullptr, sizeof( size_t ),
								PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, 0 ) );

						location = static_cast < size_t * >( mmap( nullptr, sizeof( size_t ),
								PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, sizeof( size_t ) ) );

						encodedURLs = static_cast < byte * >( mmap( nullptr, urlsMemorySize,
								PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, urlsMemoryOffset ) );

						urls = dex::utf::decoder < dex::vector < dex::string > >( )( encodedURLs );

						encodedDictionary = static_cast < byte * >( mmap( nullptr, urlsMemorySize,
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

					~indexChunk( )
						{
						dex::utf::encoder < dex::vector < dex::string > >( )( urls, encodedURLs );
						dex::utf::encoder < dex::unorderedMap < dex::string, size_t > >( )( dictionary, encodedDictionary );
						}

					// InputIt should dereference to a string
					template < class InputIt >
					bool append( InputIt first, InputIt last )
						{
						// Need to keep track of our old state in case the appendation fails
						dex::unorderedMap < dex::string, size_t > newWords;
						size_t newLocation = *location;

						for ( ;  first != last;  ++first, ++newLocation )
							{
							if ( !dictionary.count( *first ) && !newWords.count( *first ) )
								{
								// TODO: maybe there is a way to not have to do the postsChunk check?
								if ( dictionary.size( ) == postsMetadataArraySize || *postsChunkCount == postsChunkArraySize )
									return false;

								// Add a new postsMetaData
								// TODO: make this sensitive to non-BODY types
								postsMetadataArray[ dictionary.size( ) + newWords.size( ) ]
										= postsMetadata( *postsChunkCount, postsMetadata::BODY, nullptr );

								// Add a new postsChunk
								postsChunkArray[ *postsChunkCount++ ] = postsChunk( 0 );

								newWords[ *first ] = dictionary.size( ) + newWords.size( );
								}

							postsMetadata &wordMetadata = postsMetadataArray[ dictionary[ *first ] ]
							while ( !wordMetadata.append( *location, ostsChunkArray, postsMetadataArray ) )
								{
								if ( *postsChunkCount == postsChunkArraySize )
									return false;

								postsChunkArray[ wordMetadata.lastPostsChunkOffset ].nextPostsChunkOffset = postChunkCount;
								postsChunkArray[ *postsChunkCount ] = postsChunk( wordMetadata.lastPostsChunkOffset );
								wordMetadata.lastPostsChunkOffset = postChunkCount++;
								}
							}

						// Copy over newWords into dict
						for ( const dex::unorderedMap::constIterator &it : newWords )
							dictionary.insert( *it );

						*location = newLocation;

						return true;
						}
				};

		public:
			// TODO: public interface
		};
	}

#endif
