#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 4  // Tamanho da matriz (N x N)

// Função para preencher a matriz com valores simples (ex: A[i][j] = i + j)
void preencherMatriz(int matriz[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matriz[i][j] = i + j;
}

// Função para imprimir a matriz
void imprimirMatriz(int matriz[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%4d ", matriz[i][j]);
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    int rank, size;

    int A[N][N], B[N][N], C[N][N];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int linhas_por_processo = N / size;

    // Envia as matrizes para todos os processos
    if (rank == 0) {
        preencherMatriz(A);
        preencherMatriz(B);

        printf("Matriz A:\n");
        imprimirMatriz(A);

        printf("Matriz B:\n");
        imprimirMatriz(B);
    }

    // Cada processo vai receber suas linhas de A
    int subA[linhas_por_processo][N];
    int subC[linhas_por_processo][N];

    MPI_Scatter(A, linhas_por_processo * N, MPI_INT,
                subA, linhas_por_processo * N, MPI_INT,
                0, MPI_COMM_WORLD);

    // Envia a matriz B para todos os processos
    MPI_Bcast(B, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada processo calcula sua parte de C
    for (int i = 0; i < linhas_por_processo; i++) {
        for (int j = 0; j < N; j++) {
            subC[i][j] = 0;
            for (int k = 0; k < N; k++)
                subC[i][j] += subA[i][k] * B[k][j];
        }
    }

    // Junta o resultado em C
    MPI_Gather(subC, linhas_por_processo * N, MPI_INT,
               C, linhas_por_processo * N, MPI_INT,
               0, MPI_COMM_WORLD);

    // Processo mestre imprime a matriz resultado
    if (rank == 0) {
        printf("Matriz C (A x B):\n");
        imprimirMatriz(C);
    }

    MPI_Finalize();
    return 0;
}
