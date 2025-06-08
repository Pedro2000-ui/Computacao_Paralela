#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 16

// --------------------------- FILTER (pares) ---------------------------
void filter(int *input, int *output, int n, int *out_size) {
    int i, count = 0;
    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        if (input[i] % 2 == 0) {
            int idx;
            #pragma omp atomic capture
            idx = count++;
            output[idx] = input[i];
        }
    }
    *out_size = count;
}

// --------------------------- BROADCAST ---------------------------
void broadcast(int *array, int n, int value) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        array[i] = value;
    }
}

// --------------------------- COMPACT (remove zeros) ---------------------------
void compact(int *input, int *output, int n, int *out_size) {
    int count = 0;
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        if (input[i] != 0) {
            int idx;
            #pragma omp atomic capture
            idx = count++;
            output[idx] = input[i];
        }
    }
    *out_size = count;
}

// --------------------------- PARTITION ---------------------------
int partition(int *arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
        }
    }
    int tmp = arr[i+1];
    arr[i+1] = arr[high];
    arr[high] = tmp;
    return i + 1;
}

// --------------------------- PARALLEL QUICKSORT ---------------------------
void quicksort_parallel(int *arr, int low, int high, int depth) {
    if (low < high) {
        int pi = partition(arr, low, high);

        if (depth > 0) {
            #pragma omp parallel sections
            {
                #pragma omp section
                quicksort_parallel(arr, low, pi - 1, depth - 1);
                #pragma omp section
                quicksort_parallel(arr, pi + 1, high, depth - 1);
            }
        } else {
            quicksort_parallel(arr, low, pi - 1, 0);
            quicksort_parallel(arr, pi + 1, high, 0);
        }
    }
}

// --------------------------- MAIN ---------------------------
int main() {
    omp_set_num_threads(4);
    int input[N] = {1, 0, 4, 3, 0, 6, 0, 8, 10, 13, 0, 12, 0, 0, 15, 16};
    int output[N];
    int size;

    printf("=== FILTER ===\n");
    filter(input, output, N, &size);
    for (int i = 0; i < size; i++) printf("%d ", output[i]);
    printf("\n");

    printf("=== BROADCAST ===\n");
    broadcast(output, N, 99);
    for (int i = 0; i < N; i++) printf("%d ", output[i]);
    printf("\n");

    printf("=== COMPACT ===\n");
    compact(input, output, N, &size);
    for (int i = 0; i < size; i++) printf("%d ", output[i]);
    printf("\n");

    printf("=== PARALLEL QUICKSORT ===\n");
    int sort_arr[N] = {5, 2, 9, 1, 3, 7, 6, 8, 4, 0, 10, 11, 13, 12, 14, 15};
    quicksort_parallel(sort_arr, 0, N - 1, 4);
    for (int i = 0; i < N; i++) printf("%d ", sort_arr[i]);
    printf("\n");

    return 0;
}
