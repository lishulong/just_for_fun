#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/Basic.h>
#include <CUnit/TestRun.h>

#include "rbtree.c"

typedef struct test_node_s test_node_t;
struct test_node_s {
    int key;
    rbtree_node_t rbnode;
} test_node_s;


#define rbtree_owner(ptr, type, field) \
    (type *)((uintptr_t)ptr - offsetof(type, field))

int
test_node_compare(rbtree_node_t *na, rbtree_node_t *nb)
{
    test_node_t *ta = rbtree_owner(na, test_node_t, rbnode);
    test_node_t *tb = rbtree_owner(nb, test_node_t, rbnode);

    int akey = ta->key;
    int bkey = tb->key;

    return akey - bkey;
}


void
test_left_rotate(void)
{
    rbtree_t tree;
    rbtree_node_t node = rbtree_null_node(&tree);

    int ret = rbtree_left_rotate(NULL, &node);
    CU_ASSERT(ret == RBTREE_INVALID_ARG);

    ret = rbtree_left_rotate(&tree, NULL);
    CU_ASSERT(ret == RBTREE_INVALID_ARG);

    /** node is sentinel */
    ret = rbtree_left_rotate(&tree, &tree.sentinel);
    CU_ASSERT(ret == RBTREE_INVALID_TOPOLOGY);

    /** node.right is sentinel */
    node.right = &tree.sentinel;
    ret = rbtree_left_rotate(&tree, &node);
    CU_ASSERT(ret == RBTREE_OK);

    /** node is root */
    rbtree_node_t lchild = rbtree_null_node(&tree);
    rbtree_node_t rchild = rbtree_null_node(&tree);
    rbtree_node_t rlchild = rbtree_null_node(&tree);
    rbtree_node_t rrchild = rbtree_null_node(&tree);

    tree.root = &node;

    rbtree_init_node(&tree, &node, &lchild, &rchild, NULL, 0, 0);
    rbtree_init_node(&tree, &rchild, &rlchild, &rrchild, &node, 0, 0);

    ret = rbtree_left_rotate(&tree, &node);
    CU_ASSERT(ret == RBTREE_OK);

    CU_ASSERT(tree.root == &rchild);

    CU_ASSERT(rbtree_is_sentinel(&tree, rchild.parent));
    CU_ASSERT(rchild.left == &node);
    CU_ASSERT(rchild.right == &rrchild);

    CU_ASSERT(node.parent == &rchild);
    CU_ASSERT(node.left == &lchild);
    CU_ASSERT(node.right == &rlchild);

    CU_ASSERT(rrchild.parent == &rchild);
    CU_ASSERT(rrchild.left == &tree.sentinel);
    CU_ASSERT(rrchild.right == &tree.sentinel);

    CU_ASSERT(lchild.parent == &node);
    CU_ASSERT(lchild.left == &tree.sentinel);
    CU_ASSERT(lchild.right == &tree.sentinel);

    CU_ASSERT(rlchild.parent == &node);
    CU_ASSERT(rlchild.left == &tree.sentinel);
    CU_ASSERT(rlchild.right == &tree.sentinel);

    /** node is not root */
    for (int idx = 0; idx < 2; idx++) {
        rbtree_node_t parent = rbtree_null_node(&tree);

        tree.root = &parent;
        if (idx == 0) {
            rbtree_init_node(&tree, &parent, &tree.sentinel, &node, NULL, 0, 0);
        }
        else {
            rbtree_init_node(&tree, &parent, &node, &tree.sentinel, NULL, 0, 0);
        }
        /** idx is 1 or 0 to indicate left child or right child */
        rbtree_init_node(&tree, &node, &lchild, &rchild, &parent, idx, 0);
        rbtree_init_node(&tree, &lchild, &tree.sentinel, &tree.sentinel, &node, 1, 0);
        rbtree_init_node(&tree, &rchild, &rlchild, &rrchild, &node, 0, 0);

        ret = rbtree_left_rotate(&tree, &node);
        CU_ASSERT(ret == RBTREE_OK);

        CU_ASSERT(rbtree_is_root(&tree, &parent) == 1);
        CU_ASSERT(rbtree_is_sentinel(&tree, parent.parent));
        if (idx == 0) {
            CU_ASSERT(parent.left == &tree.sentinel);
            CU_ASSERT(parent.right == &rchild);
        }
        else {
            CU_ASSERT(parent.left == &rchild);
            CU_ASSERT(parent.right == &tree.sentinel);
        }

        CU_ASSERT(rchild.parent == &parent);
        CU_ASSERT(rchild.left == &node);
        CU_ASSERT(rchild.right == &rrchild);

        CU_ASSERT(node.parent == &rchild);
        CU_ASSERT(node.left == &lchild);
        CU_ASSERT(node.right == &rlchild);

        CU_ASSERT(rrchild.parent == &rchild);
        CU_ASSERT(rrchild.left == &tree.sentinel);
        CU_ASSERT(rrchild.right == &tree.sentinel);

        CU_ASSERT(lchild.parent == &node);
        CU_ASSERT(lchild.left == &tree.sentinel);
        CU_ASSERT(lchild.right == &tree.sentinel);

        CU_ASSERT(rlchild.parent == &node);
        CU_ASSERT(rlchild.left == &tree.sentinel);
        CU_ASSERT(rlchild.right == &tree.sentinel);
    }
}


