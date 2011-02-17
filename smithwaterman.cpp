#include <iostream>
#include <string.h>
#include <getopt.h>
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
         << endl
         << "When called with literal reference and query sequences, smithwaterman" << endl
         << "prints the cigar match positional string and the match position for the" << endl
         << "query sequence against the reference sequence." << endl;
}


int main (int argc, char** argv) {

    int c;

    char* reference;
    char* query;

    int bandwidth = 0;

    float matchScore = 10.0f;
    float mismatchScore = -9.0f;
    float gapOpenPenalty = 15.0f;
    float gapExtendPenalty = 6.66f;
    // CSmithWatermanGotoh::CSmithWatermanGotoh(float matchScore, float mismatchScore, float gapOpenPenalty, float gapExtendPenalty)

    while (true) {
        static struct option long_options[] =
        {
            /* These options set a flag. */
            //{"verbose", no_argument,       &verbose_flag, 1},
            //{"brief",   no_argument,       &verbose_flag, 0},
            {"help", no_argument, 0, 'h'},
            {"match-score",  required_argument, 0, 'm'},
            {"mismatch-score",  required_argument, 0, 'n'},
            {"gap-open-penalty",  required_argument, 0, 'g'},
            {"gap-extend-penalty",  required_argument, 0, 'e'},
            //{"length",  no_argument, &printLength, true},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "hm:n:g:r:",
                         long_options, &option_index);

      /* Detect the end of the options. */
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
        reference = argv[optind];
        ++optind;
        query = argv[optind];
    } else {
        cerr << "please specify a reference and query sequence" << endl
             << "execute " << argv[0] << " --help for command-line usage" << endl;
        exit(1);
    }

	// initialize
	
	const unsigned int referenceLen = strlen(reference);
	const unsigned int queryLen     = strlen(query);

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
        bsw.Align(referencePos, cigar, reference, referenceLen, query, queryLen, hr);
    } else {
        CSmithWatermanGotoh sw(matchScore, mismatchScore, gapOpenPenalty, gapExtendPenalty);
        sw.Align(referencePos, cigar, reference, referenceLen, query, queryLen);
    }

    printf("%s %3u\n", cigar.c_str(), referencePos);

	return 0;

}
