/**
 @file
 Red-Black Binary Search Tree - Validation checks
 
 @author Ryan Powell
 @date 23-12-12
 @copyright Copyright (c) 2012  Ryan Powell
 @licence https://raw.github.com/Ryandev/RBTreelib/master/LICENSE
 */


#include "rbtree.h"
#include "rbtree_common.h"
#include "rbtree_checks.h"


static inline RBT_NODE * rbtree_checks_prv_getNext ( RBT_NODE * node );
static inline RBT_NODE * rbtree_checks_prv_getPrev ( RBT_NODE * node );
static inline RBT_NODE * rbtree_checks_prv_getFirst( RBT_NODE * node );
static inline RBT_NODE * rbtree_checks_prv_getLast( RBT_NODE * node );
static inline RBT_COLOUR rbtree_checks_prv_getColour( RBT_NODE * node );


#define getNext(n) rbtree_checks_prv_getNext(n);
#define getPrev(n) rbtree_checks_prv_getPrev(n);
#define getFirst(n) rbtree_checks_prv_getFirst(n);
#define getLast(n) rbtree_checks_prv_getLast(n);
#define getColour(n) rbtree_checks_prv_getColour(n);


static inline RBT_NODE * rbtree_checks_prv_getNext ( RBT_NODE * node )
{
	RBT_NODE * parent;
    
	if ( node->right )
    {
		return getFirst(node->right);
    }
    
    parent = node->parent;
    
    while ( ( parent != NULL ) && ( parent->right == node ) )
    {
        node = parent;
        parent = parent->parent;
    }
    
    return parent;
}

static inline RBT_NODE * rbtree_checks_prv_getPrev ( RBT_NODE * node )
{
	RBT_NODE * parent;
    
	if ( node->left != NULL )
    {
		return getLast(node->left);
    }
    
    parent = node->parent;
    
	while ( ( parent != NULL ) && ( parent->left == node ) )
    {
		node = parent;
        parent = parent->parent;
    }
    
	return parent;
}

static inline RBT_NODE * rbtree_checks_prv_getFirst( RBT_NODE * node )
{
    if ( node )
    {
        while (node->left)
        {
            node = node->left;
        }
    }
    
	return node;
}

static inline RBT_NODE * rbtree_checks_prv_getLast( RBT_NODE * node )
{
    if ( node )
    {
        while (node->right)
        {
            node = node->right;
        }
    }
    
	return node;
}

static inline RBT_COLOUR rbtree_checks_prv_getColour( RBT_NODE * node )
{
    RBT_COLOUR colour = RBT_COLOUR_BLACK;
    
    if ( node )
    {
        colour = node->colour;
    }
    
    return colour;
}

bool rbtree_checks_validateNodeLinks ( RBT_NODE * node )
{
    bool isValid = true;
    
    if ( node )
    {
        if ( (node->parent!=NULL) && (node->left!=NULL) && (node->parent==node->left) )
        {
            RBTPRINT_DBG_E("node: %p has matching left&parent nodes",node);
            isValid = false;
        }
        
        if ( (node->parent!=NULL) && (node->right!=NULL) && (node->parent==node->right) )
        {
            RBTPRINT_DBG_E("node: %p has matching right&parent nodes",node);
            isValid = false;
        }
        
        if ( (node->left!=NULL) && (node->right!=NULL) && (node->left==node->right) )
        {
            RBTPRINT_DBG_E("node: %p has matching right&left nodes",node);
            isValid = false;
        }
        
        if ( node->parent )
        {
            if ( ( node->parent->left != node ) &&
                 ( node->parent->right != node ) )
            {
                RBTPRINT_DBG_E("node: %p parent does not link to child ?!",node);
                isValid = false;
            }
        }
        
        if ( node->left )
        {
            if ( node->left->parent != node )
            {
                RBTPRINT_DBG_E("node: %p left child does not link to parent ?!",node);
                isValid = false;
            }
        }

        if ( node->right )
        {
            if ( node->right->parent != node )
            {
                RBTPRINT_DBG_E("node: %p right child does not link to parent ?!",node);
                isValid = false;
            }
        }        
    }

    return isValid;
}

bool rbtree_checks_validateAllNodeLinks ( RBT_TREE * tree )
{
    bool isValid = true;
    RBT_NODE * node = tree->rootNode;
    
    while ( node )
    {
        if ( rbtree_checks_validateNodeLinks(node) == false )
        {
            RBTPRINT_DBG_E("node: %p failed validation",node);
            isValid = false;
            break;
        }

        node = getNext(node);
    }
    
    return isValid;
}

bool rbtree_checks_prv_isTreeValid_BST ( RBT_NODE * rootNode )
{
    bool isValid = true;

#ifdef RBT_PRINT_DEBUG
    if ( rootNode )
    {
        RBT_NODE * node = getFirst(rootNode);
        RBT_NODE * next = getNext(node);
        
        while ( node && next )
        {
            if ( node->key < next->key )
            {
                node = next;
                next = getNext(next);
            }
            else
            {
                RBTPRINT_DBG_E("Is not BST! %u<%u ",node->key,next->key);
                isValid = FALSE;
                break;
            }
        }
    }
    else
    {
        /* nothing here. So it must be */
        isValid = TRUE;
    }
#endif

    return isValid;
}

