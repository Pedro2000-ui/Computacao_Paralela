#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 30

int main () {
    
    int *vetor = (int *)malloc(N * sizeof(int));
    int maior_por_thread, maior_global;
    
    srand(time(NULL));

    printf("Vetor: ");
    for (int i = 0; i < N; i++) {
        vetor[i] = rand() % (N * 5);
        printf("[%d] ", vetor[i]);
    }
    printf("\n\n\n\n");
    maior_global = vetor[0];
    omp_set_num_threads(2);

    #pragma omp parallel private(maior_por_thread)
    {
        maior_por_thread = 0;
        int thread = omp_get_thread_num();
        
        #pragma omp for 
        for(int i = 0; i < N; i++) {
            if(vetor[i] > maior_por_thread) {
                maior_por_thread = vetor[i];
            }
        }
        printf("\n\n Thread %d: \n Maior Elemento encontrado: %d \n", thread, maior_por_thread);

        #pragma omp critical
        if(maior_por_thread > maior_global) {
            maior_global = maior_por_thread;
        }
    }

    printf("\n\n\n Maior elemento global: %d \n\n\n", maior_global);
    free(vetor);
    return 0;
}

/*
/   #pragma omp parallel private(maior_por_thread)
/       Garante que cada thread tenha sua própria cópia da variável maior_por_thread, 
/       que inicialmente tem o valor 0.
/
/   #pragma omp for: 
/       Divide o loop entre as threads, assim cada thread processa uma parte do vetor 
/       apenas e atualiza sua cópia da variável maior_por_thread.
/   
/   #pragma omp critical:
/       Garante que apenas uma thread por vez atualize a variável 'maior_global'.
*/ 
