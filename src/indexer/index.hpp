// index.hpp
// MVP indexer
//
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

							bool full( )
								{
								// We're "full" if we can't insert a "widest" UTF-8 character
								return currentPostOffset + 8 > postsChunkSize;
								}

							bool append( size_t delta )
								{
								// TODO: maybe remove this?
								if ( full( ) )
									return false;

								// TODO: This should use something like longLongToUTF to take care of the "big delta" case
								vector < byte > utf8Delta = dex::utf::longToUTF( delta );
								for ( byte b : utf8Delta )
									posts[ currentPostOffset++ ] = b;
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
					static const size_t postsChunkArraySize = 1 << 40;
					static const size_t postsMetadataArraySize = 1 << 30;

					// TODO: make these just pointers. When mmapping in, then "assign" their sizes (i.e. make a huge blank
					// file first, and then map stuff in)
					postsChunk *postsChunkArray;
					postsMetadata *postsMetadataArray;

					// How many spots are filled in the postsChunkArray
					size_t postsChunkCount;

					// Number of tokens in the index
					size_t location;

					// Use urls.size( ) to get how many documents there are in the index
					vector < string > urls;

					//  Use dictionary.size( ) to get "postsMetadataCount" (cf. postsChunkCount)
					unorderedMap < string, size_t > dictionary;

				public:
					indexChunk( int fileDescriptor, bool reinitialize = true )
						{
						struct stat fileInfo;
						fstat( fileDescriptor, &fileInfo );
						byte *map = static_cast < byte * >( mmap( nullptr, fileInfo.st_size,
								PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, 0 ) );

						postsChunkArray = static_cast < postsChunk * >( mmap( nullptr, sizeof( postsChunk ) * postsChunkArraySize,
								PROT_READ || PROT_WRITE, MAP_PRIVATE, fileDescriptor, asdlkfjlkasdjfklsj ) );

						if ( reinitialize )
							{
							postsChunkCount = 0;
							location = 0;
							}
						else
							{
							
							}
						}

					~indexChunk( )
						{
						// Write stuff back into the memory-mapped file maybe.
						}

					// InputIt should dereference to a string
					template < class InputIt >
					void append( InputIt first, InputIt last )
						{
						while ( first != last )
							{
							if ( !dictionary.count( *first ) )
								{
								// Add a new postsMetaData
								// TODO: make this sensitive to non-BODY types
								postsMetadataArray[ dictionary.size( ) ] 
										= postsMetadata( postsChunkCount, postsMetadata::BODY, nullptr );

								// Add a new postsChunk
								postsChunkArray[ postsChunkCount++ ] = postsChunk( 0 );

								dictionary[ *first ] = dictionary.size( );
								}

							postsMetadata &wordMetadata = postsMetadataArray[ dictionary[ *first ] ]
							while ( !wordMetadata.append( location, ostsChunkArray, postsMetadataArray ) )
								{
								postsChunkArray[ wordMetadata.lastPostsChunkOffset ].nextPostsChunkOffset = postChunkCount;
								postsChunkArray[ postsChunkCount ] = postsChunk( wordMetadata.lastPostsChunkOffset );
								wordMetadata.lastPostsChunkOffset = postChunkCount++;
								}

							++location;
							}
						}
				};

		public:
			// TODO: public interface
		};
	}

#endif
