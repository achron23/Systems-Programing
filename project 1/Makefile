SOURCE = issuejob.c jobCommander.c jobExecutorServer.c queue.c main.c
OBJS = issuejob.o jobCommander.o jobExecutorServer.o queue.o main.o
HEADER = jobCommander.h queue.h
OUT = jobCommander
FILES =  jobExecutorServer.txt fifo fifo2 

CC = gcc

FLAGS = -c

all : $(OBJS)
		$(CC) $(OBJS) -o $(OUT)

main.o : main.c	
		$(CC) $(FLAGS) main.c


issuejob.o : issuejob.c	
		$(CC) $(FLAGS) issuejob.c

jobCommander.o : jobCommander.c	
		$(CC) $(FLAGS) jobCommander.c


jobExecutorServer.o : jobExecutorServer.c	
		$(CC) $(FLAGS) jobExecutorServer.c

queue.o : queue.c	
		$(CC) $(FLAGS) queue.c

build : $(SOURCE)
		$(CC) -o $(OUT) $(SOURCE)

clean : 
		rm -rf $(OBJS) $(OUT) $(FILES) myfifo myfifo2 myfile.txt
