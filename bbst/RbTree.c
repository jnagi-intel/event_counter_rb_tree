//
// This file implements the functions for 
// red black trees
//

#include "RbTree.h"

// Local Function Declarations
BOOLEAN __insertRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, UINT ID, UINT Count);

PRB_TREE_CONTEXT createRbTreeContext()
{
    PRB_TREE_CONTEXT    pRbTreeContext = NULL;

    // Allocate memory for Red Black Tree
    pRbTreeContext = (PRB_TREE_CONTEXT)malloc(sizeof(RB_TREE_CONTEXT));

    // Initialize the variables
    pRbTreeContext->pMinRbTreeNode = NULL;

    // Initilize the function table
    pRbTreeContext->RbTreeFnTbl.insertRbTreeNode = __insertRbTreeNode;

    return pRbTreeContext;
}

VOID destroyRbTreeContext(PRB_TREE_CONTEXT *ppRbTreeContext)
{

}

PRB_TREE_NODE __buildRbTreeNode(UINT ID, UINT Count)
{
    PRB_TREE_NODE   RbTreeNode = NULL;

    // Build a temp node to be inserted in the Red Black Tree 
    RbTreeNode = (PRB_TREE_NODE)malloc(sizeof(RB_TREE_NODE));
    RbTreeNode->Count = Count;
    RbTreeNode->ID = ID;
    RbTreeNode->Color = RED;
    RbTreeNode->pLeftChild = NULL;
    RbTreeNode->pRightChild = NULL;
    RbTreeNode->pParent = NULL;

    return RbTreeNode;
}

