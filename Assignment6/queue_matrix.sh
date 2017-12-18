#!/bin/sh

. ./params_matrix.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#strong scaling


for N in ${NS};
do	

for PROC in ${PROCS}
do
	
	    FILE=${RESULTDIR}/matrix_multiplication_${N}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l mem=120GB -l procs=${PROC} -v N=${N},PROC=${PROC} ./run_matrix.sh
	    fi

done

done



