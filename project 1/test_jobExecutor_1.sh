./jobCommander issueJob ./progDelay 1000
./jobCommander issueJob ls
./jobCommander issueJob ls

./jobCommander issueJob ls
./jobCommander issueJob ls
./jobCommander issueJob ls
./jobCommander setConcurrency 4
./jobCommander poll running
./jobCommander poll queued
./jobCommander stop 4
./jobCommander stop 5
./jobCommander poll running
./jobCommander poll queued
./jobCommander exit


