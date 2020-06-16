/**
 * Team Members:
 * - Will Croteau
 * crote010@umn.edu
 * - Renda Zhang
 * zhan4067@umn.edu
 *
 * Project 1, CSci5103
 **/
#include <stdio.h>
#include "../time_slicing.h"
#include "../uthread.h"

void* tick(void* args){
  int i;
  for(i=0; i<10; i++){
    printf("tick\n");
    int j;
    for(j=0; j<100000000; j++){}
  }
}

void* tock(void* args){
  int i;
  for(i=0; i<10; i++){
    printf("tock\n");
    int j;
    for(j=0; j<100000000; j++){}
  }
}

void* infinite(void* args){
  while(1){
    printf("running\n");
    int j;
    for(j=0; j<100000000; j++){}
  }
}

int main()
{
  printf("Test 1. switch between two threads using clock\n");
  uthread_init(SECOND);
  int tick1 = uthread_create(tick,NULL);
  int tock1 = uthread_create(tock,NULL);
  uthread_join(tick1,NULL);
  uthread_join(tock1,NULL);
  
  printf("\nTest 2. Terminate infinite loop\n");
  int inf = uthread_create(infinite,NULL);
  uthread_terminate(inf);
  printf("terminated\n");
  
  printf("\nTest 3. Suspend then resume loop\n");
  inf = uthread_create(infinite,NULL);
  printf("suspended\n");
  uthread_suspend(inf);
  long j;
  for(j=0; j<1000000000; j++){}
  printf("resumed\n");
  uthread_resume(inf);
  printf("terminated\n");
  uthread_yield();
  uthread_terminate(inf);
  
  return 0;
}
