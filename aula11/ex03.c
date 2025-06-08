#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i*i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int max = 100000;  // limite superior

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Processo 0 pode receber max por argumento
    if (argc > 1) max = atoi(argv[1]);

    // Contador local dos primos encontrados
    int count_local = 0;
    int capacity = 1000;
    int *primos_local = malloc(capacity * sizeof(int));

    // Cada processo testa números com passo "size"
    for (int num = 2 + rank; num <= max; num += size) {
        if (is_prime(num)) {
            if (count_local == capacity) {
                capacity *= 2;
                primos_local = realloc(primos_local, capacity * sizeof(int));
            }
            primos_local[count_local++] = num;
        }
    }

    // Processo 0 precisa receber resultados dos outros processos
    // 1. Envia o tamanho de cada vetor de primos
    int *recv_counts = NULL;
    if (rank == 0) {
        recv_counts = malloc(size * sizeof(int));
    }

    MPI_Gather(&count_local, 1, MPI_INT, recv_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 2. Envia os vetores
    int *displs = NULL;
    int total_count = 0;
    int *primos_all = NULL;

    if (rank == 0) {
        displs = malloc(size * sizeof(int));
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            displs[i] = displs[i-1] + recv_counts[i-1];
        }
        total_count = displs[size-1] + recv_counts[size-1];
        primos_all = malloc(total_count * sizeof(int));
    }

    MPI_Gatherv(primos_local, count_local, MPI_INT,
                primos_all, recv_counts, displs, MPI_INT,
                0, MPI_COMM_WORLD);

    // Processo 0 imprime todos os primos encontrados
    if (rank == 0) {
        printf("Primos encontrados até %d:\n", max);
        for (int i = 0; i < total_count; i++) {
            printf("%d ", primos_all[i]);
        }
        printf("\n");

        free(recv_counts);
        free(displs);
        free(primos_all);
    }

    free(primos_local);
    MPI_Finalize();
    return 0;
}
