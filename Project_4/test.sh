#!/bin/bash
# A Bash script for testing scullbuffer device


echo Tests:
echo These tests are assuming a buffer size of 5

echo =============================================
echo Test 1. Testing basic functionality
echo 
./producer 1 10 0
./producer 2 11 0
./consumer 1 0
./producer 3 12 0
./producer 4 13 0
./consumer 2 0
./producer 5 14 0
./producer 6 15 0
./consumer 3 0
./consumer 4 0
./consumer 5 0
./consumer 6 0

echo =============================================
echo Test 2: Producing more than 512 bytes causes
echo buffer to save only the first 512.
echo 
./producer 1 600 0
./consumer 1 0

echo =============================================
echo Test 3. Producing more than the max buffer
echo size results in immediate return of 0 bytes
echo if no consumer is waiting.
echo
./producer 1 10 0
./producer 2 11 0
./producer 3 12 0
./producer 4 13 0
./producer 5 14 0
./producer 6 15 0
./consumer 1 0
./consumer 2 0
./consumer 3 0
./consumer 4 0
./consumer 5 0
./consumer 6 0

echo =============================================
echo Test 4. Producing more than the max buffer
echo size results in the producer blocking if a
echo consumer has opened the file.
echo
./producer 1 10 0
./producer 2 11 0
./producer 3 12 0
./producer 4 13 0
./producer 5 14 0
./consumer 1 1 &
sleep 0.2
./producer 6 15 0
./consumer 2 0
./consumer 3 0
./consumer 4 0
./consumer 5 0
./consumer 6 0

echo =============================================
echo Test 5. Consuming when the buffer is empty
echo returns 0 immediatly if there are no writers.
echo
./consumer 1 0

echo =============================================
echo Test 6. Consuming when the buffer is empty
echo blocks if there is an active writer.
echo
./producer 1 50 1 &
sleep .2
./consumer 1 0


