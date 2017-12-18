#!/bin/sh

RESULTDIR=result/
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
    

make heat_eqn_seq

. ./params.sh

for power in $POWERS;
do
    for n in $NS;
    do
	FILE=${RESULTDIR}/heateqn_seq_${n}_${power}

	if [ ! -f ${FILE} ]
	then
	    ./heat_eqn_seq ${n} ${power} 2>${RESULTDIR}/heateqn_seq_${n}_${power}  >/dev/null
	fi
    done
done
