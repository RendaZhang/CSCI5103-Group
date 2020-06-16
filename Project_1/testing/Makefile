# Will Croteau
# crote010@umn.edu
# Renda Zhang
# zhan4067@umn.edu

CC = gcc
DEBUG = -g
LFLAG = -lrt -lm

all: test_uthread test_time_slicing

test_uthread: uthread.o time_slicing.o test_uthread.o
	$(CC) -o $@ uthread.o time_slicing.o test_uthread.o $(LFLAG)
  
test_time_slicing: uthread.o time_slicing.o test_time_slicing.o
	$(CC) -o $@ uthread.o time_slicing.o test_time_slicing.o $(LFLAG)

test_uthread.o: test_uthread.c
	$(CC) -o $@ test_uthread.c -c
  
test_time_slicing.o: test_time_slicing.c
	$(CC) -o $@ test_time_slicing.c -c

uthread.o: ../uthread.c ../uthread.h
	$(CC) -o $@ ../uthread.c -c

time_slicing.o: ../time_slicing.c ../time_slicing.c
	$(CC) -o $@ ../time_slicing.c -c

clean:
	rm *.o test_all test_uthread test_time_slicing
