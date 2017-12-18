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

mpirun ./heatEquation ${N} 30 2> ${RESULTDIR}/heat_weak_${M}_${PROC}  >/dev/null

