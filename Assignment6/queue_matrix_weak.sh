#!/bin/sh

RESULTDIR=result_weak/


if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#weak scaling


qsub -d $(pwd) -q mamba -l mem=120GB -l procs=4 -v N=36000,PROC=4 ./run_matrix_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=16 -v N=72000,PROC=16 ./run_matrix_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=25 -v N=90000,PROC=25 ./run_matrix_weak.sh

qsub -d $(pwd) -q mamba -l mem=120GB -l procs=4 -v N=48000,PROC=4 ./run_matrix_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=16 -v N=96000,PROC=16 ./run_matrix_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=25 -v N=120000,PROC=25 ./run_matrix_weak.sh

qsub -d $(pwd) -q mamba -l mem=120GB -l procs=4 -v N=60000,PROC=4 ./run_matrix_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=16 -v N=120000,PROC=16 ./run_matrix_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=25 -v N=150000,PROC=25 ./run_matrix_weak.sh



