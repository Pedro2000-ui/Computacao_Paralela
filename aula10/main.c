#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAXN 1000
#define MAX_CHILD 100

typedef struct {
    int children[MAX_CHILD];
    int child_count;
} Node;

Node tree[MAXN];
int n;

// Adiciona filho mantendo ordem
void addEdge(int u, int v) {
    tree[u].children[tree[u].child_count++] = v;
    tree[v].children[tree[v].child_count++] = u;
}

// Contar tamanho do Euler tour da subárvore em u
int eulerSize(int u, int parent) {
    int size = 1; // visita o nó u
    for (int i = 0; i < tree[u].child_count; i++) {
        int v = tree[u].children[i];
        if (v != parent) {
            size += eulerSize(v, u) + 1; // filho + retorno ao u
        }
    }
    return size;
}

// Euler Tour paralelo
void eulerTourParallel(int u, int parent, int* euler, int* pos) {
    int idx = pos[0];
    euler[idx] = u;
    pos[0]++;

    int child_count = 0;
    int children[MAX_CHILD];

    // Filhos exceto o pai
    for (int i = 0; i < tree[u].child_count; i++) {
        int v = tree[u].children[i];
        if (v != parent) {
            children[child_count++] = v;
        }
    }

    int* child_sizes = malloc(child_count * sizeof(int));

    // Conta os tamanhos em paralelo
    #pragma omp parallel for
    for (int i = 0; i < child_count; i++) {
        child_sizes[i] = eulerSize(children[i], u);
    }

    int offset = pos[0];

    // Processa filhos em paralelo
    #pragma omp parallel for
    for (int i = 0; i < child_count; i++) {
        int child_pos = offset;
        for (int j = 0; j < i; j++) {
            child_pos += child_sizes[j] + 1;
        }
        int local_pos = child_pos;
        eulerTourParallel(children[i], u, euler, &local_pos);
        euler[local_pos] = u; // volta para o pai
    }

    pos[0] = offset;
    for (int i = 0; i < child_count; i++) {
        pos[0] += child_sizes[i] + 1;
    }

    free(child_sizes);
}

int main() {
    n = 7;

    for (int i = 0; i < n; i++) {
        tree[i].child_count = 0;
    }

    addEdge(0, 1);
    addEdge(0, 2);
    addEdge(1, 3);
    addEdge(1, 4);
    addEdge(2, 5);
    addEdge(2, 6);

    int size = eulerSize(0, -1);
    int *euler = malloc(size * sizeof(int));

    int pos = 0;
    eulerTourParallel(0, -1, euler, &pos);

    printf("Euler Tour paralelo (ordem natural dos filhos):\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", euler[i]);
    }
    printf("\n");

    free(euler);

    return 0;
}
