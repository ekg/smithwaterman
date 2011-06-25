#include <iostream>
#include <string.h>
#include <string>
#include <sstream>
#include <getopt.h>
#include <utility>
#include <vector>
#include <stdlib.h>
#include "SmithWatermanGotoh.h"
#include "BandedSmithWaterman.h"

using namespace std;

void printSummary(void) {
    cerr << "usage: smithwaterman [options] <reference sequence> <query sequence>" << endl
         << endl
         << "options:" << endl 
         << "    -m, --match-score         the match score (default 10.0)" << endl
         << "    -n, --mismatch-score      the mismatch score (default -9.0)" << endl
         << "    -g, --gap-open-penalty    the gap open penalty (default 15.0)" << endl
         << "    -e, --gap-extend-penalty  the gap extend penalty (default 6.66)" << endl
         << "    -b, --bandwidth           bandwidth to use (default 0, or non-banded algorithm)" << endl
         << "    -p, --print-alignment     print out the alignment" << endl
         << endl
         << "When called with literal reference and query sequences, smithwaterman" << endl
         << "prints the cigar match positional string and the match position for the" << endl
         << "query sequence against the reference sequence." << endl;
}


int main (int argc, char** argv) {

    int c;

    string reference;
    string query;

    int bandwidth = 0;

    float matchScore = 10.0f;
    float mismatchScore = -9.0f;
    float gapOpenPenalty = 15.0f;
    float gapExtendPenalty = 6.66f;
    
    bool print_alignment = false;

    while (true) {
        static struct option long_options[] =
        {
            {"help", no_argument, 0, 'h'},
            {"match-score",  required_argument, 0, 'm'},
            {"mismatch-score",  required_argument, 0, 'n'},
            {"gap-open-penalty",  required_argument, 0, 'g'},
            {"gap-extend-penalty",  required_argument, 0, 'e'},
            {"print-alignment",  required_argument, 0, 'p'},
            {"bandwidth", required_argument, 0, 'b'},
            {0, 0, 0, 0}
        };
        int option_index = 0;

        c = getopt_long (argc, argv, "hpm:n:g:r:e:b:",
                         long_options, &option_index);

          if (c == -1)
            break;
 
          switch (c)
            {
            case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
              break;
            printf ("option %s", long_options[option_index].name);
            if (optarg)
              printf (" with arg %s", optarg);
            printf ("\n");
            break;

          case 'm':
            matchScore = atof(optarg);
            break;
 
          case 'n':
            mismatchScore = atof(optarg);
            break;
 
          case 'g':
            gapOpenPenalty = atof(optarg);
            break;
 
          case 'e':
            gapExtendPenalty = atof(optarg);
            break;

          case 'b':
            bandwidth = atoi(optarg);
            break;

          case 'p':
            print_alignment = true;
            break;
 
          case 'h':
            printSummary();
            exit(0);
            break;
 
          case '?':
            /* getopt_long already printed an error message. */
            printSummary();
            exit(1);
            break;
 
          default:
            abort ();
          }
      }

    /* Print any remaining command line arguments (not options). */
    if (optind == argc - 2) {
        //cerr << "fasta file: " << argv[optind] << endl;
        reference = string(argv[optind]);
        ++optind;
        query = string(argv[optind]);
    } else {
        cerr << "please specify a reference and query sequence" << endl
             << "execute " << argv[0] << " --help for command-line usage" << endl;
        exit(1);
    }

	// initialize
	
	unsigned int referencePos;
	string cigar;

	// create a new Smith-Waterman alignment object
    if (bandwidth > 0) {
        pair< pair<unsigned int, unsigned int>, pair<unsigned int, unsigned int> > hr;
        hr.first.first   = 2;
        hr.first.second  = 18;
        hr.second.first  = 1;
        hr.second.second = 17;
        CBandedSmithWaterman bsw(matchScore, mismatchScore, gapOpenPenalty, gapExtendPenalty, bandwidth);
        bsw.Align(referencePos, cigar, reference, query, hr);
    } else {
        CSmithWatermanGotoh sw(matchScore, mismatchScore, gapOpenPenalty, gapExtendPenalty);
        sw.Align(referencePos, cigar, reference, query);
    }

    printf("%s %3u\n", cigar.c_str(), referencePos);

    // optionally print out the alignment
    if (print_alignment) {
        int alignmentLength = 0;
        int len;
        string slen;
        vector<pair<int, char> > cigarData;
        for (string::iterator c = cigar.begin(); c != cigar.end(); ++c) {
            switch (*c) {
                case 'I':
                    len = atoi(slen.c_str());
                    slen.clear();
                    cigarData.push_back(make_pair(len, *c));
                    break;
                case 'D':
                    len = atoi(slen.c_str());
                    alignmentLength += len;
                    slen.clear();
                    cigarData.push_back(make_pair(len, *c));
                    break;
                case 'M':
                    len = atoi(slen.c_str());
                    alignmentLength += len;
                    slen.clear();
                    cigarData.push_back(make_pair(len, *c));
                    break;
                case 'S':
                    len = atoi(slen.c_str());
                    alignmentLength += len;
                    slen.clear();
                    cigarData.push_back(make_pair(len, *c));
                    break;
                default:
                    len = 0;
                    slen += *c;
                    break;
            }
        }

        string gapped_ref = string(reference).substr(referencePos, alignmentLength);
        string gapped_query = string(query);

        int refpos = 0;
        int readpos = 0;
        for (vector<pair<int, char> >::iterator c = cigarData.begin(); c != cigarData.end(); ++c) {
            int len = c->first;
            switch (c->second) {
                case 'I':
                    gapped_ref.insert(refpos, string(len, '-'));
                    readpos += len;
                    break;
                case 'D':
                    gapped_query.insert(readpos, string(len, '-'));
                    refpos += len;
                    break;
                case 'M':
                case 'S':
                    readpos += len;
                    refpos += len;
                    break;
                default:
                    break;
            }
        }

        cout << gapped_ref << endl << gapped_query << endl;
    }

	return 0;

}
