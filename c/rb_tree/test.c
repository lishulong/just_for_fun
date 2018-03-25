#include <stdlib.h>

#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <CUnit/Basic.h>
#include <CUnit/TestRun.h>

#include "rbtree.c"


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

    rbtree_init_node(&tree, &node, &lchild, &rchild, NULL, 0);
    rbtree_init_node(&tree, &rchild, &rlchild, &rrchild, &node, 0);

    ret = rbtree_left_rotate(&tree, &node);
    CU_ASSERT(ret == RBTREE_OK);

    CU_ASSERT(tree.root == &rchild);

    CU_ASSERT(rchild.parent == NULL);
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
            rbtree_init_node(&tree, &parent, &tree.sentinel, &node, NULL, 0);
        }
        else {
            rbtree_init_node(&tree, &parent, &node, &tree.sentinel, NULL, 0);
        }
        /** idx is 1 or 0 to indicate left child or right child */
        rbtree_init_node(&tree, &node, &lchild, &rchild, &parent, idx);
        rbtree_init_node(&tree, &lchild, &tree.sentinel, &tree.sentinel, &node, 1);
        rbtree_init_node(&tree, &rchild, &rlchild, &rrchild, &node, 0);

        ret = rbtree_left_rotate(&tree, &node);
        CU_ASSERT(ret == RBTREE_OK);

        CU_ASSERT(rbtree_is_root(&tree, &parent) == 1);
        CU_ASSERT(parent.parent == NULL);
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
    rbtree_init_node(&tree, &node, &lchild, &rchild, NULL, 0);
    rbtree_init_node(&tree, &lchild, &llchild, &lrchild, &node, 1);

    ret = rbtree_right_rotate(&tree, &node);
    CU_ASSERT(ret == RBTREE_OK);

    CU_ASSERT(tree.root == &lchild);

    CU_ASSERT(lchild.parent == NULL);
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
            rbtree_init_node(&tree, &parent, &tree.sentinel, &node, NULL, 0);
        }
        else {
            rbtree_init_node(&tree, &parent, &node, &tree.sentinel, NULL, 0);
        }
        /** idx is 1 or 0 to indicate left child or right child */
        rbtree_init_node(&tree, &node, &lchild, &rchild, &parent, idx);
        rbtree_init_node(&tree, &lchild, &llchild, &lrchild, &node, 1);
        rbtree_init_node(&tree, &rchild, &tree.sentinel, &tree.sentinel, &node, 0);

        ret = rbtree_right_rotate(&tree, &node);
        CU_ASSERT(ret == RBTREE_OK);

        CU_ASSERT(rbtree_is_root(&tree, &parent) == 1);
        CU_ASSERT(parent.parent == NULL);
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

/** test cases for one single suit */
static CU_TestInfo test_rbtree_rotate[] = {
    { "test_left_rotate", test_left_rotate},
    { "test_right_rotate", test_right_rotate },
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

