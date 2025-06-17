#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_NODES 500
#define BUF_SIZ 512

typedef struct queue_t queue_t;
struct queue_t
{
    /* Dimensiunea maxima a cozii */
    unsigned int max_size;
    /* Dimensiunea cozii */
    unsigned int size;
    /* Dimensiunea in octeti a tipului de date stocat in coada */
    unsigned int data_size;
    /* Indexul de la care se vor efectua operatiile de front si dequeue */
    unsigned int read_idx;
    /* Indexul de la care se vor efectua operatiile de enqueue */
    unsigned int write_idx;
    /* Bufferul ce stocheaza elementele cozii */
    void **buff;
};

typedef struct b_node_t b_node_t;
struct b_node_t
{
    /* left child */
    b_node_t *left;
    /* right child */
    b_node_t *right;

    /* data contained by the node */
    void *data;
};

typedef struct b_tree_t b_tree_t;
struct b_tree_t
{
    /* root of the tree */
    b_node_t *root;

    /* size of the data contained by the nodes */
    size_t data_size;
};

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

int q_dequeue(queue_t *q)
{
    if (!q || !q->size)
        return 0;

    free(q->buff[q->read_idx]);

    q->read_idx = (q->read_idx + 1) % q->max_size;
    --q->size;
    return 1;
}

int q_enqueue(queue_t *q, void *new_data)
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

void q_clear(queue_t *q)
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

void q_free(queue_t *q)
{
    if (!q)
        return;

    q_clear(q);
    free(q->buff);
    free(q);
}

/**
 * Helper function to create a node
 * @data: the data to be added in the node
 * @data_size: data's size
 */
static b_node_t *
__b_node_create(void *data, size_t data_size)
{
    b_node_t *b_node;

    b_node = malloc(sizeof(*b_node));
    DIE(b_node == NULL, "b_node malloc");

    b_node->left = b_node->right = NULL;

    b_node->data = malloc(data_size);
    DIE(b_node->data == NULL, "b_node->data malloc");
    memcpy(b_node->data, data, data_size);

    return b_node;
}

b_tree_t *
b_tree_create(size_t data_size)
{
    /* TODO */
    b_tree_t *tree = malloc(sizeof(b_tree_t));
    if (!tree) {
        printf("malloc() failed!\n");
        exit(0);
    }

    tree->root = NULL;
    tree->data_size = data_size;

    return tree;
}

void b_tree_insert(b_tree_t *b_tree, void *data)
{
    queue_t *q;
    b_node_t *b_node, *b_node_tmp;
    b_node_t **b_node_tmp_addr;

    b_node = __b_node_create(data, b_tree->data_size);

    if (!b_tree->root)
    {
        b_tree->root = b_node;
        return;
    }

    q = q_create(sizeof(b_node_t *), MAX_NODES);

    /* TODO */
    q_enqueue(q, &(b_tree->root));

    while (!q_is_empty(q)) {
        b_node_tmp_addr = q_front(q);
        b_node_tmp = *b_node_tmp_addr;

        if (!b_node_tmp->left) {
            b_node_tmp->left = b_node;
            q_dequeue(q);
            break;
        } else if (!b_node_tmp->right) {
            b_node_tmp->right = b_node;
            q_dequeue(q);
            break;
        } else {
            q_enqueue(q, &(b_node_tmp->left));
            q_enqueue(q, &(b_node_tmp->right));
        }

        q_dequeue(q);
    }

    q_free(q);
    
}

static void
__b_tree_print_preorder(b_node_t *b_node, void (*print_data)(void *))
{
    if (!b_node)
        return;

    /* TODO */
    print_data(b_node->data);
    __b_tree_print_preorder(b_node->left, print_data);
    __b_tree_print_preorder(b_node->right, print_data);
}

void b_tree_print_preorder(b_tree_t *b_tree, void (*print_data)(void *))
{
    __b_tree_print_preorder(b_tree->root, print_data);
    printf("\n");
}

static void
__b_tree_print_inorder(b_node_t *b_node, void (*print_data)(void *))
{
    if (!b_node)
        return;

    /* TODO */
    __b_tree_print_inorder(b_node->left, print_data);
    print_data(b_node->data);
    __b_tree_print_inorder(b_node->right, print_data);

}

void b_tree_print_inorder(b_tree_t *b_tree, void (*print_data)(void *))
{
    __b_tree_print_inorder(b_tree->root, print_data);
    printf("\n");
}

static void
__b_tree_print_postorder(b_node_t *b_node, void (*print_data)(void *))
{
    if (!b_node)
        return;

    /* TODO */
    __b_tree_print_postorder(b_node->left, print_data);
    __b_tree_print_postorder(b_node->right, print_data);
    print_data(b_node->data);
}

void b_tree_print_postorder(b_tree_t *b_tree, void (*print_data)(void *))
{
    __b_tree_print_postorder(b_tree->root, print_data);
    printf("\n");
}

