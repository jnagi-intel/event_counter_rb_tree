//
// This file implements the functions for 
// red black trees
//

#include "RbTree.h"

// Local Function Declarations
PRB_TREE_NODE   __insertRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, INT ID, INT Count);
VOID            __deleteRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, PRB_TREE_NODE pRbTreeNode);
PRB_TREE_NODE   __buildRbTreeNode(INT ID, INT Count);
PRB_TREE_NODE   __findRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, INT ID);
VOID            __freeRbTreeNode(PRB_TREE_NODE *ppRbTreeNode);
VOID            __deleteDegree1RbTreeNode(PRB_TREE_CONTEXT pRbTreeContext, PRB_TREE_NODE pRbTreeNode, PRB_TREE_NODE pChildRbTreeNode);
PRB_TREE_NODE   __getNextIDRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, PRB_TREE_NODE pRbTreeNode);
PRB_TREE_NODE   __getPrevIDRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, PRB_TREE_NODE pRbTreeNode);
VOID            __initializeRbTreeNodeArrayList(struct _RB_TREE_CONTEXT *pRbTreeContext, UINT Length);
VOID            __insertRbTreeNodeArrayList(struct _RB_TREE_CONTEXT *pRbTreeContext, INT ID, INT Count, UINT Index);
VOID            __initializeRbTree(struct _RB_TREE_CONTEXT *pRbTreeContext);
PRB_TREE_NODE   __sortedArrayToRbTree(PRB_TREE_CONTEXT pRbTreeContext, INT StartIndex, INT EndIndex, UINT Height);


// createRbTreeContext()
// This function allocates memory for the context and initilize the variables and function pointers
PRB_TREE_CONTEXT createRbTreeContext()
{
    PRB_TREE_CONTEXT    pRbTreeContext = NULL;

    // Allocate memory for Red Black Tree
    pRbTreeContext = (PRB_TREE_CONTEXT)malloc(sizeof(RB_TREE_CONTEXT));

    // Initialize the variables
    pRbTreeContext->pMinRbTreeNode = NULL;

    // Initilize the function table
    pRbTreeContext->stRbTreeFnTbl.insertRbTreeNode              = __insertRbTreeNode;
    pRbTreeContext->stRbTreeFnTbl.deleteRbTreeNode              = __deleteRbTreeNode;
    pRbTreeContext->stRbTreeFnTbl.findRbTreeNode                = __findRbTreeNode;
    pRbTreeContext->stRbTreeFnTbl.getNextIDRbTreeNode           = __getNextIDRbTreeNode;
    pRbTreeContext->stRbTreeFnTbl.getPrevIDRbTreeNode           = __getPrevIDRbTreeNode;
    pRbTreeContext->stRbTreeFnTbl.initializeRbTreeNodeArrayList = __initializeRbTreeNodeArrayList;
    pRbTreeContext->stRbTreeFnTbl.insertRbTreeNodeArrayList     = __insertRbTreeNodeArrayList;
    pRbTreeContext->stRbTreeFnTbl.initializeRbTree              = __initializeRbTree;
    
    return pRbTreeContext;
}

// destroyRbTreeContext()
// This function deallocates and frees up the context
VOID destroyRbTreeContext(PRB_TREE_CONTEXT *ppRbTreeContext)
{
    if ((*ppRbTreeContext)->pRbTreeNodeArrayList)
    {
        free((*ppRbTreeContext)->pRbTreeNodeArrayList);
        (*ppRbTreeContext)->pRbTreeNodeArrayList = NULL;
    }

    (*ppRbTreeContext)->pMinRbTreeNode = NULL;

    if (*ppRbTreeContext)
    {
        free(*ppRbTreeContext);
        *ppRbTreeContext = NULL;
    }
}

// __buildRbTreeNode()
// This function allocates and initializes the Rb Tree Node from ID and Count
PRB_TREE_NODE __buildRbTreeNode(INT ID, INT Count)
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

