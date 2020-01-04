// indexer.hpp
// Indexer.
//
// 2019-12-09: Fix bugs in building index: lougheem, jasina
// 2019-12-08: Begin IndexStreamReader class: lougheem
// 2019-11-22: Wrote addDocument, update sync points, encoder/decoder for metadata, add stemming, change how to go
//             between offsets and posts,move things to indexer.cpp: jasina, lougheem
// 2019-11-13: Define most of index chunk internals: jasina, lougheem
// 2019-11-10: Outline of main classes and functions created: jasina, lougheem
// 2019-11-03: File created

#ifndef DEX_INDEX
#define DEX_INDEX

#include <cstddef>
#include <cstdint>
#include "constraintSolver/constraintSolver.hpp"
#include "utils/basicString.hpp"
#include "utils/stemming.hpp"
#include "utils/unorderedMap.hpp"
#include "utils/unorderedSet.hpp"
#include "utils/utf.hpp"
#include "utils/utility.hpp"
#include "utils/vector.hpp"

#include <iostream>

namespace dex
	{
	namespace index
		{
		class indexChunk
			{
			private:
				typedef unsigned char byte;

				template < class T, class InputIt >
				friend class dex::utf::decoder;

				template < class T >
				friend class dex::utf::encoder;

				class postsChunk
					{
					private:
						friend class indexChunk;
						friend class indexStreamReader;

						// Posting list is size 2^10 for now.
						static const size_t postsChunkSize = 1 << 10;
						byte posts[ postsChunkSize ];

						// This is 0 if they do not "point" to anything meaningful.
						uint32_t nextPostsChunkOffset;

						// Keep track of where we should append the next post.
						uint32_t currentPostOffset;
					public:
						postsChunk( );

						bool append( uint32_t delta );
					};

				class postsMetadata
					{
					private:
						friend class indexChunk;

						// Common header.
						uint32_t occurenceCount;
						uint32_t documentCount;

						// Offsets from the beginning of the postsChunkArray to let us access the chunks we want.
						uint32_t firstPostsChunkOffset;
						uint32_t lastPostsChunkOffset;

						// Keep track of the index of the last inserted word so that we can calculate the next delta.
						uint32_t lastLocation;

						struct synchronizationPoint
							{
							static const uint32_t npos = static_cast< uint32_t >( -1 );

							uint32_t postsChunkArrayOffset;
							uint32_t postsChunkOffset;
							uint32_t inverseLocation;
							};
						// First 32 bits of each long long form the seek offset in posting. The last 32 bits are actual
						// location of that post. We use a long long since it is (practically) guaranteed to be 64 bits.
						static const uint32_t synchronizationPointCount = 1 << 8;
						synchronizationPoint synchronizationPoints[ synchronizationPointCount ];

					public:
						postsMetadata( uint32_t chunkOffset );

						bool append( uint32_t location, postsChunk *postsChunkArray );
					};

			public:
				// This is public so we can write the encoder and decoders. There might be a better way of doing this, but
				// time is short.
				struct endOfDocumentMetadataType
					{
					uint32_t documentLength;
					uint32_t numberUniqueWords;
					dex::string url;
					dex::string title;

					template < class T, class InputIt >
					friend class dex::utf::decoder;

					template < class T >
					friend class dex::utf::encoder;
					};

				// These consts can be adjusted if necessary.
				// static const uint32_t maxURLCount = 1L << 17;
				// static const uint32_t maxURLLength = 1L << 10;
				// static const uint32_t maxTitleLength = 1 << 10;
				// static const uint32_t maxWordLength = 64;
				// static const uint32_t postsChunkArraySize = 1L << 19;
				// static const uint32_t postsMetadataArraySize = 1L << 19;

				// Keep these for testing
				static const uint32_t maxURLCount = 1L << 8;
				static const uint32_t maxURLLength = 1L << 10;
				static const uint32_t maxTitleLength = 1 << 10;
				static const uint32_t maxWordLength = 64;
				static const uint32_t postsChunkArraySize = 1L << 10;
				static const uint32_t postsMetadataArraySize = 1L << 10;

				// TODO: Double check these very carefully.
				// Note: these sizes should be such that they are block-aligned. The required offest for block alignment
				// is surrounded by parentheses.
				static const size_t endOfDocumentMetadataTypeMemorySize = 7 + ( 1 ) + 7 + ( 1 )
						+ ( 7 + maxURLLength + ( 1 ) )
						+ ( 7 + maxTitleLength + ( 1 ) );
				static const size_t urlsToOffsetsMemorySize = 7 + maxURLCount * ( 7 + maxURLLength + ( 1 ) + 7 + ( 1 ) ) + ( 1 );
				static const size_t offsetsToEndOfDocumentMetadatasMemorySize =
						7 + maxURLCount * ( 7 + endOfDocumentMetadataTypeMemorySize + ( 1 ) ) + ( 1 );
				static const size_t dictionaryMemorySize = 7
						+ ( 7 + maxWordLength + 7 + ( 2 ) ) * postsMetadataArraySize + ( 1 );
				static const size_t postsMetadataArrayMemorySize = postsMetadataArraySize * sizeof( postsMetadata );
				static const size_t postsChunkArrayMemorySize = postsChunkArraySize * sizeof( postsChunk );

				static const size_t urlsToOffsetsMemoryOffset = 200;
				static const size_t offsetsToEndOfDocumentMetadatasMemoryOffset =
						urlsToOffsetsMemoryOffset + 2 * urlsToOffsetsMemorySize;
				static const size_t dictionaryOffset =
						offsetsToEndOfDocumentMetadatasMemoryOffset + 2 * offsetsToEndOfDocumentMetadatasMemorySize;
				static const size_t postsMetadataArrayMemoryOffset = dictionaryOffset + 2 * dictionaryMemorySize;
				static const size_t postsChunkArrayMemoryOffset =
						postsMetadataArrayMemoryOffset + 2 * postsMetadataArrayMemorySize;
				static const size_t fileSize = postsChunkArrayMemoryOffset + 2 * postsChunkArrayMemorySize;

				// Our mmaped file.
				void *filePointer;

				// How many spots are filled in the postsChunkArray.
				uint32_t *postsChunkCount;

				// Number of tokens in the index.
				uint32_t *location;
				uint32_t *maxLocation;

				// Use urls.size( ) to get how many documents there are in the index.
				// The following two maps are effectively inverses of each other.
				dex::unorderedMap< dex::string, uint32_t > urlsToOffsets;
				dex::unorderedMap< uint32_t, endOfDocumentMetadataType > offsetsToEndOfDocumentMetadatas;
				byte *encodedURLsToOffsets;
				byte *encodedOffsetsToEndOfDocumentMetadatas;

				// Use dictionary.size( ) to get "postsMetadataCount" (cf. postsChunkCount).
				dex::unorderedMap< dex::string, uint32_t > dictionary;
				byte *encodedDictionary;

				postsMetadata *postsMetadataArray;
				postsChunk *postsChunkArray;

			public:
				indexChunk( int fileDescriptor, bool initialize = true );
				~indexChunk( );

				void printDictionary( );

			private:
				// InputIt should dereference to a string.
				// Note: This has to be defined in the header due to the templating.
				template < class InputIt >
				bool append( InputIt first, InputIt last, dex::unorderedMap< dex::string, uint32_t > &postsMetadataChanges,
						const dex::string &decorator = "" )
					{
					// Need to keep track of our old state in case the appendation fails.
					dex::unorderedMap< dex::string, uint32_t > newWords;
					uint32_t newLocation = *location;

					for ( ;  first != last;  ++first, ++newLocation )
						{
						string wordToAdd = dex::porterStemmer::stem( *first );
						string decoratedWordToAdd = decorator + wordToAdd;

						if ( wordToAdd.size( ) > maxWordLength )
							continue;

						postsMetadata *wordMetadata = nullptr;
						if ( !dictionary.count( decoratedWordToAdd ) && !newWords.count( decoratedWordToAdd ) )
							{
							if ( dictionary.size( ) + newWords.size( ) >= postsMetadataArraySize
									|| *postsChunkCount >= postsChunkArraySize )
								return false;

							// Add a new postsMetaData.
							uint32_t newWordIndex = dictionary.size( ) + newWords.size( );
							wordMetadata = &postsMetadataArray[ newWordIndex ];
							*wordMetadata = postsMetadata( *postsChunkCount );

							// Add a new postsChunk
							uint32_t newPostsChunkOffset = ( *postsChunkCount )++;
							postsChunkArray[ newPostsChunkOffset ] = postsChunk( );
							wordMetadata->firstPostsChunkOffset = newPostsChunkOffset;

							newWords[ decoratedWordToAdd ] = newWordIndex;
							}
						else
							{
							if ( dictionary.count( decoratedWordToAdd ) )
								wordMetadata = &postsMetadataArray[ dictionary[ decoratedWordToAdd ] ];
							else
								wordMetadata = &postsMetadataArray[ newWords[ decoratedWordToAdd ] ];
							}

						// This loop will exectue at most once, unless things go terribly wrong somehow.
						while ( !wordMetadata->append( newLocation, postsChunkArray ) )
							{
							if ( *postsChunkCount >= postsChunkArraySize )
								return false;

							postsChunkArray[ wordMetadata->lastPostsChunkOffset ].nextPostsChunkOffset = *postsChunkCount;
							postsChunkArray[ *postsChunkCount ] = postsChunk( );
							wordMetadata->lastPostsChunkOffset = ( *postsChunkCount )++;
							}

						++postsMetadataChanges[ decoratedWordToAdd ];
						}

					// Copy over newWords into dict.
					for ( dex::unorderedMap< dex::string, uint32_t >::constIterator it = newWords.cbegin( );
							it != newWords.cend( );  ++it )
						dictionary.insert( *it );

					*location = newLocation;

					return true;
					}

			public:
				bool addDocument( const dex::string &url, const dex::vector< dex::string > &title,
						const dex::string &titleString, const dex::vector< dex::string > &body );

				class indexStreamReader : public dex::constraintSolver::ISR
					{
					private:
						friend class indexChunk;

						byte *post;
						postsMetadata *postsMetadatum;
						postsChunk *postsChunkum; // Bad naming to disambiguate chunk types
						indexChunk *indexChunkum;
						size_t absoluteLocation;
						bool begun;

						dex::string word;

					public:
						// An ISR for the empty string is just and end of document ISR.
						indexStreamReader( indexChunk *chunk, dex::string word = "", dex::string decorator = "" );
						size_t seek( size_t target );
						size_t next( );
						size_t nextDocument( );
						size_t get( ) const;
					};

				class endOfDocumentIndexStreamReader
						: public indexStreamReader, public dex::constraintSolver::endOfDocumentISR
					{
					public:
						endOfDocumentIndexStreamReader( indexChunk *chunk, dex::string );
						size_t seek( size_t target );
						size_t next( );
						size_t nextDocument( );
						size_t get( ) const;
						size_t documentSize( ) const;
					};
			};
		}

	namespace utf
		{
		template < class InputIt >
		class decoder< dex::index::indexChunk::endOfDocumentMetadataType, InputIt > {
			public:
				dex::index::indexChunk::endOfDocumentMetadataType operator( )
						( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt *localAdvancedEncoding = &encoding;
					uint32_t documentLength = dex::utf::decoder< uint32_t, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );
					uint32_t numberUniqueWords = dex::utf::decoder< uint32_t, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );
					dex::string url = dex::utf::decoder< dex::string, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );
					dex::string title = dex::utf::decoder< dex::string, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return dex::index::indexChunk::endOfDocumentMetadataType
						{ documentLength, numberUniqueWords, url, title };
					}
			};

		template < >
		class encoder< dex::index::indexChunk::endOfDocumentMetadataType >
			{
			public:
				template < class InputIt >
				InputIt operator( )( const dex::index::indexChunk::endOfDocumentMetadataType &data, InputIt it ) const
					{
					it = dex::utf::encoder< uint32_t >( )( data.documentLength, it );
					it = dex::utf::encoder< uint32_t >( )( data.numberUniqueWords, it );
					it = dex::utf::encoder< dex::string >( )( data.url, it );
					it = dex::utf::encoder< dex::string >( )( data.title, it );
					return it;
					}

				dex::vector< unsigned char > operator( )
						( const dex::index::indexChunk::endOfDocumentMetadataType &data) const
					{
					dex::vector< unsigned char > encodedData = dex::utf::encoder< uint32_t >( )( data.documentLength );
					dex::vector< unsigned char > encodedDataNext = dex::utf::encoder< uint32_t >( )
							( data.numberUniqueWords );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					encodedDataNext =  dex::utf::encoder< dex::string >( )( data.url );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					encodedDataNext =  dex::utf::encoder< dex::string >( )( data.title );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					return encodedData;
					}
			};
		}
	}
#endif
