#!/bin/sh

. ./params.sh

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#strong scaling

for POWER in ${POWERS};
do
    for N in ${NS};
    do	

	for PROC in ${PROCS}
	do
	
	    FILE=${RESULTDIR}/heatEquation_${N}_${POWER}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l procs=${PROC} -v N=${N},POWER=${POWER},PROC=${PROC} ./run_heat.sh
	    fi

	done

    done
done


#weak scaling

for POWER in ${POWERS};
do
    for N in ${NS};
    do	

	for PROC in ${PROCS}
	do
	    
	    REALN=$( echo "sqrt ( ${PROC} )*${N}"  | bc -l);
	    REALN=$(printf "%.0f" ${REALN})
	    REALN=$(( ${REALN}-(( ${REALN} % ${PROC} )) ))

	    FILE=${RESULTDIR}/heatEquation_${REALN}_${INTENSITY}_${PROC}
	    
	    if [ ! -f ${FILE} ]
	    then
		qsub -d $(pwd) -q mamba -l procs=${PROC} -v N=${REALN},POWER=${POWER},PROC=${PROC} ./run_heat.sh
	    fi

	done

    done
done


