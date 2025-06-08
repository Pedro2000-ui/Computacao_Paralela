#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 4

// Aloca matriz NxN
int** alocarMatriz() {
    int** m = malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++)
        m[i] = calloc(N, sizeof(int));
    return m;
}

void imprimirMatriz(int** M) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            printf("%d ", M[i][j]);
        printf("\n");
    }
}

void somar(int** A, int** B, int** C, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] + B[i][j];
}

void subtrair(int** A, int** B, int** C, int n) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] - B[i][j];
}

void multiplicarDC(int** A, int** B, int** C, int n) {
    if (n == 1) {
        C[0][0] = A[0][0] * B[0][0];
        return;
    }
    int novoN = n / 2;
    int** A11 = alocarMatriz(); int** A12 = alocarMatriz();
    int** A21 = alocarMatriz(); int** A22 = alocarMatriz();
    int** B11 = alocarMatriz(); int** B12 = alocarMatriz();
    int** B21 = alocarMatriz(); int** B22 = alocarMatriz();
    int** C11 = alocarMatriz(); int** C12 = alocarMatriz();
    int** C21 = alocarMatriz(); int** C22 = alocarMatriz();
    int** t1 = alocarMatriz(); int** t2 = alocarMatriz();

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < novoN; i++)
        for (int j = 0; j < novoN; j++) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + novoN];
            A21[i][j] = A[i + novoN][j];
            A22[i][j] = A[i + novoN][j + novoN];
            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + novoN];
            B21[i][j] = B[i + novoN][j];
            B22[i][j] = B[i + novoN][j + novoN];
        }

    multiplicarDC(A11, B11, t1, novoN);
    multiplicarDC(A12, B21, t2, novoN);
    somar(t1, t2, C11, novoN);

    multiplicarDC(A11, B12, t1, novoN);
    multiplicarDC(A12, B22, t2, novoN);
    somar(t1, t2, C12, novoN);

    multiplicarDC(A21, B11, t1, novoN);
    multiplicarDC(A22, B21, t2, novoN);
    somar(t1, t2, C21, novoN);

    multiplicarDC(A21, B12, t1, novoN);
    multiplicarDC(A22, B22, t2, novoN);
    somar(t1, t2, C22, novoN);

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < novoN; i++)
        for (int j = 0; j < novoN; j++) {
            C[i][j] = C11[i][j];
            C[i][j + novoN] = C12[i][j];
            C[i + novoN][j] = C21[i][j];
            C[i + novoN][j + novoN] = C22[i][j];
        }
}

void merge(int* arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    int* L = malloc(n1 * sizeof(int));
    int* R = malloc(n2 * sizeof(int));
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int i = 0; i < n2; i++) R[i] = arr[m + 1 + i];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    free(L); free(R);
}

void mergeSort(int* arr, int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        #pragma omp parallel sections
        {
            #pragma omp section
            mergeSort(arr, l, m);
            #pragma omp section
            mergeSort(arr, m + 1, r);
        }
        merge(arr, l, m, r);
    }
}

int buscaBinaria(int* arr, int n, int x) {
    int esq = 0, dir = n - 1;
    while (esq <= dir) {
        int meio = (esq + dir) / 2;
        if (arr[meio] == x) return meio;
        else if (arr[meio] < x) esq = meio + 1;
        else dir = meio - 1;
    }
    return -1;
}

int main() {
    int** A = alocarMatriz();
    int** B = alocarMatriz();
    int** C = alocarMatriz();

    // Preenche A e B
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }

    printf("Matriz A:\n"); imprimirMatriz(A);
    printf("Matriz B:\n"); imprimirMatriz(B);

    multiplicarDC(A, B, C, N);
    printf("\nMultiplicacao por Divisao e Conquista:\n");
    imprimirMatriz(C);

    int arr[] = {7, 2, 9, 4, 3, 8, 5, 1};
    int n = sizeof(arr) / sizeof(arr[0]);
    mergeSort(arr, 0, n - 1);
    printf("\nVetor ordenado com MergeSort: ");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    printf("\n");

    int x = 4;
    int idx = buscaBinaria(arr, n, x);
    printf("Elemento %d encontrado na posicao: %d\n", x, idx);

    return 0;
}
