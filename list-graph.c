#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define DIE(assertion, call_description)            \
    do                                              \
    {                                               \
        if (assertion)                              \
        {                                           \
            fprintf(stderr, "(%s, %d): ", __FILE__, \
                    __LINE__);                      \
            perror(call_description);               \
            exit(errno);                            \
        }                                           \
    } while (0)

typedef struct ll_node_t ll_node_t;
typedef struct linked_list_t linked_list_t;
typedef struct stack_t stack_t;
typedef struct queue_t queue_t;
typedef struct list_graph_t list_graph_t;

/* Helper data structures definitions */
struct ll_node_t
{
	void* data;
	ll_node_t* next;
};

struct linked_list_t
{
	ll_node_t* head;
	unsigned int data_size;
	unsigned int size;
};

struct stack_t
{
	struct linked_list_t* list;
};

struct queue_t
{
	unsigned int max_size;
	unsigned int size;
	unsigned int data_size;
	unsigned int read_idx;
	unsigned int write_idx;
	void **buff;
};

struct list_graph_t
{
	linked_list_t** neighbors;
	int nodes;
};

linked_list_t*
ll_create(unsigned int data_size)
{
	linked_list_t* ll = calloc(1, sizeof(*ll));
	DIE(!ll, "calloc list");

	ll->data_size = data_size;

	return ll;
}

static ll_node_t*
get_nth_node(linked_list_t* list, unsigned int n)
{
	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t* node = list->head;

	n = MIN(n, len);

	for (i = 0; i < n; ++i)
		node = node->next;

	return node;
}

static ll_node_t*
create_node(const void* new_data, unsigned int data_size)
{
	ll_node_t* node = calloc(1, sizeof(*node));
	DIE(!node, "calloc node");

	node->data = malloc(data_size);
	DIE(!node->data, "malloc data");

	memcpy(node->data, new_data, data_size);

	return node;
}

void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
	ll_node_t *new_node, *prev_node;

	if (!list)
		return;

	new_node = create_node(new_data, list->data_size);

	if (!n || !list->size) {
		new_node->next = list->head;
		list->head = new_node;
	} else {
		prev_node = get_nth_node(list, n - 1);
		new_node->next = prev_node->next;
		prev_node->next = new_node;
	}

	++list->size;
}

ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
	ll_node_t *prev_node, *removed_node;

	if (!list || !list->size)
		return NULL;

	if (!n) {
		removed_node = list->head;
		list->head = removed_node->next;
		removed_node->next = NULL;
	} else {
		prev_node = get_nth_node(list, n - 1);
		removed_node = prev_node->next;
		prev_node->next = removed_node->next;
		removed_node->next = NULL;
	}

	--list->size;

	return removed_node;
}

unsigned int
ll_get_size(linked_list_t* list)
{
	return !list ? 0 : list->size;
}

void
ll_free(linked_list_t** pp_list)
{
	ll_node_t* node;

	if (!pp_list || !*pp_list)
		return;

	while ((*pp_list)->size) {
		node = ll_remove_nth_node(*pp_list, 0);
		free(node->data);
		free(node);
	}

	free(*pp_list);
	*pp_list = NULL;
}

void
ll_print_int(linked_list_t* list)
{
	ll_node_t* node = list->head;

	for (; node; node = node->next)
		printf("%d ", *(int*)node->data);
	printf("\n");
}

void
ll_print_string(linked_list_t* list)
{
	ll_node_t* node = list->head;

	for (; node; node = node->next)
		printf("%s ", (char*)node->data);
	printf("\n");
}

stack_t*
st_create(unsigned int data_size)
{
	stack_t* st = malloc(sizeof(*st));
	DIE(!st, "malloc st failed");
	st->list = ll_create(data_size);
	DIE(!st->list, "malloc list failed");

	return st;
}

unsigned int
st_get_size(stack_t* st)
{
	if (!st || !st->list)
		return 0;
	return st->list->size;
}

unsigned int
st_is_empty(stack_t* st)
{
	return !st || !st->list || !st->list->size;
}

void*
st_peek(stack_t* st)
{
	if (!st || !st->list || !st->list->size)
		return NULL;

	return st->list->head->data;
}

void
st_pop(stack_t* st)
{
	ll_node_t *node;

	if (!st || !st->list)
		return;

	node = ll_remove_nth_node(st->list, 0);
	free(node->data);
	free(node);
}

void
st_push(stack_t* st, void* new_data)
{
    if (!st || !st->list)
        return;
    
    ll_add_nth_node(st->list, 0, new_data);
}

