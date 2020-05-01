/*
 * this program tests if the thread are being created and run
 * normally
 * compile this program with any mythread library (one-one or many-one)
 * and then run the executable by giving it the number of times for
 * which each function prints a line
 * after all functions print lines for that number, then they exit and
 * main program also exits
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "mythread.h"

void *fun1(void *arg) {
	int i = 1, j = *((int *)arg), k = 1, l;
	while(i++ <= j) {
		for(l = 0; l < 1000000; l++);
		printf("I am in function 1: k = %d\n", k++);
	}
	return NULL;
}

void *fun2(void *arg) {
	int i = 1, j = *((int *)arg), k = 1, l;
	while(i++ <= j) {
		for(l = 0; l < 1000000; l++);
		printf("I am in function 2: k = %d\n", k++);
	}
	return NULL;
}

void *fun3(void *arg) {
	int i = 1, j = *((int *)arg), k = 1, l;
	while(i++ <= j) {
		for(l = 0; l < 1000000; l++);
		printf("I am in function 3: k = %d\n", k++);
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	mythread_t t1, t2, t3;
	int repeat;
	if(argc < 2) {
		printf("Usage: %s number_of_repeat\n", argv[0]);
		exit(0);
	}
	repeat = atoi(argv[1]);
	mythread_init();
	printf("creating thread 1\n");
	mythread_create(&t1, fun1, &repeat);
	printf("creating thread 2\n");
	mythread_create(&t2, fun2, &repeat);
	printf("creating thread 3\n");
	mythread_create(&t3, fun3, &repeat);
	printf("calling join on t1\n");
	mythread_join(t1, NULL);
	printf("calling join on t2\n");
	mythread_join(t2, NULL);
	printf("calling join on t3\n");
	mythread_join(t3, NULL);
	return 0;
}
