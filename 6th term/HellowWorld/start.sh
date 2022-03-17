#!/bin/bash
#PBS -l walltime=00:01:00,nodes=7:ppn=4
#PBS -N hellow
#PBS -q batch
cd $PBS_O_WORKDIR
mpirun --hostfile $PBS_NODEFILE -np 28 ./hello
