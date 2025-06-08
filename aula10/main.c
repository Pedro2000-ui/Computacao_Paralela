#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAXN 1000

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

Node* adj[MAXN];
int n; // número de nós

// Função para contar tamanho do Euler tour da subárvore em u
int eulerSize(int u, int parent) {
    int size = 1; // visita o nó u
    for (Node* cur = adj[u]; cur != NULL; cur = cur->next) {
        int v = cur->vertex;
        if (v != parent) {
            size += eulerSize(v, u) + 1; // filho + retorno ao u
        }
    }
    return size;
}

// Função paralela para construir Euler Tour
void eulerTourParallel(int u, int parent, int* euler, int* pos) {
    int idx = pos[0];   // posição inicial para escrever u
    euler[idx] = u;
    pos[0]++;

    Node* cur = adj[u];
    // Criar array para armazenar tamanho de cada subárvore dos filhos
    int child_count = 0;
    int children[100];    // até 100 filhos

    while(cur) {
        int v = cur->vertex;
        if(v != parent) {
            children[child_count++] = v;
        }
        cur = cur->next;
    }

    int* child_sizes = malloc(child_count * sizeof(int));
    
    // Conta os tamanhos em paralelo
    #pragma omp parallel for
    for (int i = 0; i < child_count; i++) {
        child_sizes[i] = eulerSize(children[i], u);
    }

    int offset = pos[0];
    // Processar filhos em paralelo
    #pragma omp parallel for
    for (int i = 0; i < child_count; i++) {
        int child_pos = offset;
        // Calcular deslocamento
        for (int j = 0; j < i; j++) {
            child_pos += child_sizes[j] + 1;  // +1 pelo retorno ao pai
        }
        int local_pos = child_pos;
        eulerTourParallel(children[i], u, euler, &local_pos);
        // Voltar ao pai após visitar filho
        euler[local_pos] = u;
    }

    pos[0] = offset;
    for (int i = 0; i < child_count; i++) {
        pos[0] += child_sizes[i] + 1;
    }

    free(child_sizes);
}

void addEdge(int u, int v) {
    Node* node = malloc(sizeof(Node));
    node->vertex = v;
    node->next = adj[u];
    adj[u] = node;

    node = malloc(sizeof(Node));
    node->vertex = u;
    node->next = adj[v];
    adj[v] = node;
}

int main() {
    n = 7;

    for (int i = 0; i < n; i++) {
        adj[i] = NULL;
    }

    addEdge(0,1);
    addEdge(0,2);
    addEdge(1,3);
    addEdge(1,4);
    addEdge(2,5);
    addEdge(2,6);

    int size = eulerSize(0, -1);
    int *euler = malloc(size * sizeof(int));

    int pos = 0;
    eulerTourParallel(0, -1, euler, &pos);

    printf("Euler Tour paralelo:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", euler[i]);
    }
    printf("\n");

    free(euler);

    return 0;
}
