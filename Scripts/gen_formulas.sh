#!/bin/bash
N=$1
cnfdir="../cnf/testCNF"
if [ "$3" == rm ]; then
  cp ../cnf ./cnf_save -r
  rm -rf "../cnf/testCNF/{N}/*"
fi
mkdir "${cnfdir}/${N}"
for i in {"4.21","4.22","4.23","4.24"}; do
  rm -rf "${cnfdir}/${N}/*"

	mkdir "${cnfdir}/${N}/${i}"
	rm -rf
	for j in {1..50}; do
		nvars=$(echo "$i * $N" | bc)
		echo "N=${N}, nvars = ${nvars}, i = ${i}, j = ${j}"
		../../satgenerators/random  -m $N -n $nvars -o "${cnfdir}/${N}/${i}/cnf_${N}_${nvars}_${j}.cnf" -s $2 || exit 1
	done
done
