#include "avl-tree.h"

static struct avl_node *rotate_left(struct avl_node *parent, struct avl_node *right_child) 
{
    struct avl_node *tmp = right_child->avl_left; 

    parent->avl_right = tmp;
    if (tmp != NULL)
        avl_set_parent(tmp, parent);
    right_child->avl_left = parent;
    avl_set_parent(parent, right_child);

    if (avl_balance(right_child) == AVL_BALANCED) {
        avl_set_balance(parent, AVL_RIGHT_HEAVY);
        avl_set_balance(right_child, AVL_LEFT_HEAVY);
    } else { 
        avl_set_balance(parent, AVL_BALANCED);
        avl_set_balance(right_child, AVL_BALANCED);
    }

    return right_child; 
}

static struct avl_node *rotate_rightleft(struct avl_node *parent, struct avl_node *right_child) 
{
    /* right left grand child */
    struct avl_node *grand_child = right_child->avl_left; 
    struct avl_node *tmp = grand_child->avl_right;

    right_child->avl_left = tmp;
    if (tmp != NULL)
        avl_set_parent(tmp, right_child);
    grand_child->avl_right = right_child;
    avl_set_parent(right_child, grand_child);
    tmp = grand_child->avl_left;
    parent->avl_right = tmp;
    if (tmp != NULL)
        avl_set_parent(tmp, parent);
    grand_child->avl_left = parent;
    avl_set_parent(parent, grand_child);

    if (avl_balance(grand_child) == AVL_RIGHT_HEAVY) {
        avl_set_balance(parent, AVL_LEFT_HEAVY);
        avl_set_balance(right_child, AVL_BALANCED);
    } else if (avl_balance(grand_child) == AVL_BALANCED) { 
        avl_set_balance(parent, AVL_BALANCED);
        avl_set_balance(right_child, AVL_BALANCED);
    } else {
        avl_set_balance(parent, AVL_BALANCED);
        avl_set_balance(right_child, AVL_RIGHT_HEAVY);
    }
    avl_set_balance(grand_child, AVL_BALANCED);

    return grand_child; 
}

static struct avl_node *rotate_right(struct avl_node *parent, struct avl_node *left_child) 
{
    struct avl_node *tmp = left_child->avl_right; 

    parent->avl_left = tmp;
    if (tmp != NULL)
        avl_set_parent(tmp, parent);
    left_child->avl_right = parent;
    avl_set_parent(parent, left_child);

    if (avl_balance(left_child) == AVL_BALANCED) {
        avl_set_balance(parent, AVL_LEFT_HEAVY);
        avl_set_balance(left_child, AVL_RIGHT_HEAVY);
    } else { 
        avl_set_balance(parent, AVL_BALANCED);
        avl_set_balance(left_child, AVL_BALANCED);
    }

    return left_child; 
}

static struct avl_node *rotate_leftright(struct avl_node *parent, struct avl_node *left_child) 
{
    /* left right grand child */
    struct avl_node *grand_child = left_child->avl_right;
    struct avl_node *tmp = grand_child->avl_left;

    left_child->avl_right = tmp;
    if (tmp != NULL)
        avl_set_parent(tmp, left_child);
    grand_child->avl_left = left_child;
    avl_set_parent(left_child, grand_child);
    tmp = grand_child->avl_right;
    parent->avl_left = tmp;
    if (tmp != NULL)
        avl_set_parent(tmp, parent);
    grand_child->avl_right = parent;
    avl_set_parent(parent, grand_child);

    if (avl_balance(grand_child) == AVL_LEFT_HEAVY) {
        avl_set_balance(parent, AVL_RIGHT_HEAVY);
        avl_set_balance(left_child, AVL_BALANCED);
    } else if (avl_balance(grand_child) == AVL_BALANCED) { 
        avl_set_balance(parent, AVL_BALANCED);
        avl_set_balance(left_child, AVL_BALANCED);
    } else {
        avl_set_balance(parent, AVL_BALANCED);
        avl_set_balance(left_child, AVL_LEFT_HEAVY);
    }
    avl_set_balance(grand_child, AVL_BALANCED);

    return grand_child; 
}


