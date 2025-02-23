#include <stdio.h>
#include <omp.h>

void dobro(int *x) {
    *x = 2 * (*x);
}

void map(int vetor[], int tamanho, void (*funcao)(int*)) {
    #pragma omp parallel for
    for (int i = 0; i < tamanho; i++) {
        funcao(&vetor[i]);
    }
}

int main() {
    int vetor[] = {1, 2, 3, 4, 5};
    int tamanho = 5;

    map(vetor, tamanho, dobro);

    for (int i = 0; i < tamanho; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");

    return 0;
}