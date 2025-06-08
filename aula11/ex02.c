#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int local_sum = 0, total_sum = 0;
    int vetor_global[8] = {1, 2, 3, 4, 5, 6, 7, 8};  // Exemplo

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int elementos_por_processo = 8 / size;
    int *subvetor = (int *)malloc(elementos_por_processo * sizeof(int));

    // Distribui partes do vetor para cada processo
    MPI_Scatter(vetor_global, elementos_por_processo, MPI_INT,
                subvetor, elementos_por_processo, MPI_INT,
                0, MPI_COMM_WORLD);

    // Cada processo calcula sua soma local
    for (int i = 0; i < elementos_por_processo; i++) {
        local_sum += subvetor[i];
    }

    // Topologia anel: soma circula de processo em processo
    int recv_sum;
    if (rank == 0) {
        total_sum = local_sum;
        MPI_Send(&total_sum, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&total_sum, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Soma total = %d\n", total_sum);
    } else {
        MPI_Recv(&recv_sum, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        recv_sum += local_sum;
        int destino = (rank + 1) % size;
        MPI_Send(&recv_sum, 1, MPI_INT, destino, 0, MPI_COMM_WORLD);
    }

    free(subvetor);
    MPI_Finalize();
    return 0;
}
