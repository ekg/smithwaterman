#include "LeftAlign.h"

//bool debug;
#define VERBOSE_DEBUG

// Attempts to left-realign all the indels represented by the alignment cigar.
//
// This is done by shifting all indels as far left as they can go without
// mismatch, then merging neighboring indels of the same class.  leftAlign
// updates the alignment cigar with changes, and returns true if realignment
// changed the alignment cigar.
//
// To left-align, we move multi-base indels left by their own length as long as
// the preceding bases match the inserted or deleted sequence.  After this
// step, we handle multi-base homopolymer indels by shifting them one base to
// the left until they mismatch the reference.
//
// To merge neighboring indels, we iterate through the set of left-stabilized
// indels.  For each indel we add a new cigar element to the new cigar.  If a
// deletion follows a deletion, or an insertion occurs at the same place as
// another insertion, we merge the events by extending the previous cigar
// element.
//
// In practice, we must call this function until the alignment is stabilized.
//
bool leftAlign(string& querySequence, string& cigar, string& referenceSequence, bool debug) {

    int arsOffset = 0; // pointer to insertion point in aligned reference sequence
    string alignedReferenceSequence, alignedQuerySequence;
    if (debug) alignedReferenceSequence = referenceSequence;
    if (debug) alignedQuerySequence = querySequence;
    int aabOffset = 0;

    // store information about the indels
    vector<IndelAllele> indels;

    int rp = 0;  // read position, 0-based relative to read
    int sp = 0;  // sequence position

    string softBegin;
    string softEnd;

    string cigarbefore = cigar;

    vector<pair<int, string> > cigarData = splitCigar(cigar);
    for (vector<pair<int, string> >::const_iterator c = cigarData.begin();
        c != cigarData.end(); ++c) {
        unsigned int l = c->first;
        string t = c->second;
        if (t == "M") { // match or mismatch
            sp += l;
            rp += l;
        } else if (t == "D") { // deletion
            indels.push_back(IndelAllele(false, l, sp, rp, referenceSequence.substr(sp, l)));
            if (debug) alignedQuerySequence.insert(rp + aabOffset, string(l, '-'));
            aabOffset += l;
            sp += l;  // update reference sequence position
        } else if (t == "I") { // insertion
            indels.push_back(IndelAllele(true, l, sp, rp, querySequence.substr(rp, l)));
            if (debug) alignedReferenceSequence.insert(sp + softBegin.size() + arsOffset, string(l, '-'));
            arsOffset += l;
            rp += l;
        } else if (t == "S") { // soft clip, clipped sequence present in the read not matching the reference
            // remove these bases from the refseq and read seq, but don't modify the alignment sequence
            if (rp == 0) {
                alignedReferenceSequence = string(l, '*') + alignedReferenceSequence;
                softBegin = querySequence.substr(0, l);
            } else {
                alignedReferenceSequence = alignedReferenceSequence + string(l, '*');
                softEnd = querySequence.substr(querySequence.size() - l, l);
            }
            rp += l;
        } else if (t == "H") { // hard clip on the read, clipped sequence is not present in the read
        } else if (t == "N") { // skipped region in the reference not present in read, aka splice
            sp += l;
        }
    }


    int alignedLength = sp;

    if (debug) cerr << "| " << cigarbefore << endl
		    << "| " << alignedReferenceSequence << endl
		    << "| " << alignedQuerySequence << endl;

    // if no indels, return the alignment
    if (indels.empty()) { return false; }

    // for each indel, from left to right
    //     while the indel sequence repeated to the left and we're not matched up with the left-previous indel
    //         move the indel left

    vector<IndelAllele>::iterator previous = indels.begin();
    for (vector<IndelAllele>::iterator id = indels.begin(); id != indels.end(); ++id) {

        // left shift by repeats
        //
        // from 1 base to the length of the indel, attempt to shift left
        // if the move would cause no change in alignment optimality (no
        // introduction of mismatches, and by definition no change in gap
        // length), move to the new position.
        // in practice this moves the indel left when we reach the size of
        // the repeat unit.
        //
        int steppos, readsteppos;
        IndelAllele& indel = *id;
        int i = 1;
        while (i <= indel.length) {

            int steppos = indel.position - i;
            int readsteppos = indel.readPosition - i;

#ifdef VERBOSE_DEBUG
            if (debug) {
                if (steppos >= 0 && readsteppos >= 0) {
                    cerr << referenceSequence.substr(steppos, indel.length) << endl;
                    cerr << querySequence.substr(readsteppos, indel.length) << endl;
                    cerr << indel.sequence << endl;
                }
            }
#endif
            while (steppos >= 0 && readsteppos >= 0
                   && indel.sequence == referenceSequence.substr(steppos, indel.length)
                   && indel.sequence == querySequence.substr(readsteppos, indel.length)
                   && (id == indels.begin()
                       || (previous->insertion && steppos >= previous->position)
                       || (!previous->insertion && steppos >= previous->position + previous->length))) {
                LEFTALIGN_DEBUG((indel.insertion ? "insertion " : "deletion ") << indel << " shifting " << i << "bp left" << endl);
                indel.position -= i;
                indel.readPosition -= i;
                steppos = indel.position - i;
                readsteppos = indel.readPosition - i;
            }
            do {
                ++i;
            } while (i <= indel.length && indel.length % i != 0);
        }

        // left shift indels with exchangeable flanking sequence
        //
        // for example:
        //
        //    GTTACGTT           GTTACGTT
        //    GT-----T   ---->   G-----TT
        //
        // GTGTGACGTGT           GTGTGACGTGT
        // GTGTG-----T   ---->   GTG-----TGT
        //
        // GTGTG-----T           GTG-----TGT
        // GTGTGACGTGT   ---->   GTGTGACGTGT
        //
        //
        steppos = indel.position - 1;
        readsteppos = indel.readPosition - 1;
        while (steppos >= 0 && readsteppos >= 0
               //`&& querySequence.at(readsteppos) == referenceSequence.at(steppos)
	       && referenceSequence.size() > steppos + indel.length
	       && indel.sequence.at((int) indel.sequence.size() - 1) == referenceSequence.at(steppos + indel.length) // are the exchanged bases going to match wrt. the reference?
               && querySequence.at(readsteppos) == indel.sequence.at((int) indel.sequence.size() - 1)
               && (id == indels.begin()
                   || (previous->insertion && indel.position - 1 >= previous->position)
                   || (!previous->insertion && indel.position - 1 >= previous->position + previous->length))) {
            LEFTALIGN_DEBUG((indel.insertion ? "insertion " : "deletion ") << indel << " exchanging bases " << 1 << "bp left" << endl);
            indel.sequence = indel.sequence.at(indel.sequence.size() - 1) + indel.sequence.substr(0, indel.sequence.size() - 1);
            indel.position -= 1;
            indel.readPosition -= 1;
            steppos = indel.position - 1;
            readsteppos = indel.readPosition - 1;
        }
        // tracks previous indel, so we don't run into it with the next shift
        previous = id;
    }

    // bring together floating indels
    // from left to right
    // check if we could merge with the next indel
    // if so, adjust so that we will merge in the next step
    if (indels.size() > 1) {
        previous = indels.begin();
        for (vector<IndelAllele>::iterator id = (indels.begin() + 1); id != indels.end(); ++id) {
            IndelAllele& indel = *id;
            // parsimony: could we shift right and merge with the previous indel?
            // if so, do it
            int prev_end_ref = previous->insertion ? previous->position : previous->position + previous->length;
            int prev_end_read = !previous->insertion ? previous->readPosition : previous->readPosition + previous->length;
            if (previous->insertion == indel.insertion
                    && ((previous->insertion
                        && (previous->position < indel.position
                        && previous->readPosition + previous->readPosition < indel.readPosition))
                        ||
                        (!previous->insertion
                        && (previous->position + previous->length < indel.position)
                        && (previous->readPosition < indel.readPosition)
                        ))) {
                if (previous->homopolymer()) {
                    string seq = referenceSequence.substr(prev_end_ref, indel.position - prev_end_ref);
                    string readseq = querySequence.substr(prev_end_read, indel.position - prev_end_ref);
                    LEFTALIGN_DEBUG("seq: " << seq << endl << "readseq: " << readseq << endl);
                    if (previous->sequence.at(0) == seq.at(0)
                            && homopolymer(seq)
                            && homopolymer(readseq)) {
                        LEFTALIGN_DEBUG("moving " << *previous << " right to " 
                                << (indel.insertion ? indel.position : indel.position - previous->length) << endl);
                        previous->position = indel.insertion ? indel.position : indel.position - previous->length;
                    }
                } 
                else {
                    int pos = previous->position;
		    int readpos = previous->readPosition;
                    while (pos < (int) referenceSequence.length() &&
                            ((previous->insertion && pos + previous->length <= indel.position)
                            ||
                            (!previous->insertion && pos + previous->length < indel.position))
			   && previous->sequence == referenceSequence.substr(pos + previous->length, previous->length)
			   && previous->sequence == querySequence.substr(readpos + previous->length, previous->length)
			) {
                        pos += previous->length;
			readpos += previous->length;
                    }
		    string seq = previous->sequence;
                    if (pos > previous->position) {
			// wobble bases right to left as far as we can go
			int steppos = previous->position + seq.size();
			int readsteppos = previous->readPosition + seq.size();

			while (querySequence.at(readsteppos) == referenceSequence.at(steppos)
			       && querySequence.at(readsteppos) == seq.at(0)
			       && (id == indels.begin()
				   || (indel.insertion && pos + seq.size() - 1 <= indel.position)
				   || (!previous->insertion && indel.position - 1 >= pos + previous->length))) {
			    seq = seq.substr(1) + seq.at(0);
			    ++pos;
			    ++readpos;
			    steppos = pos + 1;
			    readsteppos = readpos + 1;
			}

			if (((previous->insertion && pos + previous->length == indel.position)
			     ||
			     (!previous->insertion && pos == indel.position - previous->length))
			    ) {
			    LEFTALIGN_DEBUG("right-merging tandem repeat: moving " << *previous << " right to " << pos << endl);
			    previous->position = pos;
			}
                    }
                }
            }
            previous = id;
        }
    }

    // for each indel
    //     if ( we're matched up to the previous insertion (or deletion) 
    //          and it's also an insertion or deletion )
    //         merge the indels
    //
    // and simultaneously reconstruct the cigar

    vector<pair<int, string> > newCigar;

    if (!softBegin.empty()) {
        newCigar.push_back(make_pair(softBegin.size(), "S"));
    }

    vector<IndelAllele>::iterator id = indels.begin();
    IndelAllele last = *id++;
    if (last.position > 0) {
        newCigar.push_back(make_pair(last.position, "M"));
        newCigar.push_back(make_pair(last.length, (last.insertion ? "I" : "D")));
    } else {
	newCigar.push_back(make_pair(last.length, (last.insertion ? "I" : "D")));
    }
    int lastend = last.insertion ? last.position : (last.position + last.length);
    LEFTALIGN_DEBUG(last << ",");

    for (; id != indels.end(); ++id) {
        IndelAllele& indel = *id;
        LEFTALIGN_DEBUG(indel << ",");
        if (indel.position < lastend) {
            cerr << "impossibility?: indel realigned left of another indel" << endl;
            exit(1);
        } else if (indel.position == lastend && indel.insertion == last.insertion) {
            pair<int, string>& op = newCigar.back();
            op.first += indel.length;
        } else if (indel.position >= lastend) {  // also catches differential indels, but with the same position
            newCigar.push_back(make_pair(indel.position - lastend, "M"));
            newCigar.push_back(make_pair(indel.length, (indel.insertion ? "I" : "D")));
        }
        last = *id;
        lastend = last.insertion ? last.position : (last.position + last.length);
    }
    
    if (lastend < alignedLength) {
        newCigar.push_back(make_pair(alignedLength - lastend, "M"));
    }

    if (!softEnd.empty()) {
        newCigar.push_back(make_pair(softEnd.size(), "S"));
    }

    LEFTALIGN_DEBUG(endl);

    cigar = joinCigar(newCigar);

    LEFTALIGN_DEBUG(cigar << endl);

    // check if we're realigned
    if (cigar == cigarbefore) {
        return false;
    } else {
        return true;
    }

}

