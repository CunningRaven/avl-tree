#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stddef.h>
#include <stdlib.h>

struct avl_node {
    unsigned long avl_parent_balance;
#define AVL_BALANCED 0
#define AVL_LEFT_HEAVY 1
#define AVL_RIGHT_HEAVY 2
    struct avl_node *avl_right;
    struct avl_node *avl_left;
} __attribute__((aligned(sizeof(long))));

struct avl_root {
    struct avl_node *avl_node;
};

#define avl_parent(a) ((struct avl_node *)((a)->avl_parent_balance & ~3))
#define avl_balance(a) ((a)->avl_parent_balance & 3)
#define avl_is_balanced(a) (!avl_balance(a))
#define avl_is_left_heavy(a) (avl_balance(a) == AVL_LEFT_HEAVY)
#define avl_is_right_heavy(a) (avl_balance(a) == AVL_RIGHT_HEAVY)
#define avl_set_parent(a, p) \
    ( (a)->avl_parent_balance = ((a)->avl_parent_balance & 3) \
                                | ((unsigned long)(p)) )
#define avl_set_balance(a, b) \
    ( (a)->avl_parent_balance = ((a)->avl_parent_balance & ~3) \
                                | (b))

#define container_of(ptr, type, member) \
    ((type *) ((char *)ptr - offsetof(type, member)))
#define avl_entry(ptr, type, member) container_of(ptr, type, member)

static inline void avl_init_node(struct avl_node *avl)
{
    avl->avl_parent_balance = 0;
    avl->avl_right = NULL;
    avl->avl_left = NULL;
}

static inline void avl_link_node(struct avl_node *node,
        struct avl_node *parent, struct avl_node **avl_link)
{
	node->avl_parent_balance = (unsigned long )parent;
	node->avl_left = node->avl_right = NULL;

	*avl_link = node;
}

extern void avl_insert_balance(struct avl_node *, struct avl_root *);
void avl_erase_balance(struct avl_node *node, struct avl_node *parent, struct avl_root *root);
extern void avl_erase(struct avl_node *, struct avl_root *);

#define AVL_DEFAULT_STACK_SIZE 10 // default size of the stack used in traversal functions

#endif
