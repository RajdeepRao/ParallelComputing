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


#N="1000000000"
THREADS="1 2 4 8 12 16"

make lcs


for t in $THREADS;
do
    ./lcs $t 100000 100100 >/dev/null 2> ${RESULTDIR}/lcs_${t}
done
	     

for t in $THREADS;
do
	#output in format "thread seq par"
	echo ${t} \
	     $(cat ${RESULTDIR}/lcs_1) \
	     $(cat ${RESULTDIR}/lcs_${t})
done   > ${RESULTDIR}/speedup_lcs

gnuplot <<EOF

set terminal pdf
set output 'lcs_plots_100000.pdf'

set style data linespoints


set key top left;
set xlabel 'threads'; 
set ylabel 'speedup';
set xrange [1:20];
set yrange [0:20];
set title 'm=100100  n=100000';
plot '${RESULTDIR}/speedup_lcs' u 1:(\$2/\$3) t 'lcs' lc 1, \

EOF
