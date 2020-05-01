/*
 * this program checks mythread_exit function by checking if the program 
 * directly exits properly through the thread and returns correct value
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mythread.h"

struct returnval {
	int test_int;
	char *test_str;
	float test_flt;
};

void stack_increaser(int n) {
	struct returnval *rtval;
	static int iterations = 0;
	if(!n) {
		rtval = (struct returnval *)malloc(sizeof(struct returnval));
		rtval->test_int = 100;
		rtval->test_str = (char *)malloc(32);
		strcpy(rtval->test_str, "this is a test string");
		rtval->test_flt = 9.0019;
		printf("thread returning from %dth recursion\n", iterations);
		mythread_exit((void *)rtval);
	}
	else {
		iterations++;
		stack_increaser(n - 1);
	}
}

void *fun(void *arg) {
	/* this function will call many functions and add 
	 * many function frames in the stack
	 */	
	srand((int)time(NULL));
	stack_increaser(rand() % 100);
	return NULL;
}

int main() {
	struct returnval *check;
	mythread_t t1;
	mythread_init();
	mythread_create(&t1, fun, NULL);
	mythread_join(t1, (void **)&check);
	printf("RETURNED VALUES:\ncheck->test_int: %d\ncheck->test_str: %s\ncheck->test_flt: %f\n", check->test_int, check->test_str, check->test_flt);
	return 0;
}
