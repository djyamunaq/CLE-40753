/*
 * Compilation command: gcc -Wall -O3 -o bitonicSort bitonicSort.c -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

void bitonicSort(int val[], int N);
void CAPS(int vec[], int i, int j);
void printArray(int val[], int N);
void validateArray(int val[], int N);

int main(int argc, char *argv[]) {
    int vec[] = {3, 7, 4, 8, 6, 2, 1, 5};

    int N = sizeof(vec)/sizeof(int);
    printArray(vec, N);
    bitonicSort(vec, N);
    printArray(vec, N);

    return 0;
}

void bitonicSort(int vec[], int N) {
    int k, j, l, i, temp;
    /*  */
    for (k = 2; k <= N; k *= 2) {
        for (j = k/2; j > 0; j /= 2) {
            for (i = 0; i < N; i++) {
                l = i ^ j;
                if (l > i) {
                    if ( (((i&k)==0) && (vec[i] > vec[l])) || (( ( (i&k)!=0) && (vec[i] < vec[l]))) )  {
                        temp = vec[i];
                        vec[i] = vec[l];
                        vec[l] = temp;

                        printf("k=%d j=%d i=%d l=%d\n\t", k, j, i, l);
                        printArray(vec, N);
                    }
                }
            }
        }
    }
}

void CAPS(int vec[], int i, int j) {
    if(vec[i] < vec[j]) {
        int temp = vec[i];
        vec[i] = vec[j];
        vec[j] = temp;
    }
}

void printArray(int val[], int N) {
    for (int i=0; i<N; i++) {
        printf("%d ", val[i]);
    }
    printf("\n");
}

void validateArray(int val[], int N) {
    int i = 0;

    for (i=0; i<N-1; i++) {
        if (val[i] < val[i+1]) {
            printf("Error in position %d between element %d and %d\n", i, val[i], val[i + 1]);
            break;
        }
    }
        
    if (i == (N-1)) {
        printf("Everything is OK!\n");
    }
}