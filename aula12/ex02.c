#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Merge sequencial
void merge(int *arr, int *temp, int left, int mid, int right) {
    int i = left, j = mid, k = left;
    while (i < mid && j < right) {
        if (arr[i] <= arr[j]) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i < mid) temp[k++] = arr[i++];
    while (j < right) temp[k++] = arr[j++];

    for (i = left; i < right; i++) {
        arr[i] = temp[i];
    }
}

// Split sort recursivo paralelo
void split_sort(int *arr, int *temp, int left, int right, int threshold) {
    int n = right - left;
    if (n <= threshold) {
        // Ordena sequencialmente (qsort)
        qsort(arr + left, n, sizeof(int), (int (*)(const void*,const void*)) strcmp);
        return;
    }

    int mid = left + n / 2;

    #pragma omp task shared(arr, temp)
    split_sort(arr, temp, left, mid, threshold);

    #pragma omp task shared(arr, temp)
    split_sort(arr, temp, mid, right, threshold);

    #pragma omp taskwait

    merge(arr, temp, left, mid, right);
}

int main() {
    int n = 1 << 16;  // tamanho do vetor (potência de 2)
    int threshold = 1024;  // limite para ordenar sequencialmente

    int *arr = malloc(n * sizeof(int));
    int *temp = malloc(n * sizeof(int));

    // Preenche vetor com valores aleatórios
    srand(42);
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100000;
    }

    printf("Iniciando split sort paralelo...\n");

    double start = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        split_sort(arr, temp, 0, n, threshold);
    }

    double end = omp_get_wtime();

    printf("Tempo paralelo: %f segundos\n", end - start);

    // Verifica se ordenado
    int sorted = 1;
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) {
            sorted = 0;
            break;
        }
    }
    printf("Vetor ordenado? %s\n", sorted ? "Sim" : "Não");

    free(arr);
    free(temp);
    return 0;
}
