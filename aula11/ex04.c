#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ITER 10000
#define TOL 1e-6

// Inicializa a matriz u com zeros, exceto bordas com condição de contorno fixa
void initialize(double *u, int N, int local_rows, int rank, int size) {
    // Define bordas da matriz global (só processo 0 e size-1 tem bordas "fixas")
    for (int i = 0; i < local_rows + 2; i++) {
        for (int j = 0; j < N; j++) {
            u[i * N + j] = 0.0;
        }
    }

    // Borda superior para processo 0
    if (rank == 0) {
        for (int j = 0; j < N; j++) {
            u[j] = 100.0; // topo da matriz
        }
    }
    // Borda inferior para processo size-1
    if (rank == size - 1) {
        for (int j = 0; j < N; j++) {
            u[(local_rows + 1) * N + j] = 0.0; // fundo da matriz
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int N = 100; // tamanho da matriz NxN (pode ser parâmetro)
    double *u, *u_new;
    int local_rows;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc > 1) N = atoi(argv[1]);

    // Divide linhas igualmente (ignorar resto)
    local_rows = N / size;

    // Aloca com linhas extras para linhas fantasmas (topo e baixo)
    u = malloc(sizeof(double) * (local_rows + 2) * N);
    u_new = malloc(sizeof(double) * (local_rows + 2) * N);

    initialize(u, N, local_rows, rank, size);

    int iter = 0;
    double tol = TOL;
    double diff;

    MPI_Request reqs[4];
    MPI_Status stats[4];

    do {
        // Troca linhas fantasmas entre vizinhos

        // Enviar para cima e receber de baixo
        if (rank != 0) {
            MPI_Isend(&u[N], N, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[0]);  // envia 1a linha útil
            MPI_Irecv(&u[0], N, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &reqs[1]);    // recebe linha fantasma de cima
        } else {
            reqs[0] = MPI_REQUEST_NULL;
            reqs[1] = MPI_REQUEST_NULL;
        }

        // Enviar para baixo e receber de cima
        if (rank != size - 1) {
            MPI_Isend(&u[local_rows * N], N, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &reqs[2]); // envia última linha útil
            MPI_Irecv(&u[(local_rows + 1) * N], N, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[3]); // recebe linha fantasma de baixo
        } else {
            reqs[2] = MPI_REQUEST_NULL;
            reqs[3] = MPI_REQUEST_NULL;
        }

        MPI_Waitall(4, reqs, stats);

        diff = 0.0;

        // Atualiza u_new pela fórmula de Jacobi
        for (int i = 1; i <= local_rows; i++) {
            for (int j = 1; j < N-1; j++) {
                u_new[i * N + j] = 0.25 * (u[(i-1)*N + j] + u[(i+1)*N + j] + u[i*N + j - 1] + u[i*N + j + 1]);
                double d = u_new[i*N + j] - u[i*N + j];
                if (d < 0) d = -d;
                if (d > diff) diff = d;
            }
        }

        // Copia u_new em u (exceto linhas fantasmas)
        for (int i = 1; i <= local_rows; i++) {
            for (int j = 1; j < N-1; j++) {
                u[i*N + j] = u_new[i*N + j];
            }
        }

        // Reduz diff global para ver se convergiu
        double global_diff;
        MPI_Allreduce(&diff, &global_diff, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

        iter++;

        if (rank == 0 && iter % 100 == 0) {
            printf("Iteração %d, dif max global = %g\n", iter, global_diff);
        }

        if (global_diff < tol) break;

    } while (iter < MAX_ITER);

    if (rank == 0) {
        printf("Convergência em %d iterações, dif max = %g\n", iter, diff);
    }

    free(u);
    free(u_new);

    MPI_Finalize();
    return 0;
}
