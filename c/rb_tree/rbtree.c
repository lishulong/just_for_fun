/**
 * file name: rb_tree.c
 *
 * rb_tree implemention
 */
#include <stdio.h>

#include "rbtree.h"


#define rbtree_validate_search_mode(mode) do {                                 \
    rbtree_must(mode > 0 && mode < RBTREE_SEARCH_MODE_MAX, RBTREE_INVALID_ARG);\
} while (0);


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


/**
 * `new` node must not in tree
 */
int
rbtree_replace_node(rbtree_t *tree, rbtree_node_t *new, rbtree_node_t *replaced)
{
    int ret = rbtree_init_node(tree,
                               new,
                               replaced->left,
                               replaced->right,
                               replaced->parent,
                               rbtree_is_left_child(replaced),
                               rbtree_is_red(replaced));
    if (ret != RBTREE_OK) {
        return ret;
    }

    if (rbtree_is_root(tree, replaced)) {
        tree->root = new;
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


/**
 * node must be in tree
 * sentinel means no valid maxmum node
 */
static rbtree_node_t *
rbtree_minimum(rbtree_t *tree, rbtree_node_t *node)
{
    while (node->left != &tree->sentinel) {
        node = node->left;
    }

    return node;
}


/**
 * node must be in tree
 * sentinel means no valid maxmum node
 */
static rbtree_node_t *
rbtree_maximum(rbtree_t *tree, rbtree_node_t *node)
{
    while (node->right != &tree->sentinel) {
        node = node->right;
    }

    return node;
}


/**
 * node must be in tree
 * sentinel means no valid successor
 */
static rbtree_node_t *
rbtree_successor(rbtree_t *tree, rbtree_node_t *node)
{
    rbtree_node_t *sentinel = &tree->sentinel;

    if (node->right != sentinel) {
        return rbtree_minimum(tree, node->right);
    }

    rbtree_node_t *parent = node->parent;
    while (parent != sentinel && rbtree_is_right_child(node)) {
        node   = parent;
        parent = node->parent;
    }

    return parent;
}


/**
 * node must be in tree
 * sentinel means no valid predecessor
 */
static rbtree_node_t *
rbtree_predecessor(rbtree_t *tree, rbtree_node_t *node)
{
    rbtree_node_t *sentinel = &tree->sentinel;

    if (node->left != sentinel) {
        return rbtree_maximum(tree, node->left);
    }

    rbtree_node_t *parent = node->parent;
    while (parent != sentinel && rbtree_is_left_child(node)) {
        node   = parent;
        parent = node->parent;
    }

    return parent;
}


static int
rbtree_delete_fixup(rbtree_t *tree, rbtree_node_t *node)
{
    while (!rbtree_is_root(tree, node) && rbtree_is_black(node)) {
        rbtree_node_t *brother = NULL;

        if (rbtree_is_left_child(node)) {
            /** since node is black, its brother must exist */
            brother = node->parent->right;

            /** case 1: brother is red */
            if (rbtree_is_red(brother)) {
                rbtree_set_black(brother);
                rbtree_set_red(node->parent);
                rbtree_left_rotate(tree, node->parent);

                brother = node->parent->right;
            }

            if (rbtree_is_black(brother->left) && rbtree_is_black(brother->right)) {
                /**
                 * case 2
                 *
                 * brother is black,
                 * two children of brother are black
                 */
                rbtree_set_red(brother);
                node = node->parent;
            }
            else {
                if (rbtree_is_black(brother->right)) {
                    /**
                     * case 3
                     *
                     * brother is black
                     * left child is red and right child is black
                     */
                    rbtree_set_black(brother->left);
                    rbtree_set_red(brother);
                    rbtree_right_rotate(tree, brother);
                    brother = node->parent->right;
                }

                /**
                 * case 4
                 *
                 * brother is black
                 * right child is red
                 */
                brother->color = node->parent->color;
                /** make up one black for node */
                rbtree_set_black(node->parent);

                rbtree_set_black(brother->right);
                rbtree_left_rotate(tree, node->parent);
                node = tree->root;
            }
        }
        else {
            /** exchange left and right */
            brother = node->parent->left;

            if (rbtree_is_red(brother)) {
                rbtree_set_black(brother);
                rbtree_set_red(node->parent);
                rbtree_right_rotate(tree, node->parent);

                brother = node->parent->left;
            }

            if (rbtree_is_black(brother->left) && rbtree_is_black(brother->right)) {
                rbtree_set_red(brother);
                node = node->parent;
            }
            else {
                if (rbtree_is_black(brother->left)) {
                    rbtree_set_black(brother->right);
                    rbtree_set_red(brother);
                    rbtree_left_rotate(tree, brother);
                    brother = node->parent->right;
                }

                brother->color = node->parent->color;
                rbtree_set_black(node->parent);

                rbtree_set_black(brother->left);
                rbtree_right_rotate(tree, node->parent);
                node = tree->root;
            }
        }
    }

    /** make up for one black */
    rbtree_set_black(node);

    return RBTREE_OK;
}


/** node must be in tree */
int
rbtree_delete(rbtree_t *tree, rbtree_node_t *node)
{
    rbtree_must(tree != NULL, RBTREE_INVALID_ARG);
    rbtree_must(node != NULL && node != &tree->sentinel, RBTREE_INVALID_ARG);

    rbtree_node_t *sentinel = &tree->sentinel;

    /** use node `replace` to replace the position of to-removed node `node` */
    rbtree_node_t *replace = NULL;
    if (rbtree_is_sentinel(tree, node->left) || rbtree_is_sentinel(tree, node->right)) {
        replace = node;
    }
    else {
        /**
         * node has both left and right child,
         * so its successor must be the minmum node in its right sub-tree,
         * and the successor has no left child.
         */
        replace = rbtree_successor(tree, node);
    }

    /** `replace2` is used to replace the position of `replace`*/
    rbtree_node_t *replace2 = NULL;
    if (!rbtree_is_sentinel(tree, replace->left)) {
        replace2 = replace->left;
    }
    else {
        replace2 = replace->right;
    }

    replace2->parent = replace->parent;
    /** use `replace2` to replace node `replace` */
    if (rbtree_is_root(tree, replace)) {
        tree->root = replace2;
    }
    else {
        if (rbtree_is_left_child(replace)) {
            replace->parent->left = replace2;
        }
        else {
            replace->parent->right = replace2;
        }
    }

    int ret = RBTREE_OK;
    int is_replace_black = rbtree_is_black(replace);
    /** use `replace` to replace `node` */
    if (replace != node) {
        ret = rbtree_replace_node(tree, replace, node);

        if (ret != RBTREE_OK) {
            return ret;
        }
    }

    /** if `node` is red, nothing else is needed */
    if (is_replace_black) {
        rbtree_delete_fixup(tree, replace2);
    }

    return RBTREE_OK;
}


int
rbtree_search(rbtree_t *tree,
              rbtree_node_t *value,
              rbtree_search_mode_t mode,
              rbtree_node_t **ret)
{
    rbtree_must(tree != NULL, RBTREE_INVALID_ARG);
    rbtree_must(value != NULL, RBTREE_INVALID_ARG);
    rbtree_must(ret != NULL, RBTREE_INVALID_ARG);
    rbtree_validate_search_mode(mode);

    rbtree_node_t *result = NULL;
    rbtree_node_t *traverse = tree->root;

    while (rbtree_is_sentinel(tree, traverse)) {
        int cmp = tree->compare(traverse, value);

        if (cmp == 0) {
            result = traverse;

            break;
        }
        else if (cmp > 0) {
            if (mode == RBTREE_SEARCH_MODE_GE) {
                result = traverse;
            }

            traverse = traverse->left;
        }
        else {
            if (mode == RBTREE_SEARCH_MODE_LE) {
                result = traverse;
            }

            traverse = traverse->right;
        }
    }

    if (result != NULL) {
        *ret = result;

        return RBTREE_OK;
    }

    return RBTREE_NOT_FOUND;
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
