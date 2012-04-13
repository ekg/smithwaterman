#include "Repeats.h"

map<string, int> repeatCounts(long int position, const string& sequence, int maxsize) {
    map<string, int> counts;
    for (int i = 1; i <= maxsize; ++i) {
        // subseq here i bases
        string seq = sequence.substr(position, i);
        // go left.
        /*
        int j = position - i;
        int left = 0;
        while (j - i >= 0 && seq == sequence.substr(j, i)) {
            j -= i;
            ++left;
        }
        */
        // go right.
        int j = position;
        int steps = 0;
        while (j + i < sequence.size() && seq == sequence.substr(j, i)) {
            j += i;
            ++steps;
        }
        // if we went left and right a non-zero number of times, 
        if (steps > 1) {
            counts[seq] = steps;
        }
    }

    // filter out redundant repeat information
    if (counts.size() > 1) {
        map<string, int> filteredcounts;
        map<string, int>::iterator c = counts.begin();
        string prev = c->first;
        filteredcounts[prev] = c->second;  // shortest sequence
        ++c;
        for (; c != counts.end(); ++c) {
            int i = 0;
            string seq = c->first;
            while (i + prev.length() <= seq.length() && seq.substr(i, prev.length()) == prev) {
                i += prev.length();
            }
            if (i < seq.length()) {
                filteredcounts[seq] = c->second;
                prev = seq;
            }
        }
        return filteredcounts;
    } else {
        return counts;
    }
}
