#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

#define N 10000000

// -------------------- Soma Sequencial --------------------
long long sum_sequential(int *array, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++)
        sum += array[i];
    return sum;
}

// -------------------- Soma com OpenMP --------------------
long long sum_openmp(int *array, int n) {
    long long sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++)
        sum += array[i];
    return sum;
}

// -------------------- Soma com MPI --------------------
long long sum_mpi(int *array, int n) {
    int rank, size;
    long long local_sum = 0, global_sum = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_n = n / size;
    int *local_array = malloc(local_n * sizeof(int));
    MPI_Scatter(array, local_n, MPI_INT, local_array, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_n; i++)
        local_sum += local_array[i];

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    free(local_array);

    return global_sum;
}

int main(int argc, char **argv) {
    int *array = NULL;
    long long result;
    double start, end;

    // Inicialização MPI
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        array = malloc(N * sizeof(int));
        for (int i = 0; i < N; i++)
            array[i] = 1;
    }

    // SOMA SEQUENCIAL (apenas no rank 0)
    if (rank == 0) {
        start = omp_get_wtime();
        result = sum_sequential(array, N);
        end = omp_get_wtime();
        printf("Soma Sequencial: %lld | Tempo: %.4f s\n", result, end - start);
    }

    // SOMA COM OpenMP (apenas no rank 0)
    if (rank == 0) {
        start = omp_get_wtime();
        result = sum_openmp(array, N);
        end = omp_get_wtime();
        printf("Soma OpenMP:     %lld | Tempo: %.4f s\n", result, end - start);
    }

    // SOMA COM MPI (todos os ranks participam)
    start = omp_get_wtime();
    result = sum_mpi(array, N);
    end = omp_get_wtime();
    if (rank == 0)
        printf("Soma MPI:        %lld | Tempo: %.4f s\n", result, end - start);

    if (rank == 0)
        free(array);

    MPI_Finalize();
    return 0;
}