void avl_insert_balance(struct avl_node *node, struct avl_root *root)
{
    struct avl_node *parent, *grand_parent, *sub;

    for (parent = avl_parent(node); parent != NULL; parent = avl_parent(node)) {
        /* avl_balance(parent) has to be updated: */
        if (node == parent->avl_right) { 
            if (avl_balance(parent) == AVL_RIGHT_HEAVY) {
                grand_parent = avl_parent(parent); /* Save parent of parent around rotations */
                if (avl_balance(node) == AVL_LEFT_HEAVY)      
                    sub = rotate_rightleft(parent, node); 
                else
                    sub = rotate_left(parent, node);
                /* After rotation adapt parent link */
            } else {
                if (avl_balance(parent) == AVL_LEFT_HEAVY) {
                    avl_set_balance(parent, AVL_BALANCED); /* node’s height increase is absorbed at parent. */
                    break; 
                }
                avl_set_balance(parent, AVL_RIGHT_HEAVY);
                node = parent; /* Height(node) increases by 1 */
                continue;
            }
        } else { /* if (node == parent->avl_left) */
            if (avl_balance(parent) == AVL_LEFT_HEAVY) { 
                grand_parent = avl_parent(parent); /* Save parent of parent around rotations */
                if (avl_balance(node) == AVL_RIGHT_HEAVY)
                    sub = rotate_leftright(parent, node);
                else                           
                    sub = rotate_right(parent, node);
                /* After rotation adapt parent link */
            } else {
                if (avl_balance(parent) == AVL_RIGHT_HEAVY) {
                    avl_set_balance(parent, AVL_BALANCED); /* node’s height increase is absorbed at parent. */
                    break;
                }
                avl_set_balance(parent, AVL_LEFT_HEAVY);
                node = parent; /* Height(node) increases by 1 */
                continue;
            }
        }
        avl_set_parent(sub, grand_parent);
        if (grand_parent != NULL) {
            if (parent == grand_parent->avl_left)
                grand_parent->avl_left = sub;
            else
                grand_parent->avl_right = sub;
        } else {
            root->avl_node = sub;
        }
        break;
    }
}

void avl_erase_balance(struct avl_node *node, struct avl_node *parent, struct avl_root *root)
{
    struct avl_node *grand_parent, *sibling;
    int balance;

    for (; parent != NULL; parent = grand_parent) {
        grand_parent = avl_parent(parent);
        /* avl_balance(parent) has not yet been updated! */
        if (node == parent->avl_left) {
            if (avl_balance(parent) == AVL_RIGHT_HEAVY) { 
                sibling = parent->avl_right;
                balance = avl_balance(sibling);
                if (balance == AVL_LEFT_HEAVY)                     
                    node = rotate_rightleft(parent, sibling); 
                else                           
                    node = rotate_left(parent, sibling); 
                /* After rotation adapt parent link */
            } else {
                if (avl_balance(parent) == AVL_BALANCED) {
                    avl_set_balance(parent, AVL_RIGHT_HEAVY);
                    break;
                }
                node = parent;
                avl_set_balance(node, AVL_BALANCED);
                continue;
            }
        } else { /* if (node == parent->avl_right) */
            if (avl_balance(parent) == AVL_LEFT_HEAVY) {
                sibling = parent->avl_left; 
                balance = avl_balance(sibling);
                if (balance == AVL_RIGHT_HEAVY)
                    node = rotate_leftright(parent, sibling); 
                else                        
                    node = rotate_right(parent, sibling);    
                /* After rotation adapt parent link */
            } else {
                if (avl_balance(parent) == AVL_BALANCED) {
                    avl_set_balance(parent, AVL_LEFT_HEAVY);
                    break;
                }
                node = parent;
                avl_set_balance(node, AVL_BALANCED);
                continue;
            }
        }
        /* After a rotation adapt parent link: */
        /* node is the new root of the rotated subtree */
        avl_set_parent(node, grand_parent);
        if (grand_parent != NULL) {
            if (parent == grand_parent->avl_left)
                grand_parent->avl_left = node;
            else
                grand_parent->avl_right = node;
            if (balance == AVL_BALANCED)
                break; /* Height does not change: Leave the loop */
        } else {
            root->avl_node = node;
        }
    }
}

void avl_erase(struct avl_node *node, struct avl_root *root)
{
    struct avl_node *parent, *child, *old, *tmp;

    if (!node->avl_left) {
        child = node->avl_right;
    } else if (!node->avl_right) {
        child = node->avl_left;
    } else {
        old = node;
        node = old->avl_right;
        while ((tmp = node->avl_left) != NULL) 
            node = tmp;
        parent = avl_parent(node);
        child = node->avl_right;

        if (old != parent) {
           if (child)
               avl_set_parent(child, parent);
           parent->avl_left = child;
           tmp = old->avl_right;
           avl_set_parent(tmp, node);
        } else {
            parent->avl_right = child;
            parent = node;
        }
        tmp = avl_parent(old);
        if (tmp) {
            if (old == tmp->avl_right)
                tmp->avl_right = node;
            else
                tmp->avl_left = node;
        } else
            root->avl_node = node;
        tmp = old->avl_left;
        avl_set_parent(tmp, node);
        *node = *old;
        goto balance;
    }

    if ((parent = avl_parent(node)) != NULL) {
        if (parent->avl_left == node)
            parent->avl_left = child;
        else
            parent->avl_right = child;
    } else {
        root->avl_node = child;
    }
    if (child)
        avl_set_parent(child, parent);

balance:
    if (parent) {
        if (!parent->avl_left && !parent->avl_right) {
            avl_set_balance(parent, AVL_BALANCED);
            child = parent;
            parent = avl_parent(parent);
            if (!parent)
                return;
        }
        avl_erase_balance(child, parent, root);
    }
}

