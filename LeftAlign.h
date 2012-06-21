#ifndef __LEFTALIGN_H
#define __LEFTALIGN_H

#include <algorithm>
#include <map>
#include <vector>
#include <utility>
#include <sstream>

#include "IndelAllele.h"
#include "convert.h"

#ifdef VERBOSE_DEBUG
#define LEFTALIGN_DEBUG(msg) \
    if (debug) { cerr << msg; }
#else
#define LEFTALIGN_DEBUG(msg)
#endif

using namespace std;

bool leftAlign(string& alternateQuery, string& cigar, string& referenceSequence, int& offset, bool debug = false);
bool stablyLeftAlign(string alternateQuery, string& cigar, string referenceSequence, int& offset, int maxiterations = 20, bool debug = false);
int countMismatches(string& alternateQuery, string& cigar, string& referenceSequence);

string mergeCigar(const string& c1, const string& c2);
vector<pair<int, string> > splitCigar(const string& cigarStr);
string joinCigar(const vector<pair<int, string> >& cigar);


#endif
