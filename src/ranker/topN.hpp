// topN.hpp
// Linked-list implementation of topN
//
// 2019-12-11:  Port over from HW4 as done by Matthew, Chris, and Tejas: lougheem

#ifndef DEX_TOP_N
#define DEX_TOP_N

#include "algorithm.hpp"
#include "exception.hpp"
#include "utility.hpp"
#include "vector.hpp"

struct documentInfo
	{
	size_t documentIndex;
	double score;
	};

struct node
	{
	node *next;
	node *prev;
	documentInfo *doc;
	};

documentInfo **topN( dex::vector< double > scores, int N )
	{
	dex::vector< documentInfo > documentsInformation;
	documentsInformation.reserve( scores.size( ) );
	for ( size_t index = 0;  index < scores.size( );  index++ )
		documentsInformation.pushBack( documentInfo{ index, scores[ index ] } );

	if ( documentsInformation.size( ) == 0 )
		{
		documentInfo **returnTopN = new documentInfo *[ N ];
		for ( int index = 0;  index < N;  index++ )
			returnTopN[ index ] = nullptr;
		return returnTopN;
		}

	node *list = nullptr;
	
	size_t listCount = 0;

	for ( dex::vector< documentInfo >::iterator documentIt = documentsInformation.begin( );
			documentIt != documentsInformation.cend( );  documentIt++ )
		{
		node *currentNode = list;
		if ( !currentNode )
			{
			list = new node{ nullptr, nullptr, &( *documentIt ) };
			listCount++;
			continue;
			}

		if( !currentNode->next )
			{
			node *newNode = new node{ nullptr, nullptr, &( *documentIt ) };
			node *greater;
			node *lesser;
			if ( newNode->doc->score > currentNode->doc->score )
				{
				greater = currentNode;
				lesser = newNode;
				}
			else
				{
				greater = currentNode;
				lesser = newNode;
				}
			greater->prev = lesser;
			lesser->next = greater;
			list = lesser;
			listCount++;
			continue;
			}

		node *prevNode = nullptr;

		while ( currentNode && documentIt->score > currentNode->doc->score )
			{
			if ( currentNode == list )
				prevNode = list;
			else
				prevNode = prevNode->next;
			currentNode = currentNode->next;
			}

		if ( !prevNode )
			{
			if ( listCount == N )
				continue;
			node *newNode = new node{ currentNode, nullptr, &( *documentIt ) };
			currentNode->prev = newNode;
			list = newNode;
			listCount++;
			continue;
			}
		else
			{
			node *newNode = new node{ currentNode, prevNode, &( *documentIt ) };
			if ( currentNode )
				currentNode->prev = newNode;
			prevNode->next = newNode;
			if ( listCount == N )
				{
				list = list->next;
				delete list->prev;
				list->prev = nullptr;
				}
			else
				{
				listCount++;
				}
			}
		}
	node *top = list;
	while ( top->next )
		top = top->next;

	documentInfo **docs = new documentInfo *[ N ];
	int index = 0;

	while ( top )
		{
		docs[ index ] = top->doc;
		top = top->prev;
		index++;
		}

	for ( ;  index < N;  index++ )
		docs[ index ] = nullptr;

	node *next;
	while ( list )
		{
		next = list->next;
		delete list;
		list = next;
		}

	return docs;
	}

#endif