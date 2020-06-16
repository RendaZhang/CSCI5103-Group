/**
 * Team Members:
 * - Will Croteau
 * crote010@umn.edu
 * - Renda Zhang
 * zhan4067@umn.edu
 *
 * Project 1, CSci5103
 **/
#ifndef TIMESLICING_H
#define TIMESLICING_H

#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <aio.h>
#include <errno.h>
 //link with -lrt
#include "uthread.h"

#define SECOND 1000000

void resetTimer();

void stopAlarmSignal(sigset_t sigset);

void startAlarmSignal(sigset_t sigset);

void switch_thread_handler(int signal);

int uthread_init(int time_slice);

int uthread_terminate(int tid);

int uthread_suspend(int tid);

int uthread_resume(int tid);

ssize_t async_read(int fildes, void *buf, size_t nbytes);

#endif