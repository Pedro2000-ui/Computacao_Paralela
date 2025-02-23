#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 30

int main () {
    int *vetor = (int *)malloc(N * sizeof(int));
    int *vetor2 = (int *)malloc(N * sizeof(int));
    int *vetor3 = (int *)malloc(N * sizeof(int));
    
    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        vetor[i] = rand() % (N * 5);
        vetor2[i] = rand() % (N * 10);
    }
    omp_set_num_threads(4);

    #pragma omp parallel
    {
        #pragma omp for 
        for(int i = 0; i < N; i++) {
            vetor3[i] = vetor[i] + vetor2[i];
        }
    }

    printf("\n\nVetor 1: ");
    for (int i = 0; i < N; i++) printf("[%d] ", vetor[i]);

    printf("\n\nVetor 2: ");
    for (int i = 0; i < N; i++) printf("[%d] ", vetor2[i]);

    printf("\n\nVetor 3: ");
    for (int i = 0; i < N; i++) printf("[%d] ", vetor3[i]);

    free(vetor);
    free(vetor2);
    free(vetor3);
    return 0;
}