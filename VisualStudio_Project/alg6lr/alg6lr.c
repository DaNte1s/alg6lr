#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <windows.h>

struct Edge {
    int u;
    int v;
};

struct Graph {
    int n;
    int m;
    int** adj;
    struct Edge* edges;
    int* labels;
};

struct Node {
    int val;
    struct Node* next;
};

struct AdjList {
    int n;
    struct Node** head;
};

struct Graph* create_graph(int n) {
    struct Graph* g = (struct Graph*)malloc(sizeof(struct Graph));
    g->n = n;
    g->m = 0;
    g->edges = NULL;
    g->labels = NULL;
    g->adj = NULL;
    if (n > 0) {
        g->labels = (int*)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            g->labels[i] = i + 1;
        }
        g->adj = (int**)malloc(n * sizeof(int*));
        for (int i = 0; i < n; i++) {
            g->adj[i] = (int*)calloc(n, sizeof(int));
        }
    }
    return g;
}

void free_graph(struct Graph* g) {
    if (!g) return;
    if (g->adj) {
        for (int i = 0; i < g->n; i++) {
            free(g->adj[i]);
        }
        free(g->adj);
    }
    if (g->edges) free(g->edges);
    if (g->labels) free(g->labels);
    free(g);
}

void build_edges(struct Graph* g) {
    if (!g || g->n == 0 || !g->adj) return;
    if (g->edges) {
        free(g->edges);
        g->edges = NULL;
    }
    g->m = 0;
    for (int i = 0; i < g->n; i++) {
        for (int j = i; j < g->n; j++) {
            if (g->adj[i][j]) g->m++;
        }
    }
    if (g->m == 0) return;
    g->edges = (struct Edge*)malloc(g->m * sizeof(struct Edge));
    int idx = 0;
    for (int i = 0; i < g->n; i++) {
        for (int j = i; j < g->n; j++) {
            if (g->adj[i][j]) {
                g->edges[idx].u = g->labels[i];
                g->edges[idx].v = g->labels[j];
                idx++;
            }
        }
    }
}

void print_adj(struct Graph* g) {
    if (!g || g->n == 0) {
        printf("Граф пуст.\n");
        return;
    }
    printf("Матрица смежности (%d x %d):\n      ", g->n, g->n);
    for (int j = 0; j < g->n; j++) printf("v%-2d", g->labels[j]);
    printf("\n");
    for (int i = 0; i < g->n; i++) {
        printf("v%-3d ", g->labels[i]);
        for (int j = 0; j < g->n; j++) {
            printf("%-3d", g->adj[i][j]);
        }
        printf("\n");
    }
}

void print_edges(struct Graph* g) {
    if (!g || g->n == 0) {
        printf("Список рёбер: рёбер нет (граф пуст)\n");
        return;
    }
    build_edges(g);
    printf("Список рёбер (m = %d):\n", g->m);
    if (g->m == 0) {
        printf("  Рёбер нет\n");
        return;
    }
    for (int i = 0; i < g->m; i++) {
        if (g->edges[i].u == g->edges[i].v) {
            printf("  e%d: (v%d, v%d) (петля)\n", i + 1, g->edges[i].u, g->edges[i].v);
        }
        else {
            printf("  e%d: (v%d, v%d)\n", i + 1, g->edges[i].u, g->edges[i].v);
        }
    }
}

struct Graph* random_graph(int n, int prob) {
    struct Graph* g = create_graph(n);
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (rand() % 100 < prob) {
                g->adj[i][j] = g->adj[j][i] = 1;
            }
        }
    }
    build_edges(g);
    return g;
}

struct AdjList* create_adj_list(int n) {
    struct AdjList* l = (struct AdjList*)malloc(sizeof(struct AdjList));
    l->n = n;
    l->head = (struct Node**)calloc(n, sizeof(struct Node*));
    return l;
}

void free_adj_list(struct AdjList* l) {
    if (!l) return;
    for (int i = 0; i < l->n; i++) {
        struct Node* curr = l->head[i];
        while (curr) {
            struct Node* tmp = curr;
            curr = curr->next;
            free(tmp);
        }
    }
    free(l->head);
    free(l);
}

void add_list_node(struct AdjList* l, int u, int v) {
    if (u < 1 || u > l->n || v < 1 || v > l->n) return;
    struct Node* curr = l->head[u - 1];
    while (curr) {
        if (curr->val == v) return;
        curr = curr->next;
    }
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->val = v;
    node->next = l->head[u - 1];
    l->head[u - 1] = node;
}

