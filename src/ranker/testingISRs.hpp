#ifndef DEX_TESTING_ISRS
#define DEX_TESTING_ISRS

#include <cstddef>
#include "constraintSolver/constraintSolver.hpp"
#include "utils/vector.hpp"

namespace dex
	{
	namespace rankerTesting
		{
		class endOfDocumentISR : public constraintSolver::endOfDocumentISR
			{
			private:
				dex::vector< size_t > internal;
				size_t pos;
				size_t toGet;
			public:
				endOfDocumentISR( ) : toGet( npos ) { }
				endOfDocumentISR( dex::vector< size_t > vecIn ) : internal( vecIn ), pos( 0 ), toGet( 0 ) { }
				endOfDocumentISR &operator =( const endOfDocumentISR other )
					{
					internal = other.internal;
					pos = other.pos;
					toGet = other.toGet;
					return *this;
					}
				const static size_t npos = size_t ( -1 );
				size_t seek( size_t target )
					{
					for ( size_t index = 0;  index < internal.size( );  ++index )
						{
						if ( internal[ index ] >= target )
							{
							pos = index;
							return toGet = internal[ pos ];
							}
						}
					pos = internal.size( );
					return toGet = npos;
					}
				size_t next( )
					{
					if ( pos == internal.size( ) )
						{
						return toGet = npos;
						}
					size_t toReturn = internal[ pos ];
					pos++;

					return toGet = toReturn;
					}
				size_t nextDocument( )
					{
					return toGet = next( );
					}
				size_t get( ) const
					{
					return toGet;
					}
				size_t documentSize( ) const
					{
					if ( pos == 0 )
						{
						return internal[ pos ];
						}
					return internal[ pos ] - internal[ pos - 1 ];
					}
			};

		class ISR : public constraintSolver::ISR
			{
			private:
				dex::vector< size_t > internal;
				dex::string word;
				size_t pos;
				dex::rankerTesting::endOfDocumentISR ends;
				size_t toGet;
			public:

				const static size_t npos = size_t ( -1 );
				ISR( ) : toGet ( npos ) { }
				ISR( dex::string word, dex::vector< size_t > vecIn, dex::rankerTesting::endOfDocumentISR endsIn ) :
					internal( vecIn ), word( word ), pos( 0 ), ends( endsIn ), toGet( 0 ) { }

				void reset( )
					{
					pos = 0;
					toGet = npos;
					}
				// Jump this ISR to the first instance of our pattern that is at or after target. Return the location of the
				// instance, or -1 if there is none.
				size_t seek( size_t target )
					{
					for ( size_t index = 0;  index < internal.size( );  ++index )
						{
						if ( internal[ index ] >= target )
							{
							pos = index;
							return toGet = next( );
							}
						}
					pos = internal.size( );
					return toGet = npos;
					}

				// Jump ISR to next location of our pattern. Return location of instance or -1 if there is none.
				size_t next( )
					{
					if ( pos == internal.size( ) )
						{
						return toGet = npos;
						}
					toGet = internal[ pos ];
					pos++;

					return toGet;
					}

				// Jump this ISR to the first location of our pattern that is in the next document. Return the location of the
				// instance, or -1 if there is none.
				// Note that this requires some way of accessing document boundaries.
				size_t nextDocument( )
					{
					if ( pos == internal.size( ) )
						{
						std::cout << "CALLED NEXT DOCUMENT, POS AT END OF INTERNAL" << std::endl;
						return toGet = npos;
						}
					ends.seek( internal[ pos ] );
					return toGet = seek( ends.next( ) );
					}
				size_t get( ) const 
					{
					return toGet;
					}
				dex::string getWord( )
					{
					return word;
					}
			};
		}
	}

#endif
