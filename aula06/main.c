#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define TAMANHO 10000000 
#define REPETICOES 10


void merge(int *v, int esquerda, int meio, int direita) {
    int n1 = meio - esquerda + 1;
    int n2 = direita - meio;

    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = v[esquerda + i];
    for (int i = 0; i < n2; i++) R[i] = v[meio + 1 + i];

    int i = 0, j = 0, k = esquerda;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) v[k++] = L[i++];
        else v[k++] = R[j++];
    }

    while (i < n1) v[k++] = L[i++];
    while (j < n2) v[k++] = R[j++];

    free(L);
    free(R);
}

// -------------------- RECURSIVO --------------------

void mergesort_rec(int *v, int esquerda, int direita) {
    if (esquerda < direita) {
        int meio = (esquerda + direita) / 2;
        mergesort_rec(v, esquerda, meio);
        mergesort_rec(v, meio + 1, direita);
        merge(v, esquerda, meio, direita);
    }
}

void mergesort_recursivo(int *v, int n) {
    mergesort_rec(v, 0, n - 1);
}

// -------------------- ITERATIVO --------------------

void mergesort_iterativo(int *v, int n) {
    for (int tam = 1; tam < n; tam *= 2) {
        for (int esquerda = 0; esquerda < n - 1; esquerda += 2 * tam) {
            int meio = esquerda + tam - 1;
            int direita = (esquerda + 2 * tam - 1 < n - 1) ? esquerda + 2 * tam - 1 : n - 1;

            if (meio < direita) {
                merge(v, esquerda, meio, direita);
            }
        }
    }
}

// -------------------- RECURSIVO PARALELO --------------------

void mergesort_paralelo(int *v, int esquerda, int direita, int profundidade) {
    if (esquerda < direita) {
        int meio = (esquerda + direita) / 2;

        if (profundidade <= 0) {
            mergesort_rec(v, esquerda, meio);
            mergesort_rec(v, meio + 1, direita);
        } else {
            #pragma omp parallel sections
            {
                #pragma omp section
                mergesort_paralelo(v, esquerda, meio, profundidade - 1);

                #pragma omp section
                mergesort_paralelo(v, meio + 1, direita, profundidade - 1);
            }
        }
        merge(v, esquerda, meio, direita);
    }
}

void mergesort_rec_paralelo(int *v, int n) {
    int profundidade = 4; // Para dividir bem entre threads
    mergesort_paralelo(v, 0, n - 1, profundidade);
}

// -------------------- FERRAMENTAS --------------------

void preencher_vetor(int *v, int n) {
    for (int i = 0; i < n; i++) {
        v[i] = rand();
    }
}

void copiar_vetor(int *dest, int *orig, int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = orig[i];
    }
}

double medir_tempo(void (*func)(int*, int), int *vetor_base, int n) {
    double total = 0.0;
    int *copia = malloc(n * sizeof(int));

    for (int i = 0; i < REPETICOES; i++) {
        copiar_vetor(copia, vetor_base, n);
        double inicio = omp_get_wtime();
        func(copia, n);
        double fim = omp_get_wtime();
        total += (fim - inicio);
    }

    free(copia);
    return total / REPETICOES;
}

// Para chamada da versão paralela (recebe ponteiro para função compatível)
void chamar_paralelo(int *v, int n) {
    mergesort_rec_paralelo(v, n);
}

// -------------------- MAIN --------------------

int main() {
    int *original = malloc(TAMANHO * sizeof(int));
    int *vetor = malloc(TAMANHO * sizeof(int));
    srand(time(NULL));
    preencher_vetor(original, TAMANHO);

    printf("Comparando MergeSort Recursivo, Iterativo e Paralelo (%d elementos, média de %d execuções):\n\n", TAMANHO, REPETICOES);

    double tempo_rec = medir_tempo(mergesort_recursivo, original, TAMANHO);
    printf("Recursivo Sequencial: %.4f segundos\n", tempo_rec);

    double tempo_it = medir_tempo(mergesort_iterativo, original, TAMANHO);
    printf("Iterativo Sequencial: %.4f segundos\n", tempo_it);

    for (int t = 2; t <= 16; t *= 2) {
        omp_set_num_threads(t);
        double tempo_par = medir_tempo(chamar_paralelo, original, TAMANHO);
        printf("Paralelo (%2d threads):   %.4f segundos\n", t, tempo_par);
    }

    free(original);
    free(vetor);
    return 0;
}
