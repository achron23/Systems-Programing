ΧΡΟΝΕΑΣ ΑΝΔΡΕΑΣ 1115202100217

Makefile is included to compile jobCommander program.

To run the jobCommander use the following menu:

./jobCommander issuejob "job"
e.g ./jobCommander issuejob touch myFile.txt  (Executes the command : touch myFile.txt)

./jobCommander setConcurrency N
e.g ./jobCommander setConcurrency 3  (Changes the number of jobs that can run at the same time)

./jobCommander stop jobID
e.g ./jobCommander stop 2  (Stops the job with id 2)

./jobCommander poll [running,queued]
e.g ./jobCommander poll running   (Displays the jobs that are currently running)

./jobCommander exit
The jobCommander exits

Two bash scripts are included

multijob.sh <file1> <file2>...
To run the jobs in files given (You need to import the files)
Execution : ./multijob.sh filesname.txt ..

alljobsStop.sh
To stop all jobs running or queued
Execution : ./alljobsStop.sh

