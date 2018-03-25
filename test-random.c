#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "avl-tree.h"
#include <errno.h>

#define CHECK_INSERT 1    // "插入"动作的检测开关(0，关闭；1，打开)
#define CHECK_DELETE 1    // "删除"动作的检测开关(0，关闭；1，打开)
#define LENGTH(a) ( (sizeof(a)) / (sizeof(a[0])) )

typedef int Type;

struct my_node {
    struct avl_node avl_node;    // 红黑树节点
    Type key;                // 键值
    // ... 用户自定义的数据
};

/*
 * 查找"红黑树"中键值为key的节点。没找到的话，返回NULL。
 */
struct my_node *my_search(struct avl_root *root, Type key)
{
    struct avl_node *node = root->avl_node;

    while (node!=NULL)
    {
        struct my_node *mynode = container_of(node, struct my_node, avl_node);

        if (key < mynode->key)
            node = node->avl_left;
        else if (key > mynode->key)
            node = node->avl_right;
        else
            return mynode;
    }
    
    return NULL;
}

/*
 * 将key插入到红黑树中。插入成功，返回0；失败返回-1。
 */
int my_insert(struct avl_root *root, Type key)
{
    struct my_node *mynode; // 新建结点
    struct avl_node **tmp = &(root->avl_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*tmp)
    {
        struct my_node *my = container_of(*tmp, struct my_node, avl_node);

        parent = *tmp;
        if (key < my->key)
            tmp = &((*tmp)->avl_left);
        else if (key > my->key)
            tmp = &((*tmp)->avl_right);
        else
            return -1;
    }

    // 如果新建结点失败，则返回。
    errno = 0;
    if ((mynode=malloc(sizeof(struct my_node))) == NULL)
        return -1; 
    mynode->key = key;

    /* Add new node and rebalance tree. */
    avl_link_node(&mynode->avl_node, parent, tmp);
    avl_insert_balance(&mynode->avl_node, root);

    return 0;
}

/* 
 * 删除键值为key的结点
 */
void my_delete(struct avl_root *root, Type key)
{
    struct my_node *mynode;

    // 在红黑树中查找key对应的节点mynode
    if ((mynode = my_search(root, key)) == NULL)
        return ;

    // 从红黑树中删除节点mynode
    avl_erase(&mynode->avl_node, root);
    free(mynode);
}

/*
 * "reverse middle traversal" : visit right-child first, and then self with left-child last.
 */
static void avl_rmid_tra(struct avl_node *node, void (*visit)(void *, unsigned),
                         int visit_null)
{
  struct {
      struct avl_node *avl_node;
      unsigned height;
  } *stk, *new_stk;
  size_t sz = AVL_DEFAULT_STACK_SIZE, cnt = 0;
  unsigned h = 0;

  if ((stk = malloc(AVL_DEFAULT_STACK_SIZE * sizeof(*stk))) == NULL) {
      perror("malloc");
      return;
  }

  while (1) {
    if (node) {
        /* visit right-child first, and push node to stack temporarily */
        if (cnt >= sz) {
            sz += AVL_DEFAULT_STACK_SIZE;
            if ((new_stk = realloc(stk, sz * sizeof (*stk))) == NULL) {
                perror("realloc");
                goto end_func;
            }
            stk = new_stk;
        }
        stk[cnt].avl_node = node;
        stk[cnt++].height = h;
        node = node->avl_right;
        h++;
        continue;
    } else {
        if (visit_null)
            visit(node, h);
        if (cnt == 0) /* stack is empty */
            break;
        else {
            node = stk[--cnt].avl_node;
            h = stk[cnt].height;
        }
    }
    visit(node, h);
    node = node->avl_left;
    h++;
  }
end_func:
  free(stk);
}

static void my_print(void *vp, unsigned h)
{
    struct my_node *my_node;
    int i;
    
    h *= 5;
    for (i = 0; i < h; i++)
        printf(" ");
    if (vp) {
        my_node = avl_entry(vp, struct my_node, avl_node);
        printf("%d\n", my_node->key);
    } else {
        printf("-\n");
    }
}

static void print_avl_tree(struct avl_root *root)
{
    avl_rmid_tra(root->avl_node, my_print, 1);
    fflush(stdout);
}

static void avl_mid_tra(struct avl_root *root, void (*visit)(void *))
{
    struct avl_node *node = root->avl_node, **stk, **new_stk;
    size_t sz = AVL_DEFAULT_STACK_SIZE, cnt = 0;

    if ((stk = malloc(sz * sizeof(*stk))) == NULL) {
        perror("malloc");
        return;
    }

    while (1) {
        if (node) {
            if (node->avl_left) {
                if (cnt >= sz) {
                    sz += AVL_DEFAULT_STACK_SIZE;
                    if ((new_stk = realloc(stk, sz * sizeof(*stk))) == NULL) {
                        perror("realloc");
                        goto end_func;
                    } else
                        stk = new_stk;
                }
                stk[cnt++] = node;
                node = node->avl_left;
                continue;
            }
        } else if (cnt) {
            node = stk[--cnt];
        } else {
            break;
        }
        visit(node);
        node = node->avl_right;
    }
end_func:
    free(stk);
}

static void visit_my_node(void *vnode)
{
    struct my_node *my_node = avl_entry(vnode, struct my_node, avl_node);

    printf("%d\n", my_node->key);
}

static void comp_my_node(void *vnode)
{
    static int i = -1;
    struct my_node *my_node = avl_entry(vnode, struct my_node, avl_node);

    if (i > my_node->key)
        printf("not sorted.\n");
    i = my_node->key;
}

int main()
{
#define NELE 2048
#define DEL_SIZ 20
    int i, j;
    struct avl_root mytree = { NULL };
    Type a[DEL_SIZ], r;
    int in_del_stk = 0;

    srand(time(NULL));

    for (i = 0; i < NELE; i++)
    {
        r = rand();
        j = my_insert(&mytree, r);
        if (j == -1 && errno) {
            perror("malloc");
            return 1;
        }
        if (j == 0 && in_del_stk < DEL_SIZ && rand() < RAND_MAX / 6)
            a[in_del_stk++] = r;
        if (in_del_stk && rand() < RAND_MAX / 3)
            my_delete(&mytree, a[--in_del_stk]);
    }
    avl_mid_tra(&mytree, comp_my_node);
    
    return 0;
}
