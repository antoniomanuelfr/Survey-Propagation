#!/bin/bash
sat=0
loop_dir ()
{
	if [ -d "$1" ]; then
		nformulas=$(ls -l $1 | egrep -c '^-')
		for f in $1/*; do
			res=$(../../glucose-syrup/simp/glucose $f | tail -n 1)
			if [ "${res}" == "s SATISFIABLE" ]; then
				sat=$((sat+1))
			fi
		done
		echo "Total of SAT formulas = ${sat} from ${nformulas}"
	fi
}

loop_dir $1
