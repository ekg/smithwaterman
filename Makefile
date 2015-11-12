# =========================================
# MOSAIK Banded Smith-Waterman Makefile
# (c) 2009 Michael Stromberg & Wan-Ping Lee
# =========================================

# ----------------------------------
# define our source and object files
# ----------------------------------
SOURCES= smithwaterman.cpp BandedSmithWaterman.cpp SmithWatermanGotoh.cpp Repeats.cpp LeftAlign.cpp IndelAllele.cpp
OBJECTS= $(SOURCES:.cpp=.o) disorder.o
OBJECTS_NO_MAIN= disorder.o BandedSmithWaterman.o SmithWatermanGotoh.o Repeats.o LeftAlign.o IndelAllele.o

# ----------------
# compiler options
# ----------------

CFLAGS:= -O3
LDFLAGS:=-Wl,-s
#CFLAGS=-g
EXE:=smithwaterman
LIBS=

all: $(EXE) sw.o

.PHONY: all

libsw.a: smithwaterman.o BandedSmithWaterman.o SmithWatermanGotoh.o LeftAlign.o Repeats.o IndelAllele.o disorder.o
	ar rs $@ smithwaterman.o d SmithWatermanGotoh.o disorder.o BandedSmithWaterman.o LeftAlign.o Repeats.o IndelAllele.o

sw.o:  BandedSmithWaterman.o SmithWatermanGotoh.o LeftAlign.o Repeats.o IndelAllele.o disorder.o
	ld -r $^ -o sw.o -L.
	#$(CXX) $(CFLAGS) -c -o smithwaterman.cpp $(OBJECTS_NO_MAIN) -I.

### @$(CXX) $(LDFLAGS) $(CFLAGS) -o $@ $^ -I.
$(EXE): smithwaterman.o BandedSmithWaterman.o SmithWatermanGotoh.o disorder.o LeftAlign.o Repeats.o IndelAllele.o
	$(CXX) $(CFLAGS) $^ -I. -o $@

#smithwaterman: $(OBJECTS)
#	$(CXX) $(CFLAGS) -o $@ $< -I.

smithwaterman.o: smithwaterman.cpp disorder.o
	$(CXX) $(CFLAGS) -c -o $@ smithwaterman.cpp -I.

disorder.o: disorder.cpp disorder.h
	$(CXX) $(CFLAGS) -c -o $@ $< -I.
BandedSmithWaterman.o: BandedSmithWaterman.cpp BandedSmithWaterman.h
	$(CXX) $(CFLAGS) -c -o $@ $< -I.
SmithWatermanGotoh.o: SmithWatermanGotoh.cpp SmithWatermanGotoh.h disorder.o
	$(CXX) $(CFLAGS) -c -o $@ $< -I.
Repeats.o: Repeats.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< -I.
LeftAlign.o: LeftAlign.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< -I.
IndelAllele.o: IndelAllele.cpp
	$(CXX) $(CFLAGS) -c -o $@ $< -I.

.PHONY: clean

clean:
	@echo "Cleaning up."
	@rm -f *.o $(PROGRAM) *~
