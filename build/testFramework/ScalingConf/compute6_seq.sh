#!/bin/sh
#SBATCH --no-requeue
#SBATCH --mem 1000000
#SBATCH --nodes=1
#SBATCH --partition=production
#SBATCH --ntasks=1
#SBATCH --exclusive
#SBATCH --mail-type=ALL # Type of email notification- BEGIN,END,FAIL,ALL
#SBATCH --mail-user={d803f16@cs.aau.dk}
#SBATCH --time=00:30:05
#SBATCH --nodelist=compute6

MODEL=$1
ALG=$2
TIMEOUT=$3
I=$4
RUN_NO=$5
OUTPUTFILE="$MODEL"-CTLCardinality-"$I"-ALG"$ALG"-T"$TIMEOUT"-R"$RUN_NO".log

mkdir ~/results
mkdir ~/results/master
mkdir ~/results/master/ScalingConf
mkdir ~/results/master/ScalingConf/ALG"$ALG"-R"$RUN_NO"

ulimit -S -v 16000000
ulimit -l 16000000

export MAXMEM_KB=16000000
{ timeout "$TIMEOUT" /user/smni12/launchpad/master/verifypn-linux64 /user/smni12/launchpad/modelDatabase/allModels/"$MODEL"/model.pnml /user/smni12/launchpad/modelDatabase/allModels/"$MODEL"/CTLCardinality.xml -ctl "$ALG" -s DFS -x "$I"; } >> ~/results/master/ScalingConf/ALG"$ALG"-R"$RUN_NO"/$OUTPUTFILE 2>&1

ulimit -S -v unlimited

echo Successful result move
echo $SLURM_NODELIST
echo $SLURM_NPROCS
pwd
hostname
rm -- "$0"