struct AdjList* matrix_to_list(struct Graph* g) {
    if (!g || g->n == 0) return create_adj_list(0);
    struct AdjList* l = create_adj_list(g->n);
    for (int i = 0; i < g->n; i++) {
        for (int j = 0; j < g->n; j++) {
            if (g->adj[i][j]) {
                add_list_node(l, i + 1, j + 1);
            }
        }
    }
    return l;
}

void print_list(struct AdjList* l) {
    if (!l || l->n == 0) {
        printf("Списки смежности: список пуст\n");
        return;
    }
    printf("Списки смежности (вершин: %d):\n", l->n);
    for (int i = 0; i < l->n; i++) {
        printf("  v%d: ", i + 1);
        struct Node* curr = l->head[i];
        if (!curr) {
            printf("нет смежных вершин");
        }
        while (curr) {
            if (curr->val == i + 1) {
                printf("-> v%d(петля) ", curr->val);
            }
            else {
                printf("-> v%d ", curr->val);
            }
            curr = curr->next;
        }
        printf("\n");
    }
}

struct Graph* identify_vertices(struct Graph* g, int u, int v) {
    if (u < 1 || u > g->n || v < 1 || v > g->n || u == v) {
        printf("Некорректные номера вершин!\n");
        return NULL;
    }
    if (u > v) { int t = u; u = v; v = t; }

    int u_idx = u - 1;
    int v_idx = v - 1;
    int had_edge = g->adj[u_idx][v_idx];

    struct Graph* res = create_graph(g->n - 1);
    int* map = (int*)malloc(g->n * sizeof(int));
    int cur = 0;
    for (int i = 0; i < g->n; i++) {
        if (i == v_idx) {
            map[i] = map[u_idx];
        }
        else {
            res->labels[cur] = g->labels[i];
            map[i] = cur++;
        }
    }

    for (int i = 0; i < g->n; i++) {
        for (int j = i + 1; j < g->n; j++) {
            if (g->adj[i][j]) {
                int ni = map[i];
                int nj = map[j];
                if (ni != nj) {
                    res->adj[ni][nj] = res->adj[nj][ni] = 1;
                }
            }
        }
    }

    for (int i = 0; i < g->n; i++) {
        if (g->adj[i][i]) {
            res->adj[map[i]][map[i]] = 1;
        }
    }

    if (had_edge) {
        int new_uv = map[u_idx];
        res->adj[new_uv][new_uv] = 1;
    }

    build_edges(res);
    free(map);
    return res;
}

struct Graph* contract_edge(struct Graph* g, int u, int v) {
    if (u < 1 || u > g->n || v < 1 || v > g->n || u == v) {
        printf("Некорректные номера вершин!\n");
        return NULL;
    }
    if (!g->adj[u - 1][v - 1]) {
        printf("Ребра (v%d, v%d) нет! Стягивание невозможно.\n", u, v);
        return NULL;
    }
    printf("Стягивание ребра (v%d, v%d)...\n", u, v);
    if (u > v) { int t = u; u = v; v = t; }

    int u_idx = u - 1;
    int v_idx = v - 1;

    struct Graph* res = create_graph(g->n - 1);
    int* map = (int*)malloc(g->n * sizeof(int));
    int cur = 0;
    for (int i = 0; i < g->n; i++) {
        if (i == v_idx) {
            map[i] = map[u_idx];
        }
        else {
            res->labels[cur] = g->labels[i];
            map[i] = cur++;
        }
    }

    for (int i = 0; i < g->n; i++) {
        for (int j = i + 1; j < g->n; j++) {
            if (g->adj[i][j]) {
                int ni = map[i];
                int nj = map[j];
                if (ni != nj) {
                    res->adj[ni][nj] = res->adj[nj][ni] = 1;
                }
            }
        }
    }

    for (int i = 0; i < g->n; i++) {
        if (i != u_idx && i != v_idx && g->adj[i][i]) {
            res->adj[map[i]][map[i]] = 1;
        }
    }

    build_edges(res);
    free(map);
    return res;
}

struct Graph* split_vertex(struct Graph* g, int u) {
    if (u < 1 || u > g->n) {
        printf("Некорректный номер вершины!\n");
        return NULL;
    }
    int u_idx = u - 1;
    struct Graph* res = create_graph(g->n + 1);

    for (int i = 0; i < g->n; i++) {
        res->labels[i] = g->labels[i];
    }
    res->labels[g->n] = g->n + 1;

