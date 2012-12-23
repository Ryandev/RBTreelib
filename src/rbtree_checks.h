/**
 @file
 Red-Black Binary Search Tree - Validation checks
 
 @author Ryan Powell
 @date 23-12-12
 @copyright Copyright (c) 2012  Ryan Powell
 @licence https://raw.github.com/mcCuppaT/RBTreelib/master/LICENSE
 */


#ifndef __RBTREE_CHECKS_H
#define __RBTREE_CHECKS_H


#ifdef __cplusplus
extern "C" {
#endif
    

#include "rbtree_common.h"


bool rbtree_checks_validateNodeLinks ( RBT_NODE * node );

bool rbtree_checks_validareAllNodeLinks ( RBT_TREE * tree );

bool rbtree_checks_prv_isTreeValid_BST ( RBT_NODE * rootNode );

bool rbtree_checks_prv_isTreeValid_RedHasBlackChildren ( RBT_NODE * rootNode );

uint32_t rbtree_checks_prv_getNodeCount ( RBT_NODE * rootNode );

bool rbtree_checks_prv_isTreeValid_isBlackHeightCorrect ( RBT_NODE * rootNode );

RBTREE_STATUS rbtree_checks_isTreeValid ( RBT_TREE * tree );


#ifdef __cplusplus
}
#endif


#endif /* __RBTREE_CHECKS_H */
