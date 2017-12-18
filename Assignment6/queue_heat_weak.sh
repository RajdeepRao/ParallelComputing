#!/bin/sh

. ./param_heat_weak.sh


if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi

#weak scaling


qsub -d $(pwd) -q mamba -l mem=120GB -l procs=2 -v N=10240,PROC=2,M=5 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=4 -v N=14484,PROC=4,M=5 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=8 -v N=22120,PROC=8,M=5 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=16 -v N=31000,PROC=16,M=5 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=32 -v N=42000,PROC=32,M=5 ./run_heat_weak.sh

qsub -d $(pwd) -q mamba -l mem=120GB -l procs=2 -v N=16000,PROC=2,M=10 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=4 -v N=22000,PROC=4,M=10 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=8 -v N=32000,PROC=8,M=10 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=16 -v N=45000,PROC=16,M=10 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=32 -v N=64000,PROC=32,M=10 ./run_heat_weak.sh

qsub -d $(pwd) -q mamba -l mem=120GB -l procs=2 -v N=72000,PROC=2,M=20 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=4 -v N=102000,PROC=4,M=20 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=8 -v N=140000,PROC=8,M=20 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=16 -v N=200000,PROC=16,M=20 ./run_heat_weak.sh
qsub -d $(pwd) -q mamba -l mem=120GB -l procs=32 -v N=280000,PROC=32,M=20 ./run_heat_weak.sh



