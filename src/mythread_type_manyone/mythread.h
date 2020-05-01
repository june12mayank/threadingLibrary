/* 
 * Mythread C threading library
 * This is an implementation of user level
 * many-one threads in C 
 * 
 */

#ifndef MYTHREAD_H

#define MYTHREAD_H
#define MYTHREAD_MANY_ONE

#include <signal.h>
#include <sys/ucontext.h>

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

/* mythread_t stores the index of the thread in the 2d 
 * array of thread pointers
 * for this, just one unsigned number storage is enough
 * same for spinlock type mythread_spinlock_t which will
 * only take values 0 and 1
 */
typedef unsigned long int mythread_t;
typedef volatile unsigned short int mythread_spinlock_t;

/* pending signals to a thread for which the handler will
 * be activated once that thread comes in running (its context
 * is currently in action)
 * it also contains a pointers to the next structure along with 
 * the signal number to form a queue data structure
 */
struct pending_signal_node {	
	int sig;
	struct pending_signal_node *next;
};

/* a queue data structure which holds head and tail pointers 
 * to the node structures containing signal numbers of the signals
 * which are pending to a thread
 */
typedef struct pending_signals_queue {
	struct pending_signal_node *head, *tail;
} pending_signals_queue;

/* a structure which will store information about one thread
 * only
 * this structure is a little different than the structure for 
 * kernel level threads (one-one) as it needs to contain some 
 * different information like signal handlers for the threads (as 
 * signal handling has to be explicitely handled because these threads
 * are user level threads), context of thread, pending signals of the
 * thread etc
 */
struct mythread_struct {
	int state;
	void *(*fun)(void *);
	void *args;
	void *returnval;
	__sighandler_t handlers[32];
	ucontext_t thread_context;
	pending_signals_queue pending_signals;
};

/* those threads which have completed their execution need not be 
 * included in context switching when SIGALRM is received
 * so maintaining a different list of active threads is necessary 
 * still all threads which were created will NOT be freed as 
 * user may still call functions on them
 */
struct active_thread_node {
	mythread_t thread;
	ucontext_t *c;
	struct active_thread_node *next;
};

/* static functions are not included/declared in header
 */
void __mythread_wrapper(int ind);
void __mythreadfill(void *(*fun)(void *), void *args);

/* the information about various functions is written in mythread.c
 * file
 * still, the functions mythread_xyz are similar in functioning 
 * to pthread_xyz
 */
void mythread_init();
int mythread_create(mythread_t *mythread, void *(*fun)(void *), void *args);
int mythread_join(mythread_t mythread, void **returnval);
int mythread_kill(mythread_t mythread, int sig);
void mythread_exit(void *returnval);
__sighandler_t set_active_thread_signal(int signum, __sighandler_t handler);
mythread_t mythread_self(void);
int mythread_spin_init(mythread_spinlock_t *lock);
int mythread_spin_lock(mythread_spinlock_t *lock);
int mythread_spin_unlock(mythread_spinlock_t *lock);
int mythread_spin_trylock(mythread_spinlock_t *lock);

#endif
