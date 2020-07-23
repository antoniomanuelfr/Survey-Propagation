#!/bin/bash
N=$1
cnfdir="../cnf/testCNF"

rm -rf "${cnfdir}/{N}"

mkdir "${cnfdir}/${N}"
inc=1
for i in {"4.21","4.22","4.23","4.24"}; do
	mkdir "${cnfdir}/${N}/${i}"
	for j in {1..50}; do
		n_clauses=$(echo "$i * $N" | bc)
		seed=$(echo "($inc * $2) + $j " | bc)
		inc=$(echo "$inc + 1" | bc)
		echo "N=${N}, n_clauses = ${n_clauses}, i = ${i}, j = ${j}"

		../../satgenerators/random  -n $N -m $n_clauses -o "${cnfdir}/${N}/${i}/cnf_${N}_${n_clauses}_${j}.cnf" -s $seed || exit 1
	done
done
