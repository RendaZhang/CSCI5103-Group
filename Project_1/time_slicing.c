/**
 * Team Members:
 * - Will Croteau
 * crote010@umn.edu
 * - Renda Zhang
 * zhan4067@umn.edu
 *
 * Project 1, CSci5103
 **/
#include "time_slicing.h"

int switch_counter = 0;
int time_slice_size = 0;

//reset timer
void resetTimer() {
  struct itimerval timer;
  getitimer(ITIMER_VIRTUAL, &timer);
  //begining value
  timer.it_value.tv_sec = time_slice_size / 1000000;
  timer.it_value.tv_usec = time_slice_size % 1000000;
  //interval value
  timer.it_interval.tv_sec = time_slice_size / 1000000;
  timer.it_interval.tv_usec = time_slice_size % 1000000;
  setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

//stop signal from SIGVTALRM
void stopAlarmSignal(sigset_t sigset) {
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGVTALRM);
  sigprocmask(SIG_BLOCK, &sigset, NULL);
}

//start signal from SIGVTALRM
void startAlarmSignal(sigset_t sigset) {
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGVTALRM);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);
}


void switch_thread_handler(int signal) {
  switch_counter++;
  //printf("Switch thread\n");
  uthread_yield();
}

int uthread_init(int time_slice) {
  struct sigaction sa;
  struct itimerval timer;

  time_slice_size = time_slice;

  sa.sa_handler = &switch_thread_handler;
  //setting up signal SIGVTALRM
  if (sigaction(SIGVTALRM, &sa, NULL) < 0) {
    printf("error in calling sigaction");
  }

  //begining value
  timer.it_value.tv_sec = time_slice / 1000000;
  timer.it_value.tv_usec = time_slice % 1000000;
  //interval value
  timer.it_interval.tv_sec = time_slice / 1000000;
  timer.it_interval.tv_usec = time_slice % 1000000;
  
  //start a virtual timer for SIGVTALRM
  if (setitimer(ITIMER_VIRTUAL, &timer, NULL)) {
    printf("error in calling setitimer\n");
  }
}

int uthread_terminate(int tid) {
  sigset_t set;
  //block SIGVTALRM for terminating thread
  stopAlarmSignal(set);
  
  set_status(tid, THREAD_TERMINATED);
  
  startAlarmSignal(set);
}

int uthread_suspend(int tid) {
  sigset_t set;
  //block SIGVTALRM for suspending thread
  stopAlarmSignal(set);

  /** code for suspending thread */
  // if the thread is already done, error
  int status = get_status(tid);
  if(status == THREAD_TERMINATED || status == THREAD_JOINED || status == THREAD_RETURNED){
    return -1;
  }
  // set it to suspended
  set_status(tid, THREAD_SUSPENDED);

  startAlarmSignal(set);
}

int uthread_resume(int tid) {
  sigset_t set;
  //block SIGVTALRM for resuming thread
  stopAlarmSignal(set);

  /** code for resuming thread */
  if(get_status(tid) != THREAD_SUSPENDED){
    return -1;
  }
  // set thread to ready and place on ready queue
  set_status(tid, THREAD_READY);
  push(tid);

  startAlarmSignal(set);
}

ssize_t async_read(int fildes, void *buf, size_t nbytes) {
  struct aiocb aiocb;
  memset(&aiocb, 0, sizeof(struct aiocb));
  aiocb.aio_fildes = fildes;
  aiocb.aio_buf = buf;
  aiocb.aio_nbytes = nbytes;
  aiocb.aio_lio_opcode = LIO_WRITE;

  /** suspend and resumed the thread by a SIGIO signal sent when
   * the input fd is ready*/

  if (aio_read(&aiocb) == -1) {
    printf("Error in calling aio_read(): %s\n", strerror(errno));
    exit(1);
  }

  int err, ret;

  //wait until the end of read
  while((err = aio_error(&aiocb)) == EINPROGRESS);

  err = aio_error(&aiocb);
  ret = aio_return(&aiocb);

  //error handling
  if (err != 0) {
    printf("Error in aio_error(): %s\n", strerror(err));
    exit(1);
  }
  if (ret != nbytes) {
    printf("Error in aio_return()\n");
    exit(1);
  }
}
