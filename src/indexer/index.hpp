// index.hpp
// MVP indexer
//
// 2019-11-03: File created

#ifndef DEX_INDEX
#define DEX_INDEX

#include <cstddef>
#include "../utils/basicString.hpp"
#include "../utils/unorderedMap.hpp"
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
					vector < string > urls;
					unorderedMap < string, size_t > dictionary;
					// "Dictionary" may also include (1) number of tokens in index,
					//                               (2) number of unique tokens,
					//                               (3) number of documents

					typedef unsigned char byte;
					byte *postingLists;

					class postingList
						{
						private:
							// Common header
							size_t occurenceCount;
							size_t documentCount;

							enum : byte { END_OF_DOCUMENT, ANCHOR_WORD, URL, TITLE, BODY }; // Types of tokens
							byte postType;

							// Meta-data
							struct endOfDocumentMetadataType
								{
								size_t documentLength;
								string url;
								string title;

								size_t numberUniqueWords;

								unsigned numberIncomingLinks;
								};
							endOfDocumentMetadataType *endOfDocumentMetadata;

							// First 32 bits are seek offset in posting. Last 32 bits are actual location of that post.
							unsigned long long synchronizationPoints[ 1 << 8 ];
						public:
							// TODO
						};
				public:
					// TODO
				};

		public:
			// TODO: public interface
		};
	}

#endif