void
test_right_rotate(void)
{
    rbtree_t tree;
    rbtree_node_t node = rbtree_null_node(&tree);

    int ret = rbtree_right_rotate(NULL, &node);
    CU_ASSERT(ret == RBTREE_INVALID_ARG);

    ret = rbtree_right_rotate(&tree, NULL);
    CU_ASSERT(ret == RBTREE_INVALID_ARG);

    /** node is sentinel */
    ret = rbtree_left_rotate(&tree, &tree.sentinel);
    CU_ASSERT(ret == RBTREE_INVALID_TOPOLOGY);

    /** node.left is sentinel */
    node.left = &tree.sentinel;
    ret = rbtree_right_rotate(&tree, &node);
    CU_ASSERT(ret == RBTREE_INVALID_TOPOLOGY);

    /** node is root */
    rbtree_node_t lchild = rbtree_null_node(&tree);
    rbtree_node_t rchild = rbtree_null_node(&tree);
    rbtree_node_t llchild = rbtree_null_node(&tree);
    rbtree_node_t lrchild = rbtree_null_node(&tree);

    tree.root = &node;
    rbtree_init_node(&tree, &node, &lchild, &rchild, NULL, 0, 0);
    rbtree_init_node(&tree, &lchild, &llchild, &lrchild, &node, 1, 0);

    ret = rbtree_right_rotate(&tree, &node);
    CU_ASSERT(ret == RBTREE_OK);

    CU_ASSERT(tree.root == &lchild);

    CU_ASSERT(rbtree_is_sentinel(&tree, lchild.parent));
    CU_ASSERT(lchild.left == &llchild);
    CU_ASSERT(lchild.right == &node);

    CU_ASSERT(llchild.parent == &lchild);
    CU_ASSERT(llchild.left == &tree.sentinel);
    CU_ASSERT(llchild.right == &tree.sentinel);

    CU_ASSERT(node.parent == &lchild);
    CU_ASSERT(node.left == &lrchild);
    CU_ASSERT(node.right == &rchild);

    CU_ASSERT(lrchild.parent == &node);
    CU_ASSERT(lrchild.left == &tree.sentinel);
    CU_ASSERT(lrchild.right == &tree.sentinel);

    CU_ASSERT(rchild.parent == &node);
    CU_ASSERT(rchild.left == &tree.sentinel);
    CU_ASSERT(rchild.right == &tree.sentinel);

    /** node is not root */
    for (int idx = 0; idx < 2; idx++) {
        rbtree_node_t parent = rbtree_null_node(&tree);

        tree.root = &parent;
        if (idx == 0) {
            rbtree_init_node(&tree, &parent, &tree.sentinel, &node, NULL, 0, 0);
        }
        else {
            rbtree_init_node(&tree, &parent, &node, &tree.sentinel, NULL, 0, 0);
        }
        /** idx is 1 or 0 to indicate left child or right child */
        rbtree_init_node(&tree, &node, &lchild, &rchild, &parent, idx, 0);
        rbtree_init_node(&tree, &lchild, &llchild, &lrchild, &node, 1, 0);
        rbtree_init_node(&tree, &rchild, &tree.sentinel, &tree.sentinel, &node, 0, 0);

        ret = rbtree_right_rotate(&tree, &node);
        CU_ASSERT(ret == RBTREE_OK);

        CU_ASSERT(rbtree_is_root(&tree, &parent) == 1);
        CU_ASSERT(rbtree_is_sentinel(&tree, parent.parent));
        if (idx == 0) {
            CU_ASSERT(parent.left == &tree.sentinel);
            CU_ASSERT(parent.right == &lchild);
        }
        else {
            CU_ASSERT(parent.left == &lchild);
            CU_ASSERT(parent.right == &tree.sentinel);
        }

        CU_ASSERT(lchild.parent == &parent);
        CU_ASSERT(lchild.left == &llchild);
        CU_ASSERT(lchild.right == &node);

        CU_ASSERT(llchild.parent == &lchild);
        CU_ASSERT(llchild.left == &tree.sentinel);
        CU_ASSERT(llchild.right == &tree.sentinel);

        CU_ASSERT(node.parent == &lchild);
        CU_ASSERT(node.left == &lrchild);
        CU_ASSERT(node.right == &rchild);

        CU_ASSERT(lrchild.parent == &node);
        CU_ASSERT(lrchild.left == &tree.sentinel);
        CU_ASSERT(lrchild.right == &tree.sentinel);

        CU_ASSERT(rchild.parent == &node);
        CU_ASSERT(rchild.left == &tree.sentinel);
        CU_ASSERT(rchild.right == &tree.sentinel);
    }
}


