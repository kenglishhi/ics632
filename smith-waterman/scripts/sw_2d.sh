#!/bin/sh
### VERSION 0.1

### Set the job name
#PBS -N sw_2d


### Uncomment to send email when the job is completed:
### #PBS -m ae 
### #PBS -M userName@hawaii.edu 

### Optionally specifiy destinations for your myprogram's output
### #PBS -e $PBS_O_WORKDIR/myTestJob.err 
### #PBS -o $PBS_O_WORKDIR/myTestJob.out 

### Specify the number of NODES for your jobs. 
### ** ALWAYS USE "ppn=2" **
### using 4 nodes is done as
#PBS -l nodes=4:ppn=2

P4_GLOBMEMSIZE=33554432

### You can override the default 1 hour time limit.  -l walltime=HH:MM:SS
#PBS -l walltime=99:00:00

####### DO NOT MODIFY THE FOLLOWING BLOCK ###############

### Set the queue to "default".
#PBS -q default


### Switch to the working directory; by default Torque launches processes from your home directory.
### Jobs should only be run from /home or a lower directory
cd $PBS_O_WORKDIR

echo "###################Begin Job Information#################"
echo Working directory is $PBS_O_WORKDIR
### Run some informational commands.
echo Running on host `hostname`
echo Time is `date`
echo Directory is `pwd`
### ### Strip the file of duplicate entries
echo $PBS_NODEFILE
cat $PBS_NODEFILE | /home/casanova/public/remove_duplicates.pl > /tmp/kenglish-nodefile.$PBS_JOBID
echo This jobs runs on the following nodes:
echo `cat /tmp/kenglish-nodefile.$PBS_JOBID`
### ### Define number of processors
NPROCS=`wc -l < /tmp/kenglish-nodefile.$PBS_JOBID`
echo This job runs on $NPROCS nodes
echo "###################End Job Information##################"

### Clean up even if termination signal is received 
trap cleanup 1 2 3 6 14 15

cleanup()
{
  echo "Caught Termination Signal ... cleaning up anyway" >&2
  /share/apps/clusterScripts/bin/mpich_clean_up_batch /tmp/kenglish-nodefile.$PBS_JOBID
  exit 1
}
####### END OF BLOCK ###################################

#### YOUR PROGRAM: modivy only the last two items on the line

PROGRAM_EXE="/home/kenglish/workspace/ics632/smith-waterman/2d/sw_2d"

#for ((j=100;j<=100;j+=100)); do
#  for ((i=1000;i<=15000;i+=1000)); do
#    mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  $i $j
#  done
#done

mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  100 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  200 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  400 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  800 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  1000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  1000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  2000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  3000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  4000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  5000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  6000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  7000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  8000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  9000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  10000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  11000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  12000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  13000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  14000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  15000 
mpirun -v -machinefile /tmp/kenglish-nodefile.$PBS_JOBID -np $NPROCS $PROGRAM_EXE  16000 

## DO NOT CHANGE ANYTHING BELOW ###############

## CLEAN UP SCRIPT AFTER THE PROGRAM HAS EXECUTED
/share/apps/clusterScripts/bin/mpich_clean_up_batch /tmp/kenglish-nodefile.$PBS_JOBID

/bin/rm -f /tmp/kenglish-nodefile.$PBS_JOBID



