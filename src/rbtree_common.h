/**
 @file
 Red-Black Binary Search Tree - common
 
 @author Ryan Powell
 @date 23-12-12
 @copyright Copyright (c) 2012  Ryan Powell
 @licence https://raw.github.com/Ryandev/RBTreelib/master/LICENSE
 */


#ifndef _RBLIB_COMMON_H
#define _RBLIB_COMMON_H



#ifdef __cplusplus
extern "C" {
#endif
    

#include <stdio.h>      /* for FILE */
#include <stdlib.h>     /* for malloc/free */

#if (__STDC_VERSION__ >= 201112L) && !defined(__STDC_NO_THREADS__)
#  define RBT_USE_C11THREADS
#  include <threads.h>
#elif defined(RBT_USE_TINYCTHREAD) || defined(TINYCTHREAD_VERSION)
#  if !defined(TINYCTHREAD_VERSION)
#    include <tinycthread.h>
#  endif
#  define RBT_USE_C11THREADS
#else
#  include <pthread.h>
#endif

#include "rbtree.h"

#if defined(RBT_USE_C11THREADS)
#define RBT_MUTEX_TYPE mtx_t
#define RBT_LOCK_MUTEX(a) do { mtx_lock(&(a)); } while(0)
#define RBT_UNLOCK_MUTEX(a) do { mtx_unlock(&(a)); } while(0)
#define RBT_INIT_MUTEX(a) do { mtx_init(&(a),mtx_plain); } while(0)
#define RBT_TERM_MUTEX(a) do { mtx_destroy(&(a)); } while(0)
#else
#define RBT_MUTEX_TYPE pthread_mutex_t
#define RBT_LOCK_MUTEX(a) do { pthread_mutex_lock(&(a)); } while(0)
#define RBT_UNLOCK_MUTEX(a) do { pthread_mutex_unlock(&(a)); } while(0)
#define RBT_INIT_MUTEX(a) do { pthread_mutex_init(&(a),NULL); } while(0)
#define RBT_TERM_MUTEX(a) do { pthread_mutex_destroy(&(a)); } while(0)
#endif

typedef enum _RBT_COLOUR
{
    RBT_COLOUR_UNDEF = 0,
    RBT_COLOUR_RED,
    RBT_COLOUR_BLACK,
    RBT_COLOUR_LAST_VALUE,
} RBT_COLOUR;

typedef struct _RBT_NODE
{
    RBT_COLOUR colour;
    RBTREE_KEY key;
    void * value;
    struct _RBT_NODE * left;
    struct _RBT_NODE * right;
    struct _RBT_NODE * parent;
} RBT_NODE;

typedef struct _RBT_TREE
{
    uint32_t nodeCount;
    uint32_t keySeed;
    RBT_MUTEX_TYPE mutex;
    RBT_NODE * rootNode;
    rbtree_memalloc_t mem_alloc;
    rbtree_memfree_t mem_free;
} RBT_TREE;

    
#define RBT_TREE_KEYSEED_MAXVALUE (0xFFFFFFFFU)
#define RBT_TREE_NODECOUNT_MAXVALUE (0xFFFFFFFFU)

    
#define rbtree_default_memAlloc malloc
#define rbtree_default_memFree free

    
void rbtree_prv_printSummary ( RBT_TREE * tree, FILE * fp );


/* Enable to trace any defects in DebugPrint */
/* #define RBT_PRINT_DEBUG */

#ifdef RBT_PRINT_DEBUG

    #include <stdio.h>

    #define RBTPRINT_DBG(level,format, ... ) \
        do \
        { \
            fprintf (stdout,"rbtree %s (%s:%d): ",level,__FUNCTION__,__LINE__); \
            fprintf (stdout,format, ##__VA_ARGS__ ); \
            fprintf (stdout, "\n"); \
        } while (0)

    #define RBTPRINT_DBG_E(fmt, ... ) RBTPRINT_DBG("-ERROR-",fmt, ##__VA_ARGS__)
    #define RBTPRINT_DBG_W(fmt, ... ) RBTPRINT_DBG("-WARN-",fmt, ##__VA_ARGS__)
    #define RBTPRINT_DBG_I(fmt, ... ) RBTPRINT_DBG("-INFO-",fmt, ##__VA_ARGS__)

    #define RBTPRINT_ASSERT(cond) \
        if ( (cond) != TRUE ) \
        { \
            RBTPRINT_DBG_E("Assertion failure (%s:%d)",__FILE__,__LINE__); \
        }

#else

    #define RBTPRINT_DBG_E(fmt, ...) {}
    #define RBTPRINT_DBG_W(fmt, ...) {}
    #define RBTPRINT_DBG_I(fmt, ...) {}
    #define RBTPRINT_ASSERT(cond) {}

#endif


    
#ifdef __cplusplus
}
#endif


#endif /* _RBLIB_COMMON_H */