#define check_node(tree, node, lnode, rnode, pnode, lr, clr) do {         \
    __typeof__((node)) _sentinel = &(tree)->sentinel;                     \
    CU_ASSERT((node)->left   == ((lnode) == NULL ? _sentinel : (lnode))); \
    CU_ASSERT((node)->right  == ((rnode) == NULL ? _sentinel : (rnode))); \
    CU_ASSERT((node)->parent == ((pnode) == NULL ? _sentinel : (pnode))); \
    if ((pnode) != NULL) {                                                \
        CU_ASSERT((node) == ((rbtree_node_t *)(pnode))->lr);              \
    }                                                                     \
    else {                                                                \
        CU_ASSERT((tree)->root == (node));                                \
    }                                                                     \
    CU_ASSERT((node)->color == RBTREE_##clr);                             \
} while (0)


/** according to `introduction to algotithms` */
#define check_left_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15) do {    \
    CU_ASSERT(tree.root == &n7.rbnode);                                              \
    check_node(&tree, &n7.rbnode, &n2.rbnode, &n11.rbnode, NULL, left, BLACK);       \
    check_node(&tree, &n2.rbnode, &n1.rbnode, &n5.rbnode, &n7.rbnode, left, RED);    \
    check_node(&tree, &n11.rbnode, &n8.rbnode, &n14.rbnode, &n7.rbnode, right, RED); \
    check_node(&tree, &n1.rbnode, NULL, NULL, &n2.rbnode, left, BLACK);              \
    check_node(&tree, &n5.rbnode, &n4.rbnode, NULL, &n2.rbnode, right, BLACK);       \
    check_node(&tree, &n8.rbnode, NULL, NULL, &n11.rbnode, left, BLACK);             \
    check_node(&tree, &n14.rbnode, NULL, &n15.rbnode, &n11.rbnode, right, BLACK);    \
    check_node(&tree, &n4.rbnode, NULL, NULL, &n5.rbnode, left, RED);                \
    check_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, right, RED);             \
} while (0)


