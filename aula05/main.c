#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void prefix_sum_sequencial(int *v, int *out, int n) {
    out[0] = v[0];
    for (int i = 1; i < n; i++) {
        out[i] = out[i - 1] + v[i];
    }
}

void prefix_sum_ineficiente(int *v, int *out, int n) {
    int i, j;
    #pragma omp parallel for private(j)
    for (i = 0; i < n; i++) {
        int sum = 0;
        for (j = 0; j <= i; j++) {
            sum += v[j];
        }
        out[i] = sum;
    }
}


void prefix_sum_arvore(int *v, int *out, int n) {
    int *temp = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) temp[i] = v[i];

    // Up-Sweep
    for (int d = 1; d < n; d *= 2) {
        #pragma omp parallel for
        for (int i = 0; i < n; i += 2 * d) {
            if (i + 2 * d - 1 < n) {
                temp[i + 2 * d - 1] += temp[i + d - 1];
            }
        }
    }

    // Down-Sweep
    temp[n - 1] = 0;
    for (int d = n / 2; d >= 1; d /= 2) {
        #pragma omp parallel for
        for (int i = 0; i < n; i += 2 * d) {
            if (i + 2 * d - 1 < n) {
                int t = temp[i + d - 1];
                temp[i + d - 1] = temp[i + 2 * d - 1];
                temp[i + 2 * d - 1] += t;
            }
        }
    }

    for (int i = 0; i < n; i++) out[i] = temp[i] + v[i];
    free(temp);
}


void prefix_sum_brent(int *v, int *out, int n) {
    int n_threads;

    #pragma omp parallel
    {
        #pragma omp single
        n_threads = omp_get_num_threads();
    }

    int *somas = malloc(n_threads * sizeof(int));

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int start = tid * (n / n_threads);
        int end = (tid == n_threads - 1) ? n : start + (n / n_threads);

        out[start] = v[start];
        for (int i = start + 1; i < end; i++) {
            out[i] = out[i - 1] + v[i];
        }

        somas[tid] = out[end - 1];
    }

    for (int i = 1; i < n_threads; i++) {
        somas[i] += somas[i - 1];
    }

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        if (tid == 0) return;

        int start = tid * (n / n_threads);
        int end = (tid == n_threads - 1) ? n : start + (n / n_threads);

        for (int i = start; i < end; i++) {
            out[i] += somas[tid - 1];
        }
    }

    free(somas);
}


int main() {
    int v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int n = sizeof(v) / sizeof(v[0]);
    int out[n];

    prefix_sum_sequencial(v, out, n);
    printf("Sequencial: ");
    for (int i = 0; i < n; i++) printf("%d ", out[i]);
    printf("\n");

    prefix_sum_ineficiente(v, out, n);
    printf("Ingênuo:     ");
    for (int i = 0; i < n; i++) printf("%d ", out[i]);
    printf("\n");

    prefix_sum_arvore(v, out, n);
    printf("Árvore:      ");
    for (int i = 0; i < n; i++) printf("%d ", out[i]);
    printf("\n");

    prefix_sum_brent(v, out, n);
    printf("Brent:       ");
    for (int i = 0; i < n; i++) printf("%d ", out[i]);
    printf("\n");

    return 0;
}
