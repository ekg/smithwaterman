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

# Use ?= to allow overriding from the env or command-line
CXX?=		c++
CXXFLAGS?=	-O3
#CXXFLAGS+=	-g

# I don't think := is useful here, since there is nothing to expand
LDFLAGS:=	-Wl,-s
EXE:=		smithwaterman
LIBS=

all: $(EXE) libsw.a

.PHONY: all

libsw.a: $(OBJECTS_NO_MAIN)
	ar rs $@ $(OBJECTS_NO_MAIN)

sw.o:  $(OBJECTS_NO_MAIN)
	ld -r $^ -o sw.o -L.
	@#$(CXX) $(CFLAGS) -c -o smithwaterman.cpp $(OBJECTS_NO_MAIN) -I.

### @$(CXX) $(LDFLAGS) $(CFLAGS) -o $@ $^ -I.
$(EXE): $(OBJECTS)
	$(CXX) $(CFLAGS) $^ -I. -o $@

#smithwaterman: $(OBJECTS)
#	$(CXX) $(CXXFLAGS) -o $@ $< -I.

smithwaterman.o: smithwaterman.cpp disorder.o
	$(CXX) $(CXXFLAGS) -c -o $@ smithwaterman.cpp -I.

disorder.o: disorder.cpp disorder.h
	$(CXX) $(CXXFLAGS) -c -o $@ $< -I.

BandedSmithWaterman.o: BandedSmithWaterman.cpp BandedSmithWaterman.h
	$(CXX) $(CXXFLAGS) -c -o $@ $< -I.

SmithWatermanGotoh.o: SmithWatermanGotoh.cpp SmithWatermanGotoh.h disorder.o
	$(CXX) $(CXXFLAGS) -c -o $@ $< -I.

Repeats.o: Repeats.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< -I.

LeftAlign.o: LeftAlign.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< -I.

IndelAllele.o: IndelAllele.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $< -I.

.PHONY: clean

clean:
	@echo "Cleaning up."
	@rm -f *.o $(PROGRAM) *~ *.a
