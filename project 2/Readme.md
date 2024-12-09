ΧΡΟΝΕΑΣ ΑΝΔΡΕΑΣ 1115202100217

Makefile is included to compile jobCommander and jobExecutor programs.

To run:

./jobExecutorServer [portNum] [bufferSize] [threadPoolSize]
e.g ./jobExecutorServer 7856 8 5 (Creates port 7856 and wait for connections ,creates a buffer who can handle 8 connections and creates 5 worker threads )

./jobCommander [ServerName] [portNum] [jobCommanderInputCommand]
e.g ./jobCommander 127.0.0.1 7856 'command' (Server 127.0.0.1 connects to port 7856 and executes the command)

'command' options : 

issuejob "job"
e.g ./jobCommander 127.0.0.1 7856 'touch myFile.txt' (Executes the command : touch myFile.txt)

setConcurrency N
e.g ./jobCommander 127.0.0.1 7856 setConcurrency 3  (Changes the number of jobs that can run at the same time)

stop job_ID
e.g ./jobCommander 127.0.0.1 7856 stop 2  (Stops the job with id 2)

poll 
e.g ./jobCommander 127.0.0.1 7856 poll (Displays the jobs that are waiting to run)

exit
e.g ./jobCommander 127.0.0.1 7856 exit(jobCommander and jobExecutorServer exit*)

*There is a small bug here , jobCommander exits immidiately but jobExecutorServer fully exits one command after exit


