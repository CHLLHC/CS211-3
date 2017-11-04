#!/bin/bash
echo "Loading modules"
module purge
module load gcc-4.7.2
module load mvapich2-1.9/gcc-4.7.2
echo "Modules loaded, compiling."
set -x
mpic++ -o run1 source1.cpp -std=c++11
mpic++ -o run2 source2.cpp -std=c++11
mpic++ -o run3 source3.cpp -std=c++11
set +x
echo "Submitting Jobs, please wait"
king="CHL_P3"
part="P"
dash="_"
node="N"		
jobfileext=".job"
for nc in 1 2 4 8
do
	for pt in 1 2 3
	do
		name=$king$dash$part$pt$dash$nc$node
		jobfile=$name$jobfileext
		echo "#!/bin/sh" > $jobfile
		echo "#PBS -l nodes=$nc:ppn=32,walltime=01:00:00" >> $jobfile
		echo "#PBS -N $name" >> $jobfile
		echo "module load gcc-4.7.2" >> $jobfile
		echo "module load mvapich2-1.9/gcc-4.7.2" >> $jobfile
		echo "PBS_O_WORKDIR=$(pwd)" >> $jobfile
		echo "cd \$PBS_O_WORKDIR" >> $jobfile
		echo "mpirun ./run$pt 10000000000" >> $jobfile
		handle=$(qsub $jobfile)
		printf "."
	done
done
printf "\n"
cusr=$(whoami)
echo "Jobs submitted, please come back later."
{
while [[ ! -z "$(qstat | grep "CHL_P3")" ]]
do
	sleep 5
done
printf "====>CHL<====\nhchen070's jobs have finished, please check.\n=============\n" | write $cusr
echo "hchen070's jobs have finished, please check." | mail -s "hchen070's jobs Done." $cusr
} &

