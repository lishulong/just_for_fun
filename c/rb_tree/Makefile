CC=gcc
AR=ar

CC_INCLUDE=
CFLAGS+=-c -m64 -std=c11 -g -fPIC
CFLAGS+=$(CC_INCLUDE)

RB_TREE_OBJS=rbtree.o
RB_TREE_DYN_LIB=librbtree.so
RB_TREE_STATIC_LIB=librbtree.a


all: rbtree_dyn_lib rbtree_static_lib rbtree_test

rbtree_dyn_lib: $(RB_TREE_OBJS)
	$(CC) -shared -fPIC -o $(RB_TREE_DYN_LIB) $(RB_TREE_OBJS)

rbtree_static_lib: $(RB_TREE_OBJS)
	$(AR) -rcs $(RB_TREE_STATIC_LIB) $(RB_TREE_OBJS)

rbtree_test: rbtree_test.o $(RB_TREE_OBJS)
	$(CC) $< -o $@ -lcunit
	@echo "run unit test" && ./rbtree_test

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	find . -name "*.o" | xargs rm -f
	rm -f $(RB_TREE_DYN_LIB)
	rm -f $(RB_TREE_STATIC_LIB)
	rm -rf rbtree_test
