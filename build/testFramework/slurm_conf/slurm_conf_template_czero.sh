#!/bin/sh
#SBATCH --nodes {NODES}
#SBATCH --partition=production
#SBATCH -n 1
#SBATCH --mail-type=ALL # Type of email notification- BEGIN,END,FAIL,ALL
#SBATCH --mail-user={{d803f16@cs.aau.dk}}

OUTPUTFILE={MODELNAME}-{QUERYCAT}-{QUERY_NUM}-{STRATEGY}.log

ulimit -S -v {MEMLIMIT}
{{ timeout {TIMEOUT} {OPT_MPI} {ENGINEPATH} {MODELPATH} {QUERYPATH} {ALGFLAG} -s {STRATEGY} -x {QUERY_NUM}; }} >> $OUTPUTFILE 2>&1
ulimit -S -v unlimited
echo Exitcode: $? >> $OUTPUTFILE 2>&1
echo Run Complete!
mkdir /user/smni12/results/master/{EXPERIMENT}-{MODELCONF}-{TIMESTAMP}-{ENGINENAME}-M{MEMLIMIT}-T{TIMEOUT}
mv $OUTPUTFILE /user/smni12/results/master/{EXPERIMENT}-{MODELCONF}-{TIMESTAMP}-{ENGINENAME}-M{MEMLIMIT}-T{TIMEOUT}/$OUTPUTFILE
echo Successful result move
echo $SLURM_NODELIST
echo $SLURM_NPROCS
pwd
hostname
rm -- "$0"
