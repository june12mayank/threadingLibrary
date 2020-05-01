#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mythread.h"

/* all threads will be malloced and addresses stored in pointers in this 2d array 
 * of thread pointers __allthreads and even after this thread terminates, these pointers
 * will NOT be freed, because anyone can call join on these threads, also can use functions
 * on these threads or order for returned value from these threads, so it is better to keep 
 * all this information like state of thread, return value etc
 */
static struct mythread_struct **__allthreads[16] = {0};

/* it keeps track of a count of threads created until now
 */
static int __ind = 0;

/* a superlock variable which is used to lock and unlock (spinlock)
 * internally in thread structures (while modifying data structures)
 */
static volatile int superlock = 0;

/* a static lock which will only be used internally by thread functions
 * this function locks the lock
 */
static inline void superlock_lock() {
	while(__sync_lock_test_and_set(&superlock, 1));
}

/* unlocks the static superlock 
 */
static inline void superlock_unlock() {
	__sync_synchronize();
	superlock = 0;
}

void mythread_init() {
	/*
	 * no need to initialise anything in one-one model, just created
	 * empty function so that same testing code can be used with many-one
	 * and one-one model
	 */
}

/* wrapper function of type int (*f)(void *) which wraps the function
 * of type void *(*f)(void *) in it so that it can be passed to 
 * clone() call.
 * it also stores the returned value in the mythread_struct
 */
int __mythread_wrapper(void *mythread_struct_cur) {
	((struct mythread_struct *)mythread_struct_cur)->returnval = ((struct mythread_struct *)mythread_struct_cur)->fun(((struct mythread_struct *)mythread_struct_cur)->args);
	superlock_lock();
	((struct mythread_struct *)mythread_struct_cur)->state = THREAD_TERMINATED;
	superlock_unlock();
	return 0;
}

/* if any error occures, then this function removes the newly allocated
 * thread and decrements the index __ind by one 
 */
void __mythread_removelastfilled(void) {
	__ind--;
	int cur = __ind / 16;
	int locind = __ind % 16;
	free(__allthreads[cur][locind]);
}

/* it takes function and arguments and returns a structure of type
 * mythread_struct which contains useful information of the current 
 * thread and can be passwed to function __mythread_wrapper
 */
struct mythread_struct *__mythread_fill(void *(*fun)(void *), void *args) {
	int cur = __ind / 16;
	int locind = __ind % 16;
	if(!__allthreads[cur])
		__allthreads[cur] = (struct mythread_struct **)malloc(sizeof(struct mythread_struct *) * 16);
	__allthreads[cur][locind] = (struct mythread_struct *)malloc(sizeof(struct mythread_struct));
	__allthreads[cur][locind]->fun = fun;
	__allthreads[cur][locind]->args = args;
	__allthreads[cur][locind]->returnval = NULL;
	__allthreads[cur][locind]->state = THREAD_NOT_STARTED;
	__allthreads[cur][locind]->stack = (char *)malloc(STACK_SIZE);
	__ind++;
	return __allthreads[cur][locind];
}

/* creates a oneone thread and starts it for given function and given 
 * argument (fun and args)
 * it returns 0 on success and -1 on error
 * the thread id is stored in the location pointed by mythread
 * if any error occures, it frees the allocated structure
 */
int mythread_create(mythread_t *mythread, void *(*fun)(void *), void *args) {
	int status;
	struct mythread_struct *t;
	superlock_lock();
   	t = __mythread_fill(fun, args);
	*mythread = __ind;
	if(!t) {
		superlock_unlock();
		return -1;
	}
	t->state = THREAD_RUNNING;
	status = clone(__mythread_wrapper, (void *)(t->stack + STACK_SIZE), SIGCHLD | CLONE_VM | CLONE_SIGHAND | CLONE_FS | CLONE_FILES, (void *)t);
	if(status == -1) {
		__mythread_removelastfilled();
		superlock_unlock();
		return -1;
	}
	else
		t->tid = status;
	superlock_unlock();
	return 0;
}

/* returns ID of the calling thread, if there are no threads created, 
 * then it returns -1
 * if the thread calling it is main thread, then it returns 0
 */
