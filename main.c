#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mst.h"

int main(int argc, char *argv[])
{
    Edge sparse[SPARSE_E], dense[DENSE_E];
    int graph[V][V];
    unsigned seed = (unsigned)time(NULL);
    if (argc > 1)
        seed = (unsigned)atoi(argv[1]);
    srand(seed);
    generate_graph(sparse, SPARSE_E);
    generate_graph(dense, DENSE_E);
    build_matrix(graph, sparse, SPARSE_E);
    if (!is_connected(graph)) return 1;
    build_matrix(graph, dense, DENSE_E);
    if (!is_connected(graph)) return 1;
    printf("seed = %u, REPS = %d (Execution Time = average per run)\n\n", seed, REPS);
    int sk = run_kruskal(1, "Sparse Graph - Adjacency List - Kruskal", sparse, SPARSE_E);
    int dk = run_kruskal(2, "Dense Graph - Adjacency List - Kruskal", dense, DENSE_E);
    int sp = run_prim(3, "Sparse Graph - Adjacency Matrix - Prim", sparse, SPARSE_E);
    int dp = run_prim(4, "Dense Graph - Adjacency Matrix - Prim", dense, DENSE_E);
    if (sk != sp || dk != dp) {
        fprintf(stderr, "MST costs do not match.\n");
        return 1;
    }
    return 0;
}