#define check_right_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15) do {   \
    CU_ASSERT(tree.root == &n7.rbnode);                                              \
    check_node(&tree, &n7.rbnode, &n4.rbnode, &n14.rbnode, NULL, left, BLACK);       \
    check_node(&tree, &n4.rbnode, &n2.rbnode, &n5.rbnode, &n7.rbnode, left, RED);    \
    check_node(&tree, &n14.rbnode, &n8.rbnode, &n15.rbnode, &n7.rbnode, right, RED); \
    check_node(&tree, &n2.rbnode, &n1.rbnode, NULL, &n4.rbnode, left, BLACK);        \
    check_node(&tree, &n5.rbnode, NULL, NULL, &n4.rbnode, right, BLACK);             \
    check_node(&tree, &n8.rbnode, NULL, &n11.rbnode, &n14.rbnode, left, BLACK);      \
    check_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, right, BLACK);           \
    check_node(&tree, &n1.rbnode, NULL, NULL, &n2.rbnode, left, RED);                \
    check_node(&tree, &n11.rbnode, NULL, NULL, &n8.rbnode, right, RED);              \
} while (0)


void
test_insert_fixup(void)
{
    rbtree_t tree;

    /**  test compare function */
    test_node_t a = { .key = 1, .rbnode = rbtree_null_node(&tree), };
    test_node_t b = { .key = 2, .rbnode = rbtree_null_node(&tree), };
    CU_ASSERT(test_node_compare(&a.rbnode, &b.rbnode) < 0);
    CU_ASSERT(test_node_compare(&b.rbnode, &a.rbnode) > 0);
    CU_ASSERT(test_node_compare(&a.rbnode, &a.rbnode) == 0);

    /** prepare nodes according to `introduction to algorithms` */
    test_node_t n1  = { .key = 1,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n2  = { .key = 2,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n4  = { .key = 4,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n5  = { .key = 5,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n7  = { .key = 7,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n8  = { .key = 8,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n11 = { .key = 11, .rbnode = rbtree_null_node(&tree), };
    test_node_t n14 = { .key = 14, .rbnode = rbtree_null_node(&tree), };
    test_node_t n15 = { .key = 15, .rbnode = rbtree_null_node(&tree), };

    /** empty tree */
    rbtree_init(&tree, test_node_compare);
    tree.root = &n1.rbnode;
    int ret = rbtree_insert_fixup(&tree, &n1.rbnode);
    CU_ASSERT(ret == RBTREE_OK);
    CU_ASSERT(tree.root == &n1.rbnode);

    /** left fixup case 3 */
    tree.root = &n11.rbnode;
    rbtree_init_node(&tree, &n11.rbnode, &n7.rbnode,  &n14.rbnode, NULL, 0, 0);
    rbtree_init_node(&tree, &n7.rbnode,  &n2.rbnode,  &n8.rbnode,  &n11.rbnode, 1, 1);
    rbtree_init_node(&tree, &n14.rbnode, NULL, &n15.rbnode, &n11.rbnode, 0, 0);
    rbtree_init_node(&tree, &n2.rbnode,  &n1.rbnode,  &n5.rbnode,  &n7.rbnode,  1, 1);
    rbtree_init_node(&tree, &n8.rbnode,  NULL, NULL, &n7.rbnode,  0, 0);
    rbtree_init_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, 0, 1);
    rbtree_init_node(&tree, &n1.rbnode,  NULL, NULL, &n2.rbnode,  1, 0);
    rbtree_init_node(&tree, &n5.rbnode,  &n4.rbnode,  NULL, &n2.rbnode,  0, 0);
    rbtree_init_node(&tree, &n4.rbnode,  NULL, NULL, &n5.rbnode,  1, 1);

    ret = rbtree_insert_fixup(&tree, &n2.rbnode);
    CU_ASSERT(ret == RBTREE_OK);

    check_left_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15);

    /** left fixup case 2 */
    tree.root = &n11.rbnode;
    rbtree_init_node(&tree, &n11.rbnode, &n2.rbnode, &n14.rbnode, NULL, 0, 0);
    rbtree_init_node(&tree, &n2.rbnode, &n1.rbnode, &n7.rbnode, &n11.rbnode, 1, 1);
    rbtree_init_node(&tree, &n14.rbnode, NULL, &n15.rbnode, &n11.rbnode, 0, 0);
    rbtree_init_node(&tree, &n1.rbnode, NULL, NULL, &n2.rbnode, 1, 0);
    rbtree_init_node(&tree, &n7.rbnode, &n5.rbnode, &n8.rbnode, &n2.rbnode, 0, 1);
    rbtree_init_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, 0, 1);
    rbtree_init_node(&tree, &n5.rbnode, &n4.rbnode, NULL, &n7.rbnode, 1, 0);
    rbtree_init_node(&tree, &n8.rbnode, NULL, NULL, &n7.rbnode, 0, 0);
    rbtree_init_node(&tree, &n4.rbnode, NULL, NULL, &n5.rbnode, 1, 1);

    ret = rbtree_insert_fixup(&tree, &n7.rbnode);
    CU_ASSERT(ret == RBTREE_OK);

    check_left_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15);

    /** left fixup case 1 */
    tree.root = &n11.rbnode;
    rbtree_init_node(&tree, &n11.rbnode, &n2.rbnode, &n14.rbnode, NULL, 0, 0);
    rbtree_init_node(&tree, &n2.rbnode, &n1.rbnode, &n7.rbnode, &n11.rbnode, 1, 1);
    rbtree_init_node(&tree, &n14.rbnode, NULL, &n15.rbnode, &n11.rbnode, 0, 0);
    rbtree_init_node(&tree, &n1.rbnode, NULL, NULL, &n2.rbnode, 1, 0);
    rbtree_init_node(&tree, &n7.rbnode, &n5.rbnode, &n8.rbnode, &n2.rbnode, 0, 0);
    rbtree_init_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, 0, 1);
    rbtree_init_node(&tree, &n5.rbnode, &n4.rbnode, NULL, &n7.rbnode, 1, 1);
    rbtree_init_node(&tree, &n8.rbnode, NULL, NULL, &n7.rbnode, 0, 1);
    rbtree_init_node(&tree, &n4.rbnode, NULL, NULL, &n5.rbnode, 1, 1);

    ret = rbtree_insert_fixup(&tree, &n4.rbnode);
    CU_ASSERT(ret == RBTREE_OK);

    check_left_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15);

    /** right fixup case 3 */
    tree.root = &n4.rbnode;
    rbtree_init_node(&tree, &n4.rbnode, &n2.rbnode, &n7.rbnode, NULL, 0, 0);
    rbtree_init_node(&tree, &n2.rbnode, &n1.rbnode, NULL, &n4.rbnode, 1, 0);
    rbtree_init_node(&tree, &n7.rbnode, &n5.rbnode, &n14.rbnode, &n4.rbnode, 0, 1);
    rbtree_init_node(&tree, &n1.rbnode, NULL, NULL, &n2.rbnode, 1, 1);
    rbtree_init_node(&tree, &n5.rbnode, NULL, NULL, &n7.rbnode, 1, 0);
    rbtree_init_node(&tree, &n14.rbnode, &n8.rbnode, &n15.rbnode, &n7.rbnode, 0, 1);
    rbtree_init_node(&tree, &n8.rbnode, NULL, &n11.rbnode, &n14.rbnode, 1, 0);
    rbtree_init_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, 0, 0);
    rbtree_init_node(&tree, &n11.rbnode, NULL, NULL, &n8.rbnode, 0, 1);

    ret = rbtree_insert_fixup(&tree, &n14.rbnode);
    CU_ASSERT(ret == RBTREE_OK);

    check_right_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15);

    /** right fixup case 2 */
    tree.root = &n4.rbnode;
    rbtree_init_node(&tree, &n4.rbnode, &n2.rbnode, &n14.rbnode, NULL, 0, 0);
    rbtree_init_node(&tree, &n2.rbnode, &n1.rbnode, NULL, &n4.rbnode, 1, 0);
    rbtree_init_node(&tree, &n14.rbnode, &n7.rbnode, &n15.rbnode, &n4.rbnode, 0, 1);
    rbtree_init_node(&tree, &n1.rbnode, NULL, NULL, &n2.rbnode, 1, 1);
    rbtree_init_node(&tree, &n7.rbnode, &n5.rbnode, &n8.rbnode, &n14.rbnode, 1, 1);
    rbtree_init_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, 0, 0);
    rbtree_init_node(&tree, &n5.rbnode, NULL, NULL, &n7.rbnode, 1, 0);
    rbtree_init_node(&tree, &n8.rbnode, NULL, &n11.rbnode, &n7.rbnode, 0, 0);
    rbtree_init_node(&tree, &n11.rbnode, NULL, NULL, &n8.rbnode, 0, 1);

    ret = rbtree_insert_fixup(&tree, &n7.rbnode);
    CU_ASSERT(ret == RBTREE_OK);

    check_right_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15);

    /** right fixup case 1 */
    tree.root = &n4.rbnode;
    rbtree_init_node(&tree, &n4.rbnode, &n2.rbnode, &n14.rbnode, NULL, 0, 0);
    rbtree_init_node(&tree, &n2.rbnode, &n1.rbnode, NULL, &n4.rbnode, 1, 0);
    rbtree_init_node(&tree, &n14.rbnode, &n7.rbnode, &n15.rbnode, &n4.rbnode, 0, 1);
    rbtree_init_node(&tree, &n1.rbnode, NULL, NULL, &n2.rbnode, 1, 1);
    rbtree_init_node(&tree, &n7.rbnode, &n5.rbnode, &n8.rbnode, &n14.rbnode, 1, 0);
    rbtree_init_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, 0, 0);
    rbtree_init_node(&tree, &n5.rbnode, NULL, NULL, &n7.rbnode, 1, 1);
    rbtree_init_node(&tree, &n8.rbnode, NULL, &n11.rbnode, &n7.rbnode, 0, 1);
    rbtree_init_node(&tree, &n11.rbnode, NULL, NULL, &n8.rbnode, 0, 1);

    ret = rbtree_insert_fixup(&tree, &n11.rbnode);
    CU_ASSERT(ret == RBTREE_OK);

    check_right_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15);
}


