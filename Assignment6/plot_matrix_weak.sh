#!/bin/sh

RESULTDIR=result_weak/
NS="24000 30000"
PROCS="4 16 25"

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#weak scaling

for N in ${NS};
do
    for PROC in ${PROCS}
    do
	SQRT=$(echo "sqrt (${PROC})" | bc -l)
	SQRT=${SQRT%.*}
        REALN=$(echo ${N}\* ${SQRT} | bc)
	#REALN=$(($((${N}*${PROC})) * ${PROC}))
 
	FILE=${RESULTDIR}/matrix_multiplication_weaak_${REALN}_${PROC}
        
        if [ ! -f ${FILE} ]
        then
    	echo missing heateqn result file "${FILE}". Have you run queue_heateqn and waited for completion? - WEAK_SCALING
        fi

        partime=$(cat ${RESULTDIR}/matrix_multiplication_weaak_${REALN}_${PROC})
        
        echo ${PROC} ${partime}
    done > ${RESULTDIR}/time_mat_weak_ni_${N}


    GNUPLOTWEAK="${GNUPLOTWEAK} set title 'weak scaling. n=${N}'; plot '${RESULTDIR}/time_mat_weak_ni_${N}' u 1:2;"
done


gnuplot <<EOF
set terminal pdf
set output 'matrix_weak_plots.pdf'

set style data linespoints
set yrange [20:80]
set key top left

set xlabel 'Proc'
set ylabel 'Speedup'

${GNUPLOTSTRONG}

set xlabel 'Proc'
set ylabel 'Time (in s)'

${GNUPLOTWEAK}


EOF