/**
 * Free the left and the right subtree of a node, its data and itself
 * @b_node: the node which has to free its children and itself
 * @free_data: function used to free the data contained by a node
 */
static void
__b_tree_free(b_node_t *b_node, void (*free_data)(void *))
{
    if (!b_node)
        return;

    /* TODO */
    __b_tree_free(b_node->left, free_data);
    __b_tree_free(b_node->right, free_data);
    free_data(b_node->data);
    free(b_node);

}

void b_tree_free(b_tree_t *b_tree, void (*free_data)(void *))
{
    __b_tree_free(b_tree->root, free_data);
    free(b_tree);
}

void print_int(void *data)
{
    printf("%d ", *(int *)data);
}

void print_string(void *data)
{
    printf("%s ", (char *)data);
}

typedef struct ll_node_t ll_node_t;
struct ll_node_t
{
    void* data;
    ll_node_t* next;
};

typedef struct linked_list_t linked_list_t;
struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
};

linked_list_t *
ll_create(unsigned int data_size)
{
    linked_list_t* ll;

    ll = malloc(sizeof(*ll));
    DIE(ll == NULL, "ll_create malloc");

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}

/*
 * Returneaza nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se returneaza
 * ultimul nod.
*/
ll_node_t* get_nth_node(linked_list_t* list, unsigned int n)
{
    if (!list) {
        return NULL;
    }

    unsigned int len = list->size - 1;
    unsigned int i;
    ll_node_t* node = list->head;

    n = MIN(n, len);

    for (i = 0; i < n; ++i) {
        node = node->next;
    }

    return node;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */
void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

    if (!list) {
        return;
    }

    if (n > list->size) {
        n = list->size;
    } else if (n < 0) {
        return;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    DIE(new_node == NULL, "new_node malloc");
    new_node->data = malloc(list->data_size);
    DIE(new_node->data == NULL, "new_node->data malloc");
    memcpy(new_node->data, new_data, list->data_size);

    new_node->next = curr;
    if (prev == NULL) {
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0. Daca n >= nr_noduri - 1,
 * se elimina nodul de la finalul listei. Daca n < 0, eroare. Functia intoarce
 * un pointer spre nodul eliminat din lista. Este responsabilitatea apelantului
 * sa elibereze memoria acestui nod.
 */
ll_node_t *
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return NULL;
    }

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

    return curr;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
ll_get_size(linked_list_t* list)
{
     if (!list) {
        return -1;
    }

    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void
ll_free(linked_list_t** pp_list)
{
    ll_node_t* curr_node;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0) {
        curr_node = ll_remove_nth_node(*pp_list, 0);
        free(curr_node->data);
        curr_node->data = NULL;
        free(curr_node);
        curr_node = NULL;
    }

    free(*pp_list);
    *pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void
ll_print_int(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void
ll_print_string(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)curr->data);
        curr = curr->next;
    }

    printf("\n");
}

typedef struct stack_t stack_t;
struct stack_t
{
	struct linked_list_t *list;
};

stack_t *
st_create(unsigned int data_size)
{
	/* TODO */
    stack_t *stack = malloc(sizeof(stack_t));
    if (!stack) {
        printf("malloc() failed!\n");
        exit(0);
    }
    stack->list = ll_create(data_size);
    return stack;
}

/*
 * Functia intoarce numarul de elemente din stiva al carei pointer este trimis
 * ca parametru.
 */
unsigned int
st_get_size(stack_t *st)
{
	/* TODO */
	return st->list->size;
}

/*
 * Functia intoarce 1 daca stiva al carei pointer este trimis
 * ca parametru este goala si 0 in caz contrar.
 */
unsigned int
st_is_empty(stack_t *st)
{
	/* TODO */
    if (!st || !st->list || !st->list->head) {
        return 1;
    }
	return 0;
}

/*
 * Functia intoarce elementul din vârful stivei al carei pointer este trimis
 * ca parametru fără a efectua nicio modificare asupra acesteia.
 */
void *
st_peek(stack_t *st)
{
    /* TODO */
    if (!st || !st->list || !st->list->head) {
        return NULL;
    }
	return st->list->head->data;;
}

/*
 * Functia elimina elementul din vârful stivei al carei pointer este trimis
 * ca parametru. Atentie! Este responsabilitatea acestei functii de a elibera
 * memoria pentru elementul eliminat.
 */
void
st_pop(stack_t *st)
{
	/* TODO */
    if (!st || !st->list || !st->list->head) {
        return;
    }
    ll_node_t *tmp = ll_remove_nth_node(st->list, 0);
    free(tmp->data);
    free(tmp);
    
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se adauga un nou element in
 * vârful stivei al carei pointer este trimis ca parametru.
 */
void
st_push(stack_t *st, void *new_data)
{
	/* TODO */
    ll_add_nth_node(st->list, 0, new_data);
}

/*
 * Functia elimina toate elementele stivei al carei pointer este trimis
 * ca parametru.
 */
void
st_clear(stack_t *st)
{
	/* TODO */
    ll_free(&(st->list));
}

/*
 * Functia elibereaza memoria folosita de toate elementele, iar la
 * sfarsit elibereaza memoria folosita de structura stivei.
 */
void
st_free(stack_t *st)
{
    /* TODO */
    st_clear(st);
    free(st);
    st = NULL;
}



// ------------------- CHEAT SHEET START HERE -------------------

// BFS, DFS, level k printing, level printing, mirror, height, find node, LCA

void BFS(b_tree_t *b_tree, void (*print_func)(void *))
{
    queue_t *q;
    b_node_t *b_node;

    if (!b_tree || !b_tree->root)
        return;

    q = q_create(sizeof(b_node_t *), MAX_NODES);
    q_enqueue(q, &(b_tree->root));

    while (!q_is_empty(q)) {
        b_node = *(b_node_t **)q_front(q);
        q_dequeue(q);

        print_func(b_node->data);

        if (b_node->left)
            q_enqueue(q, &(b_node->left));
        if (b_node->right)
            q_enqueue(q, &(b_node->right));
    }

    q_free(q);
}

void print_level_k(b_tree_t *b_tree, int k, void (*print_func)(void *))
{
    if (!b_tree || !b_tree->root || k < 0)
        return;

    queue_t *q = q_create(sizeof(b_node_t *), MAX_NODES);
    q_enqueue(q, &(b_tree->root));

    int level = 0;
    while (!q_is_empty(q)) {
        int level_size = q_get_size(q); // corectat aici
        for (int i = 0; i < level_size; i++) {
            b_node_t *b_node = *(b_node_t **)q_front(q);
            q_dequeue(q);

            if (level == k) {
                print_func(b_node->data);
            }

            if (b_node->left)
                q_enqueue(q, &(b_node->left));
            if (b_node->right)
                q_enqueue(q, &(b_node->right));
        }
        if (level == k)
            break;
        level++;
    }

    q_free(q);
}

void print_bfs_levels(b_tree_t *b_tree, void (*print_func)(void *))
{
    if (!b_tree || !b_tree->root)
        return;

    queue_t *q = q_create(sizeof(b_node_t *), MAX_NODES);
    q_enqueue(q, &(b_tree->root));

    while (!q_is_empty(q)) {
        int level_size = q_get_size(q);
        for (int i = 0; i < level_size; i++) {
            b_node_t *b_node = *(b_node_t **)q_front(q);
            q_dequeue(q);

            print_func(b_node->data);

            if (b_node->left)
                q_enqueue(q, &(b_node->left));
            if (b_node->right)
                q_enqueue(q, &(b_node->right));
        }
        printf("\n");
    }

    q_free(q);
}

void mirror_b_tree(b_node_t *node)
{
    if (!node)
        return;
    // Schimbă stânga cu dreapta
    b_node_t *tmp = node->left;
    node->left = node->right;
    node->right = tmp;
    // Recursiv pentru subarbori
    mirror_b_tree(node->left);
    mirror_b_tree(node->right);
}

void b_tree_mirror(b_tree_t *b_tree)
{
    if (!b_tree || !b_tree->root)
        return;
    mirror_b_tree(b_tree->root);
}

int b_tree_height(b_node_t *node)
{
    if (!node)
        return 0;

    int left_height = b_tree_height(node->left);
    int right_height = b_tree_height(node->right);

    return (left_height > right_height ? left_height : right_height) + 1;
}

int find_node(b_node_t *node, int value)
{
    if (!node)
        return 0;

    if (*(int *)node->data == value)
        return 1;

    return find_node(node->left, value) || find_node(node->right, value);
}

int find_path(b_node_t *node, int target, int *path, int *len) {
    if (!node) return 0;

    path[*len] = *(int *)node->data;
    (*len)++;

    if (*(int *)node->data == target)
        return 1;

    if (find_path(node->left, target, path, len) || find_path(node->right, target, path, len))
        return 1;

    (*len)--; // backtrack
    return 0;
}

b_node_t *LCA(b_node_t *node, int val1, int val2)
{
    if (!node)
        return NULL;

    if (*(int *)node->data == val1 || *(int *)node->data == val2)
        return node;

    b_node_t *left = LCA(node->left, val1, val2);
    b_node_t *right = LCA(node->right, val1, val2);

    if (left && right)
        return node;

    return left ? left : right;
}

int sum_path_mod(b_node_t *node, int target, int mod, int *sum) {
    if (!node) return 0;

    int val = *(int *)node->data;
    if (val == target) {
        if (val % mod == 0) *sum += val;
        return 1;
    }

    if (sum_path_mod(node->left, target, mod, sum) || sum_path_mod(node->right, target, mod, sum)) {
        if (val % mod == 0) *sum += val;
        return 1;
    }

    return 0;
}

int main(void) {

    // This is a binary tree cheatsheet for SDA - Summer Exam
    // I love ponies
    return 0;
}
