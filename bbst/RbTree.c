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
BOOLEAN         __deleteDegree1RbTreeNode(PRB_TREE_CONTEXT pRbTreeContext, PRB_TREE_NODE pRbTreeNode, PRB_TREE_NODE pChildRbTreeNode);

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
        if (pTempRbTreeNode->pParent && pTempRbTreeNode->pParent->Color == BLACK)
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
        printf("__insertRbTreeNode: Restoring Red Black Property, Illegal scenario, Exit!\r\n");
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
    PRB_TREE_NODE   pRbTreeNode             = NULL;
    PRB_TREE_NODE   pMaxSubTreeRbTreeNode   = NULL;
    RB_TREE_NODE    TempRbTreeNode          = { 0 };

    // Get the Tree Node with the ID
    pRbTreeNode = __findRbTreeNode(pRbTreeContext, ID);

    if (!pRbTreeNode)
    {
        // Making a check that the node was found
        printf("__deleteRbTreeNode: Node with the ID %u not found\r\n", ID);
        return FALSE;
    }

    // Check if its a degree 0/1/2 node
    if (pRbTreeNode->pLeftChild && pRbTreeNode->pRightChild)
    {
        // Degree 2 Node
        // Convert this to a degree 0 or degree 1 node by replacing with the largest node in the left subtree 
        pMaxSubTreeRbTreeNode = pRbTreeNode->pLeftChild;
        while (pMaxSubTreeRbTreeNode->pRightChild != NULL)
        {
            pMaxSubTreeRbTreeNode = pMaxSubTreeRbTreeNode->pRightChild;
        }
        
        // Exchange the nodes and preserve the color
        TempRbTreeNode.ID             = pMaxSubTreeRbTreeNode->ID;
        TempRbTreeNode.Count          = pMaxSubTreeRbTreeNode->Count;
        pMaxSubTreeRbTreeNode->ID       = pRbTreeNode->ID;
        pMaxSubTreeRbTreeNode->Count    = pRbTreeNode->Count;
        pRbTreeNode->ID                 = TempRbTreeNode.ID;
        pRbTreeNode->Count              = TempRbTreeNode.Count;

        // Now check whether this is a Degree 0 or Degree 1 node
        pRbTreeNode = pMaxSubTreeRbTreeNode;
    }
    
    // Degree 2 nodes after exchange will also enter here
    if (pRbTreeNode->pLeftChild && !pRbTreeNode->pRightChild)
    {
        // Degree 1 node, adjust the pointers 
        if (pRbTreeNode->pParent)
        {
            if (pRbTreeNode->pParent->pLeftChild == pRbTreeNode)
            {
                pRbTreeNode->pParent->pLeftChild = pRbTreeNode->pLeftChild;
            }
            else
            {
                pRbTreeNode->pParent->pRightChild = pRbTreeNode->pLeftChild;
            }
        }
        pRbTreeNode->pLeftChild->pParent = pRbTreeNode->pParent;

        __deleteDegree1RbTreeNode(pRbTreeContext, pRbTreeNode, pRbTreeNode->pLeftChild);
    }
    else if (!pRbTreeNode->pLeftChild && pRbTreeNode->pRightChild)
    {
        // Degree 1 node, adjust the pointers 
        if (pRbTreeNode->pParent)
        {
            if (pRbTreeNode->pParent->pLeftChild == pRbTreeNode)
            {
                pRbTreeNode->pParent->pLeftChild = pRbTreeNode->pRightChild;
            }
            else
            {
                pRbTreeNode->pParent->pRightChild = pRbTreeNode->pRightChild;
            }
        }
        pRbTreeNode->pRightChild->pParent = pRbTreeNode->pParent;

        __deleteDegree1RbTreeNode(pRbTreeContext, pRbTreeNode, pRbTreeNode->pRightChild);
    }
    else
    {
        // Degree 0 node
        // Adjust the pointers first
        if (pRbTreeNode->pParent)
        {
            if (pRbTreeNode->pParent->pLeftChild == pRbTreeNode)
            {
                pRbTreeNode->pParent->pLeftChild = NULL;
            }
            else
            {
                pRbTreeNode->pParent->pRightChild = NULL;
            }
        }

        // Similar to deleting from a degree 1 node with y as NULL 
        __deleteDegree1RbTreeNode(pRbTreeContext, pRbTreeNode, NULL);
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

BOOLEAN __deleteDegree1RbTreeNode(PRB_TREE_CONTEXT pRbTreeContext, PRB_TREE_NODE pRbTreeNode, PRB_TREE_NODE pChildRbTreeNode)
{
    PRB_TREE_NODE   pTempRbTreeNode                         = NULL;
    PRB_TREE_NODE   pSiblingRbTreeNode                      = NULL;
    PRB_TREE_NODE   pParentRbTreeNode                       = NULL;
    PRB_TREE_NODE   pSiblingRedChildRbTreeNode              = NULL;
    PRB_TREE_NODE   pSiblingRightChildRbTreeNode            = NULL;
    PRB_TREE_NODE   pSiblingRightChildRedChildRbTreeNode    = NULL;
    PRB_TREE_NODE   pSiblingLeftChildRedChildRbTreeNode     = NULL;
    PRB_TREE_NODE   pSiblingLeftChildRbTreeNode             = NULL;
    BOOLEAN         IsTempNodeLeftChild                     = FALSE;
    UINT            NumSiblingRedChildren                   = 0;
    UINT            NumSiblingRightChildRedChildren         = 0;
    UINT            NumSiblingLeftChildRedChildren          = 0;
    
    // if removed node is red, free up the node and done! 
    // else define y or pChildRbTreeNode to be root of the deficient subtree and 
    // py to be the parent of y
    if (pRbTreeNode->Color == RED)
    {
        __freeRbTreeNode(&pRbTreeNode);
    }
    else
    {
        // Special case when y is NULL 
        if (pChildRbTreeNode == NULL)
        {
            pParentRbTreeNode = pRbTreeNode->pParent;
        }

        __freeRbTreeNode(&pRbTreeNode);

        // Simple case handled first
        // removed node is black, but y is red
        if (pChildRbTreeNode && pChildRbTreeNode->Color == RED)
        {
            // Color this node black and done! 
            pChildRbTreeNode->Color = BLACK;
        }
        else
        {
            // Complex Case, Both the node removed and Child (y) were black
            pTempRbTreeNode = pChildRbTreeNode;
            do
            {
                // if y is the root, then the entire tree is deficient, so done
                if (pTempRbTreeNode && pTempRbTreeNode->pParent == NULL)
                {
                    pRbTreeContext->pMinRbTreeNode = pTempRbTreeNode;
                    break;
                }

                // Storing some values to decide case 
                if (pTempRbTreeNode)
                {
                    pParentRbTreeNode = pTempRbTreeNode->pParent;
                    pSiblingRbTreeNode = (pTempRbTreeNode->pParent->pLeftChild == pTempRbTreeNode) ? pTempRbTreeNode->pRightChild : pTempRbTreeNode->pLeftChild;;
                    IsTempNodeLeftChild = (pTempRbTreeNode->pParent->pLeftChild == pTempRbTreeNode) ? TRUE : FALSE;
                }
                else
                {
                    // This would be a case where black leaf is being removed
                    if (pParentRbTreeNode->pLeftChild != NULL)
                    {
                        pSiblingRbTreeNode = pParentRbTreeNode->pLeftChild;
                        IsTempNodeLeftChild = FALSE;
                    }
                    else if (pParentRbTreeNode->pRightChild != NULL)
                    {
                        pSiblingRbTreeNode = pParentRbTreeNode->pRightChild;
                        IsTempNodeLeftChild = TRUE;
                    }
                }
                
                if (pSiblingRbTreeNode->pLeftChild && pSiblingRbTreeNode->pLeftChild->Color == RED &&
                    pSiblingRbTreeNode->pRightChild && pSiblingRbTreeNode->pRightChild->Color == RED)
                {
                    NumSiblingRedChildren = 2;
                }
                else if ((pSiblingRbTreeNode->pLeftChild && pSiblingRbTreeNode->pLeftChild->Color == RED) ||
                    (pSiblingRbTreeNode->pRightChild && pSiblingRbTreeNode->pRightChild->Color == RED))
                {
                    NumSiblingRedChildren = 1;
                }
                else
                {
                    NumSiblingRedChildren = 0;
                }
                
                // Notation Xcn where 
                // X is the relationship between Temp and Parent - IsTempNodeLeftChild 
                // c defines the color of Sibling
                // n is the num of red children of sibling - NumSiblingRedChildren

                // Lets begin !!! 

                // Rb0/Lb0 and Parent is BLACK
                if (pSiblingRbTreeNode->Color == BLACK && NumSiblingRedChildren == 0 && pParentRbTreeNode->Color == BLACK)
                {
                    // Change the Color of Sibling to red and now Parent is the new root of deficient sub tree 
                    pSiblingRbTreeNode->Color = RED;
                    pTempRbTreeNode = pParentRbTreeNode;

                    continue;
                }

                // Rb0/Lb0 and Parent is RED
                if (pSiblingRbTreeNode->Color == BLACK && NumSiblingRedChildren == 0 && pParentRbTreeNode->Color == RED)
                {
                    // Flip the colors of Sibling and parent, and done! 
                    pSiblingRbTreeNode->Color = RED;
                    pParentRbTreeNode->Color = BLACK;

                    break;
                }

                // Rb1 case 1 Sibling's left child is Red
                if (!IsTempNodeLeftChild && pSiblingRbTreeNode->Color == BLACK && NumSiblingRedChildren == 1 && 
                    pSiblingRbTreeNode->pLeftChild && pSiblingRbTreeNode->pLeftChild->Color == RED)
                {
                    // This will lead to an LL rotation
                    pSiblingRbTreeNode->pLeftChild->Color = BLACK;

                    pParentRbTreeNode->pLeftChild = pSiblingRbTreeNode->pRightChild;
                    pSiblingRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pSiblingRbTreeNode->pRightChild = pParentRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRbTreeNode;
                    pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;

                    if (pSiblingRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingRbTreeNode;
                    }

                    break;
                }

                // Lb1 case 1 Sibling's Right child is Red
                if (IsTempNodeLeftChild && pSiblingRbTreeNode->Color == BLACK && NumSiblingRedChildren == 1 && 
                    pSiblingRbTreeNode->pRightChild && pSiblingRbTreeNode->pRightChild->Color == RED)
                {
                    // This will lead to an RR rotation
                    pSiblingRbTreeNode->pRightChild->Color = BLACK;

                    pParentRbTreeNode->pRightChild = pSiblingRbTreeNode->pLeftChild;
                    pSiblingRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pSiblingRbTreeNode->pLeftChild = pParentRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRbTreeNode;
                    pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;

                    if (pSiblingRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingRbTreeNode;
                    }

                    break;
                }

                // Rb1 case 2 Sibling's right child is red or Rb2
                if ((!IsTempNodeLeftChild && pSiblingRbTreeNode->Color == BLACK && NumSiblingRedChildren == 1 && 
                    pSiblingRbTreeNode->pRightChild && pSiblingRbTreeNode->pRightChild->Color == RED) ||
                     (!IsTempNodeLeftChild && pSiblingRbTreeNode->Color == BLACK && NumSiblingRedChildren == 2))
                {
                    // This will lead to a LR Rotation
                    pSiblingRbTreeNode->pRightChild->Color = pParentRbTreeNode->Color;
                    pParentRbTreeNode->Color = BLACK;

                    pSiblingRedChildRbTreeNode = pSiblingRbTreeNode->pRightChild;
                    pSiblingRedChildRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pSiblingRbTreeNode->pRightChild = pSiblingRedChildRbTreeNode->pLeftChild;
                    pParentRbTreeNode->pLeftChild = pSiblingRedChildRbTreeNode->pRightChild;
                    pSiblingRedChildRbTreeNode->pLeftChild = pSiblingRbTreeNode;
                    pSiblingRedChildRbTreeNode->pRightChild = pParentRbTreeNode;
                    pSiblingRbTreeNode->pRightChild->pParent = pSiblingRbTreeNode;
                    pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;
                    pSiblingRbTreeNode->pParent = pSiblingRedChildRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRedChildRbTreeNode;

                    if (pSiblingRedChildRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingRedChildRbTreeNode;
                    }
                    
                    break;
                }

                // Lb1 case 2 Sibling's left child is red or Lb2
                if ((IsTempNodeLeftChild && pSiblingRbTreeNode->Color == BLACK && NumSiblingRedChildren == 1 && 
                    pSiblingRbTreeNode->pLeftChild && pSiblingRbTreeNode->pLeftChild->Color == RED) ||
                    (IsTempNodeLeftChild && pSiblingRbTreeNode->Color ==BLACK && NumSiblingRedChildren == 2))
                {
                    // This will lead to a RL Rotation
                    pSiblingRbTreeNode->pLeftChild->Color = pParentRbTreeNode->Color;
                    pParentRbTreeNode->Color = BLACK;

                    pSiblingRedChildRbTreeNode = pSiblingRbTreeNode->pLeftChild;
                    pSiblingRedChildRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pSiblingRbTreeNode->pLeftChild = pSiblingRedChildRbTreeNode->pRightChild;
                    pParentRbTreeNode->pRightChild = pSiblingRedChildRbTreeNode->pLeftChild;
                    pSiblingRedChildRbTreeNode->pRightChild = pSiblingRbTreeNode;
                    pSiblingRedChildRbTreeNode->pLeftChild = pParentRbTreeNode;
                    pSiblingRbTreeNode->pLeftChild->pParent = pSiblingRbTreeNode;
                    pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;
                    pSiblingRbTreeNode->pParent = pSiblingRedChildRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRedChildRbTreeNode;

                    if (pSiblingRedChildRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingRedChildRbTreeNode;
                    }
                    
                    break;
                }

                // Now if the color of the Sibling is red, then define it as 
                // Rrn where n is the number of red children of v's right child
                // Lrn where n is the number of red children of v's left child
                if (!IsTempNodeLeftChild)
                {
                    if (pSiblingRbTreeNode->pRightChild->pLeftChild && pSiblingRbTreeNode->pRightChild->pLeftChild->Color == RED &&
                        pSiblingRbTreeNode->pRightChild->pRightChild && pSiblingRbTreeNode->pRightChild->pRightChild->Color == RED)
                    {
                        NumSiblingRightChildRedChildren = 2;
                    }
                    else if ((pSiblingRbTreeNode->pRightChild->pLeftChild && pSiblingRbTreeNode->pRightChild->pLeftChild->Color == RED) ||
                        (pSiblingRbTreeNode->pRightChild->pRightChild && pSiblingRbTreeNode->pRightChild->pRightChild->Color == RED))
                    {
                        NumSiblingRightChildRedChildren = 1;
                    }
                    else
                    {
                        NumSiblingRightChildRedChildren = 0;
                    }
                }
                else
                {
                    if (pSiblingRbTreeNode->pLeftChild->pLeftChild && pSiblingRbTreeNode->pLeftChild->pLeftChild->Color == RED &&
                        pSiblingRbTreeNode->pLeftChild->pRightChild && pSiblingRbTreeNode->pLeftChild->pRightChild->Color == RED)
                    {
                        NumSiblingLeftChildRedChildren = 2;
                    }
                    else if ((pSiblingRbTreeNode->pLeftChild->pLeftChild && pSiblingRbTreeNode->pLeftChild->pLeftChild->Color == RED) ||
                        (pSiblingRbTreeNode->pLeftChild->pRightChild && pSiblingRbTreeNode->pLeftChild->pRightChild->Color == RED))
                    {
                        NumSiblingLeftChildRedChildren = 1;
                    }
                    else
                    {
                        NumSiblingLeftChildRedChildren = 0;
                    }
                }

                // Rr0 
                if (!IsTempNodeLeftChild && pSiblingRbTreeNode->Color == RED && NumSiblingRightChildRedChildren == 0)
                {
                    // This will lead to a LL rotation
                    pSiblingRbTreeNode->Color = BLACK;
                    pSiblingRbTreeNode->pRightChild->Color = RED;

                    pSiblingRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pParentRbTreeNode->pLeftChild = pSiblingRbTreeNode->pRightChild;
                    pSiblingRbTreeNode->pRightChild = pParentRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRbTreeNode;
                    pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;

                    if (pSiblingRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingRbTreeNode;
                    }

                    break;
                }

                // Lr0 
                if (IsTempNodeLeftChild && pSiblingRbTreeNode->Color == RED && NumSiblingLeftChildRedChildren == 0)
                {
                    // This will lead to a RR rotation
                    pSiblingRbTreeNode->Color = BLACK;
                    pSiblingRbTreeNode->pLeftChild->Color = RED;

                    pSiblingRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pParentRbTreeNode->pRightChild = pSiblingRbTreeNode->pLeftChild;
                    pSiblingRbTreeNode->pLeftChild = pParentRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRbTreeNode;
                    pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;

                    if (pSiblingRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingRbTreeNode;
                    }
                    
                    break;
                }

                // Rr1 case 1 Sibling's right child's left child is red  
                if (!IsTempNodeLeftChild && pSiblingRbTreeNode->Color == RED && NumSiblingRightChildRedChildren == 1 && 
                    pSiblingRbTreeNode->pRightChild && pSiblingRbTreeNode->pRightChild->pLeftChild && pSiblingRbTreeNode->pRightChild->pLeftChild->Color == RED)
                {
                    // This will lead to a LR rotation 
                    pSiblingRbTreeNode->pRightChild->pLeftChild->Color = BLACK;

                    pSiblingRightChildRbTreeNode = pSiblingRbTreeNode->pRightChild;
                    pSiblingRightChildRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pSiblingRbTreeNode->pRightChild = pSiblingRightChildRbTreeNode->pLeftChild;
                    pParentRbTreeNode->pLeftChild = pSiblingRightChildRbTreeNode->pRightChild;
                    pSiblingRightChildRbTreeNode->pLeftChild = pSiblingRbTreeNode;
                    pSiblingRightChildRbTreeNode->pRightChild = pParentRbTreeNode;
                    pSiblingRbTreeNode->pRightChild->pParent = pSiblingRbTreeNode;
                    pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;
                    pSiblingRbTreeNode->pParent = pSiblingRightChildRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRightChildRbTreeNode;

                    if (pSiblingRightChildRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingRightChildRbTreeNode;
                    }
                    
                    break;
                }

                // Lr1 case 1 Sibling's left child's right child is red  
                if (IsTempNodeLeftChild && pSiblingRbTreeNode->Color == RED && NumSiblingLeftChildRedChildren == 1 && 
                    pSiblingRbTreeNode->pLeftChild && pSiblingRbTreeNode->pLeftChild->pRightChild && pSiblingRbTreeNode->pLeftChild->pRightChild->Color == RED)
                {
                    // This will lead to a LR rotation 
                    pSiblingRbTreeNode->pLeftChild->pRightChild->Color = BLACK;

                    pSiblingLeftChildRbTreeNode = pSiblingRbTreeNode->pLeftChild;
                    pSiblingLeftChildRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pSiblingRbTreeNode->pLeftChild = pSiblingLeftChildRbTreeNode->pRightChild;
                    pParentRbTreeNode->pRightChild = pSiblingLeftChildRbTreeNode->pLeftChild;
                    pSiblingLeftChildRbTreeNode->pRightChild = pSiblingRbTreeNode;
                    pSiblingLeftChildRbTreeNode->pLeftChild = pParentRbTreeNode;
                    pSiblingRbTreeNode->pLeftChild->pParent = pSiblingRbTreeNode;
                    pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;
                    pSiblingRbTreeNode->pParent = pSiblingLeftChildRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingLeftChildRbTreeNode;

                    if (pSiblingLeftChildRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingLeftChildRbTreeNode;
                    }
                    
                    break;
                }

                // Rr1 case 2 Sibling's right child's right child is red or Rr2
                if ((!IsTempNodeLeftChild && pSiblingRbTreeNode->Color == RED && NumSiblingRightChildRedChildren == 1 && 
                    pSiblingRbTreeNode->pRightChild && pSiblingRbTreeNode->pRightChild->pRightChild && pSiblingRbTreeNode->pRightChild->pRightChild->Color == RED) ||
                    (!IsTempNodeLeftChild && pSiblingRbTreeNode->Color == RED && NumSiblingRightChildRedChildren == 2))
                {
                    pSiblingRbTreeNode->pRightChild->pRightChild->Color = BLACK;
                    
                    pSiblingRightChildRbTreeNode = pSiblingRbTreeNode->pRightChild;
                    pSiblingRightChildRedChildRbTreeNode = pSiblingRbTreeNode->pRightChild->pRightChild;
                    pSiblingRightChildRedChildRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pSiblingRightChildRbTreeNode->pRightChild = pSiblingRightChildRedChildRbTreeNode->pLeftChild;
                    pParentRbTreeNode->pLeftChild = pSiblingRightChildRedChildRbTreeNode->pRightChild;
                    pSiblingRightChildRedChildRbTreeNode->pLeftChild = pSiblingRbTreeNode;
                    pSiblingRightChildRedChildRbTreeNode->pRightChild = pParentRbTreeNode;
                    pSiblingRightChildRbTreeNode->pRightChild->pParent = pSiblingRightChildRbTreeNode;
                    pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;
                    pSiblingRbTreeNode->pParent = pSiblingRightChildRedChildRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRightChildRedChildRbTreeNode;

                    if (pSiblingRightChildRedChildRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingRightChildRedChildRbTreeNode;
                    }
                    
                    break;
                }

                // Lr1 case 2 Sibling's left child's left child is red or Lr2
                if ((IsTempNodeLeftChild && pSiblingRbTreeNode->Color == RED && NumSiblingLeftChildRedChildren == 1 && 
                    pSiblingRbTreeNode->pLeftChild && pSiblingRbTreeNode->pLeftChild->pLeftChild && pSiblingRbTreeNode->pLeftChild->pLeftChild->Color == RED) ||
                    (IsTempNodeLeftChild && pSiblingRbTreeNode->Color == RED && NumSiblingLeftChildRedChildren == 2))
                {
                    pSiblingRbTreeNode->pLeftChild->pLeftChild->Color = BLACK;

                    pSiblingLeftChildRbTreeNode = pSiblingRbTreeNode->pLeftChild;
                    pSiblingLeftChildRedChildRbTreeNode = pSiblingRbTreeNode->pLeftChild->pLeftChild;
                    pSiblingLeftChildRedChildRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pSiblingLeftChildRbTreeNode->pRightChild = pSiblingLeftChildRedChildRbTreeNode->pRightChild;
                    pParentRbTreeNode->pRightChild = pSiblingLeftChildRedChildRbTreeNode->pLeftChild;
                    pSiblingLeftChildRedChildRbTreeNode->pRightChild = pSiblingRbTreeNode;
                    pSiblingLeftChildRedChildRbTreeNode->pLeftChild = pParentRbTreeNode;
                    pSiblingLeftChildRbTreeNode->pLeftChild->pParent = pSiblingLeftChildRbTreeNode;
                    pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;
                    pSiblingRbTreeNode->pParent = pSiblingLeftChildRedChildRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingLeftChildRedChildRbTreeNode;

                    if (pSiblingLeftChildRedChildRbTreeNode->pParent == NULL)
                    {
                        pRbTreeContext->pMinRbTreeNode = pSiblingLeftChildRedChildRbTreeNode;
                    }

                    break;
                }

                // Phew !! 
                // Should never reach here, leave a print to catch the error 
                printf("__deleteDegree1RbTreeNode: Restoring Red Black Property, Illegal scenario, Exit!\r\n");
                return FALSE;

            } while (TRUE);
        }
    }

    return TRUE;
}


