# Project 1
**Group Members:**
- Will Croteau
- Renda Zhang

**Tesing:**
- Going into the directory "testing".
```
Run the makefile inside:
$ make
$ ./test_uthread
$ ./test_time_slicing
```

- Basic tests of the api have been provided.
- Testing of all the functions is done by editing the "test_all.c";
- Testing of the functions in file "uthread.c" and "uthread.h" is done by editing
- the "test_uthread.c";
- Testing of the functions in file time_slice.c and time_slice.h is done by
- editing "test_time_slicing.c".



**important design choices:**


    a. What (additional) assumptions did you make?
     We assume that The Maximun threads is 100. We assume this is a single CPU machine. There is no processing.
  
    b. Describe your API, including input, output, return value.
     The input is Passing time slice to thread_init, and create two threads, output would be the functions that executed inside the threads. return value would be the function returned value.
     
     For all uthread functions, the input is the same as the pthread library. All functions return 0 on success and -1 on error, except for uthread_create and uthread_self, which return the thread ID.

    c. Did you add any customized APIs? What functionalities do they provide?
      Nothing very useful to the user. Functions to get or set the status of individual threads is provided, but would not be very useful.
  
    d. How did your library pass input/output parameters to a thread entry function?
      The thread entry function is called in uthread_create. uthread_create does the equivilent of forking to a seperate thread. The calling thread will return, while the new thread has a new stack attached and the entry function is called with its parameters like a normal function.
 
    e. How did you implement context switch? E.g. using sigsetjmp or getcontext?
       Context switch is implemented using sigsetjmp.
       
    f. How do different lengths of time slice affect performance?
        The shorted time slice has more overhead of switching, and long time slice has less overhead of switching.

    g. What are the critical sections your code has protected against interruptions and why?
        The critical sections of joining, yield, create, terminate, suspend and resume thread, has been protected against interruptions.
        Because they need to be protected, so the critical data inside won't be affected by switching of threads.
        
    h. If you have implemented more advanced scheduling algorithm, describe them.
       No, we didn't implemented more advanced scheduling algorithm.
       
    i. How did you implement asynchronous I/O? E.g. through polling, or asynchronous signals? If your asynchronous read is based on asynchronous signals, how did you implement it? Does it support concurrent signals, e.g. when multiple fds are ready at the same time?
       We implement asychronous I/O through polling.

    j. Does your library allow each thread to have its own signal mask, or do all threads share the same signal mask? How did you guarantee this?
       Our library allow each thread to share the same signal mask. It is guarantee by using startAlarmSignal and stopAlarmSignal functions, and inherits the same signal mask from the main thread.
