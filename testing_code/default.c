/*
 * this is correct code for multiplication of matrices which does 
 * not use any threads, it may be slow but it can be used to compare 
 * the result which mythreadmult.c program prints
 */

#include <stdio.h>
#include <stdlib.h>

void printmatrix(int **m, int r, int c) {
	printf("%d %d\n", r, c);
	for(int i = 0; i < r; i++) {
		for(int j = 0; j < c; j++)
			printf("%d ", m[i][j]);
		putchar('\n');
	}
}

int main() {
	int **m1, **m2, **m3;
	int a, b, c, d;
	scanf("%d%d", &a, &b);
	m1 = (int **)malloc(sizeof(int *) * a);
	for(int i = 0; i < a; i++)
		m1[i] = (int *)malloc(sizeof(int) * b);
	for(int i = 0; i < a; i++)
		for(int j = 0; j < b; j++)
			scanf("%d", &m1[i][j]);
	scanf("%d%d", &c, &d);
	m2 = (int **)malloc(sizeof(int *) * c);
	for(int i = 0; i < c; i++)
		m2[i] = (int *)malloc(sizeof(int) * d);
	for(int i = 0; i < c; i++)
		for(int j = 0; j < d; j++)
			scanf("%d", &m2[i][j]);
	if(b == c) {
		m3 = (int **)malloc(sizeof(int *) * a);
		for(int i = 0; i < a; i++)
			m3[i] = (int *)malloc(sizeof(int) * d);
		for(int i = 0; i < a; i++)
			for(int j = 0; j < d; j++) {
				m3[i][j] = 0;
				for(int k = 0; k < b; k++)
					m3[i][j] += (m1[i][k] * m2[k][j]);
			}
		printmatrix(m3, a, d);
	}
	else
		printf("WRONG INPUT!\n");
	return 0;
}