// __insertRbTreeNode()
// This function inserts the Rb Tree node in the Red black Tree considering all the scenrios and also 
// rebalances the tree maintaining the Red Black property
PRB_TREE_NODE __insertRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, INT ID, INT Count)
{
    PRB_TREE_NODE   pNewRbTreeNode          = NULL;
    PRB_TREE_NODE   pTempRbTreeNode         = NULL;
    PRB_TREE_NODE   pParentRbTreeNode       = NULL;
    PRB_TREE_NODE   pGrandParentRbTreeNode  = NULL;
    PRB_TREE_NODE   pUncleRbTreeNode        = NULL;
    BOOLEAN         IsTempNodeLeftChild     = FALSE;
    BOOLEAN         IsParentNodeLeftChild   = FALSE;

    // First check if the node already exists or not
    if (pRbTreeContext->pMinRbTreeNode == NULL)
    {
        // Node doesnt exist! Build a node and add it to the root of the tree and return
        pRbTreeContext->pMinRbTreeNode = __buildRbTreeNode(ID, Count);
        pNewRbTreeNode = pRbTreeContext->pMinRbTreeNode;
    }
    else
    {
        // search for the node, if it doesnt exist we will hit a NULL node
        pTempRbTreeNode = pRbTreeContext->pMinRbTreeNode;
        while (pTempRbTreeNode != NULL)
        {
            if (ID == pTempRbTreeNode->ID)
            {
                // Node already exists! 
                // Add the Count to the existing Count of the Node and return 
                pTempRbTreeNode->Count += Count;
                return pTempRbTreeNode;
            }
            else if (ID < pTempRbTreeNode->ID)
            {
                if (pTempRbTreeNode->pLeftChild != NULL)
                {
                    pTempRbTreeNode = pTempRbTreeNode->pLeftChild;
                }
                else
                {
                    // Hit a leaf, Node doesnt exist! Build one and add it to the tree 
                    pNewRbTreeNode = __buildRbTreeNode(ID, Count);
                    pTempRbTreeNode->pLeftChild = pNewRbTreeNode;
                    pNewRbTreeNode->pParent = pTempRbTreeNode;
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
                    // Hit a leaf, Node doesnt exist! Build one and add it to the tree 
                    pNewRbTreeNode = __buildRbTreeNode(ID, Count);
                    pTempRbTreeNode->pRightChild = pNewRbTreeNode;
                    pNewRbTreeNode->pParent = pTempRbTreeNode;
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

    // pNewRbTreeNode is the new node added
    // Using pTempRbTreeNode for recursion
    pTempRbTreeNode = pNewRbTreeNode;

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
        pParentRbTreeNode = pTempRbTreeNode->pParent;
        pGrandParentRbTreeNode = pTempRbTreeNode->pParent->pParent;
        IsTempNodeLeftChild = pTempRbTreeNode == pParentRbTreeNode->pLeftChild ? TRUE : FALSE;
        IsParentNodeLeftChild = pParentRbTreeNode == pGrandParentRbTreeNode->pLeftChild ? TRUE : FALSE;
        pUncleRbTreeNode = IsParentNodeLeftChild ? pGrandParentRbTreeNode->pRightChild : pGrandParentRbTreeNode->pLeftChild;

        // Case III : XYr, uncle is red. color flip will do the trick
        if (pUncleRbTreeNode && pUncleRbTreeNode->Color == RED)
        {
            pParentRbTreeNode->Color = BLACK;
            pGrandParentRbTreeNode->Color = RED;
            pUncleRbTreeNode->Color = BLACK;

            pNewRbTreeNode = pTempRbTreeNode;
            pTempRbTreeNode = pGrandParentRbTreeNode;
            continue;
        }

        // Case IV : LLb Rotation
        // Reached here, so that means UncleRbTreeNode is black, 
        // Only care about relationships between node, parent and grandparent
        if (IsTempNodeLeftChild && IsParentNodeLeftChild)
        {
            // Make the Color changes first 
            pParentRbTreeNode->Color = BLACK;
            pGrandParentRbTreeNode->Color = RED;

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

            pGrandParentRbTreeNode->pLeftChild = pParentRbTreeNode->pRightChild;
            pParentRbTreeNode->pParent = pGrandParentRbTreeNode->pParent;
            pParentRbTreeNode->pRightChild = pGrandParentRbTreeNode;
            pGrandParentRbTreeNode->pParent = pParentRbTreeNode;
            if (pGrandParentRbTreeNode->pLeftChild) pGrandParentRbTreeNode->pLeftChild->pParent = pGrandParentRbTreeNode;

            break;
        }

        // Case V : LRb Rotation
        if (!IsTempNodeLeftChild && IsParentNodeLeftChild)
        {
            // Make the color changes first 
            pTempRbTreeNode->Color = BLACK;
            pGrandParentRbTreeNode->Color = RED;

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

            pParentRbTreeNode->pRightChild = pTempRbTreeNode->pLeftChild;
            pGrandParentRbTreeNode->pLeftChild = pTempRbTreeNode->pRightChild;

            pTempRbTreeNode->pParent = pGrandParentRbTreeNode->pParent;
            pTempRbTreeNode->pLeftChild = pParentRbTreeNode;
            pTempRbTreeNode->pRightChild = pGrandParentRbTreeNode;

            pParentRbTreeNode->pParent = pTempRbTreeNode;
            pGrandParentRbTreeNode->pParent = pTempRbTreeNode;

            if (pParentRbTreeNode->pRightChild) pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;
            if (pGrandParentRbTreeNode->pLeftChild) pGrandParentRbTreeNode->pLeftChild->pParent = pGrandParentRbTreeNode;

            break;
        }

        // Case VI : RRb Rotation
        if (!IsTempNodeLeftChild && !IsParentNodeLeftChild)
        {
            // Make the Color changes first 
            pParentRbTreeNode->Color = BLACK;
            pGrandParentRbTreeNode->Color = RED;

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
            pParentRbTreeNode->pParent = pGrandParentRbTreeNode->pParent;
            pParentRbTreeNode->pLeftChild = pGrandParentRbTreeNode;
            pGrandParentRbTreeNode->pParent = pParentRbTreeNode;
            if (pGrandParentRbTreeNode->pRightChild) pGrandParentRbTreeNode->pRightChild->pParent = pGrandParentRbTreeNode;

            break;
        }

        // Case VII : RLb Rotation
        if (IsTempNodeLeftChild && !IsParentNodeLeftChild)
        {
            // Make the color changes first 
            pTempRbTreeNode->Color = BLACK;
            pGrandParentRbTreeNode->Color = RED;

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

            pParentRbTreeNode->pLeftChild = pTempRbTreeNode->pRightChild;
            pGrandParentRbTreeNode->pRightChild = pTempRbTreeNode->pLeftChild;

            pTempRbTreeNode->pParent = pGrandParentRbTreeNode->pParent;
            pTempRbTreeNode->pRightChild = pParentRbTreeNode;
            pTempRbTreeNode->pLeftChild = pGrandParentRbTreeNode;

            pParentRbTreeNode->pParent = pTempRbTreeNode;
            pGrandParentRbTreeNode->pParent = pTempRbTreeNode;

            if (pParentRbTreeNode->pRightChild) pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;
            if (pGrandParentRbTreeNode->pLeftChild) pGrandParentRbTreeNode->pLeftChild->pParent = pGrandParentRbTreeNode;

            break;
        }

        // Phew !! 
        // Should never reach here, leave a print to catch the error 
        printf("__insertRbTreeNode: Restoring Red Black Property, Illegal scenario, Exit!\r\n");
        return NULL;

    } while (TRUE);

    return pNewRbTreeNode;;
}

// __findRbTreeNode()
// This function finds the Rb Tree Node with the particular ID in the Red Black Tree
// or if the ID doesnt exist returns the node with closest ID 
// Will return NULL if root is NULL
PRB_TREE_NODE __findRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, INT ID)
{
    PRB_TREE_NODE    pTempRbTreeNode = NULL;

    if (pRbTreeContext->pMinRbTreeNode)
    {
        // Verifying that the root of the tree exists, now recurse!
        pTempRbTreeNode = pRbTreeContext->pMinRbTreeNode;
        while (TRUE)
        {
            if (ID == pTempRbTreeNode->ID)
            {
                break;
            }
            else if (ID < pTempRbTreeNode->ID)
            {
                if (pTempRbTreeNode->pLeftChild != NULL)
                {
                    pTempRbTreeNode = pTempRbTreeNode->pLeftChild;
                }
                else
                {
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
                    break;
                }
            }
        }
    }

    return pTempRbTreeNode;
}

// __deleteRbTreeNode()
// This function deletes the Rb Tree node in the Red black Tree. It assumes that the node is already present in the 
// tree. It checks if the node to be removed was degree 0/1/2 and then adjusts the pointers to call delete 
// using Degree 1 delete algorithm from Red Black Tree
VOID __deleteRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, PRB_TREE_NODE pRbTreeNode)
{
    PRB_TREE_NODE   pMaxSubTreeRbTreeNode   = NULL;
    RB_TREE_NODE    TempRbTreeNode          = { 0 };

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
}

// __freeRbTreeNode()
// This function frees up the memory for a Red Black Tree Node
VOID __freeRbTreeNode(PRB_TREE_NODE *ppRbTreeNode)
{
    // Make the pointers NULL and free up the Tree Node pointer 
    if (*ppRbTreeNode)
    {
        (*ppRbTreeNode)->pLeftChild     = NULL;
        (*ppRbTreeNode)->pRightChild    = NULL;
        (*ppRbTreeNode)->pParent        = NULL;

        // Dont free the memory it will be done while clearing the Array List
        *ppRbTreeNode = NULL;
    }
}

// __deleteDegree1RbTreeNode()
// This function implements all the scenarios and rebalances the red black tree preserving the properties.
VOID __deleteDegree1RbTreeNode(PRB_TREE_CONTEXT pRbTreeContext, PRB_TREE_NODE pRbTreeNode, PRB_TREE_NODE pChildRbTreeNode)
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

            if (pParentRbTreeNode == NULL)
            {
                // Looks like removed node was the root and the only node
                // Nothing to be done 
                pRbTreeContext->pMinRbTreeNode = NULL;
                return;
            }
        }

        __freeRbTreeNode(&pRbTreeNode);

        // Simple case handled first
        // removed node is black, but y is red
        if (pChildRbTreeNode && pChildRbTreeNode->Color == RED)
        {
            // Color this node black and done! 
            pChildRbTreeNode->Color = BLACK;

            if (pChildRbTreeNode->pParent == NULL)
            {
                pRbTreeContext->pMinRbTreeNode = pChildRbTreeNode;
            }
        }
        else
        {
            // Complex Case, Both the node removed and Child (y) were black
            // Child can be NULL as well
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
                    // Assuming this would happen only in the first iteration
                    // pParentRbTreeNode is already defined
                    // 
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
                    else
                    {
                        // No Sibling! 
                        // Is this possible ? Need to check! 
                    }
                }
                
                // Assuming the sibling always exists
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
                    if (pParentRbTreeNode->pLeftChild) pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;

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
                    if (pParentRbTreeNode->pRightChild) pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;

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
                    if (pSiblingRbTreeNode->pRightChild) pSiblingRbTreeNode->pRightChild->pParent = pSiblingRbTreeNode;
                    if (pParentRbTreeNode->pLeftChild) pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;
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
                    if (pSiblingRbTreeNode->pLeftChild) pSiblingRbTreeNode->pLeftChild->Color = pParentRbTreeNode->Color;
                    pParentRbTreeNode->Color = BLACK;

                    pSiblingRedChildRbTreeNode = pSiblingRbTreeNode->pLeftChild;
                    pSiblingRedChildRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pSiblingRbTreeNode->pLeftChild = pSiblingRedChildRbTreeNode->pRightChild;
                    pParentRbTreeNode->pRightChild = pSiblingRedChildRbTreeNode->pLeftChild;
                    pSiblingRedChildRbTreeNode->pRightChild = pSiblingRbTreeNode;
                    pSiblingRedChildRbTreeNode->pLeftChild = pParentRbTreeNode;
                    if (pSiblingRbTreeNode->pLeftChild) pSiblingRbTreeNode->pLeftChild->pParent = pSiblingRbTreeNode;
                    if (pParentRbTreeNode->pRightChild) pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;
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
                    if (pSiblingRbTreeNode->pRightChild)
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
                }
                else
                {
                    if (pSiblingRbTreeNode->pLeftChild)
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
                }

                // Rr0 
                if (!IsTempNodeLeftChild && pSiblingRbTreeNode->Color == RED && NumSiblingRightChildRedChildren == 0)
                {
                    // This will lead to a LL rotation
                    pSiblingRbTreeNode->Color = BLACK;
                    if (pSiblingRbTreeNode->pRightChild) pSiblingRbTreeNode->pRightChild->Color = RED;

                    pSiblingRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pParentRbTreeNode->pLeftChild = pSiblingRbTreeNode->pRightChild;
                    pSiblingRbTreeNode->pRightChild = pParentRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRbTreeNode;
                    if (pParentRbTreeNode->pLeftChild) pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;

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
                    if (pSiblingRbTreeNode->pLeftChild) pSiblingRbTreeNode->pLeftChild->Color = RED;

                    pSiblingRbTreeNode->pParent = pParentRbTreeNode->pParent;
                    pParentRbTreeNode->pRightChild = pSiblingRbTreeNode->pLeftChild;
                    pSiblingRbTreeNode->pLeftChild = pParentRbTreeNode;
                    pParentRbTreeNode->pParent = pSiblingRbTreeNode;
                    if (pParentRbTreeNode->pRightChild) pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;

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
                    if (pSiblingRbTreeNode->pRightChild) pSiblingRbTreeNode->pRightChild->pParent = pSiblingRbTreeNode;
                    if (pParentRbTreeNode->pLeftChild) pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;
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
                    if (pSiblingRbTreeNode->pLeftChild) pSiblingRbTreeNode->pLeftChild->pParent = pSiblingRbTreeNode;
                    if (pParentRbTreeNode->pRightChild) pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;
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
                    if (pSiblingRightChildRbTreeNode->pRightChild) pSiblingRightChildRbTreeNode->pRightChild->pParent = pSiblingRightChildRbTreeNode;
                    if (pParentRbTreeNode->pLeftChild) pParentRbTreeNode->pLeftChild->pParent = pParentRbTreeNode;
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
                    if (pSiblingLeftChildRbTreeNode->pLeftChild) pSiblingLeftChildRbTreeNode->pLeftChild->pParent = pSiblingLeftChildRbTreeNode;
                    if (pParentRbTreeNode->pRightChild) pParentRbTreeNode->pRightChild->pParent = pParentRbTreeNode;
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

            } while (TRUE);
        }
    }
}