    for (int i = 0; i < g->n; i++) {
        for (int j = 0; j < g->n; j++) {
            res->adj[i][j] = g->adj[i][j];
        }
    }

    int new_v = g->n;
    for (int i = 0; i < g->n; i++) {
        if (g->adj[u_idx][i]) {
            res->adj[new_v][i] = res->adj[i][new_v] = 1;
        }
    }

    res->adj[u_idx][new_v] = res->adj[new_v][u_idx] = 1;

    build_edges(res);
    return res;
}

struct AdjList* list_identify_vertices(struct AdjList* l, int u, int v) {
    if (u < 1 || u > l->n || v < 1 || v > l->n || u == v) return NULL;
    if (u > v) { int t = u; u = v; v = t; }

    int had_edge = 0;
    struct Node* curr = l->head[u - 1];
    while (curr) {
        if (curr->val == v) { had_edge = 1; break; }
        curr = curr->next;
    }

    int* map = (int*)malloc((l->n + 1) * sizeof(int));
    int cur = 1;
    for (int i = 1; i <= l->n; i++) {
        if (i == v) {
            map[i] = map[u];
        }
        else {
            map[i] = cur++;
        }
    }

    struct AdjList* res = create_adj_list(l->n - 1);
    for (int i = 1; i <= l->n; i++) {
        int ni = map[i];
        curr = l->head[i - 1];
        while (curr) {
            int nj = map[curr->val];
            if (ni != nj) {
                add_list_node(res, ni, nj);
            }
            curr = curr->next;
        }
    }

    if (had_edge) {
        int new_uv = map[u];
        add_list_node(res, new_uv, new_uv);
    }

    free(map);
    return res;
}

struct AdjList* list_contract_edge(struct AdjList* l, int u, int v) {
    if (u < 1 || u > l->n || v < 1 || v > l->n || u == v) return NULL;
    int found = 0;
    struct Node* curr = l->head[u - 1];
    while (curr) {
        if (curr->val == v) { found = 1; break; }
        curr = curr->next;
    }
    if (!found) {
        printf("Ребра (v%d, v%d) нет в списке!\n", u, v);
        return NULL;
    }
    if (u > v) { int t = u; u = v; v = t; }

    int* map = (int*)malloc((l->n + 1) * sizeof(int));
    int cur = 1;
    for (int i = 1; i <= l->n; i++) {
        if (i == v) {
            map[i] = map[u];
        }
        else {
            map[i] = cur++;
        }
    }

    struct AdjList* res = create_adj_list(l->n - 1);
    for (int i = 1; i <= l->n; i++) {
        int ni = map[i];
        curr = l->head[i - 1];
        while (curr) {
            int nj = map[curr->val];
            if (ni != nj) {
                add_list_node(res, ni, nj);
            }
            curr = curr->next;
        }
    }

    free(map);
    return res;
}

struct AdjList* list_split_vertex(struct AdjList* l, int u) {
    if (u < 1 || u > l->n) return NULL;
    struct AdjList* res = create_adj_list(l->n + 1);
    int new_v = l->n + 1;

    for (int i = 1; i <= l->n; i++) {
        struct Node* node = l->head[i - 1];
        while (node) {
            add_list_node(res, i, node->val);
            node = node->next;
        }
    }

    struct Node* curr = l->head[u - 1];
    while (curr) {
        int nb = curr->val;
        if (nb != u) {
            add_list_node(res, new_v, nb);
            add_list_node(res, nb, new_v);
        }
        curr = curr->next;
    }

    add_list_node(res, u, new_v);
    add_list_node(res, new_v, u);

    return res;
}

struct Graph* union_graphs(struct Graph* g1, struct Graph* g2) {
    int n = (g1->n > g2->n) ? g1->n : g2->n;
    struct Graph* res = create_graph(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int a = (i < g1->n && j < g1->n) ? g1->adj[i][j] : 0;
            int b = (i < g2->n && j < g2->n) ? g2->adj[i][j] : 0;
            res->adj[i][j] = (a || b) ? 1 : 0;
        }
    }
    build_edges(res);
    return res;
}

struct Graph* intersect_graphs(struct Graph* g1, struct Graph* g2) {
    int n = (g1->n < g2->n) ? g1->n : g2->n;
    struct Graph* res = create_graph(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            res->adj[i][j] = (g1->adj[i][j] && g2->adj[i][j]) ? 1 : 0;
        }
    }
    build_edges(res);
    return res;
}

