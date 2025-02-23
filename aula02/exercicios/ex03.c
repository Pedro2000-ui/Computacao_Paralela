#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 30

int main () {
    
    int *vetor = (int *)malloc(N * sizeof(int));
    int menor_por_thread, menor_global;
    
    srand(time(NULL));

    printf("Vetor: ");
    for (int i = 0; i < N; i++) {
        vetor[i] = rand() % (N * 5);
        printf("[%d] ", vetor[i]);
    }
    printf("\n\n\n\n");
    menor_global = vetor[0];
    omp_set_num_threads(2);

    #pragma omp parallel
    {
        menor_por_thread = menor_global;
        int thread = omp_get_thread_num();
        
        #pragma omp for 
        for(int i = 0; i < N; i++) {
            if(vetor[i] < menor_por_thread) {
                menor_por_thread = vetor[i];
            }
        }
        printf("\n\n Thread %d: \n Menor Elemento encontrado: %d \n", thread, menor_por_thread);

        #pragma omp critical
        if(menor_por_thread < menor_global) {
            menor_global = menor_por_thread;
        }
    }

    printf("\n\n\n Menor elemento global: %d \n\n\n", menor_global);
    free(vetor);
    return 0;
}

/*
/   #pragma omp for: 
/       Divide o loop entre as threads, assim cada thread processa uma parte do vetor 
/       apenas e atualiza sua cópia da variável menor_por_thread.
/   
/   #pragma omp critical:
/       Garante que apenas uma thread por vez atualize a variável 'menor_global'.
*/ 
