#!/bin/sh
### VERSION 0.1
PBS_O_WORKDIR='/home/kenglish/workspace/output'


### Set the job name
#PBS -N naive

### Optionally specifiy destinations for your myprogram's output
### Specify localhost and an NFS file system to prevent file copy errors.
#PBS -e $PBS_O_WORKDIR/naive.err 
#PBS -o $PBS_O_WORKDIR/naive.out 

### Set the queue to "default"
#PBS -q default

### Using one node
#PBS -l nodes=1:ppn=2

### You can override the default 1 hour time limit.  -l walltime=HH:MM:SS
#PBS -l walltime=99:00:00

### Switch to the working directory; by default Torque launches processes from your home directory.
### Jobs should only be run from /home or a lower directory
cd $PBS_O_WORKDIR

# Put your command here
/home/kenglish/bin/naive.exe 200 