struct Graph* ring_sum_graphs(struct Graph* g1, struct Graph* g2) {
    int max_n = (g1->n > g2->n) ? g1->n : g2->n;
    int** temp = (int**)malloc(max_n * sizeof(int*));
    int* deg = (int*)calloc(max_n, sizeof(int));
    for (int i = 0; i < max_n; i++) {
        temp[i] = (int*)calloc(max_n, sizeof(int));
    }

    for (int i = 0; i < max_n; i++) {
        for (int j = 0; j < max_n; j++) {
            if (i == j) continue;
            int a = (i < g1->n && j < g1->n) ? g1->adj[i][j] : 0;
            int b = (i < g2->n && j < g2->n) ? g2->adj[i][j] : 0;
            if (a ^ b) {
                temp[i][j] = 1;
                deg[i]++;
            }
        }
    }

    int count = 0;
    int map[100];
    for (int i = 0; i < max_n; i++) {
        if (deg[i] > 0) {
            map[count++] = i;
        }
    }

    if (count == 0) {
        printf("Кольцевая сумма не содержит рёбер (все вершины изолированы).\n");
        for (int i = 0; i < max_n; i++) free(temp[i]);
        free(temp);
        free(deg);
        return create_graph(0);
    }

    printf("Изолированные вершины удалены. Вершины в графе: ");
    for (int i = 0; i < count; i++) {
        printf("v%d ", map[i] + 1);
    }
    printf("\n");

    struct Graph* res = create_graph(count);
    for (int i = 0; i < count; i++) {
        res->labels[i] = map[i] + 1;
    }
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            res->adj[i][j] = temp[map[i]][map[j]];
        }
    }

    build_edges(res);

    for (int i = 0; i < max_n; i++) free(temp[i]);
    free(temp);
    free(deg);
    return res;
}

void cartesian_product(struct Graph* g1, struct Graph* g2) {
    int n1 = g1->n;
    int n2 = g2->n;
    int total = n1 * n2;

    int** prod = (int**)malloc(total * sizeof(int*));
    for (int i = 0; i < total; i++) {
        prod[i] = (int*)calloc(total, sizeof(int));
    }

    for (int i = 0; i < n1; i++) {
        for (int k = 0; k < n2; k++) {
            int r = i * n2 + k;
            for (int j = 0; j < n1; j++) {
                for (int l = 0; l < n2; l++) {
                    int c = j * n2 + l;
                    if (i == j && g2->adj[k][l]) {
                        prod[r][c] = 1;
                    }
                    else if (k == l && g1->adj[i][j]) {
                        prod[r][c] = 1;
                    }
                }
            }
        }
    }

    printf("\nМатрица смежности G1 x G2 (%d x %d):\n        ", total, total);
    for (int i = 0; i < n1; i++) {
        for (int k = 0; k < n2; k++) {
            printf("(%d,%d) ", g1->labels[i], g2->labels[k]);
        }
    }
    printf("\n");

    for (int i = 0; i < n1; i++) {
        for (int k = 0; k < n2; k++) {
            int r = i * n2 + k;
            printf("(%d,%d)   ", g1->labels[i], g2->labels[k]);
            for (int j = 0; j < n1; j++) {
                for (int l = 0; l < n2; l++) {
                    int c = j * n2 + l;
                    printf("%-6d", prod[r][c]);
                }
            }
            printf("\n");
        }
    }

    printf("\nСписок рёбер декартова произведения:\n");
    int e = 1;
    for (int r = 0; r < total; r++) {
        for (int c = r + 1; c < total; c++) {
            if (prod[r][c]) {
                int u1 = g1->labels[r / n2], v1 = g2->labels[r % n2];
                int u2 = g1->labels[c / n2], v2 = g2->labels[c % n2];
                printf("  e%d: (v%d, w%d) - (v%d, w%d)\n", e++, u1, v1, u2, v2);
            }
        }
    }

    for (int i = 0; i < total; i++) free(prod[i]);
    free(prod);
}

