#mpi.sh
#!/bin/sh
#PBS -N mpi
#PBS -l nodes=node1_vir3+node1_vir5+node1_vir7+node1_vir8

pssh -h $PBS_NODEFILE mkdir -p /home/s2213917/MPI 1>&2 
scp master:/home/s2213917/MPI/mpi /home/s2213917/MPI
pscp -h $PBS_NODEFILE /home/s2213917/MPI/mpi /home/s2213917/MPI 1>&2
scp -r master:/home/s2213917/MPI/tests/ /home/s2213917/MPI
pscp -h $PBS_NODEFILE /home/s2213917/MPI/tests/ /home/s2213917/MPI 1>&2
mpiexec -np 4 -machinefile $PBS_NODEFILE /home/s2213917/MPI/mpi