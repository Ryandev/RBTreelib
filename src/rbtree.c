/**
 @file
 Red-Black Binary Search Tree
 
 @author Ryan Powell
 @date 23-12-12
 @copyright Copyright (c) 2012  Ryan Powell
 @licence https://raw.github.com/Ryandev/RBTreelib/master/LICENSE
 */


#include "rbtree.h"
#include <pthread.h>        /* for mutex */
#include <string.h>         /* memset */
#include "rbtree_checks.h"


/* private function declarations */
static void* rbtree_prv_memAlloc_default ( size_t size );        /* NOT inline */
static void rbtree_prv_memFree_default ( void * ptr );           /* NOT inline */
static inline RBT_NODE * rbtree_prv_createNode ( RBT_TREE * tree );
static inline void rbtree_prv_freeNode ( RBT_NODE * node, RBT_TREE * tree );
static inline RBT_COLOUR rbtree_prv_getColour ( RBT_NODE * node );
static inline void rbtree_prv_setColour ( RBT_COLOUR colour, RBT_NODE * node);
static inline RBT_NODE * rbtree_prv_getSibling ( RBT_NODE * node );
static inline RBT_NODE * rbtree_prv_getUncle ( RBT_NODE * node );
static inline RBT_NODE * rbtree_prv_getParent ( RBT_NODE * node );
static inline void rbtree_prv_setParent ( RBT_NODE * parent, RBT_NODE * node);
static inline bool rbtree_prv_isRoot( RBT_NODE * node);
static inline void rbtree_prv_setRoot( RBT_NODE * node, RBT_TREE * tree );
static inline RBT_NODE * rbtree_prv_getRoot( RBT_TREE * tree );
static inline bool rbtree_prv_isBlack( RBT_NODE * node );
static inline bool rbtree_prv_isRed( RBT_NODE * node );
static inline RBT_NODE * rbtree_prv_getFirst( RBT_NODE * node );
static inline RBT_NODE * rbtree_prv_getLast( RBT_NODE * node );
static inline RBT_NODE * rbtree_prv_getLeft ( RBT_NODE * node );
static inline RBT_NODE * rbtree_prv_getRight ( RBT_NODE * node );
static inline void rbtree_prv_setLeft ( RBT_NODE * node, RBT_NODE * parent );
static inline void rbtree_prv_setRight ( RBT_NODE * node, RBT_NODE * parent );
static inline RBT_NODE * rbtree_prv_getNext ( RBT_NODE * node );
static inline RBT_NODE * rbtree_prv_getPrev ( RBT_NODE * node );
static inline void rbtree_prv_severLinkParentToNode ( RBT_NODE * node );
static inline void rbtree_prv_rotateLeft ( RBT_NODE * node, RBT_TREE * tree );
static inline void rbtree_prv_rotateRight ( RBT_NODE * node, RBT_TREE * tree );
static inline RBTREE_STATUS rbtree_prv_insertNode ( RBT_NODE * ins_node, RBT_TREE * tree );
static inline void rbtree_prv_deleteBSTCheck2 ( RBT_NODE * rmnode, RBT_TREE * tree );
static inline void rbtree_prv_deleteBSTCheck1 ( RBT_NODE * rmnode, RBT_TREE * tree );
static inline void rbtree_prv_deleteRBFixUp ( RBT_NODE * rmnode, RBT_TREE * tree );
static inline void rbtree_prv_insertRBFixUp ( RBT_NODE * insnode, RBT_TREE * tree );
static inline RBTREE_STATUS rbtree_prv_removeNodeFromTree ( RBT_NODE * rmnode, RBT_TREE * tree );
static inline void rbtree_prv_resetKeySeed ( RBT_TREE * tree );
static inline RBT_NODE * rbtree_prv_findKey ( RBTREE_KEY key, RBT_NODE * node );