// __getNextIDRbTreeNode()
// This function returns the next node with ID greater than the current node 
// It assumes that current Node exists
PRB_TREE_NODE __getNextIDRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, PRB_TREE_NODE pRbTreeNode)
{
    PRB_TREE_NODE   pTempRbTreeNode = NULL;

    // The node with greater ID will be either the node with smallest ID in the right subtree 
    // or if the right subtree is empty than the parent 
    if (pRbTreeNode->pRightChild != NULL)
    {
        pTempRbTreeNode = pRbTreeNode->pRightChild;
        while (pTempRbTreeNode->pLeftChild != NULL)
        {
            pTempRbTreeNode = pTempRbTreeNode->pLeftChild;
        }

        return pTempRbTreeNode;
    }
    else
    {
        // check for parent if its a left child, if thats NULL then there is no node with ID greater
        // recurse to top of the tree, if we hit the root then no event exists
        pTempRbTreeNode = pRbTreeNode;
        while (pTempRbTreeNode->pParent != NULL)
        {
            if (pTempRbTreeNode->pParent->pLeftChild == pTempRbTreeNode)
            {
                break;
            }
            else
            {
                pTempRbTreeNode = pTempRbTreeNode->pParent;
            }
        }

        return pTempRbTreeNode->pParent;
    }
}

