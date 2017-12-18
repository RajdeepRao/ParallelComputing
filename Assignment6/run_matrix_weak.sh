#!/bin/sh

RESULTDIR=result_weak/
h=`hostname`

if [ "$h" = "mba-i1.uncc.edu"  ];
then
    echo Do not run this on the headnode of the cluster, use qsub!
    exit 1
fi

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi
#echo   ${N}  

mpirun ./matrix_multiplication ${N} 2> ${RESULTDIR}/matrix_multiplication_weaak_${N}_${PROC}  >/dev/null
