# Submit a job.  To be called with qsub.  Example usage:
# qsub qsub-script.sh 
# qsub -N MyJob qsub-script.sh     Makes the job name MyJob 

#PBS -l nodes=1:ppn=12
#PBS -l walltime=10:00:00
#PBS -j oe
#PBS -N bulletSoftBot 
#PBS -m ea

if [ "$PBS_ENVIRONMENT" != "PBS_INTERACTIVE" ] ; then
cd $PBS_O_WORKDIR
fi


./bulletSoftBot -e 40 -p 10



 