/* shorthand form's */
#define getColour(n) rbtree_prv_getColour(n)
#define setColour(colour,n) rbtree_prv_setColour(colour,n)
#define isRoot(n) rbtree_prv_isRoot(n)
#define isRed(n) rbtree_prv_isRed(n)
#define isBlack(n) rbtree_prv_isBlack(n)
#define leftRotate(n,tree) rbtree_prv_rotateLeft(n,tree)
#define rightRotate(n,tree) rbtree_prv_rotateRight(n,tree)
#define getRoot(tree) rbtree_prv_getRoot(tree)
#define getFirst(n) rbtree_prv_getFirst(n)
#define getLast(n) rbtree_prv_getLast(n)
#define getNext(n) rbtree_prv_getNext(n)
#define getPrev(n) rbtree_prv_getPrev(n)
#define getParent(n) rbtree_prv_getParent(n)
#define getGrandParent(n) getParent(getParent(n))
#define getLeft(n) rbtree_prv_getLeft(n)
#define getRight(n) rbtree_prv_getRight(n)
#define getParentLeft(n) getLeft(getParent(n))
#define getParentRight(n) getRight(getParent(n))
#define getGrandParentLeft(n) getLeft(getGrandParent(n))
#define getGrandParentRight(n) getRight(getGrandParent(n))
#define getSibling(n) rbtree_prv_getSibling(n)
#define getUncle(n) rbtree_prv_getUncle(n)
#define setRoot(n,tree) rbtree_prv_setRoot(n,tree);
#define setParent(parent,n) rbtree_prv_setParent(parent, n);


/* private functions - start */
static void* rbtree_prv_memAlloc_default ( size_t size )
{
    return rbtree_default_memAlloc(size);
}

static void rbtree_prv_memFree_default ( void * ptr )
{
    rbtree_default_memFree(ptr);
}

static inline RBT_NODE * rbtree_prv_createNode ( RBT_TREE * tree )
{
    RBT_NODE * node = tree->mem_alloc(sizeof(RBT_NODE));
    
    if ( node )
    {
        RBTPRINT_DBG_I("Alloc'ed %p",node);        
        memset(node, '\0', sizeof(RBT_NODE));
    }
    else
    {
        RBTPRINT_DBG_E("Malloc failure");        
    }
    
    return node;
}

static inline void rbtree_prv_freeNode ( RBT_NODE * node, RBT_TREE * tree )
{
    if ( node )
    {
        tree->mem_free(node);
        RBTPRINT_DBG_I("Free'd %p",node);        
    }
}

static inline RBT_COLOUR rbtree_prv_getColour ( RBT_NODE * node )
{
	return node == NULL ? RBT_COLOUR_BLACK : node->colour;
}

static inline void rbtree_prv_setColour ( RBT_COLOUR colour, RBT_NODE * node)
{
    if ( node )
    {
        node->colour = colour;
    }
}

static inline RBT_NODE * rbtree_prv_getSibling ( RBT_NODE * node )
{
    if ( node == getParent(node)->left )
    {
        return getParent(node)->right;
    }
    else
    {
        return getParent(node)->left;
    }
}

static inline RBT_NODE * rbtree_prv_getUncle ( RBT_NODE * node )
{
    RBT_NODE * unc_node = NULL;
    RBT_NODE * par_node = getParent(node);
    
	if ( ( node ) && ( par_node ) )
    {
        unc_node = getSibling(par_node);
    }
    
    return unc_node;
}

static inline RBT_NODE * rbtree_prv_getParent ( RBT_NODE * node )
{
    RBT_NODE * ret_node = NULL;
    
    if ( node )
    {
        ret_node = node->parent;
    }
    
	return ret_node;
}

static inline void rbtree_prv_setParent ( RBT_NODE * parent, RBT_NODE * node)
{
    if ( node )
    {
        node->parent = parent;
    }
}

static inline bool rbtree_prv_isRoot( RBT_NODE * node)
{
	return (bool) (rbtree_prv_getParent(node) == NULL);
}

static inline void rbtree_prv_setRoot( RBT_NODE * node, RBT_TREE * tree )
{
	tree->rootNode = node;
}

static inline RBT_NODE * rbtree_prv_getRoot( RBT_TREE * tree )
{
	return tree->rootNode;
}

static inline bool rbtree_prv_isBlack( RBT_NODE * node )
{
	return (bool) (getColour(node) == RBT_COLOUR_BLACK);
}

static inline bool rbtree_prv_isRed( RBT_NODE * node )
{
	return (bool) (getColour(node) == RBT_COLOUR_RED);
}

static inline RBT_NODE * rbtree_prv_getFirst( RBT_NODE * node )
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

static inline RBT_NODE * rbtree_prv_getLast( RBT_NODE * node )
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

static inline RBT_NODE * rbtree_prv_getLeft ( RBT_NODE * node )
{
    RBT_NODE * ret_node = NULL;
    
    if ( node )
    {
        ret_node = node->left;
    }
    
	return ret_node;
}

static inline RBT_NODE * rbtree_prv_getRight ( RBT_NODE * node )
{
    RBT_NODE * ret_node = NULL;
    
    if ( node )
    {
        ret_node = node->right;
    }
    
	return ret_node;
}