void
st_clear(stack_t* st)
{
	if (!st || !st->list)
		return;

	ll_free(&st->list);
}

void
st_free(stack_t* st)
{
	if (!st || !st->list)
		return;

	ll_free(&st->list);
	free(st);
}

queue_t *
q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *q = calloc(1, sizeof(*q));
	DIE(!q, "calloc queue failed");

	q->data_size = data_size;
	q->max_size = max_size;

	q->buff = malloc(max_size * sizeof(*q->buff));
	DIE(!q->buff, "malloc buffer failed");

	return q;
}

unsigned int
q_get_size(queue_t *q)
{
	return !q ? 0 : q->size;
}

unsigned int
q_is_empty(queue_t *q)
{
	return !q ? 1 : !q->size;
}

void *
q_front(queue_t *q)
{
	if (!q || !q->size)
		return NULL;

	return q->buff[q->read_idx];
}

int
q_dequeue(queue_t *q)
{
	if (!q || !q->size)
		return 0;

	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return 1;
}

int
q_enqueue(queue_t *q, void *new_data)
{
	void *data;
	if (!q || q->size == q->max_size)
		return 0;

	data = malloc(q->data_size);
	DIE(!data, "malloc data failed");
	memcpy(data, new_data, q->data_size);

	q->buff[q->write_idx] = data;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;

	return 1;
}

void
q_clear(queue_t *q)
{
	unsigned int i;
	if (!q || !q->size)
		return;

	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
		free(q->buff[i]);

	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;
}

void
q_free(queue_t *q)
{
	if (!q)
		return;

	q_clear(q);
	free(q->buff);
	free(q);
}


static int is_node_in_graph(int n, int nodes)
{
	return n >= 0 && n < nodes;
}

list_graph_t*
lg_create(int nodes)
{
	int i;

	list_graph_t *g = malloc(sizeof(*g));
	DIE(!g, "malloc graph failed");

	g->neighbors = malloc(nodes * sizeof(*g->neighbors));
	DIE(!g->neighbors, "malloc neighbours failed");

	for (i = 0; i != nodes; ++i)
		g->neighbors[i] = ll_create(sizeof(int));

	g->nodes = nodes;

	return g;
}

void
lg_add_edge(list_graph_t* graph, int src, int dest)
{
	if (
		!graph || !graph->neighbors
		|| !is_node_in_graph(src, graph->nodes)
		|| !is_node_in_graph(dest, graph->nodes)
	)
		return;

	ll_add_nth_node(graph->neighbors[src], graph->neighbors[src]->size, &dest);
}

static ll_node_t *find_node(linked_list_t *ll, int node, unsigned int *pos)
{
	ll_node_t *crt = ll->head;
	unsigned int i;

	for (i = 0; i != ll->size; ++i) {
		if (node == *(int *)crt->data) {
			*pos = i;
			return crt;
		}

		crt = crt->next;
	}

	return NULL;
}

int
lg_has_edge(list_graph_t* graph, int src, int dest)
{
	unsigned int pos;

	if (
		!graph || !graph->neighbors
		|| !is_node_in_graph(src, graph->nodes)
		|| !is_node_in_graph(dest, graph->nodes)
	)
		return 0;

	return find_node(graph->neighbors[src], dest, &pos) != NULL;
}

linked_list_t*
lg_get_neighbours(list_graph_t* graph, int node)
{
	if (
		!graph || !graph->neighbors
		|| !is_node_in_graph(node, graph->nodes)
	)
		return NULL;

	return graph->neighbors[node];
}

void
lg_remove_edge(list_graph_t* graph, int src, int dest)
{
	unsigned int pos;

	if (
		!graph || !graph->neighbors
		|| !is_node_in_graph(src, graph->nodes)
		|| !is_node_in_graph(dest, graph->nodes)
	)
		return;

	if (!find_node(graph->neighbors[src], dest, &pos))
		return;

	ll_remove_nth_node(graph->neighbors[src], pos);
}

void
lg_free(list_graph_t* graph)
{
	int i;

	for (i = 0; i != graph->nodes; ++i)
		ll_free(graph->neighbors + i);
	
	free(graph->neighbors);
	free(graph);
}



// ------------------- CHEAT SHEET START HERE -------------------