int countMismatches(string& querySequence, string& cigar, string referenceSequence) {

    int mismatches = 0;
    int sp = 0;
    int rp = 0;
    vector<pair<int, string> > cigarData = splitCigar(cigar);
    for (vector<pair<int, string> >::const_iterator c = cigarData.begin();
        c != cigarData.end(); ++c) {
        unsigned int l = c->first;
        string t = c->second;
        if (t == "M") { // match or mismatch
            for (int i = 0; i < l; ++i) {
                if (querySequence.at(rp) != referenceSequence.at(sp))
                    ++mismatches;
                ++sp;
                ++rp;
            }
        } else if (t == "D") { // deletion
            sp += l;  // update reference sequence position
        } else if (t == "I") { // insertion
            rp += l;  // update read position
        } else if (t == "S") { // soft clip, clipped sequence present in the read not matching the reference
            rp += l;
        } else if (t == "H") { // hard clip on the read, clipped sequence is not present in the read
        } else if (t == "N") { // skipped region in the reference not present in read, aka splice
            sp += l;
        }
    }

    return mismatches;

}

// Iteratively left-aligns the indels in the alignment until we have a stable
// realignment.  Returns true on realignment success or non-realignment.
// Returns false if we exceed the maximum number of realignment iterations.
//
bool stablyLeftAlign(string querySequence, string& cigar, string referenceSequence, int maxiterations, bool debug) {

    if (!leftAlign(querySequence, cigar, referenceSequence, debug)) {

        LEFTALIGN_DEBUG("did not realign" << endl);
        return true;

    } else {

        while (leftAlign(querySequence, cigar, referenceSequence, debug) && --maxiterations > 0) {
            LEFTALIGN_DEBUG("realigning ..." << endl);
        }

        if (maxiterations <= 0) {
            return false;
        } else {
            return true;
        }
    }
}

