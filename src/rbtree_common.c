/**
 @file
 Red-Black Binary Search Tree - common
 
 @author Ryan Powell
 @date 23-12-12
 @copyright Copyright (c) 2012  Ryan Powell
 @licence https://raw.github.com/Ryandev/RBTreelib/master/LICENSE
 */


#include "rbtree_common.h"
#include <string.h>     /* for memset */
#include <stdio.h>      /* for FILE */


static inline RBT_NODE * rbtree_common_prv_getNext ( RBT_NODE * node );
static inline RBT_NODE * rbtree_common_prv_getFirst( RBT_NODE * node );

#define getNext(n) rbtree_common_prv_getNext(n);
#define getFirst(n) rbtree_common_prv_getFirst(n);


static inline RBT_NODE * rbtree_common_prv_getNext ( RBT_NODE * node )
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

static inline RBT_NODE * rbtree_common_prv_getFirst( RBT_NODE * node )
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


void rbtree_prv_printSummary ( RBT_TREE * tree, FILE * fp )
{
    uint32_t i = 0U;
    uint32_t key = 0U;
    uint32_t parent = 0U;
    uint32_t left = 0U;
    uint32_t right = 0U;
    char colour[8];
    RBT_NODE * node = getFirst(tree->rootNode);

    fprintf(fp, "\n\n");
    fprintf(fp, "  Idx   |  Key   | Parent |  Left  | Right  | Colour |  Pointer \n");
    fprintf(fp, "________|________|________|________|________|________|__________\n");

    while ( node )
    {
        i++;
        key = (uint32_t)node->key;
        
        if ( node->parent )
        {
            parent = node->parent->key;
        }
        else
        {
            parent = 0U;
        }
        
        if ( node->left )
        {
            left = node->left->key;
        }
        else
        {
            left = 0U;
        }

        if ( node->right )
        {
            right = node->right->key;
        }
        else
        {
            right = 0U;
        }
        
        if ( node->colour == RBT_COLOUR_RED )
        {
            snprintf(colour, 8, "%s", "RED" );
        }
        else
        {
            snprintf(colour, 8, "%s", "BLACK" );
        }
        
        fprintf(fp, " %6d | %6d | %6d | %6d | %6d | %6s | %8p \n",i,key,parent,left,right,colour,node);

        node = getNext(node);
    }
    
    fprintf(fp, "\n\n");
}


#undef getNext
#undef getFirst

