# Computação Paralela

Repositório com exemplos e exercícios da disciplina de **Computação Paralela**.

## 📁 Conteúdo

- Códigos em C com OpenMP e MPI
- Algoritmos paralelos (ex: máximo de vetor, soma, etc.)
- Comparações entre versões sequenciais e paralelas

## 🚀 Tecnologias

- C
- OpenMP
- MPI
- GCC / mpicc

## ▶️ Como rodar

### OpenMP

```bash
gcc -fopenmp nome_do_arquivo.c -o programa
./programa
```

### MPI
```bash
mpicc nome_do_arquivo.c -o programa
mpirun -np 4 ./programa
```
