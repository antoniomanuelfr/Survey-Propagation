#!/bin/bash
N=$1
if [ "$2" == rm ]; then
  cp ../cnf ./cnf_save -r
  rm -rf ../cnf/testCNF/*
fi
for i in {"4.21","4.22","4.23","4.24"}; do
	mkdir "${i}"
	for j in {1..50}; do
		nvars=$(echo "$i * $N" | bc)
		echo "N=${N}, nvars = ${nvars}, i = ${i}, j = ${j}"
		../../satgenerators/random  -m $N -n $nvars -o "${i}/cnf_${N}_${nvars}_${j}.cnf"
	done
done