static inline void rbtree_prv_setLeft ( RBT_NODE * node, RBT_NODE * parent )
{
    if ( parent )
    {
        parent->left = node;
    }
}

static inline void rbtree_prv_setRight ( RBT_NODE * node, RBT_NODE * parent )
{
    if ( parent )
    {
        parent->right = node;
    }
}

static inline RBT_NODE * rbtree_prv_getNext ( RBT_NODE * node )
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

static inline RBT_NODE * rbtree_prv_getPrev ( RBT_NODE * node )
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

static inline void rbtree_prv_severLinkParentToNode ( RBT_NODE * node )
{
    if ( ( node ) && ( node->parent ) )
    {
        RBT_NODE * parent = node->parent;
        node->parent = NULL;
        
        if ( parent->left == node )
        {
            parent->left = NULL;
        }
        else if ( parent->right == node )
        {
            parent->right = NULL;
        }
    }
}

static inline void rbtree_prv_rotateLeft ( RBT_NODE * node, RBT_TREE * tree )
{
	RBT_NODE * p = node;
	RBT_NODE * q = getRight(node);

    /* These can't be NULL to rotate */
    if ( (p) && (q) )
    {
        RBT_NODE * parent = getParent(p);

        if ( isRoot(p) == false )
        {
            if (parent->left == p)
            {
                parent->left = q;
            }
            else
            {
                parent->right = q;
            }
        }
        else
        {
            setRoot(q,tree);
        }
        
        setParent(parent, q);
        setParent(q, p);
        
        p->right = q->left;
        
        if (p->right)
        {
            setParent(p, p->right);
        }
        
        q->left = p;
    }
}

static inline void rbtree_prv_rotateRight ( RBT_NODE * node, RBT_TREE * tree )
{
	RBT_NODE * p = node;
	RBT_NODE * q = getLeft(node);
    
    /* These can't be NULL to rotate */
    if ( (p) && (q) )
    {
        RBT_NODE * parent = getParent(p);

        if ( isRoot(p) == false)
        {
            if (parent->left == p)
            {
                parent->left = q;
            }
            else
            {
                parent->right = q;
            }
        }
        else
        {
            setRoot(q,tree);
        }
        
        setParent(parent, q);
        setParent(q, p);
        
        p->left = q->right;
        
        if (p->left)
        {
            setParent(p, p->left);
        }
        
        q->right = p;
    }
}

static inline RBTREE_STATUS rbtree_prv_insertNode ( RBT_NODE * ins_node, RBT_TREE * tree )
{
    RBTREE_STATUS status = RBTREE_STATUS_FAIL;
    RBT_NODE * root = getRoot(tree);
    
    if ( root == NULL )
    {
        setRoot(ins_node,tree);
        status = RBTREE_STATUS_OK;
    }
    else
    {
        RBT_NODE * cur_node = root;

        while ( cur_node != NULL )
        {
            if ( cur_node->key > ins_node->key )
            {
                RBT_NODE * l_cur_node = cur_node->left;
                
                if ( l_cur_node == NULL )
                {
                    /* no left child so insert here */
                    cur_node->left = ins_node;
                    ins_node->parent = cur_node;

                    status = RBTREE_STATUS_OK;
                    
                    break;
                }
                else
                {
                    /* and loop again */
                    cur_node = l_cur_node;
                }
            }
            else if ( cur_node->key < ins_node->key )
            {
                RBT_NODE * r_cur_node = cur_node->right;
                
                if ( r_cur_node == NULL )
                {
                    /* no right child so insert here */
                    cur_node->right = ins_node;
                    ins_node->parent = cur_node;

                    status = RBTREE_STATUS_OK;
                    
                    break;
                }
                else
                {
                    /* and loop again */
                    cur_node = r_cur_node;
                }
            }
            else
            {
                status = RBTREE_STATUS_FAIL_KEY_ALREADY_STORED;
                
                break;
            }
        } /* end while */
        
        rbtree_checks_validateNodeLinks(cur_node);
        rbtree_checks_validateNodeLinks(cur_node?cur_node->parent:NULL);
    }
    
    return status;
}

