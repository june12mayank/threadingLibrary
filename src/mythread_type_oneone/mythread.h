/* 
 * Mythread C threading library
 * This is an implementation of kernel 
 * level one-one threads in C 
 * 
 */

#ifndef MYTHREAD_H

#define MYTHREAD_H
#define MYTHREAD_ONE_ONE

#include <setjmp.h>
#include <bits/types.h>

#define SMALL_STACK_SIZE (10240)
#define STACK_SIZE (1024 * 1024)

/* these defines denote various states that a thread can 
 * have, an enumeration of these values will be equally 
 * efficient
 */
#define THREAD_RUNNING 0x0		//thread has started but not terminated
#define THREAD_NOT_STARTED 0x1	//thread not started
#define THREAD_TERMINATED 0x2	//thread terminated
#define THREAD_JOIN_CALLED 0x3	//some other thread called join on that thread
#define THREAD_COLLECTED 0x4	//the thread is already collected by some thread

#define pid_t __pid_t

/* mythread_t stores the index of the thread in the 2d 
 * array of thread pointers
 * for this, just one unsigned number storage is enough
 * same for spinlock type mythread_spinlock_t which will
 * only take values 0 and 1
 */
typedef unsigned long int mythread_t;
typedef volatile unsigned short int mythread_spinlock_t;

/* a structure which will store information about one thread
 * only
 * the information like thread id returned by clone, state of 
 * thread, the pid of the thread which called join on the thread
 * (used when join called and state of thread is THREAD_JOIN_CALLED),
 * the stack of the thread, root function from which the thread 
 * started, argument to the function and returned value is stored in 
 * the respective variables
 */
struct mythread_struct {
	int tid, state;
	__pid_t jpid;
	char *stack;
	void *(*fun)(void *);
	void *args;
	void *returnval;
};

/* static functions are not included/declared in header
 */
int __mythread_wrapper(void *mythread_struct_cur);
struct mythread_struct *__mythread_fill(void *(*fun)(void *), void *args);

/* the information about various functions is written in mythread.c
 * file
 * still, the functions mythread_xyz are similar in functioning 
 * to pthread_xyz
 */
void mythread_init(void);
int mythread_create(mythread_t *mythread, void *(*fun)(void *), void *args);
int mythread_join(mythread_t mythread, void **returnval);
int mythread_kill(mythread_t mythread, int sig);
void mythread_exit(void *returnval);
mythread_t mythread_self(void);
int mythread_spin_init(mythread_spinlock_t *lock);
int mythread_spin_lock(mythread_spinlock_t *lock);
int mythread_spin_unlock(mythread_spinlock_t *lock);
int mythread_spin_trylock(mythread_spinlock_t *lock);

#endif
