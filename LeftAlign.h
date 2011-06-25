#ifndef __LEFTALIGN_H
#define __LEFTALIGN_H

#include <iostream>
#include <getopt.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <signal.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>
#include <ctype.h>

#include "IndelAllele.h"
#include "convert.h"

#ifdef VERBOSE_DEBUG
#define LEFTALIGN_DEBUG(msg) \
    if (debug) { cerr << msg; }
#else
#define LEFTALIGN_DEBUG(msg)
#endif

using namespace std;

class CigarElement {
public:
    char Type;
    unsigned int Length;
    CigarElement(void) { }
    CigarElement(char t, unsigned int l) : Type(t), Length(l) { }
};

void parseCigar(const string& cigar, vector<CigarElement>& cigarData);

bool leftAlign(unsigned int referencePos, vector<CigarElement>& cigarData, const string& ref, const string& query, bool debug = false);
//bool stablyLeftAlign(BamAlignment& alignment, string referenceSequence, int maxiterations = 20, bool debug = false);
bool stablyLeftAlign(unsigned int referencePos, string& cigar, const string& ref, const string& query, int maxiterations = 20, bool debug = false);
int countMismatches(vector<CigarElement>& cigarData, const string& ref, const string& query);

#endif
