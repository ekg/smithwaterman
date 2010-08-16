#pragma once

#include "MosaikString.h"

using namespace std;

// all members identified as (temp) are not serialized to disk

// our alignment structure
struct Alignment {

	double RawScore;                 // unnormalized alignment quality (temp)
	unsigned int ReferenceBegin;
	unsigned int ReferenceEnd;
	unsigned int Owner;              // the temporary file that contains the alignment
	unsigned short QueryLength;      // used during filtering (temp)
	unsigned short NumMismatches;    // number of mismatches (temp)
	unsigned short ReferenceIndex;
	unsigned short QueryBegin;
	unsigned short QueryEnd;
	unsigned char Quality;           // alignment quality
	bool IsReverseComplement;	     // read orientation
	char* ReferenceName;             // only filled via CAlignmentReader (temp)
	CMosaikString Reference;
	CMosaikString Query;
	CMosaikString BaseQualities;
	CMosaikString Name;              // the read name

	// constructors
	Alignment(void)
		: RawScore(0.0)
		, ReferenceIndex(0)
		, Quality(0)
		, IsReverseComplement(false)
		, ReferenceName(NULL)
	{}

	// our less-than operator
	bool operator<(const Alignment& al) const {
		if(ReferenceIndex == al.ReferenceIndex) return ReferenceBegin < al.ReferenceBegin;
		return ReferenceIndex < al.ReferenceIndex;
	}
};