bool rbtree_checks_prv_isTreeValid_RedHasBlackChildren ( RBT_NODE * rootNode )
{
    bool isValid = true;

#ifdef RBT_PRINT_DEBUG    
    if ( rootNode )
    {
        RBT_NODE * node = getFirst(rootNode);
        RBT_NODE * next = getNext(node);
        
        while ( node && next && isValid )
        {
            if ( node->colour == RBT_COLOUR_RED )
            {
                RBT_COLOUR cLeft = getColour(node->left);
                RBT_COLOUR cRight = getColour(node->right);
                
                if ( ( cLeft != RBT_COLOUR_BLACK ) || ( cRight != RBT_COLOUR_BLACK ) )
                {
                    RBTPRINT_DBG_E("red node does not have black children. l:%d r:%d",cLeft,cRight);
                    isValid = false;
                }
            }
            
            node = next;
            next = getNext(next);
        }
    }
    else
    {
        /* nothing here. So it must be */
        isValid = true;
    }    
#endif

    return isValid;
}

bool rbtree_checks_prv_isTreeValid_isBlackHeightCorrect ( RBT_NODE * rootNode )
{
    bool isValid = true;

#ifdef RBT_PRINT_DEBUG    
    if ( rootNode == NULL )
    {
        /* tree is empty */
        return TRUE;
    }

    RBT_NODE * node = getFirst(rootNode);
    bool hasHeight = FALSE;
    uint32_t black_height = 0U;

    while ( node )
    {
        /* is it a leaf node */
        if ( (node->left==NULL) && (node->right==NULL) )
        {
            uint32_t l_black_height = 1U; /* height starts at 1 due to leaf's==black */
            RBT_NODE * parent = node;
            
            /* get the black height */
            while ( parent )
            {
                if ( parent->colour == RBT_COLOUR_BLACK )
                {
                    l_black_height++;
                }
                
                parent = parent->parent;
            }
            
            if ( hasHeight )
            {
                if ( l_black_height != black_height )
                {
                    RBTPRINT_DBG_E("Black height mismatch %d!=%d",l_black_height,black_height);
                    isValid = false;
                    break;
                }
            }
            else
            {
                /* don't have anything to comare to yet. Set & come back after another iteration */
                black_height = l_black_height;
                hasHeight = true;
            }
        }
        
        node = getNext(node);
    }
#endif
    
    return isValid;
}

uint32_t rbtree_checks_prv_getNodeCount ( RBT_NODE * rootNode )
{
    uint32_t count = 0U;
    
    if ( rootNode )
    {
        RBT_NODE * node = getFirst(rootNode);
        
        while ( node )
        {
            count++;
            RBT_NODE * new_node = getNext(node);
            
            if ( new_node != node )
            {
                node = new_node;
            }
            else
            {
                node = NULL;
            }
        }
    }
    
    return count;
}

bool rbtree_checks_rootHasNoParent ( RBT_TREE * tree )
{
    bool didPass = true;
    
    if ( tree )
    {
        if ( tree->rootNode )
        {
            if ( tree->rootNode->parent != NULL )
            {
                RBTPRINT_DBG_E("Root node has a parent?! %p -> %p",tree->rootNode,tree->rootNode->parent);
            }
        }
    }
    
    return didPass;
}

RBTREE_STATUS rbtree_checks_isTreeValid ( RBT_TREE * tree )
{
#ifdef RBT_PRINT_DEBUG    
    /* verify all node links are linked correctly */
    if ( rbtree_checks_validateAllNodeLinks(tree) == FALSE )
    {
        RBTPRINT_DBG_E("Node links validation failed");
        rbtree_prv_printSummary(tree, stderr);
        return RBTREE_STATUS_FAIL;
    }
    
    /* verify node count */
    uint32_t l_nodecount = rbtree_checks_prv_getNodeCount(tree->rootNode);
    if ( l_nodecount != tree->nodeCount )
    {
        RBTPRINT_DBG_E("Tree node count mismatch: %u!=%u",l_nodecount,tree->nodeCount);
        rbtree_prv_printSummary(tree, stderr);
        return RBTREE_STATUS_FAIL;
    }
    
    if ( rbtree_checks_rootHasNoParent(tree) == FALSE )
    {
        RBTPRINT_DBG_E("Tree root node invalid");
        rbtree_prv_printSummary(tree, stderr);
        return RBTREE_STATUS_FAIL;
    }

    /* verfiy is binary-search-tree */
    if ( rbtree_checks_prv_isTreeValid_BST(tree->rootNode) == FALSE )
    {
        RBTPRINT_DBG_E("Tree is not a BST");
        rbtree_prv_printSummary(tree, stderr);
        return RBTREE_STATUS_FAIL;
    }

    /* verify is rb tree red children colours */
    if ( rbtree_checks_prv_isTreeValid_RedHasBlackChildren(tree->rootNode) == FALSE )
    {
        RBTPRINT_DBG_E("Tree is has red nodes with red children!");
        rbtree_prv_printSummary(tree, stderr);
        return RBTREE_STATUS_FAIL;
    }

    /* verify rb tree black height */
    if ( rbtree_checks_prv_isTreeValid_isBlackHeightCorrect(tree->rootNode) == FALSE )
    {
        RBTPRINT_DBG_E("Tree is has unmatching black height!");
        rbtree_prv_printSummary(tree, stderr);
        return RBTREE_STATUS_FAIL;
    }
#endif
    return RBTREE_STATUS_OK;
}


#undef getNext
#undef getPrev
#undef getFirst
#undef getLast
#undef getColour

