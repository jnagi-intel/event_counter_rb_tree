//
// This file implements the functions for 
// red black trees
//

#include "RbTree.h"

// Local Function Declarations
BOOLEAN         __insertRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, UINT ID, UINT Count);
BOOLEAN         __deleteRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, UINT ID);
PRB_TREE_NODE   __buildRbTreeNode(UINT ID, UINT Count);
PRB_TREE_NODE   __findRbTreeNode(PRB_TREE_CONTEXT pRbTreeContext, UINT ID);
VOID            __freeRbTreeNode(PRB_TREE_NODE *ppRbTreeNode);

PRB_TREE_CONTEXT createRbTreeContext()
{
    PRB_TREE_CONTEXT    pRbTreeContext = NULL;

    // Allocate memory for Red Black Tree
    pRbTreeContext = (PRB_TREE_CONTEXT)malloc(sizeof(RB_TREE_CONTEXT));

    // Initialize the variables
    pRbTreeContext->pMinRbTreeNode = NULL;

    // Initilize the function table
    pRbTreeContext->RbTreeFnTbl.insertRbTreeNode = __insertRbTreeNode;
    pRbTreeContext->RbTreeFnTbl.deleteRbTreeNode = __deleteRbTreeNode;

    return pRbTreeContext;
}

VOID destroyRbTreeContext(PRB_TREE_CONTEXT *ppRbTreeContext)
{

}

PRB_TREE_NODE __buildRbTreeNode(UINT ID, UINT Count)
{
    PRB_TREE_NODE   pRbTreeNode = NULL;

    // Build a temp node to be inserted in the Red Black Tree 
    pRbTreeNode = (PRB_TREE_NODE)malloc(sizeof(RB_TREE_NODE));
    pRbTreeNode->Count          = Count;
    pRbTreeNode->ID             = ID;
    pRbTreeNode->Color          = RED;
    pRbTreeNode->pLeftChild     = NULL;
    pRbTreeNode->pRightChild    = NULL;
    pRbTreeNode->pParent        = NULL;

    return pRbTreeNode;
}