void BFS(list_graph_t* graph, int start_node, int *color) {
    queue_t *q;
    int *node;

    if (!graph || !graph->neighbors || start_node < 0 || start_node >= graph->nodes)
        return;

    q = q_create(sizeof(int), graph->nodes);
    q_enqueue(q, &start_node);

    while (!q_is_empty(q)) {
        node = (int *)q_front(q);
        q_dequeue(q);

        color[*node] = 1;  // Mark the node as visited

        linked_list_t *neighbours = lg_get_neighbours(graph, *node);
        ll_node_t *crt = neighbours->head;

        while (crt) {
            if (!color[*(int *)crt->data]) {
                q_enqueue(q, crt->data);
            }
            crt = crt->next;
        }
    }

    q_free(q);
}

void DFS(list_graph_t* graph, int start_node, int *color) {
    stack_t *st;
    int *node;

    if (!graph || !graph->neighbors || start_node < 0 || start_node >= graph->nodes)
        return;

    st = st_create(sizeof(int));
    st_push(st, &start_node);

    while (!st_is_empty(st)) {
        node = (int *)st_peek(st);
        st_pop(st);

        if (color[*node] == 0) {
            color[*node] = 1;  // Mark the node as visited
            printf("%d ", *node);

            linked_list_t *neighbours = lg_get_neighbours(graph, *node);
            ll_node_t *crt = neighbours->head;

            while (crt) {
                if (color[*(int *)crt->data] == 0) {
                    st_push(st, crt->data);
                }
                crt = crt->next;
            }
        }
    }

    st_free(st);
}

void print_k_BFS_level(list_graph_t *graph, int start_node, int k)
{
    if (!graph || !graph->neighbors || start_node < 0 || start_node >= graph->nodes || k < 0)
        return;

    // Inițializare vector vizitat
    int *visited = calloc(graph->nodes, sizeof(int));
    if (!visited) return;

    // Inițializare coadă
    queue_t *q = q_create(sizeof(int), graph->nodes);
    if (!q) {
        free(visited);
        return;
    }

    // Începem cu nodul de start
    q_enqueue(q, &start_node);
    visited[start_node] = 1;
    int level = 0;

    while (!q_is_empty(q)) {
        int level_size = q_get_size(q);

        for (int i = 0; i < level_size; i++) {
            int node = *(int *)q_front(q);
            q_dequeue(q);

            if (level == k)
                printf("%d ", node);

            linked_list_t *neighbours = lg_get_neighbours(graph, node);
            if (!neighbours) continue;

            ll_node_t *crt = neighbours->head;
            while (crt) {
                int v = *(int *)crt->data;
                if (!visited[v]) {
                    visited[v] = 1;
                    q_enqueue(q, &v);
                }
                crt = crt->next;
            }
        }

        // Dacă am ajuns la nivelul dorit, ne oprim
        if (level == k)
            break;

        level++;
    }

    printf("\n");
    q_free(q);
    free(visited);
}

void print_BFS_levels(list_graph_t* graph, int start_node)
{
    if (!graph || !graph->neighbors || start_node < 0 || start_node >= graph->nodes)
        return;

    int *visited = calloc(graph->nodes, sizeof(int));
    queue_t *q = q_create(sizeof(int), graph->nodes);
    q_enqueue(q, &start_node);
    visited[start_node] = 1;

    while (!q_is_empty(q)) {
        int level_size = q_get_size(q);
        for (int i = 0; i < level_size; i++) {
            int node = *(int *)q_front(q);
            q_dequeue(q);
            printf("%d ", node);
            linked_list_t *neighbours = lg_get_neighbours(graph, node);
            ll_node_t *crt = neighbours->head;
            while (crt) {
                int v = *(int *)crt->data;
                if (!visited[v]) {
                    visited[v] = 1;
                    q_enqueue(q, &v);
                }
                crt = crt->next;
            }
        }
        printf("\n");
    }

    q_free(q);
    free(visited);
}

int path_exists(list_graph_t* graph, int src, int dest, int *visited) {
    if (!graph || !graph->neighbors || src < 0 || src >= graph->nodes || dest < 0 || dest >= graph->nodes)
        return 0;

    if (src == dest)
        return 1;

    visited[src] = 1;

    linked_list_t *neighbours = lg_get_neighbours(graph, src);
    ll_node_t *crt = neighbours->head;

    while (crt) {
        int neighbour = *(int *)crt->data;
        if (!visited[neighbour]) {
            if (path_exists(graph, neighbour, dest, visited))
                return 1;
        }
        crt = crt->next;
    }

    return 0;
}

