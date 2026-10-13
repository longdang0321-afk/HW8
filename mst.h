#ifndef MST_H
#define MST_H

#include <stddef.h>

#define V 50
#define SPARSE_E 100
#define DENSE_E 600
#define MAX_W 100
#define REPS 1000

typedef struct
{
    int u, v, w;
} Edge;

void generate_graph(Edge edges[], int m);
void build_matrix(int graph[V][V], Edge edges[], int m);
int is_connected(int graph[V][V]);

int run_kruskal(int id, const char *name, Edge edges[], int m);
int run_prim(int id, const char *name, Edge edges[], int m);

#endif