string mergeCigar(const string& c1, const string& c2) {
    vector<pair<int, string> > cigar1 = splitCigar(c1);
    vector<pair<int, string> > cigar2 = splitCigar(c2);
    // check if the middle elements are the same
    if (cigar1.back().second == cigar2.front().second) {
        cigar1.back().first += cigar2.front().first;
        cigar2.erase(cigar2.begin());
    }
    for (vector<pair<int, string> >::iterator c = cigar2.begin(); c != cigar2.end(); ++c) {
        cigar1.push_back(*c);
    }
    return joinCigar(cigar1);
}

vector<pair<int, string> > splitCigar(const string& cigarStr) {
    vector<pair<int, string> > cigar;
    string number;
    string type;
    // strings go [Number][Type] ...
    for (string::const_iterator s = cigarStr.begin(); s != cigarStr.end(); ++s) {
        char c = *s;
        if (isdigit(c)) {
            if (type.empty()) {
                number += c;
            } else {
                // signal for next token, push back the last pair, clean up
                cigar.push_back(make_pair(atoi(number.c_str()), type));
                number.clear();
                type.clear();
                number += c;
            }
        } else {
            type += c;
        }
    }
    if (!number.empty() && !type.empty()) {
        cigar.push_back(make_pair(atoi(number.c_str()), type));
    }
    return cigar;
}

string joinCigar(const vector<pair<int, string> >& cigar) {
    string cigarStr;
    for (vector<pair<int, string> >::const_iterator c = cigar.begin(); c != cigar.end(); ++c) {
        if (c->first) {
            cigarStr += convert(c->first) + c->second;
        }
    }
    return cigarStr;
}
