#!/bin/sh

RESULTDIR=result/
NS="18000 72000 144000"
PROCS="9 16 25"

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#weak scaling

for N in ${NS};
do
    FILE=${RESULTDIR}/sequential_matrix_${N}
    if [ ! -f ${FILE} ]
    then
        echo missing sequential result file "${FILE}". Have you run queue_heateqn_seq and waited for completion?
    fi
    
    seqtime=$(cat ${RESULTDIR}/sequential_matrix_${N})
    
    
    for PROC in ${PROCS}
    do
        REALN=${N}
	#REALN=$(($((${N}*${PROC})) * ${PROC}))
 
	FILE=${RESULTDIR}/heat_${REALN}_${PROC}
        
        partime=$(cat ${RESULTDIR}/heat_${REALN}_${PROC})
        
        echo ${PROC} ${seqtime} ${partime}
    done > ${RESULTDIR}/speedup_matrix_ni_${N}

	GNUPLOTSTRONG="${GNUPLOTSTRONG} set title 'strong scaling. n=${N}'; plot '${RESULTDIR}/speedup_matrix_ni_${N}' u 1:(\$2/\$3);"
done


gnuplot <<EOF
set terminal pdf
set output 'matrix_strong_plots.pdf'

set style data linespoints

set key top left

set xlabel 'Proc'
set ylabel 'Speedup'

${GNUPLOTSTRONG}

set xlabel 'Proc'
set ylabel 'Time (in s)'

${GNUPLOTWEAK}


EOF
