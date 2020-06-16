# Will Croteau
# crote010@umn.edu
# Renda Zhang
# zhan4067@umn.edu

file1 = ../time_slicing
file2 = ../uthread
test1 = test_all
test2 = test_time_slicing
test3 = test_uthread
CC = gcc
DEBUG = -g
LFLAG = -lrt -lm

all: main

main: uthread.o time_slicing.o main.o
	$(CC) -o $@ uthread.o time_slicing.o main.o $(LFLAG)

main.o: main.c
	$(CC) -o $@ main.c -c  

uthread.o: uthread.c uthread.h
	$(CC) -o $@ uthread.c -c

time_slicing.o: time_slicing.c time_slicing.c
	$(CC) -o $@ time_slicing.c -c

clean:
	rm *.o main
