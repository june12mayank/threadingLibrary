/*
 * this is a testing program which calculates the multiplication
 * of two matrices by creating 3 threads which calculate the multiplication
 * by dividing rows in between them
 * use the mat.txt, bigmat.txt verybigmat.txt files for bigger matrix inputs
 */

#include <stdio.h>
#include <stdlib.h>
#include "mythread.h"

struct matrix {
	int **mat;
	int r, c;
};

struct partinfo {
	struct matrix *m1, *m2, *ans;
	int start;
	int end;
};

struct matrix readmat(void);
void *partialmult(void *args);
void printmatrix(struct matrix m);
void destroymat(struct matrix *m);

struct matrix readmat(void) {
	struct matrix m;
	scanf("%d%d", &m.r, &m.c);
	m.mat = (int **)malloc(sizeof(int *) * m.r);
	for(int i = 0; i < m.r; i++) 
		m.mat[i] = (int *)malloc(sizeof(int) * m.c);
	for(int i = 0; i < m.r; i++)
		for(int j = 0; j < m.c; j++)
			scanf("%d", &m.mat[i][j]);
	return m;
}

void destroymat(struct matrix *m) {
	for(int i = 0; i < m->r; i++)
		free(m->mat[i]);
	free(m->mat);
}

void *partialmult(void *args) {
	struct matrix *m1 = ((struct partinfo *)args)->m1, *m2 = ((struct partinfo *)args)->m2, *m3 = ((struct partinfo *)args)->ans;
	for(int i = ((struct partinfo *)args)->start; i < ((struct partinfo *)args)->end; i++) 
		for(int j = 0; j < m2->c; j++) {
			m3->mat[i][j] = 0;
			for(int k = 0; k < m1->c; k++)
				m3->mat[i][j] += (m1->mat[i][k] * m2->mat[k][j]);
		}
	mythread_exit(NULL);
	return NULL;
}

void printmatrix(struct matrix m) {
	printf("%d %d\n", m.r, m.c);
	for(int i = 0; i < m.r; i++) {
		for(int j = 0; j < m.c; j++)
			printf("%d ", m.mat[i][j]);
		putchar('\n');
	}
}

int main() {
	int t;
	mythread_t pt1, pt2, pt3;
	struct matrix mat1, mat2, ansmat;
	struct partinfo arg1, arg2, arg3;
	mythread_init();
	mat1 = readmat();
	mat2 = readmat();
	if(mat1.c != mat2.r) {
		fprintf(stderr, "error: dimensions inconsistent for multiplication - matrices can not be multiplied\n");
		exit(1);
	}
	ansmat.r = mat1.r;
	ansmat.c = mat2.c;
	ansmat.mat = (int **)malloc(sizeof(int *) * mat1.r);
	for(int i = 0; i < mat1.r; i++)
		ansmat.mat[i] = (int *)malloc(sizeof(int) * mat2.c);
	t = mat1.r / 3;
	arg1.m1 = &mat1;
	arg1.m2 = &mat2;
	arg1.ans = &ansmat;
	arg1.start = 0;
	arg1.end = t;
	mythread_create(&pt1, partialmult, &arg1);
	arg2.m1 = &mat1;
	arg2.m2 = &mat2;
	arg2.ans = &ansmat;
	arg2.start = t;
	arg2.end = 2 * t;
	mythread_create(&pt2, partialmult, &arg2);
	arg3.m1 = &mat1;
	arg3.m2 = &mat2;
	arg3.ans = &ansmat;
	arg3.start = 2 * t;
	arg3.end = 3 * t + mat1.r % 3;
	mythread_create(&pt3, partialmult, &arg3);
	mythread_join(pt1, NULL);
	mythread_join(pt2, NULL);
	mythread_join(pt3, NULL);
	printmatrix(ansmat);
	destroymat(&mat1);
	destroymat(&mat2);
	destroymat(&ansmat);
	return 0;
}
