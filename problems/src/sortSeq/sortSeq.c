/*
 * Compilation command: gcc -Wall -O3 -o sortSeq sortSeq.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

void validateArray(int val[], int N);
void sortArray(int val[], int N);
void printArray(int val[], int N);
void bitonicSort(int vec[], int N);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        /* TODO: add usage message */

        printf("No files to read\n");
        exit(0);
    }

    FILE *filePtr;

    /* Open file */
    filePtr = fopen(argv[1], "rb");

    /* File read SUCCESSFUL */
    if (filePtr != NULL) {
        int N;

        /* Read header with number of integers */
        fread(&N, sizeof(int), 1, filePtr);
        printf("%d\n", N);
        exit(0);

        /* Read numbers to array */
        int vec[N];
        for (int i = 0; i < N; i++) {
            int num;
            fread(&num, sizeof(int), 1, filePtr);
            vec[i] = num;
            // printf("\t%d\n", vec[i]);
        }
        
        /* Sort array */
        sortArray(vec, N);
        
        /* Print array */
        printArray(vec, N);
        
        /* Validate sorting */
        validateArray(vec, N);

        /* Close file */
        fclose(filePtr);
    }
    /* File read ERROR */
    else {
        printf("ERROR READING FILE: %s\n", argv[1]);
    }

    return 0;
}

void validateArray(int vec[], int N) {
    int i = 0;

    for (i=0; i<N-1; i++) {
        if (vec[i] < vec[i+1]) {
            printf("Error in position %d between element %d and %d\n", i, vec[i], vec[i + 1]);
            break;
        }
    }
        
    if (i == (N-1)) {
        printf("Everything is OK!\n");
    }
}

void sortArray(int vec[], int N) {
    float k = log2f(N);

    if ((int) trunc(k) != k) {

    }

    for (int i=0; i<N; i++) {
        vec[i] = i;
    }

    bitonicSort(vec, N);
}

void bitonicSort(int vec[], int N) {
    int k, j, l, i, temp;

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

void printArray(int vec[], int N) {
    for (int i=0; i<N; i++) {
        printf("%d ", vec[i]);
    }
    printf("\n");
}