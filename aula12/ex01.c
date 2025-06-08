#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Prefix Sum paralelo no modelo BSP/CGM
// vetor: input/output array com n elementos (n deve ser potência de 2)
// n: tamanho do vetor
void prefix_sum_bsp(int *vetor, int n) {
    int d, i;

    // Upsweep (redução) - calcular soma parcial
    for (d = 1; d < n; d <<= 1) {
        #pragma omp parallel for private(i)
        for (i = 0; i < n; i += 2 * d) {
            vetor[i + 2 * d - 1] += vetor[i + d - 1];
        }
    }

    // Zera o último elemento para preparação do downsweep
    vetor[n - 1] = 0;

    // Downsweep - distribuir valores prefixos
    for (d = n >> 1; d >= 1; d >>= 1) {
        #pragma omp parallel for private(i)
        for (i = 0; i < n; i += 2 * d) {
            int t = vetor[i + d - 1];
            vetor[i + d - 1] = vetor[i + 2 * d - 1];
            vetor[i + 2 * d - 1] += t;
        }
    }
}

int main() {
    int n = 16;  // tamanho (potência de 2)
    int *vetor = malloc(n * sizeof(int));

    // Inicializa vetor com valores de 1 a n
    for (int i = 0; i < n; i++) {
        vetor[i] = i + 1;
    }

    printf("Vetor original:\n");
    for (int i = 0; i < n; i++) printf("%d ", vetor[i]);
    printf("\n");

    prefix_sum_bsp(vetor, n);

    printf("Prefix sum paralelo (BSP/CGM):\n");
    for (int i = 0; i < n; i++) printf("%d ", vetor[i]);
    printf("\n");

    free(vetor);
    return 0;
}
