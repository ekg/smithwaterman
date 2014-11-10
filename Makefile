# =========================================
# MOSAIK Banded Smith-Waterman Makefile
# (c) 2009 Michael Stromberg & Wan-Ping Lee
# =========================================

# ----------------------------------
# define our source and object files
# ----------------------------------
SOURCES= smithwaterman.cpp BandedSmithWaterman.cpp SmithWatermanGotoh.cpp Repeats.cpp LeftAlign.cpp IndelAllele.cpp
OBJECTS= $(SOURCES:.cpp=.o) disorder.o

# ----------------
# compiler options
# ----------------

CFLAGS=-Wall -O3
LDFLAGS=-Wl,-s
#CFLAGS=-g
PROGRAM=smithwaterman
LIBS=

all: $(PROGRAM)

.PHONY: all

disorder.o: disorder.c disorder.h
	g++ -c -o disorder.o disorder.c

$(PROGRAM): $(OBJECTS)
	@echo "  * linking $(PROGRAM)"
	@$(CXX) $(LDFLAGS) $(CFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	@echo "Cleaning up."
	@rm -f *.o $(PROGRAM) *~