// __getPrevIDRbTreeNode()
// This function returns the next node with ID less than the current node 
// It assumes that current Node exists
PRB_TREE_NODE __getPrevIDRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, PRB_TREE_NODE pRbTreeNode)
{
    PRB_TREE_NODE   pTempRbTreeNode = NULL;

    // The node with lesser ID will be either the node with largest ID in the left subtree 
    // or if the left subtree is empty than the parent 
    if (pRbTreeNode->pLeftChild != NULL)
    {
        pTempRbTreeNode = pRbTreeNode->pLeftChild;
        while (pTempRbTreeNode->pRightChild != NULL)
        {
            pTempRbTreeNode = pTempRbTreeNode->pRightChild;
        }

        return pTempRbTreeNode;
    }
    else
    {
        // check for parent if its a right child, if thats NULL then there is no node with ID greater
        // recurse to top of the tree, if we hit the root then no event exists
        pTempRbTreeNode = pRbTreeNode;
        while (pTempRbTreeNode->pParent != NULL)
        {
            if (pTempRbTreeNode->pParent->pRightChild == pTempRbTreeNode)
            {
                break;
            }
            else
            {
                pTempRbTreeNode = pTempRbTreeNode->pParent;
            }
        }

        return pTempRbTreeNode->pParent;
    }
}

