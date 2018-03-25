/**
 * file name: rb_tree.c
 *
 * rb_tree implemention
 */
#include <stdio.h>

#include "rbtree.h"


static int
rbtree_init_node(rbtree_t *tree,
                 rbtree_node_t *node,
                 rbtree_node_t *lchild,
                 rbtree_node_t *rchild,
                 rbtree_node_t *parent,
                 int is_left)
{
    node->left = lchild;
    node->right = rchild;

    if (!rbtree_is_sentinel(tree, lchild)) {
        lchild->parent = node;
    }

    if (!rbtree_is_sentinel(tree, rchild)) {
        rchild->parent = node;
    }

    node->parent = parent;
    if (parent == NULL) {
        return RBTREE_OK;
    }

    if (is_left) {
        parent->left = node;
    }
    else {
        parent->right = node;
    }

    return RBTREE_OK;
}


static int
rbtree_left_rotate(rbtree_t *tree, rbtree_node_t *node)
{
    if (tree == NULL || node == NULL) {
        return RBTREE_INVALID_ARG;
    }

    if (rbtree_is_sentinel(tree, node)) {
        return RBTREE_INVALID_TOPOLOGY;
    }

    rbtree_node_t *rchild = node->right;

    /** has no right child */
    if (rbtree_is_sentinel(tree, rchild)) {
        return RBTREE_OK;
    }

    node->right = rchild->left;
    if (!rbtree_is_sentinel(tree, rchild->left)) {
        rchild->left->parent = node;
    }

    rchild->parent = node->parent;

    if (rbtree_is_root(tree, node)) {
        tree->root = rchild;
    }
    else if (rbtree_is_left_child(node)) {
        node->parent->left = rchild;
    }
    else {
        node->parent->right = rchild;
    }

    rchild->left = node;
    node->parent = rchild;

    return RBTREE_OK;
}


static int
rbtree_right_rotate(rbtree_t *tree, rbtree_node_t *node)
{
    if (tree == NULL || node == NULL) {
        return RBTREE_INVALID_ARG;
    }

    if (rbtree_is_sentinel(tree, node)) {
        return RBTREE_INVALID_ARG;
    }

    rbtree_node_t *lchild = node->left;

    /** has no left child */
    if (rbtree_is_sentinel(tree, lchild)) {
        return RBTREE_INVALID_TOPOLOGY;
    }

    node->left = lchild->right;
    if (!rbtree_is_sentinel(tree, lchild->right)) {
        lchild->right->parent = node;
    }

    lchild->parent = node->parent;

    if (rbtree_is_root(tree, node)) {
        tree->root = lchild;
    }
    else {
        if (rbtree_is_left_child(node)) {
            node->parent->left = lchild;
        }
        else {
            node->parent->right = lchild;
        }
    }

    lchild->right = node;
    node->parent = lchild;

    return RBTREE_OK;
}


int
rbtree_init(rbtree_t *tree, rbtree_compare compare)
{
    if (tree == NULL || compare == NULL) {
        return RBTREE_INVALID_ARG;
    }

    tree->root = NULL;
    tree->compare = compare;

    tree->sentinel.left = &tree->sentinel;
    tree->sentinel.right = &tree->sentinel;
    tree->sentinel.parent = NULL;
    tree->sentinel.color = RBTREE_BLACK;

    return RBTREE_OK;
}

//int
//rbtree_destroy()
//{
//}