BOOLEAN __insertRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, UINT ID, UINT Count)
{
    PRB_TREE_NODE   pNewRbTreeNode          = NULL;
    PRB_TREE_NODE   pTempRbTreeNode         = NULL;
    PRB_TREE_NODE   pParentRbTreeNode       = NULL;
    PRB_TREE_NODE   pGrandParentRbTreeNode  = NULL;
    PRB_TREE_NODE   pUncleRbTreeNode        = NULL;
    BOOLEAN         IsTempNodeLeftChild     = FALSE;
    BOOLEAN         IsParentNodeLeftChild   = FALSE;

    // Build the Red Black tree node from ID and Count 
    pNewRbTreeNode = __buildRbTreeNode(ID, Count);
    
    // Insert the Red Black Tree Node like inserting in a Min Tree
    if (pRbTreeContext->pMinRbTreeNode == NULL)
    {
        pRbTreeContext->pMinRbTreeNode = pNewRbTreeNode;
    }
    else
    {
        pTempRbTreeNode = pRbTreeContext->pMinRbTreeNode;
        while (pTempRbTreeNode != NULL)
        {
            if (pNewRbTreeNode->ID <= pTempRbTreeNode->ID)
            {
                if (pTempRbTreeNode->pLeftChild != NULL)
                {
                    pTempRbTreeNode = pTempRbTreeNode->pLeftChild;
                }
                else
                {
                    pTempRbTreeNode->pLeftChild = pNewRbTreeNode;
                    pNewRbTreeNode->pParent     = pTempRbTreeNode;
                    break;
                }
            }
            else
            {
                if (pTempRbTreeNode->pRightChild != NULL)
                {
                    pTempRbTreeNode = pTempRbTreeNode->pRightChild;
                }
                else
                {
                    pTempRbTreeNode->pRightChild    = pNewRbTreeNode;
                    pNewRbTreeNode->pParent         = pTempRbTreeNode;
                    break;
                }
            }
        }
    }

    // Now time to restore to red black property for the tree!

    // Get the relationship between p, pp and gp and color of d
    // p - node to be added
    // pp - parent of the node to be added
    // gp - grand parent of the node to be added 
    // d - other child of gp
    // To classify XYr, 
    //   X - relationship between p and pp (parent)
    //   Y - relationship between pp and gp (grandparent)
    //   z - color of node d (uncle)
    
    pTempRbTreeNode = pNewRbTreeNode;     // Using Temp again for recursion
    
    do
    {
        // Case I : Node Inserted is Root or we have back traversed the tree till root, make it black and done
        if (pTempRbTreeNode == pRbTreeContext->pMinRbTreeNode)
        {
            pRbTreeContext->pMinRbTreeNode->Color = BLACK;
            break;
        }

        // Case II : Parent exists and is black, nothing to be done
        else if (pTempRbTreeNode->pParent && pTempRbTreeNode->pParent->Color == BLACK)
        {
            // Nothing to be done here, all properties still hold! 
            break;
        }

        // For rest of the cases store parent, grand parent and uncle tree nodes
        // and also the relationship
        pParentRbTreeNode       = pTempRbTreeNode->pParent;
        pGrandParentRbTreeNode  = pTempRbTreeNode->pParent->pParent;
        IsTempNodeLeftChild     = pTempRbTreeNode == pParentRbTreeNode->pLeftChild ? TRUE : FALSE;
        IsParentNodeLeftChild   = pParentRbTreeNode == pGrandParentRbTreeNode->pLeftChild ? TRUE : FALSE;
        pUncleRbTreeNode        = IsParentNodeLeftChild ? pGrandParentRbTreeNode->pRightChild : pGrandParentRbTreeNode->pLeftChild;

        // Case III : XYr, uncle is red. color flip will do the trick
        if (pUncleRbTreeNode && pUncleRbTreeNode->Color == RED)
        {
            pParentRbTreeNode->Color        = BLACK;
            pGrandParentRbTreeNode->Color   = RED;
            pUncleRbTreeNode->Color         = BLACK;
            
            pTempRbTreeNode = pGrandParentRbTreeNode;
            continue;
        }

        // Case IV : LLb Rotation
        // Reached here, so that means UncleRbTreeNode is black, 
        // Only care about relationships between node, parent and grandparent
        if (IsTempNodeLeftChild && IsParentNodeLeftChild)
        {
            // Make the Color changes first 
            pParentRbTreeNode->Color        = BLACK;
            pGrandParentRbTreeNode->Color   = RED;

            // Now make the pointer changes due to rotation
            if (pGrandParentRbTreeNode->pParent)
            {
                if (pGrandParentRbTreeNode->pParent->pLeftChild == pGrandParentRbTreeNode)
                {
                    pGrandParentRbTreeNode->pParent->pLeftChild = pParentRbTreeNode;
                }
                else
                {
                    pGrandParentRbTreeNode->pParent->pRightChild = pParentRbTreeNode;
                }
            }
            else
            {
                // Grandparent was the root 
                pRbTreeContext->pMinRbTreeNode = pParentRbTreeNode;
            }

            pGrandParentRbTreeNode->pLeftChild  = pParentRbTreeNode->pRightChild;
            pParentRbTreeNode->pParent          = pGrandParentRbTreeNode->pParent;
            pParentRbTreeNode->pRightChild      = pGrandParentRbTreeNode;
            pGrandParentRbTreeNode->pParent     = pParentRbTreeNode;

            break;
        }
        
        // Case V : LRb Rotation
        if (!IsTempNodeLeftChild && IsParentNodeLeftChild)
        {
            // Make the color changes first 
            pTempRbTreeNode->Color          = BLACK;
            pGrandParentRbTreeNode->Color   = RED;

            // Now make the pointer changes due to rotation 
            if (pGrandParentRbTreeNode->pParent)
            {
                if (pGrandParentRbTreeNode->pParent->pLeftChild == pGrandParentRbTreeNode)
                {
                    pGrandParentRbTreeNode->pParent->pLeftChild = pTempRbTreeNode;
                }
                else
                {
                    pGrandParentRbTreeNode->pParent->pRightChild = pTempRbTreeNode;
                }
            }
            else
            {
                // Grandparent was the root 
                pRbTreeContext->pMinRbTreeNode = pTempRbTreeNode;
            }

            pParentRbTreeNode->pRightChild      = pTempRbTreeNode->pLeftChild;
            pGrandParentRbTreeNode->pLeftChild  = pTempRbTreeNode->pRightChild;

            pTempRbTreeNode->pParent            = pGrandParentRbTreeNode->pParent;
            pTempRbTreeNode->pLeftChild         = pParentRbTreeNode;
            pTempRbTreeNode->pRightChild        = pGrandParentRbTreeNode;
        
            pParentRbTreeNode->pParent          = pTempRbTreeNode;
            pGrandParentRbTreeNode->pParent     = pTempRbTreeNode;

            break;
        }

        // Case VI : RRb Rotation
        if (!IsTempNodeLeftChild && !IsParentNodeLeftChild)
        {
            // Make the Color changes first 
            pParentRbTreeNode->Color        = BLACK;
            pGrandParentRbTreeNode->Color   = RED;

            // Now make the pointer changes due to rotation
            if (pGrandParentRbTreeNode->pParent)
            {
                if (pGrandParentRbTreeNode->pParent->pLeftChild == pGrandParentRbTreeNode)
                {
                    pGrandParentRbTreeNode->pParent->pLeftChild = pParentRbTreeNode;
                }
                else
                {
                    pGrandParentRbTreeNode->pParent->pRightChild = pParentRbTreeNode;
                }
            }
            else
            {
                // Grandparent was the root 
                pRbTreeContext->pMinRbTreeNode = pParentRbTreeNode;
            }

            pGrandParentRbTreeNode->pRightChild = pParentRbTreeNode->pLeftChild;
            pParentRbTreeNode->pParent          = pGrandParentRbTreeNode->pParent;
            pParentRbTreeNode->pLeftChild       = pGrandParentRbTreeNode;
            pGrandParentRbTreeNode->pParent     = pParentRbTreeNode;

            break;
        }

        // Case VII : RLb Rotation
        if (IsTempNodeLeftChild && !IsParentNodeLeftChild)
        {
            // Make the color changes first 
            pTempRbTreeNode->Color          = BLACK;
            pGrandParentRbTreeNode->Color   = RED;

            // Now make the pointer changes due to rotation 
            if (pGrandParentRbTreeNode->pParent)
            {
                if (pGrandParentRbTreeNode->pParent->pLeftChild == pGrandParentRbTreeNode)
                {
                    pGrandParentRbTreeNode->pParent->pLeftChild = pTempRbTreeNode;
                }
                else
                {
                    pGrandParentRbTreeNode->pParent->pRightChild = pTempRbTreeNode;
                }
            }
            else
            {
                // Grandparent was the root 
                pRbTreeContext->pMinRbTreeNode = pTempRbTreeNode;
            }

            pParentRbTreeNode->pLeftChild       = pTempRbTreeNode->pRightChild;
            pGrandParentRbTreeNode->pRightChild = pTempRbTreeNode->pLeftChild;

            pTempRbTreeNode->pParent        = pGrandParentRbTreeNode->pParent;
            pTempRbTreeNode->pRightChild    = pParentRbTreeNode;
            pTempRbTreeNode->pLeftChild     = pGrandParentRbTreeNode;

            pParentRbTreeNode->pParent      = pTempRbTreeNode;
            pGrandParentRbTreeNode->pParent = pTempRbTreeNode;

            break;
        }

        // Phew !! 
        // Should never reach here, leave a print to catch the error 
        printf("__insertRbTreeNode: Restoring Red Black Property, Illegal scenario, Exit!");
        return FALSE;

    } while (TRUE);

    return TRUE;
}

