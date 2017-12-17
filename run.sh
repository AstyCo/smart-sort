#!/bin/bash -x
rm *.err *.out core* # clear run
mpisubmit.bg -n 2 -w 00:15:00 -m smp -e "OMP_NUM_THREADS=2" sorter_par
