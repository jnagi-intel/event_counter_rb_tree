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
    INT    ID; 
    INT    Count;
    enum {RED, BLACK} Color;
    struct _RB_TREE_NODE *pLeftChild;
    struct _RB_TREE_NODE *pRightChild;
    struct _RB_TREE_NODE *pParent;
}RB_TREE_NODE, *PRB_TREE_NODE;

// Red Black Tree Context Definition 
typedef struct _RB_TREE_CONTEXT
{
    PRB_TREE_NODE       pMinRbTreeNode;
    PRB_TREE_NODE       pRbTreeNodeArrayList;
    UINT                NumNodesRbTree;
    UINT                RbTreeHeight;
    struct _RB_TREE_FN_TBL
    {
        VOID(*initializeRbTreeNodeArrayList)(struct _RB_TREE_CONTEXT *pRbTreeContext, UINT Length);
        VOID(*insertRbTreeNodeArrayList)(struct _RB_TREE_CONTEXT *pRbTreeContext, INT ID, INT Count, UINT Index);
        VOID(*initializeRbTree)(struct _RB_TREE_CONTEXT *pRbTreeContext);
        PRB_TREE_NODE(*insertRbTreeNode) (struct _RB_TREE_CONTEXT *pRbTreeContext, INT ID, INT Count);
        VOID(*deleteRbTreeNode) (struct _RB_TREE_CONTEXT *pRbTreeContext, PRB_TREE_NODE pRbTreeNode);
        PRB_TREE_NODE(*findRbTreeNode) (struct _RB_TREE_CONTEXT *pRbTreeContext, INT ID);
        PRB_TREE_NODE(*getNextIDRbTreeNode) (struct _RB_TREE_CONTEXT *pRbTreeContext, PRB_TREE_NODE pRbTreeNode);
        PRB_TREE_NODE(*getPrevIDRbTreeNode) (struct _RB_TREE_CONTEXT *pRbTreeContext, PRB_TREE_NODE pRbTreeNode);
    }stRbTreeFnTbl;
}RB_TREE_CONTEXT, *PRB_TREE_CONTEXT;

// Funtion Prototypes
// Following functions can be accessed outside rb_tree.c
PRB_TREE_CONTEXT    createRbTreeContext();
VOID                destroyRbTreeContext(PRB_TREE_CONTEXT *ppRbTreeContext);
#endif 