BOOLEAN __insertRbTreeNode(struct _RB_TREE_CONTEXT *pRbTreeContext, UINT ID, UINT Count)
{
    PRB_TREE_NODE   NewRbTreeNode = NULL;
    PRB_TREE_NODE   TempRbTreeNode = NULL;
    PRB_TREE_NODE   ParentRbTreeNode = NULL;
    PRB_TREE_NODE   GrandParentRbTreeNode = NULL;
    PRB_TREE_NODE   UncleRbTreeNode = NULL;
    BOOLEAN         IsTempNodeLeftChild = FALSE;
    BOOLEAN         IsParentNodeLeftChild = FALSE;
    
    NewRbTreeNode = __buildRbTreeNode(ID, Count);

    // First insert the node as in a normal Min Tree 
    if (pRbTreeContext->pMinRbTreeNode == NULL)
    {
        pRbTreeContext->pMinRbTreeNode = NewRbTreeNode;
    }
    else
    {
        TempRbTreeNode = pRbTreeContext->pMinRbTreeNode;
        while (TempRbTreeNode != NULL)
        {
            if (NewRbTreeNode->ID <= TempRbTreeNode->ID)
            {
                if (TempRbTreeNode->pLeftChild != NULL)
                {
                    TempRbTreeNode = TempRbTreeNode->pLeftChild;
                }
                else
                {
                    TempRbTreeNode->pLeftChild = NewRbTreeNode;
                    NewRbTreeNode->pParent = TempRbTreeNode;
                    break;
                }
            }
            else
            {
                if (TempRbTreeNode->pRightChild != NULL)
                {
                    TempRbTreeNode = TempRbTreeNode->pRightChild;
                }
                else
                {
                    TempRbTreeNode->pRightChild = NewRbTreeNode;
                    NewRbTreeNode->pParent = TempRbTreeNode;
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
    
    TempRbTreeNode = NewRbTreeNode;     // Using Temp again for recursion
    
    do
    {
        // Case I : Node Inserted is Root or we have back traversed the tree till root, make it black and done
        if (TempRbTreeNode == pRbTreeContext->pMinRbTreeNode)
        {
            pRbTreeContext->pMinRbTreeNode->Color = BLACK;
            break;
        }

        // Case II : Parent exists and is black, nothing to be done
        else if (TempRbTreeNode->pParent && TempRbTreeNode->pParent->Color == BLACK)
        {
            // Nothing to be done here, all properties still hold! 
            break;
        }

        // For rest of the cases store parent, grand parent and uncle tree nodes
        // and also the relationship
        ParentRbTreeNode = TempRbTreeNode->pParent;
        GrandParentRbTreeNode = TempRbTreeNode->pParent->pParent;
        IsTempNodeLeftChild = TempRbTreeNode == ParentRbTreeNode->pLeftChild ? TRUE : FALSE;
        IsParentNodeLeftChild = ParentRbTreeNode == GrandParentRbTreeNode->pLeftChild ? TRUE : FALSE;
        UncleRbTreeNode = IsParentNodeLeftChild ? GrandParentRbTreeNode->pRightChild : GrandParentRbTreeNode->pLeftChild;

        // Case III : XYr, uncle is red. color flip will do the trick
        if (UncleRbTreeNode && UncleRbTreeNode->Color == RED)
        {
            ParentRbTreeNode->Color = BLACK;
            GrandParentRbTreeNode->Color = RED;
            UncleRbTreeNode->Color = BLACK;
            TempRbTreeNode = GrandParentRbTreeNode;
            continue;
        }

        // Case IV : LLb Rotation
        // Reached here, so that means UncleRbTreeNode is black, 
        // Only care about relationships between node, parent and grandparent
        if (IsTempNodeLeftChild && IsParentNodeLeftChild)
        {
            // Make the Color changes first 
            ParentRbTreeNode->Color = BLACK;
            GrandParentRbTreeNode->Color = RED;

            // Now make the pointer changes due to rotation
            if (GrandParentRbTreeNode->pParent)
            {
                if (GrandParentRbTreeNode->pParent->pLeftChild == GrandParentRbTreeNode)
                {
                    GrandParentRbTreeNode->pParent->pLeftChild = ParentRbTreeNode;
                }
                else
                {
                    GrandParentRbTreeNode->pParent->pRightChild = ParentRbTreeNode;
                }
            }
            else
            {
                // Grandparent was the root 
                pRbTreeContext->pMinRbTreeNode = ParentRbTreeNode;
            }

            GrandParentRbTreeNode->pLeftChild = ParentRbTreeNode->pRightChild;
            ParentRbTreeNode->pParent = GrandParentRbTreeNode->pParent;
            ParentRbTreeNode->pRightChild = GrandParentRbTreeNode;
            GrandParentRbTreeNode->pParent = ParentRbTreeNode;

            break;
        }
        
        // Case V : LRb Rotation
        if (!IsTempNodeLeftChild && IsParentNodeLeftChild)
        {
            // Make the color changes first 
            TempRbTreeNode->Color = BLACK;
            GrandParentRbTreeNode->Color = RED;

            // Now make the pointer changes due to rotation 
            if (GrandParentRbTreeNode->pParent)
            {
                if (GrandParentRbTreeNode->pParent->pLeftChild == GrandParentRbTreeNode)
                {
                    GrandParentRbTreeNode->pParent->pLeftChild = TempRbTreeNode;
                }
                else
                {
                    GrandParentRbTreeNode->pParent->pRightChild = TempRbTreeNode;
                }
            }
            else
            {
                // Grandparent was the root 
                pRbTreeContext->pMinRbTreeNode = TempRbTreeNode;
            }

            ParentRbTreeNode->pRightChild = TempRbTreeNode->pLeftChild;
            GrandParentRbTreeNode->pLeftChild = TempRbTreeNode->pRightChild;

            TempRbTreeNode->pParent = GrandParentRbTreeNode->pParent;
            TempRbTreeNode->pLeftChild = ParentRbTreeNode;
            TempRbTreeNode->pRightChild = GrandParentRbTreeNode;
        
            ParentRbTreeNode->pParent = TempRbTreeNode;
            GrandParentRbTreeNode->pParent = TempRbTreeNode;

            break;
        }

        // Case VI : RRb Rotation
        if (!IsTempNodeLeftChild && !IsParentNodeLeftChild)
        {
            // Make the Color changes first 
            ParentRbTreeNode->Color = BLACK;
            GrandParentRbTreeNode->Color = RED;

            // Now make the pointer changes due to rotation
            if (GrandParentRbTreeNode->pParent)
            {
                if (GrandParentRbTreeNode->pParent->pLeftChild == GrandParentRbTreeNode)
                {
                    GrandParentRbTreeNode->pParent->pLeftChild = ParentRbTreeNode;
                }
                else
                {
                    GrandParentRbTreeNode->pParent->pRightChild = ParentRbTreeNode;
                }
            }
            else
            {
                // Grandparent was the root 
                pRbTreeContext->pMinRbTreeNode = ParentRbTreeNode;
            }

            GrandParentRbTreeNode->pRightChild = ParentRbTreeNode->pLeftChild;
            ParentRbTreeNode->pParent = GrandParentRbTreeNode->pParent;
            ParentRbTreeNode->pLeftChild = GrandParentRbTreeNode;
            GrandParentRbTreeNode->pParent = ParentRbTreeNode;

            break;
        }

        // Case VII : RLb Rotation
        if (IsTempNodeLeftChild && !IsParentNodeLeftChild)
        {
            // Make the color changes first 
            TempRbTreeNode->Color = BLACK;
            GrandParentRbTreeNode->Color = RED;

            // Now make the pointer changes due to rotation 
            if (GrandParentRbTreeNode->pParent)
            {
                if (GrandParentRbTreeNode->pParent->pLeftChild == GrandParentRbTreeNode)
                {
                    GrandParentRbTreeNode->pParent->pLeftChild = TempRbTreeNode;
                }
                else
                {
                    GrandParentRbTreeNode->pParent->pRightChild = TempRbTreeNode;
                }
            }
            else
            {
                // Grandparent was the root 
                pRbTreeContext->pMinRbTreeNode = TempRbTreeNode;
            }

            ParentRbTreeNode->pLeftChild = TempRbTreeNode->pRightChild;
            GrandParentRbTreeNode->pRightChild = TempRbTreeNode->pLeftChild;

            TempRbTreeNode->pParent = GrandParentRbTreeNode->pParent;
            TempRbTreeNode->pRightChild = ParentRbTreeNode;
            TempRbTreeNode->pLeftChild = GrandParentRbTreeNode;

            ParentRbTreeNode->pParent = TempRbTreeNode;
            GrandParentRbTreeNode->pParent = TempRbTreeNode;

            // Handle the root 
            if (TempRbTreeNode->pParent == NULL)
            {
                pRbTreeContext->pMinRbTreeNode = TempRbTreeNode;
            }

            break;
        }

        // Phew !! 
        // Should never reach here, leave a print to catch the error 
        printf("____insertRbTreeNode: Restoring Red Black Property, Illegal scenario, Exit!");
        return FALSE;

    } while (TRUE);

    return TRUE;
}