mythread_t mythread_self(void) {
	pid_t pid = getpid();
	int ind, i, cur, locind;
	superlock_lock();
	ind = __ind;
	superlock_unlock();
	if(__ind == 0)
		return -1;
	for(i = 0; i < ind; i++) {
		cur = ind / 16;
		locind = ind % 16;
		if(pid == __allthreads[cur][locind]->tid)
			return i + 1;
	}
	return 0;
}

/* waits for the thread mythread to complete 
 * it returns 0 on success and EINVAL on wrong thread_t argument and ESRCH
 * if the thread with thread id mythread can not be found
 * if returnval is not NULL, then stores the value returned by thread 
 * in the location pointed by function which was running by the thread
 */
int mythread_join(mythread_t mythread, void **returnval) {
	int cur, locind, status, wstatus;
	mythread--;
	cur = mythread / 16;
	locind = mythread % 16;
	if(mythread < __ind) {
		superlock_lock();
		switch(__allthreads[cur][locind]->state) {
			case THREAD_RUNNING:
				__allthreads[cur][locind]->jpid = getpid();
				__allthreads[cur][locind]->state = THREAD_JOIN_CALLED;
				superlock_unlock();
				while(1) {
					waitpid(__allthreads[cur][locind]->tid, &wstatus, 0);
					if(WIFEXITED(wstatus))
						break;
				}
				superlock_lock();
				__allthreads[cur][locind]->state = THREAD_COLLECTED;
				superlock_unlock();
				if(returnval)
					*returnval = __allthreads[cur][locind]->returnval;
				status = 0;
				break;
			case THREAD_NOT_STARTED: 
			case THREAD_JOIN_CALLED: 
			case THREAD_COLLECTED:
				superlock_unlock();
				status = EINVAL;
				break;
			case THREAD_TERMINATED:
				superlock_unlock();
				if(returnval) 
					*returnval = __allthreads[cur][locind]->returnval;
				status = 0;
				break;
			default:
				superlock_unlock();
				status = EINVAL;
				break;
		}
		return status;
	}
	else 
		return ESRCH;
}

/* sends signal sig to the thread represented by
 * mythread_t mythread
 */
int mythread_kill(mythread_t mythread, int sig) {
	int cur, locind, status;
	mythread--;
	cur = mythread / 16;
	locind = mythread % 16;
	status = kill(__allthreads[cur][locind]->tid, sig);
	return status;
}

/* when called, the calling thread exits, if returnval is not 
 * NULL, then the value returned by thread is stored in the location pointed 
 * by returnval
 */
void mythread_exit(void *returnval) {
	pid_t pid = getpid();
	int i, cur, locind;
	for(i = 0; i < __ind; i++) {
		cur = i / 16;
		locind = i % 16;
		if(__allthreads[cur][locind]->tid == pid)
			break;
	}
	if(i == __ind)
		return;
	superlock_lock();
	__allthreads[cur][locind]->returnval = returnval;
	__allthreads[cur][locind]->state = THREAD_TERMINATED;
	superlock_unlock();
	exit(0);
}

/* initialises the mythread_spinlock_t pointed by lock
 */
int mythread_spin_init(mythread_spinlock_t *lock) {
	*lock = 0;
	return 0;
}

/* aquires the lock on mythread_spinlock_t pointed by lock, 
 * if the lock is already held by another thread, then it continuously
 * loops until lock is freed again
 */
inline int mythread_spin_lock(mythread_spinlock_t *lock) {
	if(*lock != 0 || *lock != 1)
		return EINVAL;
	while(__sync_lock_test_and_set(lock, 1));
	return 0;
}

/* frees the mythread_spinlock_t pointed by lock
 */
inline int mythread_spin_unlock(mythread_spinlock_t *lock) {
	if(*lock != 1)
		return EINVAL;
	__sync_synchronize();
	*lock = 0;
	return 0;
}

/* same as mythread_spin_lock with the difference that if the lock is 
 * held by some another thread, it returns immediately with the error
 * number EBUSY
 */
inline int mythread_spin_trylock(mythread_spinlock_t *lock) {
	return __sync_lock_test_and_set(lock, 1) ? EBUSY : 0;
}