void
test_insert(void)
{
    rbtree_t tree;
    rbtree_init(&tree, test_node_compare);

    /** prepare nodes according to `introduction to algorithms` */
    test_node_t n1  = { .key = 1,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n2  = { .key = 2,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n4  = { .key = 4,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n5  = { .key = 5,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n7  = { .key = 7,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n8  = { .key = 8,  .rbnode = rbtree_null_node(&tree), };
    test_node_t n11 = { .key = 11, .rbnode = rbtree_null_node(&tree), };
    test_node_t n14 = { .key = 14, .rbnode = rbtree_null_node(&tree), };
    test_node_t n15 = { .key = 15, .rbnode = rbtree_null_node(&tree), };

    /** empty tree */
    int ret = rbtree_insert(&tree, &n11.rbnode);
    CU_ASSERT(ret == RBTREE_OK);
    CU_ASSERT(tree.root == &n11.rbnode);

    /** constuct rbtree manually, left case 1 */
    rbtree_init_node(&tree, &n11.rbnode, &n2.rbnode, &n14.rbnode, NULL, 0, 0);
    rbtree_init_node(&tree, &n2.rbnode, &n1.rbnode, &n7.rbnode, &n11.rbnode, 1, 1);
    rbtree_init_node(&tree, &n14.rbnode, NULL, &n15.rbnode, &n11.rbnode, 0, 0);
    rbtree_init_node(&tree, &n1.rbnode, NULL, NULL, &n2.rbnode, 1, 0);
    rbtree_init_node(&tree, &n7.rbnode, &n5.rbnode, &n8.rbnode, &n2.rbnode, 0, 0);
    rbtree_init_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, 0, 1);
    rbtree_init_node(&tree, &n5.rbnode, NULL, NULL, &n7.rbnode, 1, 1);
    rbtree_init_node(&tree, &n8.rbnode, NULL, NULL, &n7.rbnode, 0, 1);

    ret = rbtree_insert(&tree, &n4.rbnode);
    CU_ASSERT(ret == RBTREE_OK);
    check_left_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15);

    /** construct rbtree manually, right case 1 */
    tree.root = &n4.rbnode;
    rbtree_init_node(&tree, &n4.rbnode, &n2.rbnode, &n14.rbnode, NULL, 0, 0);
    rbtree_init_node(&tree, &n2.rbnode, &n1.rbnode, NULL, &n4.rbnode, 1, 0);
    rbtree_init_node(&tree, &n14.rbnode, &n7.rbnode, &n15.rbnode, &n4.rbnode, 0, 1);
    rbtree_init_node(&tree, &n1.rbnode, NULL, NULL, &n2.rbnode, 1, 1);
    rbtree_init_node(&tree, &n7.rbnode, &n5.rbnode, &n8.rbnode, &n14.rbnode, 1, 0);
    rbtree_init_node(&tree, &n15.rbnode, NULL, NULL, &n14.rbnode, 0, 0);
    rbtree_init_node(&tree, &n5.rbnode, NULL, NULL, &n7.rbnode, 1, 1);
    rbtree_init_node(&tree, &n8.rbnode, NULL, NULL, &n7.rbnode, 0, 1);

    ret = rbtree_insert(&tree, &n11.rbnode);
    CU_ASSERT(ret == RBTREE_OK);
    check_right_final_rbtree(tree, n1, n2, n4, n5, n7, n8, n11, n14, n15);
}

/** test cases for one single suit */
static CU_TestInfo test_rbtree_rotate[] = {
    { "test_left_rotate",  test_left_rotate  },
    { "test_right_rotate", test_right_rotate },
    CU_TEST_INFO_NULL,
};

static CU_TestInfo test_rbtree_insert[] = {
    { "test_insert_fixup", test_insert_fixup },
    { "test_insert",       test_insert       },
    CU_TEST_INFO_NULL,
};

/**
 * test suites
 *
 * each test suite can have its own init and cleanup function.
 */
static CU_SuiteInfo suites[] = {
  {
      "test_rotate",
      NULL,
      NULL,
      NULL,
      NULL,
      test_rbtree_rotate,
  },
  {
      "test_insert",
      NULL,
      NULL,
      NULL,
      NULL,
      test_rbtree_insert,
  },

  CU_SUITE_INFO_NULL,
};


int
main(void)
{
    /** initialize registry */
    CU_ErrorCode err = CU_initialize_registry();
    if (err != CUE_SUCCESS) {
        printf("failed to initialize registry: %d\n", err);

        exit(err);
    }

    /** add test suits and test cases in them */
    CU_ErrorCode error = CU_register_suites(suites);

    /** set run mode */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    /** cleanup */
    CU_cleanup_registry();

    return CU_get_error();
}

