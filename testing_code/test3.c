/*
 * this testing code tests the mythread_kill function and
 * if the different signal handlers set to different threads work
 * in correct manner in many-one model
 * when started, this program creates 2 threads which will set their 
 * default handlers for SIGINT
 * main() will set its own signal handler 
 * main will send SIGINT to the 2 threads and you can check if signal is 
 * delivered correctly and specific signal handler is activated
 * you can also send signal SIGINT later by pressing Ctrl-C 
 * as all threads have while(1) loop and handler for Ctrl-C is set, 
 * use Ctrl-\ to exit the program
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "mythread.h"

void printer(int sig) {
	printf("SIGINT received function 1\n");
}

void printer2(int sig) {
	printf("SIGINT received function 2\n");
}

void printer3(int sig) {
	printf("SIGINT received main()\n");
}

void *fun1(void *arg) {
	set_active_thread_signal(SIGINT, printer);
	while(1);
	return NULL;
}

void *fun2(void *arg) {
	set_active_thread_signal(SIGINT, printer2);
	while(1);
	return NULL;
}

int main() {
	mythread_t t1, t2;
	mythread_init();
	set_active_thread_signal(SIGINT, printer3);
	mythread_create(&t1, fun1, NULL);
	mythread_create(&t2, fun2, NULL);
	printf("HERE\n");
	for(int i = 0; i < 10000; i++);
	printf("HERE\n");
	mythread_kill(t1, SIGINT);
	printf("sigint sent to t1\n");
	mythread_kill(t2, SIGINT);
	printf("sigint sent to t2\n");
	while(1);
	return 0;
}