// __initializeRbTreeNodeArrayList()
// This function allocates memory for the array list
VOID __initializeRbTreeNodeArrayList(struct _RB_TREE_CONTEXT *pRbTreeContext, UINT Length)
{
    pRbTreeContext->pRbTreeNodeArrayList    = (PRB_TREE_NODE)malloc(sizeof(RB_TREE_NODE) * Length);
    pRbTreeContext->NumNodesRbTree          = Length;
}

// __insertRbTreeNodeArrayList()
// This funcion builds the RbTree Node and adds it to the end of the list
VOID __insertRbTreeNodeArrayList(struct _RB_TREE_CONTEXT *pRbTreeContext, INT ID, INT Count, UINT Index)
{
    PRB_TREE_NODE   pRbTreeNode = NULL;

    // First build the Red Black Tree Node and color it black
    pRbTreeNode = __buildRbTreeNode(ID, Count);
    pRbTreeNode->Color = BLACK;

    // Add it to the List 
    memcpy(&pRbTreeContext->pRbTreeNodeArrayList[Index], pRbTreeNode, sizeof(RB_TREE_NODE));

    // Free up the memory
    free(pRbTreeNode);
}

// __initializeRbTree()
// This function builds the Rb Tree from the Array list in O(n) time
VOID __initializeRbTree(struct _RB_TREE_CONTEXT *pRbTreeContext)
{
    // Get the height of the RB Tree 
    pRbTreeContext->RbTreeHeight = (UINT)(log(pRbTreeContext->NumNodesRbTree) / log(2));

    // call the sorted array to rb tree function to build the RB Tree recursively 
    pRbTreeContext->pMinRbTreeNode = __sortedArrayToRbTree(pRbTreeContext, 0, pRbTreeContext->NumNodesRbTree - 1, 0);
}

