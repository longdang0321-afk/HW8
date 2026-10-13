#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "mst.h"

typedef struct Node
{
    int to, w;
    struct Node *next;
} Node;

/* 먼저 49개 간선으로 연결한 후 나머지 간선 추가 */
void generate_graph(Edge edges[], int m)
{
    int used[V][V] = {0};
    int count = 0;
    for (int i = 1; i < V; i++)
    {
        int parent = rand() % i;
        used[i][parent] = used[parent][i] = 1;
        edges[count].u = i;
        edges[count].v = parent;
        edges[count].w = rand() % MAX_W + 1;
        count++;
    }
    while (count < m)
    {
        int u = rand() % V, v = rand() % V;
        if (u == v || used[u][v])
            continue;
        used[u][v] = used[v][u] = 1;
        edges[count].u = u;
        edges[count].v = v;
        edges[count].w = rand() % MAX_W + 1;
        count++;
    }
}

void build_matrix(int graph[V][V], Edge edges[], int m)
{
    memset(graph, 0, sizeof(int) * V * V);
    for (int i = 0; i < m; i++)
    {
        int u = edges[i].u, v = edges[i].v;
        graph[u][v] = graph[v][u] = edges[i].w;
    }
}

/* DFS 연결 확인 */
int is_connected(int graph[V][V])
{
    int visited[V] = {0}, stack[V], top = 0, count = 1;
    stack[top++] = 0;
    visited[0] = 1;
    while (top > 0)
    {
        int u = stack[--top];
        for (int v = 0; v < V; v++)
        {
            if (graph[u][v] && !visited[v])
            {
                visited[v] = 1;
                stack[top++] = v;
                count++;
            }
        }
    }
    return count == V;
}

static void build_list(Node *head[], Node nodes[], Edge edges[], int m)
{
    int count = 0;
    for (int i = 0; i < V; i++)
        head[i] = NULL;
    for (int i = 0; i < m; i++)
    {
        int u = edges[i].u, v = edges[i].v, w = edges[i].w;
        nodes[count].to = v;
        nodes[count].w = w;
        nodes[count].next = head[u];
        head[u] = &nodes[count++];
        nodes[count].to = u;
        nodes[count].w = w;
        nodes[count].next = head[v];
        head[v] = &nodes[count++];
    }
}

static int compare_edges(const void *a, const void *b)
{
    const Edge *first = a, *second = b;
    return first->w - second->w;
}

static int find_root(int parent[], int x)
{
    if (parent[x] != x)
        parent[x] = find_root(parent, parent[x]);
    return parent[x];
}

static int kruskal(Node *head[], Edge all[], Edge mst[], int *count)
{
    int parent[V], rank[V] = {0}, edge_count = 0, total = 0;
    *count = 0;
    for (int u = 0; u < V; u++)
    {
        for (Node *p = head[u]; p != NULL; p = p->next)
        {
            if (u < p->to)
            {
                all[edge_count].u = u;
                all[edge_count].v = p->to;
                all[edge_count].w = p->w;
                edge_count++;
            }
        }
    }
    qsort(all, edge_count, sizeof(Edge), compare_edges);
    for (int i = 0; i < V; i++)
        parent[i] = i;
    for (int i = 0; i < edge_count && *count < V - 1; i++)
    {
        int a = find_root(parent, all[i].u);
        int b = find_root(parent, all[i].v);
        if (a == b)
            continue;
        if (rank[a] < rank[b])
            parent[a] = b;
        else if (rank[a] > rank[b])
            parent[b] = a;
        else
        {
            parent[b] = a;
            rank[a]++;
        }
        mst[(*count)++] = all[i];
        total += all[i].w;
    }
    return total;
}

static int prim(int graph[V][V], Edge mst[], int *count)
{
    int visited[V] = {0}, key[V], parent[V], total = 0;
    *count = 0;
    for (int i = 0; i < V; i++)
    {
        key[i] = INT_MAX;
        parent[i] = -1;
    }
    key[0] = 0;
    for (int step = 0; step < V; step++)
    {
        int u = -1, minimum = INT_MAX;
        for (int i = 0; i < V; i++)
        {
            if (!visited[i] && key[i] < minimum)
            {
                minimum = key[i];
                u = i;
            }
        }
        if (u == -1)
            return -1;
        visited[u] = 1;
        if (parent[u] != -1)
        {
            mst[*count].u = parent[u];
            mst[*count].v = u;
            mst[*count].w = key[u];
            (*count)++;
            total += key[u];
        }
        for (int v = 0; v < V; v++)
        {
            if (graph[u][v] && !visited[v] && graph[u][v] < key[v])
            {
                key[v] = graph[u][v];
                parent[v] = u;
            }
        }
    }
    return total;
}

static void print_result(int id, const char *name, int m, Edge mst[],
                         int count, int cost, size_t memory, double elapsed)
{
    printf("Case %d : %s\n", id, name);
    printf("Vertices      : %d\nEdges         : %d\n", V, m);
    printf("MST Edges     : %d\nMST Cost      : %d\n", count, cost);
    printf("Memory        : %zu Bytes\nExecution Time: %.6f ms\n", memory, elapsed);
    printf("MST Edge List :\n");
    for (int i = 0; i < count; i++)
    {
        printf("  (%2d - %2d : %3d)", mst[i].u, mst[i].v, mst[i].w);
        if (i % 4 == 3 || i == count - 1)
            printf("\n");
    }
    printf("\n");
}

int run_kruskal(int id, const char *name, Edge edges[], int m)
{
    Node *head[V], *nodes = malloc(2 * m * sizeof(Node));
    Edge *all = malloc(m * sizeof(Edge)), mst[V - 1];
    int count = 0, cost = 0;
    if (nodes == NULL || all == NULL)
    {
        fprintf(stderr, "Memory allocation failed.\n");
        free(nodes);
        free(all);
        exit(EXIT_FAILURE);
    }
    build_list(head, nodes, edges, m);
    clock_t start = clock();
    for (int i = 0; i < REPS; i++)
        cost = kruskal(head, all, mst, &count);
    clock_t end = clock();
    double elapsed = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / REPS;
    size_t memory = V * sizeof(Node *) + 2 * (size_t)m * sizeof(Node) + (size_t)m * sizeof(Edge) + 2 * V * sizeof(int);
    if (count != V - 1)
    {
        fprintf(stderr, "Invalid MST edge count.\n");
        free(nodes);
        free(all);
        exit(EXIT_FAILURE);
    }
    print_result(id, name, m, mst, count, cost, memory, elapsed);
    free(nodes);
    free(all);
    return cost;
}

int run_prim(int id, const char *name, Edge edges[], int m)
{
    int graph[V][V], count = 0, cost = 0;
    Edge mst[V - 1];
    build_matrix(graph, edges, m);
    clock_t start = clock();
    for (int i = 0; i < REPS; i++)
        cost = prim(graph, mst, &count);
    clock_t end = clock();
    double elapsed = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / REPS;
    size_t memory = sizeof(graph) + 3 * V * sizeof(int);
    if (cost == -1 || count != V - 1)
    {
        fprintf(stderr, "Invalid MST result.\n");
        exit(EXIT_FAILURE);
    }
    print_result(id, name, m, mst, count, cost, memory, elapsed);
    return cost;
}
