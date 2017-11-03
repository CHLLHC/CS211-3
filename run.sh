#!/bin/bash
module purge
module load gcc-4.7.2
module load mvapich2-1.9/gcc-4.7.2
make
