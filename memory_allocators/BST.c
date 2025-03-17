#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct block {
    size_t size;
    struct block *l, *r;
} block_t;

block_t **find_free_tree(block_t **root, block_t *target);
//block_t *find_predecessor_free_tree(block_t **root, block_t *node);

/*
 * Structure representing a free memory block in the memory allocator.
 * The free tree is a binary search tree that organizes free blocks (of type block_t)
 * to efficiently locate a block of appropriate size during memory allocation.
 */
void remove_free_tree(block_t **root, block_t *target)
{
    /* Locate the pointer to the target node in the tree. */
    block_t **node_ptr = find_free_tree(root, target);

    /* If the target node has two children, we need to find a replacement. */
    if ((*node_ptr)->l && (*node_ptr)->r) {
        /* Find the in-order predecessor:
         * This is the rightmost node in the left subtree.
         */
        block_t **pred_ptr = &(*node_ptr)->l;
        while ((*pred_ptr)->r)
            *pred_ptr = (*pred_ptr)->r;

        /* Verify the found predecessor using a helper function (for debugging).
         */
        //block_t *expected_pred = find_predecessor_free_tree(root, *node_ptr);
        //assert(expected_pred == *pred_ptr);

        /* If the predecessor is the immediate left child. */
        if (*pred_ptr == (*node_ptr)->l) {
            block_t *old_right = (*node_ptr)->r;
            *node_ptr = *pred_ptr; /* Replace target with its left child. */
            (*node_ptr)->r = old_right; /* Attach the original right subtree. */
            assert(*node_ptr != (*node_ptr)->l);
            assert(*node_ptr != (*node_ptr)->r);
        } else {
            /* The predecessor is deeper in the left subtree. */
            block_t *old_left = (*node_ptr)->l;
            block_t *old_right = (*node_ptr)->r;
            block_t *pred_node = *pred_ptr; // Right most node in left subtree
            /* Remove the predecessor from its original location. */
            remove_free_tree(&old_left, *pred_ptr);
            /* Replace the target node with the predecessor. */
            *node_ptr = pred_node; // Target = right most node
            (*node_ptr)->l = old_left;
            (*node_ptr)->r = old_right;
            assert(*node_ptr != (*node_ptr)->l);
            assert(*node_ptr != (*node_ptr)->r);
        }
    }
    /* If the target node has one child (or none), simply splice it out. */
    else if ((*node_ptr)->l || (*node_ptr)->r) {
        block_t *child = ((*node_ptr)->l) ? (*node_ptr)->l : (*node_ptr)->r;
        *node_ptr = child;
    } else {
        /* No children: remove the node. */
        *node_ptr = NULL;
    }

    /* Clear the removed node's child pointers to avoid dangling references. */
    target->l = NULL;
    target->r = NULL;
}

block_t **find_free_tree(block_t **root, block_t *target) {
    if (!*root)
        return NULL;

    if (*root == target)
        return root;

    // Search left subtree first
    block_t **left = find_free_tree(&((*root)->l), target);
    if (left)
        return left;

    // Search right subtree
    return find_free_tree(&((*root)->r), target);
}

/* Utility function to create a new node. */
block_t *new_block(size_t size) {
    block_t *node = (block_t *)malloc(sizeof(block_t));
    node->size = size;
    node->l = node->r = NULL;
    return node;
}

/* Inserts a node into the binary search tree. */
void insert_free_tree(block_t **root, block_t *node) {
    if (*root == NULL) {
        *root = node;
        return;
    }

    if (node->size < (*root)->size)
        insert_free_tree(&((*root)->l), node);
    else
        insert_free_tree(&((*root)->r), node);
}

/* Inorder traversal to print the tree. */
void print_free_tree(block_t *root) {
    if (root == NULL)
        return;
    print_free_tree(root->l);
    printf("%zu ", root->size);
    print_free_tree(root->r);
}

void print_free_tree_graviz(block_t *root) {
    if (root == NULL)
        return;
    if (root->l){
        printf("%zu -> %zu\n", root->size, root->l->size);
    }
    if (root->r){
        printf("%zu -> %zu\n", root->size, root->r->size);
    }
    print_free_tree_graviz(root->l);
    print_free_tree_graviz(root->r);
}

block_t *choose_rand_b(block_t **b_table, int b_num){
    int rand_idx = rand() % b_num;
    return b_table[rand_idx];
}

/* Main function to test the tree implementation. */
int main() {
    block_t *root = NULL;
    srand( time(NULL) );
    
    int b_num = 25;
    block_t *b_table[b_num];
    for (int i = 0; i < b_num; i++){
        int rand_num = rand() % 1000;
        if (i == 0)
            rand_num = 500;
        block_t *b = new_block(rand_num);
        b_table[i] = b;
        insert_free_tree(&root, b);
    }
        
    
    printf("Free Tree (Inorder Traversal):\n");
    print_free_tree(root);
    printf("\n");
    print_free_tree_graviz(root);
    printf("\n");

    /* Test removing nodes */
    block_t *remove_b = choose_rand_b(b_table, b_num);
    printf("Removing node %ld...\n", remove_b->size);
    remove_free_tree(&root, remove_b);
    print_free_tree(root);
    printf("\n");
    print_free_tree_graviz(root);
    printf("\n");

    remove_b = choose_rand_b(b_table, b_num);
    printf("Removing node %ld...\n", remove_b->size);
    remove_free_tree(&root, remove_b);
    print_free_tree(root);
    printf("\n");
    print_free_tree_graviz(root);
    printf("\n");

    remove_b = choose_rand_b(b_table, b_num);
    printf("Removing node %ld...\n", remove_b->size);
    remove_free_tree(&root, remove_b);
    print_free_tree(root);
    printf("\n");
    print_free_tree_graviz(root);
    printf("\n");

    /* Free memory */
    for (int i = 0; i < b_num; i++){
        free(b_table[i]);
    }

    return 0;
}
