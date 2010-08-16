#!/bin/sh

outDir=.
binDir=.
bin=BandedSmithWaterman

output=result.txt

begin=4001
queryBegin=0
bestMatchLength=19

while [ $begin -lt 4134 ]
do
	queryBegin=0
	while [ $queryBegin -lt 74 ]
	do
		#echo "$begin $(($begin+$bestMatchLength)) $queryBegin $(($queryBegin+$bestMatchLength)) >> $output"
		$binDir/$bin $begin $(($begin+$bestMatchLength)) $queryBegin $(($queryBegin+$bestMatchLength))
		queryBegin=$(($queryBegin+1))
	done

	begin=$(($begin+1))
done
