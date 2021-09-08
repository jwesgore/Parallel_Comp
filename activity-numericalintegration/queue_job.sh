#!/bin/sh


sbatch --partition=Centaurus --chdir=`pwd` --time=04:00:00 --nodes=1 --tasks-per-node=1 --job-name=mod1numint ./run_bench.sh
