#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Função de comparação para inteiros
int cmp_int(const void *a, const void *b) {
    int x = *(int *)a;
    int y = *(int *)b;
    return (x > y) - (x < y);  // ou: return x - y;
}

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

// Split sort paralelo
void split_sort(int *arr, int *temp, int left, int right, int threshold) {
    int n = right - left;
    if (n <= threshold) {
        qsort(arr + left, n, sizeof(int), cmp_int);
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
    int n = 1 << 16;  // 65536 elementos
    int threshold = 1024;  // abaixo disso, ordena sequencialmente

    int *arr = malloc(n * sizeof(int));
    int *temp = malloc(n * sizeof(int));

    // Preenche vetor com valores aleatórios
    srand(42);
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100000;
    }

    printf("Iniciando Split Sort paralelo...\n");

    double start = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        split_sort(arr, temp, 0, n, threshold);
    }

    double end = omp_get_wtime();

    printf("Tempo paralelo: %.6f segundos\n", end - start);

    // Verifica se está ordenado corretamente
    int sorted = 1;
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i - 1]) {
            sorted = 0;
            break;
        }
    }

    printf("Vetor ordenado? %s\n", sorted ? "Sim" : "Não");

    free(arr);
    free(temp);
    return 0;
}
