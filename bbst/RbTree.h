//
// This file contains all the header definitions for 
// Red Black Tree 
//

#ifndef _RB_TREE_H_
#define _RB_TREE_H_

// Definitions 
typedef struct _RB_TREE_NODE
{
    unsigned int ID; 
    unsigned int count;
    struct _RB_TREE_NODE *lchild;
    struct _RB_TREE_NODE *rchild;
}RB_TREE_NODE, *PRB_TREE_NODE;

// Funtion Prototypes
// Following functions can be accessed outside rb_tree.c

#endif 