// __sortedArrayToRbTree()
// THis is the recursive function to build the RB Tree from a sorted array list
PRB_TREE_NODE __sortedArrayToRbTree(PRB_TREE_CONTEXT pRbTreeContext, INT StartIndex, INT EndIndex, UINT Height)
{
    PRB_TREE_NODE   pRbTreeNode = NULL;
    INT            MidIndex = 0;

    // Get the element in the middle index, that will be the root. 
    // Recurse again for left child and right child
    if (StartIndex > EndIndex)
    {
        return NULL;
    }
    else
    {
        // Get the root node
        MidIndex = StartIndex + (EndIndex - StartIndex) / 2;
        pRbTreeNode = &pRbTreeContext->pRbTreeNodeArrayList[MidIndex];

        // Recurse for left and right child
        pRbTreeNode->pLeftChild = __sortedArrayToRbTree(pRbTreeContext, StartIndex, MidIndex - 1, Height + 1);
        pRbTreeNode->pRightChild = __sortedArrayToRbTree(pRbTreeContext, MidIndex + 1, EndIndex, Height + 1);

        // Update the parent pointers if need to
        if (pRbTreeNode->pLeftChild) pRbTreeNode->pLeftChild->pParent = pRbTreeNode;
        if (pRbTreeNode->pRightChild) pRbTreeNode->pRightChild->pParent = pRbTreeNode;

        // Color the nodes in the last level Red to maintain the Red Black Tree Property
        if (Height == pRbTreeContext->RbTreeHeight)
        {
            pRbTreeNode->Color = RED;
        }

        return pRbTreeNode;
    }
}





