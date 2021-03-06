/**
 * file name: rb_tree.h
 *
 * head file of rb_tree
 */
#ifndef __RB_TREE_H__
#define __RB_TREE_H__

/** error type */
typedef enum rbtree_ret_e rbtree_ret_t;
enum rbtree_ret_e {
    RBTREE_INVALID_ARG       = -1000,
    RBTREE_INVALID_TOPOLOGY  = -999,
    RBTREE_NOT_FOUND         = -998,

    RBTREE_OK = 0,
};

#define rbtree_must(expr, ret_code) do { \
    if (!(expr)) {                       \
        return (ret_code);               \
    }                                    \
} while (0)


#define RBTREE_BLACK 0
#define RBTREE_RED   1

#define rbtree_set_red(node)   ((node)->color = RBTREE_RED)
#define rbtree_set_black(node) ((node)->color = RBTREE_BLACK)
#define rbtree_is_red(node)    ((node)->color)
#define rbtree_is_black(node)  (!rbtree_is_red(node))

#define rbtree_null_node(tree) (rbtree_node_t) { \
    .left   = &(tree)->sentinel,                 \
    .right  = &(tree)->sentinel,                 \
    .parent = &(tree)->sentinel,                 \
    .color  = RBTREE_RED,                        \
}


typedef struct rbtree_node_s rbtree_node_t;
struct rbtree_node_s {
    rbtree_node_t *left;
    rbtree_node_t *right;
    rbtree_node_t *parent;
    int color;
};


/** compare function of two node */
typedef int (*rbtree_compare)(rbtree_node_t *na, rbtree_node_t *nb);

typedef struct rbtree_s rbtree_t;
struct rbtree_s {
    rbtree_node_t *root;
    rbtree_node_t sentinel;
    rbtree_compare compare;
};

typedef enum rbtree_search_mode_e rbtree_search_mode_t;
enum rbtree_search_mode_e {
    RBTREE_SEARCH_MODE_EQ  = 0x01,
    RBTREE_SEARCH_MODE_LE  = 0x02,
    RBTREE_SEARCH_MODE_GE  = 0x03,

    RBTREE_SEARCH_MODE_MAX = 0x04,
};


int
rbtree_search(rbtree_t *tree,
              rbtree_node_t *value,
              rbtree_search_mode_t mode,
              rbtree_node_t **ret);


static inline int
rbtree_is_sentinel(rbtree_t *tree, rbtree_node_t *node)
{
    return (node == &tree->sentinel);
}


static inline int
rbtree_is_root(rbtree_t *tree, rbtree_node_t *node)
{
    return tree->root == node;
}


static inline int
rbtree_is_left_child(rbtree_node_t *node)
{
    return node->parent->left == node;
}


static inline int
rbtree_is_right_child(rbtree_node_t *node)
{
    return node->parent->right == node;
}


static inline rbtree_node_t *
rbtree_get_uncle(rbtree_node_t *node)
{
    return (rbtree_is_left_child(node->parent)) ?
            node->parent->parent->right :
            node->parent->parent->left;
}


#endif
