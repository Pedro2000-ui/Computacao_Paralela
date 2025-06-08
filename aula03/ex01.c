#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int max_sequencial(int *v, int n) {
    int max = v[0];
    for (int i = 1; i < n; i++) {
        if (v[i] > max)
            max = v[i];
    }
    return max;
}

int max_paralelo_arvore(int *v, int n) {
    int step;
    for (step = 1; step < n; step *= 2) {
        #pragma omp parallel for
        for (int i = 0; i + step < n; i += 2 * step) {
            if (v[i] < v[i + step])
                v[i] = v[i + step];
        }
    }
    return v[0];
}

int max_paralelo_reducao_manual(int *v, int n) {
    int *temp = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) temp[i] = v[i];

    while (n > 1) {
        int new_n = n / 2;
        #pragma omp parallel for
        for (int i = 0; i < new_n; i++) {
            temp[i] = (temp[2 * i] > temp[2 * i + 1]) ? temp[2 * i] : temp[2 * i + 1];
        }
        if (n % 2 == 1) temp[new_n++] = temp[n - 1];
        n = new_n;
    }

    int max = temp[0];
    free(temp);
    return max;
}

int max_paralelo_brent(int *v, int n) {
    int num_threads;
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }

    int *local_max = malloc(num_threads * sizeof(int));

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int chunk = n / num_threads;
        int start = tid * chunk;
        int end = (tid == num_threads - 1) ? n : start + chunk;

        int local = v[start];
        for (int i = start + 1; i < end; i++) {
            if (v[i] > local)
                local = v[i];
        }
        local_max[tid] = local;
    }

    // Redução sequencial final
    int max = local_max[0];
    for (int i = 1; i < num_threads; i++) {
        if (local_max[i] > max)
            max = local_max[i];
    }

    free(local_max);
    return max;
}

int main() {
    int n = 16;
    int v[] = {1, 5, 8, 3, 9, 2, 10, 7, 6, 4, 12, 11, 14, 0, 13, 15};

    printf("Máximo Sequencial: %d\n", max_sequencial(v, n));
    printf("Máximo Paralelo Árvore: %d\n", max_paralelo_arvore(v, n));
    printf("Máximo Paralelo Redução Manual: %d\n", max_paralelo_reducao_manual(v, n));
    printf("Máximo Paralelo Brent: %d\n", max_paralelo_brent(v, n));

    return 0;
}
