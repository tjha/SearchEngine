// Config Struct to be read by ranker.hpp so we can tune without recompiling
//
// 2019-12-11: create config: jhirsh

#ifndef RANKER_CONFIG_HPP
#define RANKER_CONFIG_HPP

#include "vector.hpp"
#include "utility.hpp"

struct RankerConfiguration
	{
	dex::vector < dex::pair < unsigned, double > > TITLE_WEIGHTS = 
		{
		dex::pair( 1, 1.2 ),
		dex::pair( 2, 0.8 )
		};

	double URL_WEIGHT = 1.5;

	dex::vector < dex::pair < unsigned, double > > BODY_SPAN_HEURISTICS = 
		{
		dex::pair( 1, 1.2 ),
		dex::pair( 2, 0.8 )
		};

	dex::vector < dex::pair < unsigned, double > > TITLE_SPAN_HEURISTICS = 
		{
		dex::pair( 1, 1.2 ),
		dex::pair( 2, 0.8 )
		};
	
	double EMPHASIZED_WEIGHT = 2.0;
	double PROPORTION_CAP = 1.8;
	unsigned BODY_SPANS = 3;
	unsigned TITLE_SPANS = 4;
	};

#endif