static inline void rbtree_prv_deleteBSTCheck2 ( RBT_NODE * rmnode, RBT_TREE * tree )
{
    RBT_NODE * nLeft = rmnode->left;
    RBT_NODE * nRight = rmnode->right;
    RBT_NODE ** parent_link;

    /* find the parent link*/
    if ( rmnode->parent->left == rmnode )
    {
        parent_link = &rmnode->parent->left;
    }
    else
    {
        parent_link = &rmnode->parent->right;
        RBTPRINT_ASSERT(rmnode->parent->right==rmnode);
    }
    
    /* z has no children - simply remove from parent */
    if ( ( nLeft == NULL ) && ( nRight == NULL ) )
    {
        /* find the parent link & remove */
        *parent_link = NULL;
    }
    /* z has only child left. Replace nodes position with child left */
    else if ( (nLeft!=NULL) && (nRight==NULL) )
    {
        *parent_link = nLeft;
        nLeft->parent = rmnode->parent;
    }
    /* z has only child right. Replace nodes position with child right */
    else if ( (nLeft==NULL) && (nRight!=NULL) )
    {
        *parent_link = nRight;
        nRight->parent = rmnode->parent;
    }
    /* z has both children */
    else
    {
        /* find the closest living relative on the right side & drop in place */
        RBT_NODE * replacement_node = rbtree_prv_getFirst(nRight);
        RBTPRINT_ASSERT(replacement_node!=NULL);
        RBTPRINT_ASSERT(getLeft(replacement_node)==NULL);
        RBTPRINT_ASSERT(getRight(replacement_node)==NULL);
        *parent_link = replacement_node;
        
        /* now tidy up the links */
        replacement_node->parent = getParent(rmnode);
        replacement_node->left = nLeft;
        replacement_node->right = nRight;        
    }
}

static inline void rbtree_prv_deleteBSTCheck1 ( RBT_NODE * rmnode, RBT_TREE * tree )
{
    /* node is root. so set root to null */
    if ( rmnode->parent == NULL )
    {
        RBTPRINT_ASSERT(rmnode==tree->rootNode);

        RBT_NODE * rep_node = NULL;
        
        if ( rmnode->right )
        {
            rep_node = getFirst(rmnode->right);
            
            if ( rep_node != rmnode->right )
            {
                rep_node->left = rmnode->left;
                rep_node->right = rmnode->right;
            }

            rbtree_prv_severLinkParentToNode(rep_node);

            if ( rep_node->right )
            {
                rep_node->right->parent = rep_node;
            }

            tree->rootNode = rep_node;
        }
        else if ( rmnode->left )
        {
            rep_node = getLast(rmnode->left);

            if ( rep_node != rmnode->left )
            {
                rep_node->left = rmnode->left;
                rep_node->right = rmnode->right;
            }

            rbtree_prv_severLinkParentToNode(rep_node);
            
            if ( rep_node->left )
            {
                rep_node->left->parent = rep_node;
            }

            tree->rootNode = rep_node;
        }
        else
        {
            /* root is only node */
            tree->rootNode = NULL;
        }
    }
    else
    {
        rbtree_prv_deleteBSTCheck2(rmnode,tree);
    }
}

