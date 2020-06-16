#include "../uthread.h"
#include "../time_slicing.h"
#include <stdio.h>
#include <unistd.h>

void* x(void* arg) {
  int i;
  for(i=0; i<3; i++){
    printf("Inside x\n");
    uthread_yield();
  }
}

void* y(void* arg) {
  int i;
  for(i=0; i<3; i++){
    printf("Inside y\n");
    uthread_yield();
  }
}

void* increment(void* arg) {
  int* ret = malloc(sizeof(int));
  *ret = *(int*)arg + 1;
  return (void*)ret;
}

void* print_self(void* arg) {
  printf("I am thread %d\n",uthread_self());
}

void* print_id(void* arg) {
  printf("Inside thread with id %d\n",*(int*)arg);
}

int main(){
  printf("Test 1. Create single thread that increments value\n");
  printf("Calling: increment(5)\n");
  int arg = 5;
  int tid = uthread_create(increment,&arg);
  int* ret;
  uthread_join(tid, (void**)&ret);
  printf("Joined: result = %d\n",*ret);
  assert(*ret == 6);
  
  printf("\nTest 2. Make 10 threads then join\n");
  int i;
  int tids[10];
  for(i=0; i<10; i++) {
    int id = i;
    printf("Creating thread %d\n",i);
    tids[i] = uthread_create(print_id, (void*)&id);
  }
  for(i=0; i<10; i++) {
    printf("Joining thread %d\n",i);
    uthread_join(tids[i], NULL);
  }
  
  printf("\nTest 3. Make two threads that yield back and forth\n");
  int x1 = uthread_create(x,NULL);
  int y1 = uthread_create(y,NULL);
  uthread_join(x1, NULL);
  uthread_join(y1, NULL);
  
  printf("\nTest 4. Make a thread that calls uthread_self\n");
  printf("main is thread %d\n",uthread_self());
  int new_thread = uthread_create(print_self, NULL);
  printf("Main created thread %d\n", new_thread);
  
}