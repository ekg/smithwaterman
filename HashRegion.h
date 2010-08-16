#pragma once

struct HashRegion {
	unsigned int Begin;
	unsigned int End;
	unsigned short QueryBegin;
	unsigned short QueryEnd;
	unsigned short NumMismatches;

	HashRegion()
		: Begin(0)
		, End(0)
		, QueryBegin(0)
		, QueryEnd(0)
		, NumMismatches(0)
	{}

	bool operator<(const HashRegion& r) const {
		if(Begin      != r.Begin)      return Begin      < r.Begin;
		if(End        != r.End)        return End        < r.End;
		if(QueryBegin != r.QueryBegin) return QueryBegin < r.QueryBegin;		
		if(QueryEnd   != r.QueryEnd)   return QueryEnd   < r.QueryEnd;
		return false;
	}
};