static inline void rbtree_prv_deleteRBFixUp ( RBT_NODE * rmnode, RBT_TREE * tree )
{
    RBT_NODE * cur_node = rmnode;
    
    if ( getColour(cur_node) == RBT_COLOUR_RED )
    {
        /* only adjust if node is black */
        return;
    }
    
    while ( (cur_node!=NULL) && 
            (isBlack(cur_node)) )
    {
        if ( cur_node == getParentLeft(cur_node) )
        {            
            RBT_NODE * sibling = getParentRight(cur_node);

            if ( getColour(sibling) == RBT_COLOUR_RED )
            {
                setColour(RBT_COLOUR_BLACK, sibling);
                setColour(RBT_COLOUR_RED, getParent(cur_node));
                
                leftRotate(cur_node->parent, tree);
            }
            
            if ( ( isBlack(getLeft(sibling)) ) &&
                 ( isBlack(getRight(sibling)) ) )
            {
                setColour(RBT_COLOUR_RED, sibling);
                
                cur_node = getParent(cur_node);
            }
            else
            {
                if ( isBlack(getRight(sibling)) )
                {
                    setColour(RBT_COLOUR_BLACK, getLeft(sibling));
                    setColour(RBT_COLOUR_RED, sibling);

                    rightRotate(sibling, tree);

                    sibling = getParentRight(cur_node);
                }
                
                setColour(getColour(getParent(cur_node)), sibling);
                setColour(RBT_COLOUR_BLACK, getParent(cur_node));
                setColour(RBT_COLOUR_BLACK, getRight(sibling));
                
                leftRotate(getParent(cur_node), tree);
                
                /* adjustments finished */
                break;
            }
        }
        /* same as previous branch */
        else if ( cur_node == getParentRight(cur_node) )
        {
            RBT_NODE * sibling = getParentLeft(cur_node);
            
            if ( getColour(sibling) == RBT_COLOUR_RED )
            {
                setColour(RBT_COLOUR_BLACK, sibling);
                setColour(RBT_COLOUR_RED,   getParent(cur_node));
                
                rightRotate(getParent(cur_node), tree);
                
                sibling = getParentLeft(cur_node);
            }
            
            if ( ( isBlack(getRight(sibling)) ) &&
                 ( isBlack(getLeft(sibling)) ) )
            {
                setColour(RBT_COLOUR_RED, sibling);
                
                cur_node = getParent(cur_node);
            }
            else
            {
                if ( isBlack(getLeft(sibling)) )
                {
                    setColour(RBT_COLOUR_BLACK, getRight(sibling));
                    setColour(RBT_COLOUR_RED, sibling);
                    
                    leftRotate(sibling, tree);
                    
                    sibling = getParentLeft(cur_node);
                }
                
                setColour(getColour(getParent(cur_node)), sibling);
                setColour(RBT_COLOUR_BLACK, getParent(cur_node));
                setColour(RBT_COLOUR_BLACK, getLeft(sibling));

                rightRotate(getParent(cur_node), tree);
                
                /* adjustments complete */
                break;
            }
        }
        else
        {
            cur_node = getParent(cur_node);
        }
    }
    
    setColour(RBT_COLOUR_BLACK, tree->rootNode);
}

static inline void rbtree_prv_insertRBFixUp ( RBT_NODE * insnode, RBT_TREE * tree )
{
    setColour(RBT_COLOUR_RED,insnode);
    
    RBT_NODE * cur_node = insnode;

    while ( (cur_node!=NULL) && (isRed(getParent(cur_node))) )
    {
        if ( getParent(cur_node) == getGrandParentLeft(cur_node) )
        {
            if ( isRed(getGrandParentRight(cur_node)) )
            {
                setColour(RBT_COLOUR_BLACK, getParent(cur_node));
                setColour(RBT_COLOUR_BLACK, getGrandParentRight(cur_node));
                setColour(RBT_COLOUR_RED,   getGrandParent(cur_node));
                
                cur_node = getGrandParent(cur_node);
            }
            else 
            {
                if ( cur_node == getParentRight(cur_node) )
                {
                    leftRotate(getParent(cur_node), tree);
                    /* Move up to our parent */
                    cur_node = getParent(cur_node);
                }
                
                setColour(RBT_COLOUR_BLACK, getParent(cur_node));
                setColour(RBT_COLOUR_RED,   getGrandParent(cur_node));
                
                rightRotate(getGrandParent(cur_node), tree);
            }
        }
        /* symmetric to above branch */
        else if ( getParent(cur_node) == getGrandParentRight(cur_node) )
        {
            if ( isRed(getGrandParentLeft(cur_node)) )
            {
                setColour(RBT_COLOUR_BLACK, getParent(cur_node));
                setColour(RBT_COLOUR_BLACK, getGrandParentLeft(cur_node));
                setColour(RBT_COLOUR_RED,   getGrandParent(cur_node));
                
                cur_node = getGrandParent(cur_node);
            }
            else
            {
                if ( cur_node == getParentLeft(cur_node) )
                {
                    rightRotate(getParent(cur_node), tree);
                    /* Move up to our parent */
                    cur_node = getParent(cur_node);
                }
                
                setColour(RBT_COLOUR_BLACK, getParent(cur_node));
                setColour(RBT_COLOUR_RED,   getGrandParent(cur_node));
                
                leftRotate(getParent(cur_node), tree);
            }
        }
        else
        {
            RBTPRINT_ASSERT(getParent(cur_node)==tree->rootNode);
            /* parent link not linked to grandparent - cur_node should be child of root node since retrieving the grandParent here is NULL */
            cur_node = getParent(cur_node);
        }
    } /* end while */
    
    setColour(RBT_COLOUR_BLACK, tree->rootNode);
}

