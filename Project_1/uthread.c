/**
 * Team Members:
 * - Will Croteau
 * crote010@umn.edu
 * - Renda Zhang
 * zhan4067@umn.edu
 *
 * Project 1, CSci5103
 **/
#include "uthread.h"

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
	address_t ret;
	asm volatile("xor    %%fs:0x30,%0\n"
			"rol    $0x11,%0\n"
			: "=g" (ret)
			: "0" (addr));
	return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
	address_t ret;
	asm volatile("xor    %%gs:0x18,%0\n"
			"rol    $0x9,%0\n"
			: "=g" (ret)
			: "0" (addr));
	return ret;
}

#endif

void set_status(int tid, int status){
  threads[tid].status = status;
}

int get_status(int tid){
  return threads[tid].status;
}

void push(int tid) {
  struct node* new_node = malloc(sizeof(struct node));
  new_node->data = tid;
  new_node->next = NULL;
  if(active.last == NULL){
    active.first = new_node;
    active.last = new_node;
  } else {
    active.last->next = new_node;
    active.last = new_node;
  }
}

int pop() {
  struct node* top = active.first;
  if(active.first == active.last){
    active.first = NULL;
    active.last = NULL;
  } else {
    active.first = active.first->next;
  }
  // if list is empty, return -1
  if(top == NULL){
    return -1;
  }

  int ret = top->data;
  free(top);
  return ret;
}

int uthread_join(int tid, void** ret) {
  // disable interupts
  sigset_t set;
  stopAlarmSignal(set);
  
  // make sure thread exists
  if(tid >= num_threads){
    return -1;
  }

  while(threads[tid].status != THREAD_RETURNED
    && threads[tid].status != THREAD_JOINED
    && threads[tid].status != THREAD_TERMINATED){
    // thread is not done, wait
    uthread_yield();
  }

  if(threads[tid].status == THREAD_TERMINATED){
    *ret = NULL;
    return -1;
  }

  threads[tid].status = THREAD_JOINED;
  if(ret != NULL){
    *ret = threads[tid].ret;
  }
  
  startAlarmSignal(set);
  return 0;
}

int uthread_yield() {
  // reset timer
  resetTimer();
  // disable interupts
  sigset_t set;
  stopAlarmSignal(set);
  
  
  // if thread is running, set it to ready
  if(threads[current_thread].status == THREAD_RUNNING) {
    threads[current_thread].status = THREAD_READY;
    push(current_thread);
  }

  // save the current state
  int ret = sigsetjmp(threads[current_thread].ctx,1);

  if(ret == 0){
    // jump to the next thread
    int next_tid = pop();
    
    // run until we find one that is ready or we run out
    while(threads[next_tid].status != THREAD_READY && next_tid != -1){
      next_tid = pop();
    }
    if(next_tid == -1){
      next_tid = current_thread;
    }
    current_thread = next_tid;
    threads[next_tid].status = THREAD_RUNNING;
    siglongjmp(threads[next_tid].ctx,1);
  }
  
  startAlarmSignal(set);
  return 0;
}

int uthread_self() {
  return current_thread;
}

int uthread_create(void *(*start_routine)(void *), void *arg) {
  // disable interupts
  sigset_t set;
  stopAlarmSignal(set);
  
  int new_tid = num_threads;
  // save the current running thread
  int ret = sigsetjmp(threads[current_thread].ctx,1);

  // if this is the calling thread, then return
  // if not, then call the routine
  if(ret == 1){
    startAlarmSignal(set);
    return new_tid;
  } else {
    num_threads++;

    address_t sp, pc;

    sp = (address_t)threads[new_tid].stack + STACK_SIZE - sizeof(int);
    threads[new_tid].status = THREAD_RUNNING;
    threads[new_tid].args = arg;
    threads[new_tid].ret = NULL;
    threads[new_tid].tid = new_tid;

    // add current thread to queue
    threads[current_thread].status = THREAD_READY;
    push(current_thread);
    current_thread = new_tid;

    ret = sigsetjmp(threads[new_tid].ctx,1);
    if(ret == 0){
      (threads[new_tid].ctx->__jmpbuf)[JB_SP] = translate_address(sp);
      sigemptyset(&threads[new_tid].ctx->__saved_mask);
      siglongjmp(threads[new_tid].ctx,1);

    } else {
      startAlarmSignal(set);
      threads[current_thread].ret = start_routine(arg);
      threads[current_thread].status = THREAD_RETURNED;
      uthread_yield();
    }
  }
}
