/**
 * Team Members:
 * - Will Croteau
 * crote010@umn.edu
 * - Renda Zhang
 * zhan4067@umn.edu
 *
 * Project 1, CSci5103
 **/
 
#ifndef UTHREAD_H
#define UTHREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "time_slicing.h"

#define SECOND 1000000
#define STACK_SIZE 4096
#define MAX_THREADS 100

#define THREAD_READY 0
#define THREAD_RUNNING 1
#define THREAD_RETURNED 2
#define THREAD_JOINED 3
#define THREAD_SUSPENDED 4
#define THREAD_TERMINATED 5

typedef struct tcb_struct {
  int status;
  int tid;
  char stack[STACK_SIZE];
  sigjmp_buf ctx;
  void* args;
  void* ret;
} tcb;

static tcb threads[MAX_THREADS];

static int current_thread = 0;

static int num_threads = 1;

struct queue {
  struct node* first;
  struct node* last;
};

struct node {
  int data;
  struct node* next;
};

static struct queue active = {NULL, NULL};

int uthread_yield();

int uthread_join(int tid, void** ret);

int uthread_create(void *(*start_routine)(void *), void *arg);

int uthread_self();

#endif