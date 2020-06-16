#include "uthread.h"
#include "time_slicing.h"
#include <stdio.h>

void* x(void* arg) {
  while(1) {
    printf("in x\n");
    long i;
    for(i=0; i<90000000; i++){}
    //uthread_yield();
  }
}

void* y(void* arg) {
  while(1) {
    printf("in y\n");
    long i;
    for(i=0; i<90000000; i++){}
    //uthread_yield();
  }
}

int main(){
  uthread_init(10);
  uthread_create(x,NULL);
  uthread_create(y,NULL);
  
  uthread_join(1, NULL);
}