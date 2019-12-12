// index.hpp
// Indexer.
//
// 2019-12-09: Fix bugs in building index: lougheem, jasina
// 2019-12-08: Begin IndexStreamReader class: lougheem
// 2019-11-22: Wrote addDocument, update sync points, encoder/decoder for metadata, add stemming, change how to go
//             between offsets and posts,move things to index.cpp: jasina, lougheem
// 2019-11-13: Define most of index chunk internals: jasina, lougheem
// 2019-11-10: Outline of main classes and functions created: jasina, lougheem
// 2019-11-03: File created

#ifndef DEX_INDEX
#define DEX_INDEX

#include <cstddef>
#include "../utils/basicString.hpp"
#include "../utils/stemming.hpp"
#include "../utils/unorderedMap.hpp"
#include "../utils/unorderedSet.hpp"
#include "../utils/utf.hpp"
#include "../utils/utility.hpp"
#include "../utils/vector.hpp"

namespace dex
	{
	namespace index
		{
		class masterIndex
			{
			private:

			public:
			};

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

						// Posting list is size 2^16 for now.
						static const size_t postsChunkSize = 1 << 12;
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

						struct synchronizationPoint
							{
							size_t postsChunkArrayOffset;
							size_t postsChunkOffset;
							size_t location;
							};
						// First 32 bits of each long long form the seek offset in posting. The last 32 bits are actual
						// location of that post. We use a long long since it is (practically) guaranteed to be 64 bits.
						static const size_t synchronizationPointCount = 1 << 8;
						synchronizationPoint synchronizationPoints[ synchronizationPointCount ];

						// Offsets from the beginning of the postsChunkArray to let us access the chunks we want.
						size_t firstPostsChunkOffset;
						size_t lastPostsChunkOffset;

						// Keep track of the index of the last inserted word so that we can calculate the next delta.
						size_t lastPostIndex;

					public:
						postsMetadata( size_t chunkOffset = 0, const byte typeOfToken = BODY );

						bool append( size_t location, postsChunk *postsChunkArray );
					};

				struct endOfDocumentMetadataType
					{
					size_t documentLength;
					size_t numberUniqueWords;
					dex::string url;
					dex::string title;
					size_t numberIncomingLinks;

					template < class T, class InputIt >
					friend class dex::utf::decoder;

					template < class T >
					friend class dex::utf::encoder;
					};

				// These consts can be adjusted if necessary.
				static const size_t maxURLCount = 1L << 17;
				static const size_t maxURLLength = 1L << 10;
				static const size_t maxTitleLength = 1 << 10;
				static const size_t maxWordLength = 64;
				static const size_t postsChunkArraySize = 1L << 26;
				static const size_t postsMetadataArraySize = 1L << 25;

				// TODO: Double check these very carefully.
				// Note: these sizes should be such that they are block-aligned. The required offest for block alignment
				// is surrounded by parentheses.
				static const size_t endOfDocumentMetadataTypeMemorySize = 2 * sizeof( size_t )
						+ ( 7 + maxURLLength + ( 1 ) )
						+ ( 7 + maxTitleLength + ( 1 ) ) + sizeof( unsigned );
				static const size_t urlsToOffsetsMemorySize = 7 + maxURLCount * ( 7 + maxURLLength + ( 1 ) ) + ( 1 );
				static const size_t offsetsToEndOfDocumentMetadatasMemorySize =
						7 + maxURLCount * endOfDocumentMetadataTypeMemorySize + ( 1 );
				static const size_t dictionaryMemorySize = 7
						+ ( 7 + maxWordLength + 7 + ( 2 ) ) * postsMetadataArraySize + ( 1 );
				static const size_t postsMetadataArrayMemorySize = postsMetadataArraySize * sizeof( postsMetadata );
				static const size_t postsChunkArrayMemorySize = postsChunkArraySize * sizeof( postsChunk );

				static const size_t urlsToOffsetsMemoryOffset = 200;
				static const size_t offsetsToEndOfDocumentMetadatasMemoryOffset =
						urlsToOffsetsMemoryOffset + urlsToOffsetsMemorySize;
				static const size_t dictionaryOffset =
						offsetsToEndOfDocumentMetadatasMemoryOffset + offsetsToEndOfDocumentMetadatasMemorySize;
				static const size_t postsMetadataArrayMemoryOffset = dictionaryOffset + dictionaryMemorySize;
				static const size_t postsChunkArrayMemoryOffset =
						postsMetadataArrayMemoryOffset + postsMetadataArrayMemorySize;
				static const size_t fileSize = postsChunkArrayMemoryOffset + postsChunkArrayMemorySize;

				// Our mmaped file.
				void *filePointer;

				// How many spots are filled in the postsChunkArray.
				size_t *postsChunkCount;

				// Number of tokens in the index.
				size_t *location;
				size_t *maxLocation;

				// Use urls.size( ) to get how many documents there are in the index.
				// The following two maps are effectively inverses of each other.
				dex::unorderedMap < dex::string, size_t > urlsToOffsets;
				dex::unorderedMap < size_t, endOfDocumentMetadataType > offsetsToEndOfDocumentMetadatas;
				byte *encodedURLsToOffsets;
				byte *encodedOffsetsToEndOfDocumentMetadatas;

				// Use dictionary.size( ) to get "postsMetadataCount" (cf. postsChunkCount).
				dex::unorderedMap < dex::string, size_t > dictionary;
				byte *encodedDictionary;

				postsMetadata *postsMetadataArray;
				postsChunk *postsChunkArray;

			public:
				indexChunk( int fileDescriptor, bool initialize = true );
				~indexChunk( );

			private:
				// InputIt should dereference to a string.
				// Note: This has to be defined in the header due to the templating.
				template < class InputIt >
				bool append( InputIt first, InputIt last, dex::unorderedMap < dex::string, size_t > &postsMetadataChanges, const dex::string &decorator = "" )
					{
					// Need to keep track of our old state in case the appendation fails.
					dex::unorderedMap < dex::string, size_t > newWords;
					size_t newLocation = *location;

					for ( ;  first != last;  ++first, ++newLocation )
						{
						string wordToAdd = decorator + dex::porterStemmer::stem( *first );

						if ( wordToAdd.size( ) > maxWordLength )
							continue;

						postsMetadata *wordMetadata = nullptr;
						if ( !dictionary.count( wordToAdd ) && !newWords.count( wordToAdd ) )
							{
							if ( dictionary.size( ) == postsMetadataArraySize || *postsChunkCount == postsChunkArraySize )
								return false;

							// Add a new postsMetaData.
							// TODO: make this sensitive to non-BODY types. Idea: use enums and pass those in instead of a
							// deocrator string.
							wordMetadata = &postsMetadataArray[ dictionary.size( ) + newWords.size( ) ];
							*wordMetadata = postsMetadata( *postsChunkCount, postsMetadata::BODY );

							// Add a new postsChunk
							size_t newPostsChunkOffset = ( *postsChunkCount )++;
							postsChunkArray[ newPostsChunkOffset ] = postsChunk( 0 );
							wordMetadata->firstPostsChunkOffset = newPostsChunkOffset;

							newWords[ wordToAdd ] = dictionary.size( ) + newWords.size( );
							}
						else
							{
							if ( dictionary.count( wordToAdd ) )
								{
								wordMetadata = &postsMetadataArray[ dictionary[ wordToAdd ] ];
								}
							else
								{
								wordMetadata = &postsMetadataArray[ newWords[ wordToAdd ] ];
								}
							}

						while ( !wordMetadata->append( newLocation, postsChunkArray ) )
							{
							if ( *postsChunkCount == postsChunkArraySize )
								return false;

							postsChunkArray[ wordMetadata->lastPostsChunkOffset ].nextPostsChunkOffset = *postsChunkCount;
							postsChunkArray[ *postsChunkCount ] = postsChunk( wordMetadata->lastPostsChunkOffset );
							wordMetadata->lastPostsChunkOffset = ( *postsChunkCount )++;
							}

						if ( postsMetadataChanges.count( wordToAdd ) )
							postsMetadataChanges[ wordToAdd ]++;
						else
							postsMetadataChanges[ wordToAdd ] = 1;
						}

					// Copy over newWords into dict.
					for ( dex::unorderedMap < dex::string, size_t >::constIterator it = newWords.cbegin( );
							it != newWords.cend( );  ++it )
						dictionary.insert( *it );

					*location = newLocation;

					return true;
					}

			public:
				bool addDocument( const dex::string &url, const dex::vector < dex::string > &title,
						const dex::string &titleString, const dex::vector < dex::string > &body );

				class indexStreamReader
					{
					private:
						friend class indexChunk;

						static const size_t npos = static_cast < size_t >( -1 );

						byte *post;
						byte *documentPost;
						postsMetadata *postsMetadatum;
						postsChunk *postsChunkum; // Bad naming to disambiguate chunk types
						indexChunk *indexChunkum;
						size_t absoluteLocation;
						// For a word, will want
						// 	the word (string)
						// 	current postsChunk
						// 	offset into the indexChunk where the current pointer is

					public:
						// An ISR for the empty string is just and end of document ISR.
						indexStreamReader( indexChunk *indexChunk, dex::string word = "" );
						size_t seek( size_t target );
						size_t next( );
						size_t nextDocument( );	// This is called on a set of indexStream readers. It sets them all to their
						// 	first occurences past the end of the current document

						// size_t GetStartLocation( ); ??
						// size_t GetEndLocation( ); ??

						// Need functions to get metadata for a word for entire posting list
						// 	and for in the current document
					};
			};
		}

	namespace utf
		{
		template < class InputIt >
		class decoder < dex::index::indexChunk::endOfDocumentMetadataType, InputIt > {
			public:
				dex::index::indexChunk::endOfDocumentMetadataType operator( )
						( InputIt encoding, InputIt *advancedEncoding = nullptr ) const
					{
					InputIt *localAdvancedEncoding = &encoding;
					size_t documentLength = dex::utf::decoder < size_t, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );
					size_t numberUniqueWords = dex::utf::decoder < size_t, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );
					dex::string url = dex::utf::decoder < dex::string, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );
					dex::string title = dex::utf::decoder < dex::string, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );
					size_t numberIncomingLinks = dex::utf::decoder < unsigned, InputIt >( )
							( *localAdvancedEncoding, localAdvancedEncoding );

					if ( advancedEncoding )
						*advancedEncoding = *localAdvancedEncoding;

					return dex::index::indexChunk::endOfDocumentMetadataType
						{ documentLength, numberUniqueWords, url, title, numberIncomingLinks };
					}
			};

		template < >
		class encoder < dex::index::indexChunk::endOfDocumentMetadataType >
			{
			public:
				template < class InputIt >
				InputIt operator( )( const dex::index::indexChunk::endOfDocumentMetadataType &data, InputIt it ) const
					{
					it = dex::utf::encoder < size_t >( )( data.documentLength, it );
					it = dex::utf::encoder < size_t >( )( data.numberUniqueWords, it );
					it = dex::utf::encoder < dex::string >( )( data.url, it );
					it = dex::utf::encoder < dex::string >( )( data.title, it );
					it = dex::utf::encoder < unsigned >( )( data.numberIncomingLinks, it );
					return it;
					}

				dex::vector < unsigned char > operator( )
						( const dex::index::indexChunk::endOfDocumentMetadataType &data) const
					{
					dex::vector < unsigned char > encodedData = dex::utf::encoder < size_t >( )( data.documentLength );
					dex::vector < unsigned char > encodedDataNext = dex::utf::encoder < size_t >( )
							( data.numberUniqueWords );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					encodedDataNext =  dex::utf::encoder < dex::string >( )( data.url );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					encodedDataNext =  dex::utf::encoder < dex::string >( )( data.title );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					encodedDataNext =  dex::utf::encoder < unsigned >( )( data.numberIncomingLinks );
					encodedData.insert( encodedData.cend( ), encodedDataNext.cbegin( ), encodedDataNext.cend( ) );
					return encodedData;
					}
			};
		}
	}

#endif