void component_arrays(list_graph_t* graph, int *visited, int *component, int node, int component_id) {
    if (!graph || !graph->neighbors || node < 0 || node >= graph->nodes || visited[node])
        return;

    visited[node] = 1;
    component[node] = component_id;

    linked_list_t *neighbours = lg_get_neighbours(graph, node);
    ll_node_t *crt = neighbours->head;

    while (crt) {
        int neighbour = *(int *)crt->data;
        if (!visited[neighbour]) {
            component_arrays(graph, visited, component, neighbour, component_id);
        }
        crt = crt->next;
    }
}

int shortest_path_BFS(list_graph_t *graph, int start, int target)
{
    if (!graph || start < 0 || target < 0 ||
        start >= graph->nodes || target >= graph->nodes)
        return -1;

    int *visited = calloc(graph->nodes, sizeof(int));
    int *dist = calloc(graph->nodes, sizeof(int));

    queue_t *q = q_create(sizeof(int), graph->nodes);
    q_enqueue(q, &start);
    visited[start] = 1;
    dist[start] = 0;

    while (!q_is_empty(q)) {
        int node = *(int *)q_front(q);
        q_dequeue(q);

        if (node == target) {
            int result = dist[node];
            q_free(q);
            free(visited);
            free(dist);
            return result;
        }

        linked_list_t *neigh = lg_get_neighbours(graph, node);
        ll_node_t *crt = neigh->head;
        while (crt) {
            int v = *(int *)crt->data;
            if (!visited[v]) {
                visited[v] = 1;
                dist[v] = dist[node] + 1;
                q_enqueue(q, &v);
            }
            crt = crt->next;
        }
    }

    q_free(q);
    free(visited);
    free(dist);
    return -1; // dacă target nu e accesibil
}

void dfs_order(list_graph_t *graph, int node, int *visited, int *stack, int *stack_top) {
    visited[node] = 1;
    linked_list_t *neigh = lg_get_neighbours(graph, node);
    for (ll_node_t *crt = neigh->head; crt; crt = crt->next) {
        int v = *(int *)crt->data;
        if (!visited[v])
            dfs_order(graph, v, visited, stack, stack_top);
    }
    stack[(*stack_top)++] = node;
}

list_graph_t* transpose_graph(list_graph_t *graph) {
    list_graph_t *t_graph = lg_create(graph->nodes);
    for (int u = 0; u < graph->nodes; u++) {
        linked_list_t *neigh = lg_get_neighbours(graph, u);
        for (ll_node_t *crt = neigh->head; crt; crt = crt->next) {
            int v = *(int *)crt->data;
            lg_add_edge(t_graph, v, u);  // inversăm direcția
        }
    }
    return t_graph;
}

void dfs_assign(list_graph_t *graph, int node, int *visited, int *component, int comp_id) {
    visited[node] = 1;
    component[node] = comp_id;
    linked_list_t *neigh = lg_get_neighbours(graph, node);
    for (ll_node_t *crt = neigh->head; crt; crt = crt->next) {
        int v = *(int *)crt->data;
        if (!visited[v])
            dfs_assign(graph, v, visited, component, comp_id);
    }
}

void find_strongly_connected_components(list_graph_t *graph, int *component, int *num_components) {
    int n = graph->nodes;
    int *visited = calloc(n, sizeof(int));
    int *stack = malloc(n * sizeof(int));
    int stack_top = 0;

    // 1. DFS pentru ordine
    for (int i = 0; i < n; i++) {
        if (!visited[i])
            dfs_order(graph, i, visited, stack, &stack_top);
    }

    // 2. Transpune
    list_graph_t *t_graph = transpose_graph(graph);

    // 3. DFS în ordinea inversă
    for (int i = 0; i < n; i++) visited[i] = 0;
    *num_components = 0;

    for (int i = stack_top - 1; i >= 0; i--) {
        int node = stack[i];
        if (!visited[node]) {
            dfs_assign(t_graph, node, visited, component, *num_components);
            (*num_components)++;
        }
    }

    // curățenie
    free(stack);
    free(visited);
    lg_free(t_graph);
}

void topological_sort(list_graph_t *graph, int *visited, int *stack, int *stack_top, int node) {
    visited[node] = 1;
    linked_list_t *neigh = lg_get_neighbours(graph, node);
    for (ll_node_t *crt = neigh->head; crt; crt = crt->next) {
        int v = *(int *)crt->data;
        if (!visited[v])
            topological_sort(graph, visited, stack, stack_top, v);
    }
    stack[(*stack_top)++] = node;
}
