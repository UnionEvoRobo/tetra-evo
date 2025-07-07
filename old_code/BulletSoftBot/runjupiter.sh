# Submit n jobs via qsub
# Usage:
# ./runjupiter.sh      Submits one job
# ./runjupiter.sh 5    Submites 5 jobs


if [ $# -eq 0 ]; then
	n=1
else
	n=$1
fi

i=0
while [ $i -lt $n ]; do
	qsub -N bulletSoftBot-e40-p10-$i qsub-script.sh
	echo "job $i submitted"
	sleep 1
	let i=i+1 
done