int main() {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");
    srand((unsigned int)time(NULL));

    struct Graph* g1 = random_graph(3, 60);
    struct Graph* g2 = random_graph(4, 50);
    struct AdjList* l1 = matrix_to_list(g1);
    struct AdjList* l2 = matrix_to_list(g2);

    int choice;
    while (1) {
        printf("\n=== МЕНЮ ===\n");
        printf("1. Задание 1: Генерация графов G1, G2 (матрицы и списки)\n");
        printf("2. Задание 2: Унарные операции над G1 (матричная форма)\n");
        printf("3. Задание 2*: Унарные операции над G1 (списки смежности)\n");
        printf("4. Задание 3: Бинарные операции над G1 и G2\n");
        printf("5. Задание 4*: Декартово произведение G1 x G2\n");
        printf("6. Показать текущие графы G1 и G2\n");
        printf("0. Выход\n");
        printf("Выбор: ");

        if (scanf("%d", &choice) != 1 || choice == 0) break;

        if (choice == 1) {
            int n1 = 3, p1 = 60, n2 = 4, p2 = 50;
            printf("Вершин в G1: ");
            if (scanf("%d", &n1) != 1 || n1 < 1) n1 = 3;
            printf("Вероятность рёбер G1 (%%): ");
            if (scanf("%d", &p1) != 1) p1 = 60;

            printf("Вершин в G2: ");
            if (scanf("%d", &n2) != 1 || n2 < 1) n2 = 4;
            printf("Вероятность рёбер G2 (%%): ");
            if (scanf("%d", &p2) != 1) p2 = 50;

            free_graph(g1); free_graph(g2);
            free_adj_list(l1); free_adj_list(l2);

            g1 = random_graph(n1, p1);
            g2 = random_graph(n2, p2);
            l1 = matrix_to_list(g1);
            l2 = matrix_to_list(g2);

            printf("\n--- Граф G1 ---\n");
            print_adj(g1);
            print_edges(g1);
            print_list(l1);

            printf("\n--- Граф G2 ---\n");
            print_adj(g2);
            print_edges(g2);
            print_list(l2);
        }
        else if (choice == 2) {
            int op;
            printf("\n1. Отождествление вершин\n2. Стягивание ребра\n3. Расщепление вершины\nВыбор: ");
            if (scanf("%d", &op) != 1) continue;

            struct Graph* res = NULL;
            if (op == 1) {
                int u, v;
                printf("Введите u и v: ");
                if (scanf("%d %d", &u, &v) == 2) res = identify_vertices(g1, u, v);
            }
            else if (op == 2) {
                int u, v;
                printf("Введите u и v: ");
                if (scanf("%d %d", &u, &v) == 2) res = contract_edge(g1, u, v);
            }
            else if (op == 3) {
                int u;
                printf("Введите u: ");
                if (scanf("%d", &u) == 1) res = split_vertex(g1, u);
            }

            if (res) {
                print_adj(res);
                print_edges(res);
                free_graph(g1);
                g1 = res;
                free_adj_list(l1);
                l1 = matrix_to_list(g1);
            }
        }
        else if (choice == 3) {
            int op;
            printf("\n1. Отождествление вершин\n2. Стягивание ребра\n3. Расщепление вершины\nВыбор: ");
            if (scanf("%d", &op) != 1) continue;

            struct AdjList* res = NULL;
            if (op == 1) {
                int u, v;
                printf("Введите u и v: ");
                if (scanf("%d %d", &u, &v) == 2) res = list_identify_vertices(l1, u, v);
            }
            else if (op == 2) {
                int u, v;
                printf("Введите u и v: ");
                if (scanf("%d %d", &u, &v) == 2) res = list_contract_edge(l1, u, v);
            }
            else if (op == 3) {
                int u;
                printf("Введите u: ");
                if (scanf("%d", &u) == 1) res = list_split_vertex(l1, u);
            }

            if (res) {
                print_list(res);
                free_adj_list(l1);
                l1 = res;
            }
        }
        else if (choice == 4) {
            printf("\n--- Объединение G1 и G2 ---\n");
            struct Graph* gu = union_graphs(g1, g2);
            print_adj(gu);
            print_edges(gu);
            free_graph(gu);

            printf("\n--- Пересечение G1 и G2 ---\n");
            struct Graph* gi = intersect_graphs(g1, g2);
            print_adj(gi);
            print_edges(gi);
            free_graph(gi);

            printf("\n--- Кольцевая сумма G1 (+) G2 ---\n");
            struct Graph* gr = ring_sum_graphs(g1, g2);
            if (gr && gr->n > 0) {
                print_adj(gr);
                print_edges(gr);
            }
            free_graph(gr);
        }
        else if (choice == 5) {
            cartesian_product(g1, g2);
        }
        else if (choice == 6) {
            printf("\n--- Граф G1 ---\n");
            print_adj(g1);
            print_edges(g1);
            print_list(l1);

            printf("\n--- Граф G2 ---\n");
            print_adj(g2);
            print_edges(g2);
            print_list(l2);
        }
    }

    free_graph(g1);
    free_graph(g2);
    free_adj_list(l1);
    free_adj_list(l2);

    return 0;
}
