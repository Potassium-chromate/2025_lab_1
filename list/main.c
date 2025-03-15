#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"

typedef struct list_item {
    int value;
    struct list_item *next;
} list_item_t;

typedef struct {
    struct list_item *head;
} list_t;


#define my_assert(test, message) \
    do {                         \
        if (!(test))             \
            return message;      \
    } while (0)
#define my_run_test(test)       \
    do {                        \
        char *message = test(); \
        tests_run++;            \
        if (message)            \
            return message;     \
    } while (0)

#define N 1000

static list_item_t items[N];
static list_t l;
static inline void list_insert_before(list_t *l,
                                      list_item_t *before,
                                      list_item_t *item);

static list_t *list_reset(void)
{
    for (size_t i = 0; i < N; i++) {
        items[i].value = i;
        items[i].next = NULL;
    }
    l.head = NULL;
    return &l;
}


static int list_size(list_t *list)
{
    if (!list)
        return 0;

    int len = 0;
    list_item_t *li = list->head;

    while (li != NULL) {
        len++;
        li = li->next;
    }
    return len;
}

static char *test_list(void)
{
    /* Test inserting at the beginning */
    list_reset();
    my_assert(list_size(&l) == 0, "Initial list size is expected to be zero.");
    for (size_t i = 0; i < N; i++)
        list_insert_before(&l, l.head, &items[i]);
    my_assert(list_size(&l) == N, "Final list size should be N");
    size_t k = N - 1;
    list_item_t *cur = l.head;
    while (cur) {
        my_assert(cur->value == k, "Unexpected list item value");
        k--;
        cur = cur->next;
    }

    /* Test inserting at the end */
    list_reset();
    my_assert(list_size(&l) == 0, "Initial list size is expected to be zero.");
    for (size_t i = 0; i < N; i++)
        list_insert_before(&l, NULL, &items[i]);
    my_assert(list_size(&l) == N, "Final list size should be N");
    k = 0;
    cur = l.head;
    while (cur) {
        my_assert(cur->value == k, "Unexpected list item value");
        k++;
        cur = cur->next;
    }

    /* Reset the list and insert elements in order (i.e. at the end) */
    list_reset();
    my_assert(list_size(&l) == 0, "Initial list size is expected to be zero.");
    for (size_t i = 0; i < N; i++)
        list_insert_before(&l, NULL, &items[i]);
    my_assert(list_size(&l) == N, "list size should be N");

    return NULL;
}

int tests_run = 0;

static char *test_suite(void)
{
    my_run_test(test_list);
    return NULL;
}

static inline void list_insert_before(list_t *l,
                                      list_item_t *before,
                                      list_item_t *item)
{
    list_item_t **p;
    for (p = &l->head; *p != before; p = &(*p)->next)
        ;
    *p = item;
    item->next = before;
}

static inline list_item_t *get_middle(list_item_t *head) {
    if (!head || !head->next)
        return head;
    
    list_item_t *slow = head, *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// Function to merge two sorted linked lists
static inline list_item_t *merge(list_item_t *left, list_item_t *right) {
    if (!left) return right;
    if (!right) return left;
    
    if (left->value <= right->value) {
        left->next = merge(left->next, right);
        return left;
    } else {
        right->next = merge(left, right->next);
        return right;
    }
}

// Function to perform merge sort on a linked list
static inline list_item_t *merge_sort(list_item_t *head) {
    if (!head || !head->next)
        return head;
    
    list_item_t* middle = get_middle(head);
    list_item_t* next_to_middle = middle->next;
    middle->next = NULL;
    
    list_item_t* left = merge_sort(head);
    list_item_t* right = merge_sort(next_to_middle);
    
    return merge(left, right);
}

static inline void print_list(list_t *l){
    list_item_t *cur = l->head->next;
    while(cur){
    	printf("%d\n", cur->value);
    	cur = cur->next;
    }
}

int main(void)
{
    srand( time(NULL) );
    printf("---=[ List tests\n");
    char *result = test_suite();
    if (result)
        printf("ERROR: %s\n", result);
    else
        printf("ALL TESTS PASSED\n");
    printf("Tests run: %d\n", tests_run);
    //return !!result;
    
    printf("===========TEST Merger Sort===========\n");
    // Randomize the value in l
    list_item_t *cur = l.head->next;
    while(cur){
    	cur->value = rand();
    	cur = cur->next;
    }
    
    // Start sorting the list
    cur = l.head->next;
    list_item_t *merge_result = merge_sort(cur);
    l.head->next = merge_result;
    
    // Check if the result correct
    int prev_int = 0;
    cur = l.head->next;
    
    while(cur){
    	if (prev_int > cur->value) {
    	    printf("The result is wrong!\n");
    	    return 0;
    	}
    	cur = cur->next;
    }
    printf("The result is correct!\n");
    return 0;
}
