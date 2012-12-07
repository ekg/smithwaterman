cmake_minimum_required(VERSION 2.8)

add_library(smithwaterman
	smithwaterman.cpp
	SmithWatermanGotoh.cpp
	Repeats.cpp
	IndelAllele.cpp
	disorder.c
	LeftAlign.cpp
)

add_executable(smithwaterman-bin
	smithwaterman.cpp
	BandedSmithWaterman.cpp
)

target_link_libraries(smithwaterman-bin
	smithwaterman
)

set_target_properties(smithwaterman-bin
	PROPERTIES OUTPUT_NAME smithwaterman
)