PRB_TREE_NODE __findRbTreeNode(PRB_TREE_CONTEXT pRbTreeContext, UINT ID)
{
    PRB_TREE_NODE    pTempRbTreeNode = NULL;

    if (pRbTreeContext->pMinRbTreeNode)
    {
        // Verifying that the root of the tree exists, now recurse!
        pTempRbTreeNode = pRbTreeContext->pMinRbTreeNode;
        while (pTempRbTreeNode != NULL)
        {
            if (ID == pTempRbTreeNode->ID)
            {
                break;
            }
            else if (ID <= pTempRbTreeNode->ID)
            {
                pTempRbTreeNode = pTempRbTreeNode->pLeftChild;
            }
            else
            {
                pTempRbTreeNode = pTempRbTreeNode->pRightChild;
            }
        }
    }

    return pTempRbTreeNode;
}

BOOLEAN __deleteRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, UINT ID)
{
    PRB_TREE_NODE   pRbTreeNode = NULL;

    // get the Tree Node with the ID
    pRbTreeNode = __findRbTreeNode(pRbTreeContext, ID);

    if (!pRbTreeNode)
    {
        // Making a check that the node was found
        printf("__deleteRbTreeNode: Node with the ID %u not found\r\n", ID);
        return FALSE;
    }

    // Check if its a degree 1 node or degree 2 node
    if (pRbTreeNode->pLeftChild && pRbTreeNode->pRightChild)
    {
        // Degree 2 Node
        // Convert this to a degree 0 node by replacing with the largest node in the left subtree 


    }
    else if (pRbTreeNode->pLeftChild && !pRbTreeNode->pRightChild)
    {
        // Degree 1 Node

    }
    else if (!pRbTreeNode->pLeftChild && pRbTreeNode->pRightChild)
    {
        // Degree 1 Node
    }
    else
    {
        // Degree 0 node
        if (pRbTreeNode->Color == RED)
        {
            // if its a red node, no rebalancing needed 
            __freeRbTreeNode(&pRbTreeNode);
        }
        else
        {
            // Removing a black leaf
        }
    }

    return TRUE;
}

VOID __freeRbTreeNode(PRB_TREE_NODE *ppRbTreeNode)
{
    // Make the pointers NULL and free up the Tree Node pointer 
    if (*ppRbTreeNode)
    {
        (*ppRbTreeNode)->pLeftChild     = NULL;
        (*ppRbTreeNode)->pRightChild    = NULL;
        (*ppRbTreeNode)->pParent        = NULL;

        free(*ppRbTreeNode);
        *ppRbTreeNode = NULL;
    }
}