static inline RBTREE_STATUS rbtree_prv_removeNodeFromTree ( RBT_NODE * rmnode, RBT_TREE * tree )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    RBT_LOCK_MUTEX(tree->mutex);
    
    /* remove node from tree maintaing binary-search-tree */
    rbtree_prv_deleteBSTCheck1(rmnode, tree);
    
    /* now fix any issue's to restore red-black tree properties */
    rbtree_prv_deleteRBFixUp(rmnode, tree);
    
    /* check for rollover */
    RBTPRINT_ASSERT(tree->nodeCount>0);
    tree->nodeCount--;
    
    if ( tree->nodeCount == 0U )
    {
        /* no nodes so this is safe */
        rbtree_prv_resetKeySeed(tree);
    }
    
    RBT_UNLOCK_MUTEX(tree->mutex);            
    
    status = RBTREE_STATUS_OK;
    
    return status;
}

static inline void rbtree_prv_resetKeySeed ( RBT_TREE * tree )
{
    tree->keySeed = RBTREE_KEY_INVALID + 1U;
}

static inline RBT_NODE * rbtree_prv_findKey ( RBTREE_KEY key, RBT_NODE * node )
{
    while ( node )
    {
        if ( key < node->key )
        {
            node = rbtree_prv_getLeft(node);
        }
        else if ( key > node->key )
        {
            node = rbtree_prv_getRight(node);
        }
        else
        {
            /* match */
            break;
        }
    }
    
    return node;
}
/* private functions - end */

