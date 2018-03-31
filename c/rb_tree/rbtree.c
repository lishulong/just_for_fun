/**
 * file name: rb_tree.c
 *
 * rb_tree implemention
 */
#include <stdio.h>

#include "rbtree.h"


static int
rbtree_init_node(rbtree_t      *tree,
                 rbtree_node_t *node,
                 rbtree_node_t *lchild,
                 rbtree_node_t *rchild,
                 rbtree_node_t *parent,
                 int           is_left,
                 int           is_red)
{
    rbtree_must(tree != NULL, RBTREE_INVALID_ARG);
    rbtree_must(node != NULL && node != &tree->sentinel, RBTREE_INVALID_ARG);

    lchild = (lchild == NULL ? &tree->sentinel : lchild);
    rchild = (rchild == NULL ? &tree->sentinel : rchild);
    parent = (parent == NULL ? &tree->sentinel : parent);

    node->left  = lchild;
    node->right = rchild;
    node->color = (!!is_red) ? RBTREE_RED : RBTREE_BLACK;

    if (!rbtree_is_sentinel(tree, lchild)) {
        lchild->parent = node;
    }

    if (!rbtree_is_sentinel(tree, rchild)) {
        rchild->parent = node;
    }

    node->parent = parent;
    if (!rbtree_is_sentinel(tree, parent)) {
        if (is_left) {
            parent->left = node;
        }
        else {
            parent->right = node;
        }
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


static int
rbtree_insert_fixup(rbtree_t *tree, rbtree_node_t *node)
{
    while (rbtree_is_red(node->parent)) {
        /** node is red */
        if (rbtree_is_left_child(node->parent)) {
            /** since parent of node is red, node must have grandparent */
            rbtree_node_t *uncle = rbtree_get_uncle(node);

            if (rbtree_is_red(uncle)) {
                /** case 1: uncle is red */
                rbtree_set_black(uncle);
                rbtree_set_black(node->parent);
                rbtree_set_red(node->parent->parent);

                node = node->parent->parent;
            }
            else {
                /** case 2: uncle is black and node is right child */
                if (rbtree_is_right_child(node)) {
                    /** change to case 3 */
                    node = node->parent;
                    rbtree_left_rotate(tree, node);
                }

                /** case 3: uncle is black and node is right child */
                rbtree_set_black(node->parent);
                rbtree_set_red(node->parent->parent);
                rbtree_right_rotate(tree, node->parent->parent);
            }
        }
        else {
            /** node->parent is right child, just exchange left and right */
            rbtree_node_t *uncle = rbtree_get_uncle(node);

            if (rbtree_is_red(uncle)) {
                rbtree_set_black(uncle);
                rbtree_set_black(node->parent);
                rbtree_set_red(node->parent->parent);

                node = node->parent->parent;
            }
            else {
                if (rbtree_is_left_child(node)) {
                    node = node->parent;
                    rbtree_right_rotate(tree, node);
                }

                rbtree_set_black(node->parent);
                rbtree_set_red(node->parent->parent);
                rbtree_left_rotate(tree, node->parent->parent);
            }
        }
    }

    rbtree_set_black(tree->root);

    return RBTREE_OK;
}


int
rbtree_insert(rbtree_t *tree, rbtree_node_t *node)
{
    rbtree_must(tree != NULL, RBTREE_INVALID_ARG);
    rbtree_must(node != NULL && node != &tree->sentinel, RBTREE_INVALID_ARG);

    rbtree_node_t *parent   = tree->root;
    rbtree_node_t *traverse = tree->root;

    *node = (rbtree_node_t)rbtree_null_node(tree);
    int is_left = -1;

    while (traverse != &tree->sentinel) {
        parent = traverse;

        is_left = tree->compare(node, traverse) <= 0;

        if (is_left) {
            traverse = traverse->left;
        }
        else {
            traverse = traverse->right;
        }
    }

    node->parent = parent;
    if (rbtree_is_sentinel(tree, parent)) {
        /** empty tree */
        tree->root = node;
    }
    else {
        if (is_left) {
            parent->left = node;
        }
        else {
            parent->right = node;
        }
    }

    return rbtree_insert_fixup(tree, node);
}


int
rbtree_init(rbtree_t *tree, rbtree_compare compare)
{
    if (tree == NULL || compare == NULL) {
        return RBTREE_INVALID_ARG;
    }

    tree->root = &tree->sentinel;
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
