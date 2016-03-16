//
// This file contains all the header definitions for 
// Red Black Tree 
//

#ifndef _RB_TREE_H_
#define _RB_TREE_H_

#include "Types.h"

// Definitions 
typedef struct _RB_TREE_NODE
{
    UINT    ID; 
    UINT    Count;
    enum {RED, BLACK} Color;
    struct _RB_TREE_NODE *pLeftChild;
    struct _RB_TREE_NODE *pRightChild;
    struct _RB_TREE_NODE *pParent;
}RB_TREE_NODE, *PRB_TREE_NODE;

// Red Black Tree Context Definition 
typedef struct _RB_TREE_CONTEXT
{
    PRB_TREE_NODE       pMinRbTreeNode;
    struct _RB_TREE_FN_TBL
    {
        BOOLEAN(*insertRbTreeNode) (struct _RB_TREE_CONTEXT *pRbTreeContext, UINT ID, UINT Count);
    }RbTreeFnTbl;
}RB_TREE_CONTEXT, *PRB_TREE_CONTEXT;

// Funtion Prototypes
// Following functions can be accessed outside rb_tree.c
PRB_TREE_CONTEXT    createRbTreeContext();
VOID                destroyRbTreeContext(PRB_TREE_CONTEXT *ppRbTreeContext);
#endif 
