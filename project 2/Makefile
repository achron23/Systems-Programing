CC = gcc
CFLAGS = -Wall -pthread
EXECUTORSRC = jobExecutorServer.c queue.c
COMMANDERSRC = jobCommander.c
EXECUTOREXEC = jobExecutorServer
COMMANDEREXEC = jobCommander
OBJS = jobExecutorServer.o queue.o jobCommander.o

all: $(EXECUTOREXEC) $(COMMANDEREXEC)

$(EXECUTOREXEC): $(EXECUTORSRC)
	$(CC) $(CFLAGS) -o $(EXECUTOREXEC) $(EXECUTORSRC)

$(COMMANDEREXEC): $(COMMANDERSRC)
	$(CC) $(CFLAGS) -o $(COMMANDEREXEC) $(COMMANDERSRC)

clean:
	rm -f $(EXECUTOREXEC) $(COMMANDEREXEC) $(OBJS)