RBTREE_STATUS rbtree_createTree ( RBTREE_HANDLE * handle, rbtree_memalloc_t mem_alloc, rbtree_memfree_t mem_free )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;

    if ( handle != NULL ) /* mem pointers can be NULL */
    {
        size_t treeSize = sizeof(RBT_TREE);
        RBT_TREE * tree = NULL;
        
        if ( mem_alloc )
        {
            tree = mem_alloc(treeSize);
        }
        else
        {
            tree = rbtree_prv_memAlloc_default(treeSize);
        }

        if ( tree )
        {
            tree->nodeCount = 0U;
            tree->rootNode = NULL;
            
            rbtree_prv_resetKeySeed(tree);

            RBT_INIT_MUTEX(tree->mutex);

            if ( mem_alloc )
            {
                tree->mem_alloc = mem_alloc;
            }
            else
            {
                tree->mem_alloc = rbtree_prv_memAlloc_default;
            }
            
            if ( mem_free )
            {
                tree->mem_free = mem_free;
            }
            else
            {
                tree->mem_free = rbtree_prv_memFree_default;
            }
            
            *handle = tree;
            
            status = RBTREE_STATUS_OK;
        }
        else
        {
            RBTPRINT_DBG_E("Malloc failure");
            status = RBTREE_STATUS_FAIL_MALLOC_FAILURE;
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_destroyTree ( RBTREE_HANDLE handle )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( handle != RBTREE_HANDLE_INVALID )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        rbtree_memfree_t mem_free = tree->mem_free;
        
        /* free all elements */
        RBT_TERM_MUTEX(tree->mutex);

        RBT_NODE * cur_node = getFirst(tree->rootNode);

        while ( cur_node )
        {
            RBT_NODE * next_node = getNext(cur_node);
            
            rbtree_prv_removeNodeFromTree(cur_node,tree);
            rbtree_prv_freeNode(cur_node,tree);
            
            cur_node = next_node;
        }

        mem_free(tree);
        
        status = RBTREE_STATUS_OK;
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_insert ( RBTREE_HANDLE handle, void * storevalue, RBTREE_KEY * key )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;

    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( key != NULL ) )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        
        RBT_NODE * ins_node = rbtree_prv_createNode(tree);
        
        if ( ins_node )
        {
            RBT_LOCK_MUTEX(tree->mutex);
            
            ins_node->value = storevalue;
            ins_node->key = tree->keySeed;
            
            /* insert node into tree maintaining BST */
            status = rbtree_prv_insertNode(ins_node,tree);
            
            if ( status == RBTREE_STATUS_OK )
            {
                ins_node->colour = RBT_COLOUR_RED;
                
                /* fix the red-black tree properties */
                rbtree_prv_insertRBFixUp(ins_node, tree);

                tree->keySeed++;
                RBTPRINT_ASSERT(tree->keySeed<RBT_TREE_KEYSEED_MAXVALUE);

                tree->nodeCount++;
                RBTPRINT_ASSERT(tree->nodeCount<RBT_TREE_NODECOUNT_MAXVALUE);
                
                status = rbtree_checks_isTreeValid(handle);
                
                *key = ins_node->key;
            }
            else
            {
                rbtree_prv_freeNode(ins_node,tree);
            }
            
            RBT_UNLOCK_MUTEX(tree->mutex);            
        }
        else
        {
            RBTPRINT_DBG_E("Malloc failure");
            status = RBTREE_STATUS_FAIL_MALLOC_FAILURE;
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_retrieveByKey ( RBTREE_HANDLE handle, RBTREE_KEY key, void ** ret_data )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( key != RBTREE_KEY_INVALID ) && ( ret_data != NULL ) )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        
        RBT_NODE * node = rbtree_prv_findKey(key, tree->rootNode);
        
        if ( node )
        {
            *ret_data = node->value;
            status = RBTREE_STATUS_OK;
        }
        else
        {
            RBTPRINT_DBG_E("Key does not exist");
            status = RBTREE_STATUS_FAIL_KEY_DOES_NOT_EXIST;
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_retrieveByIndex ( RBTREE_HANDLE handle, uint32_t index, void ** ret_data, RBTREE_KEY * ret_key )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( ret_data != NULL ) && ( ret_key != NULL ) )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        
        if ( index <= tree->nodeCount )
        {
            RBT_NODE * node = getFirst(tree->rootNode);
            uint32_t i = 0U;
            
            for (i=0U; i<index; i++)
            {
                RBTPRINT_ASSERT(node!=NULL);
                node = getNext(node);
            }
            
            if ( node )
            {
                *ret_data = node->value;
                *ret_key = node->key;
                status = RBTREE_STATUS_OK;
            }
            else
            {
                status = RBTREE_STATUS_FAIL;
            }
        }
        else
        {
            RBTPRINT_DBG_E("Index: %d out of range",index);
            status = RBTREE_STATUS_FAIL_INDEX_OUT_OF_RANGE;
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_deleteByKey ( RBTREE_HANDLE handle, RBTREE_KEY key )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( key != RBTREE_KEY_INVALID ) )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        
        RBT_NODE * node = rbtree_prv_findKey(key, tree->rootNode);
        
        if ( node )
        {
            status = rbtree_prv_removeNodeFromTree(node,tree);
            rbtree_prv_freeNode(node,tree);
            
            if ( status == RBTREE_STATUS_OK )
            {
                status = rbtree_checks_isTreeValid(handle);
            }
        }
        else
        {
            RBTPRINT_DBG_W("Key:%u does not exist",key);
            status = RBTREE_STATUS_FAIL_KEY_DOES_NOT_EXIST;
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_deleteByValue ( RBTREE_HANDLE handle, void * value )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( handle != RBTREE_HANDLE_INVALID )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        RBT_NODE * node = rbtree_prv_getFirst(tree->rootNode);
        uint32_t i = 0U;
        bool matchFound = false;
        
        for ( i=0U; i<tree->nodeCount; i++ )
        {
            if ( node )
            {
                if ( node->value == value )
                {
                    rbtree_prv_removeNodeFromTree(node,tree);
                    rbtree_prv_freeNode(node,tree);

                    matchFound = true;
                }
            }
            
            node = rbtree_prv_getNext(node);
        }
        
        if ( matchFound )
        {
            /* all is ok. Check validatity of tree */
            status = rbtree_checks_isTreeValid(handle);
        }
        else
        {
            RBTPRINT_DBG_W("Value:%p does not exist",value);
            status = RBTREE_STATUS_FAIL_VALUE_DOES_NOT_EXIST;
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_deleteByIndex ( RBTREE_HANDLE handle, uint32_t index )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( handle != RBTREE_HANDLE_INVALID )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        
        if ( index <= tree->nodeCount )
        {
            RBT_NODE * node = getFirst(tree->rootNode);
            uint32_t i = 0U;
            
            for ( i=0U; i<index; i++ )
            {
                RBTPRINT_ASSERT(node!=NULL);
                node = getNext(node);
            }

            if ( node )
            {
                status = rbtree_prv_removeNodeFromTree(node,tree);
                rbtree_prv_freeNode(node,tree);

                if ( status == RBTREE_STATUS_OK )
                {
                    status = rbtree_checks_isTreeValid(handle);
                }
            }
            else
            {
                RBTPRINT_DBG_W("Node:%p does not exist",node);
                status = RBTREE_STATUS_FAIL_INDEX_OUT_OF_RANGE;
            }
        }
        else
        {
            RBTPRINT_DBG_W("Index out of range: %u",index);
            status = RBTREE_STATUS_FAIL_INDEX_OUT_OF_RANGE;
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_find ( RBTREE_HANDLE handle, rbtree_comparator_t cmp_fn, void * userdata, void ** ret_storevalue, RBTREE_KEY * ret_key )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( cmp_fn != NULL ) && ( ret_storevalue != NULL ) && ( ret_key != NULL ) )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        
        RBT_NODE * node = rbtree_prv_getFirst(tree->rootNode);
        
        bool res = false;

        status = RBTREE_STATUS_FAIL;

        while ( node != NULL )
        {
            res = cmp_fn(node->value,userdata);
            
            if ( res )
            {
                *ret_storevalue = node->value;
                *ret_key = node->key;
                
                status = RBTREE_STATUS_OK;
                
                break;
            }
            else
            {
                node = rbtree_prv_getNext(node);
            }
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_doesKeyExist ( RBTREE_HANDLE handle, RBTREE_KEY key, bool * doesExist )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( key != RBTREE_KEY_INVALID ) && ( doesExist != NULL ) )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        
        RBT_NODE * node = rbtree_prv_findKey(key, tree->rootNode);
        
        if ( node )
        {
            *doesExist = true;
        }
        else
        {
            *doesExist = false;
        }
        
        status = RBTREE_STATUS_OK;
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


bool rbtree_doesValueExist_prv_comp (void* storevalue, void* userdata)
{
    return ( storevalue == userdata ) ? true : false;
}


RBTREE_STATUS rbtree_doesValueExist ( RBTREE_HANDLE handle, void * storevalue, bool * doesExist )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( doesExist != NULL ) )
    {
        void * ret_storevalue = NULL;
        RBTREE_KEY ret_key = RBTREE_KEY_INVALID;
        
        status = rbtree_find ( handle, rbtree_doesValueExist_prv_comp, storevalue, &ret_storevalue, &ret_key );
        
        if ( status == RBTREE_STATUS_OK )
        {
            *doesExist = true;
        }
        else
        {
            *doesExist = false;
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_entryCount ( RBTREE_HANDLE handle, uint32_t * numberOfEntries )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( numberOfEntries != NULL ) )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        
        *numberOfEntries = tree->nodeCount;
        
        status = RBTREE_STATUS_OK;
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_copyInTree ( RBTREE_HANDLE handle, RBTREE_HANDLE copyInTree )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( copyInTree != RBTREE_HANDLE_INVALID ) )
    {
        uint32_t copyTreeSize = 0U;

        status = rbtree_entryCount(copyInTree, &copyTreeSize);
        
        if ( status == RBTREE_STATUS_OK )
        {
            for ( uint32_t i = 0U; i<copyTreeSize; i++ )
            {
                void * storeval = NULL;
                RBTREE_KEY key = RBTREE_KEY_INVALID;
                RBTREE_KEY retrive_key = RBTREE_KEY_INVALID;
                
                rbtree_retrieveByIndex(copyInTree, i, &storeval, &retrive_key);
                
                status = rbtree_insert(handle, storeval, &key);
                
                if ( status != RBTREE_STATUS_OK )
                {
                    RBTPRINT_DBG_E("Insertion at %u failed",i);
                    break;
                }
            }
            
            if ( status == RBTREE_STATUS_OK )
            {
                status = rbtree_checks_isTreeValid(handle);
            }
        }
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


RBTREE_STATUS rbtree_getMemoryAllocator ( RBTREE_HANDLE handle, rbtree_memalloc_t * mem_alloc, rbtree_memfree_t * mem_free )
{
    RBTREE_STATUS status = RBTREE_STATUS_UNDEF;
    
    if ( ( handle != RBTREE_HANDLE_INVALID ) && ( mem_alloc != NULL ) && ( mem_free != NULL ) )
    {
        RBT_TREE * tree = (RBT_TREE *)handle;
        
        *mem_alloc  = tree->mem_alloc;
        *mem_free   = tree->mem_free;
        
        status = RBTREE_STATUS_OK;
    }
    else
    {
        RBTPRINT_DBG_E("Invalid param");
        status = RBTREE_STATUS_FAIL_INVALID_PARAM;
    }
    
    return status;
}


#undef getColour
#undef setColour
#undef isRoot
#undef isRed
#undef isBlack
#undef leftRotate
#undef rightRotate
#undef getRoot
#undef getFirst
#undef getLast
#undef getNext
#undef getPrev
#undef getParent
#undef getGrandParent
#undef getLeft
#undef getRight
#undef getParentLeft
#undef getParentRight
#undef getGrandParentLeft
#undef getGrandParentRight
#undef getSibling
#undef getUncle
#undef setRoot
#undef setParent

