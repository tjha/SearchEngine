// Config Struct to be read by ranker.hpp so we can tune without recompiling
//
// 2019-12-12: fixed style, added comments: combsc
// 2019-12-11: create config: jhirsh

#ifndef RANKER_CONFIG_HPP
#define RANKER_CONFIG_HPP

#include "vector.hpp"
#include "utility.hpp"

struct RankerConfiguration
	{
	// pair of < maximum length of title, score awarded to title >
	static const dex::vector < dex::pair < unsigned, double > > StaticTitleWeights;
	// score weighting for URL
	constexpr static const double StaticUrlWeight = 1.5;
	// pair of < inclusive upper bound on body span range, score weighting of that range >
	static const dex::vector < dex::pair < unsigned, double > > BodySpanHeuristics;
	// pair of < inclusive upper bound on title span range, score weighting of that range >
	static const dex::vector < dex::pair < unsigned, double > > TitleSpanHeuristics;
	// double for the weighting applied to emphasized words
	constexpr static const double EmphasizedWeight = 3.0;
	// double for the maximum word occurance proportion measured for a document
	// this should be quite low.
	constexpr static const double ProportionCap = 0.06;
	// Maximum number of spans we measure for body ISRs
	constexpr static const unsigned MaxBodySpans = 5;
	// Maximum number of spans we measure for title ISRs
	constexpr static const unsigned MaxTitleSpans = 1;
	};

const dex::vector < dex::pair < unsigned, double > > RankerConfiguration::StaticTitleWeights = 
		{
		{ 15, 50 },
		{ 25, 40 },
		{ 50, 20 }
		};

const dex::vector < dex::pair < unsigned, double > > RankerConfiguration::BodySpanHeuristics =
		{
		{ 1, 50 },
		{ 2, 25 },
		{ 5, 10 }
		};

const dex::vector < dex::pair < unsigned, double > > RankerConfiguration::TitleSpanHeuristics =
		{
		{ 1, 250 },
		{ 2, 100 }
		};
#endif
