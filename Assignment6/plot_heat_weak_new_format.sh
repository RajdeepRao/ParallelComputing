#!/bin/sh

NS="5 10 20"
PROCS="2 4 8 16 32"
RESULTDIR=result_weak/

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#weak scaling

for N in ${NS};
do
    
    for PROC in ${PROCS}
    do
        REALN=${N}
        
        FILE=${RESULTDIR}/heat_weak_${REALN}_${PROC}
        
        if [ ! -f ${FILE} ]
        then
    	echo missing heateqn result file "${FILE}". Have you run queue_heateqn and waited for completion? - WEAK_SCALING
        fi

        partime=$(cat ${RESULTDIR}/heat_weak_${REALN}_${PROC})
        
        echo ${PROC} ${partime}
    done > ${RESULTDIR}/time_heateqn_ni_${N}_30


    GNUPLOTWEAK="${GNUPLOTWEAK} set title 'weak scaling. n=${N}'; plot '${RESULTDIR}/time_heateqn_ni_${N}_30' u 1:2;"
done


gnuplot <<EOF
set terminal pdf
set output 'heateqn_weak_new_format_plots.pdf'

set style data linespoints

set key top left
set yrange [0:500]
set xlabel 'Proc'
set ylabel 'Speedup'

${GNUPLOTSTRONG}

set xlabel 'Proc'
set ylabel 'Time (in s)'

${GNUPLOTWEAK}


EOF
