// ranker.hpp
// This ranks the stuff

// 2019-12-09: Init Commit: combsc + lougheem

#include "vector.hpp"
#include "../spinarak/url.hpp"

namespace dex
	{
	struct searchResult
		{
		dex::Url url;
		dex::string title;
		};

	class ISR
		{
		
		private:
			dex::vector < unsigned > internal;
			dex::string word;
			unsigned pos;

		public:
			const static unsigned npos = unsigned ( -1 );
			ISR( )
				{
				}
			ISR( dex::string word, dex::vector < unsigned > vecIn )
				{
				this->word = word;
				internal = vecIn;
				pos = 0;
				}
			unsigned first( )
				{
				return internal[ 0 ];
				}
			unsigned next( )
				{
				if ( pos == internal.size( ) )
					{
					return npos;
					}
				unsigned toRet = internal[ pos ];
				pos++;
				return toRet;
				}
			unsigned last( )
				{
				return internal.back( );
				}
			unsigned getPos( )
				{
				return pos;
				}
		};
	
	class ranker
		{
		private:
			double scoreUrl( dex::Url url )
				{
				double score = 0;
				// Promote good tlds ( .com, .org, .gov )
				dex::string host = url.getHost( );
				if ( host.size( ) > 4 )
					{
					if ( host.substr( 0, 4 ) == "www." )
						{
						score += 8;
						}
					host = host.substr( host.size( ) - 4, 4);
					if ( host == ".com" )
						{
						score += 10;
						}
					else
						{
						if ( host == ".org" )
							{
							score += 10;
							}
						else
							{
							if ( host == ".gov" )
								{
								score += 10;
								}
							else
								{
								if ( host == ".edu" )
									{
									score += 10;
									}
								}
							}
						}
					}
				

				// Promote short URLs
				// max of this should not give more than a good TLD
				// completeUrls of size 45 or lower get maximum score ( of 9 ).
				// decay after that

				// TODO: Make this more binary
				dex::string completeUrl = url.completeUrl( );
				int urlSize = dex::max( size_t( 45 ), completeUrl.size( ) );
				score += 9 * 45 / urlSize;

				if ( url.getService( ) == "https" )
					{
					score += 8;
					}

				// Promote not a ton of /'s
				// take off points for every / you have over 6 ( take off 3 for the slashes in every url )
				int numSlashes = 0;
				for ( size_t i = 0;  i < completeUrl.size( );  ++i )
					{
					if ( completeUrl[ i ] == '/' )
						++numSlashes;
					}
				numSlashes -= 6;
				if ( numSlashes > 0 )
					{
					score -= numSlashes * 2;
					}
				// Promote no queries or fragments
				if ( !url.getFragment( ).empty( ) || !url.getQuery( ).empty( ) )
					score -= 5;

				return score;
				}

			/*
			Spans  (Contain all query words contained in this body)
				Range from 0 - 5 occurrences

			Exact Match

			In order span size of 2 times query length (fraction of query words)
			In order span size of 4 times query length (fraction of query words)
			In order span size of 5 times query length (fraction of query words)

			Any order span size of 2 times query length
			Any order span size of 4 times query length
			Any order span size of 5 times query length

			pass a vector in for our heuristics
				{ 1, 2, 4, 5 }


			first { 1, 3, 5, 7, 9 }
			second { 4, 10 }
			third { 6 }

			best in order: 3, 4, 6

			best out of order: 5, 4, 6



			Spanning strategy
			while ( rarestISR != end )
				{
				unsigned prev = currentISR;
				currentISR = currentISR.next( );
				if ( currentISR > rarestISR )
					choose the best out of current and prev
				

			n rarest words and m number of words in query
			you're returning a vector of size n, of vectors of size m of unsigneds

			{ 1, 2, 3, 4, 5, }
				rarestISR++
				}
			*/
		

		public:

			ranker( )
				{
				}
			// Heuristics is a vector contianing the lengths of the spans you're looking for in the document
			// Emphasized is a vector containing whether or not the word at that index was emphasized
			// Vector of ISRs should be arranged such that the words of the ISRs line up with the order of the query
			// rarest should be the index of the ISR of the rarest word in the query
			// { 1, 2, 4, 8 }
			vector < unsigned > getDesiredSpans( vector < double > heuristics, vector < bool > emphasized, vector < ISR > isrs, unsigned rarest )
				{
				unsigned size = isrs.size( );
				vector < unsigned > spansOccurances( heuristics.size( ) );

				vector < unsigned > current( size );
				vector < unsigned > next( size );
				for ( unsigned index = 0;  index < size;  ++index )
					{
					std::cout << isrs[ index ].getPos( ) << std::endl;
					current[ index ] = isrs[ index ].next( );
					std::cout << isrs[ index ].getPos( ) << std::endl;
					next[ index ] = isrs[ index ].next( );
					}

				dex::vector < unsigned > closestLocations( size );
				while ( current[ rarest ] != ISR::npos )
					{
					closestLocations[ rarest ] = current[ rarest ];
					for ( unsigned index = 0;  index < size;  ++index )
						{
						unsigned desiredPosition = current[ rarest ] + index - rarest;
						if ( index != rarest )
							{
							// Position our ISRs such that current is less than our desired position and next is greater than our
							// desired position.
							// next[ index ] - current[ rarest ] < index - rarest
							while ( next[ index ] != ISR::npos && next[ index ] + rarest < current[ rarest ] + index )
								{
								std::cout << "increment" << std::endl;
								current[ index ] = next[ index ];
								next[ index ] = isrs[ index ].next( );
								}
							// Take the value that is closest to our desired position for this span.
							unsigned closest;
							std::cout << "first: " << desiredPosition - current[ index ] << std::endl;
							std::cout << "second: " << next[ index ] - desiredPosition << std::endl;
							if ( desiredPosition - current[ index ] < next[ index ] - desiredPosition )
								{
								closest = current[ index ];
								std::cout << desiredPosition << std::endl;
								std::cout << closest << std::endl;
								}
							else
								{
								if ( desiredPosition - current[ index ] > next[ index ] - desiredPosition || index > rarest )
									{
									closest = next[ index ];
									}
								else
									{
									closest = current[ index ];
									}
								}
							closestLocations[ index ] = closest;
							}
						}
					unsigned max = ISR::npos;
					unsigned min = 0;
					std::cout << "new span for " << current[ rarest ] << std::endl;
					for ( unsigned index = 0;  index < size;  ++index )
						{
						std::cout << closestLocations[ index ] << std::endl;
						if ( closestLocations[ index ] > max )
							{
							max = closestLocations[ index ];
							}
						if ( closestLocations[ index ] < min )
							{
							min = closestLocations[ index ];
							}
						}
					unsigned span = max - min;
					for ( unsigned index = 0;  index < heuristics.size( );  ++index )
						{
						if ( span <= heuristics[ index ] * size )
							{
							spansOccurances[ index ]++;
							break;
							}
						}
					current[ rarest ] = next[ rarest ];
					next[ rarest ] = isrs[ rarest ].next( );
					}
				return spansOccurances;
				}

			dex::vector < searchResult > getTopN( size_t n, dex::string query )
				{
				dex::vector < searchResult > results;
				// pass query to query compiler
				// pass compiled query to constraintSolver
				
				// loop this over all index chunks
					// Constraint Solver returns a vector of sets of ISRs?
					// Each document contains a vector of ISRs that correspond to the words in the query
					// Each index worker has its own constraint solver.
				return results;
				}
			
		};
	}

