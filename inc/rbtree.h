/**
 @file
 Red-Black Binary Search Tree
 
 @author Ryan Powell
 @date 23-12-12
 @copyright Copyright (c) 2012  Ryan Powell
 @licence https://raw.github.com/mcCuppaT/RBTreelib/master/LICENSE
 */


#ifndef __RBTREE_H
#define __RBTREE_H


#ifdef __cplusplus
extern "C" {
#endif
    

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

    
/**
 @enum _RBTREE_STATUS
 @brief return values from rbtree* api's \n
 #RBTREE_STATUS_OK api call success \n
 RBTREE_STATUS_FAIL api call failure - reason unknown \n
 RBTREE_STATUS_FAIL_INVALID_PARAM api call failed - invalid parameter recieved \n
 RBTREE_STATUS_FAIL_MALLOC_FAILURE api call failed - internal malloc failure \n
 RBTREE_STATUS_FAIL_VALUE_DOES_NOT_EXIST api call failed - cannot find value in tree \n
 RBTREE_STATUS_FAIL_KEY_DOES_NOT_EXIST api call failed - cannot find key in tree \n
 RBTREE_STATUS_FAIL_KEY_ALREADY_STORED api call failed - cannot store duplicate key \n
 RBTREE_STATUS_FAIL_INDEX_OUT_OF_RANGE api call failed - index of item is outside of range of tree (i>tree_size) \n
 */
typedef enum _RBTREE_STATUS
{
    RBTREE_STATUS_UNDEF = 0,
    RBTREE_STATUS_OK,
    RBTREE_STATUS_FAIL,
    RBTREE_STATUS_FAIL_INVALID_PARAM,
    RBTREE_STATUS_FAIL_MALLOC_FAILURE,
    RBTREE_STATUS_FAIL_VALUE_DOES_NOT_EXIST,
    RBTREE_STATUS_FAIL_KEY_DOES_NOT_EXIST,
    RBTREE_STATUS_FAIL_KEY_ALREADY_STORED,
    RBTREE_STATUS_FAIL_INDEX_OUT_OF_RANGE,
    RBTREE_STATUS_LAST_VALUE
} RBTREE_STATUS;


typedef void* RBTREE_HANDLE;

#define RBTREE_HANDLE_INVALID NULL

typedef uint32_t RBTREE_KEY;

#define RBTREE_KEY_INVALID 0U

/**
 @brief type definition for comparator
 @param storevalue value stored in tree
 @param userdata user data passed in to api call
 @return must return #TRUE if value has been found. #FALSE otherwise
 */
typedef bool (*rbtree_comparator_t)(void* storevalue, void* userdata);


/**
 @brief type definition for memory allocator
 @details form must take same as 'malloc'
 @param[in] size number of bytes to allocate
 @return must return pointer to allocated chunk or NULL if failed
 */
typedef void* (*rbtree_memalloc_t)(size_t size);


/**
 @brief type definition for memory allocator
 @details form must take same as 'free'
 @param[in] memptr pointer to memory to be free'd
 */
typedef void (*rbtree_memfree_t)(void* memptr);


/**
 @brief create new tree
 @param[out] handle returned tree handle
 @param[in] mem_alloc function pointer to allocate memory pool (optional)
 @param[in] mem_free function pointer to free from memory pool (optional)
 @return returns RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_createTree ( RBTREE_HANDLE * handle, rbtree_memalloc_t mem_alloc, rbtree_memfree_t mem_free );


/**
 @brief destroy tree
 @param[in] handle handle of tree to remove
 @return returns RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_destroyTree ( RBTREE_HANDLE handle );


/**
 @brief insert new value into tree
 @param[in] handle tree handle
 @param[in] storevalue value to be stored (note:duplicates are allowed)
 @param[out] key unique reference to retrieve value by
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_insert ( RBTREE_HANDLE handle, void * storevalue, RBTREE_KEY * key );


/**
 @brief retrieves value from tree by key
 @param[in] handle tree handle
 @param[in] key unique reference to retrieve value by
 @param[out] ret_data value to be populated upon success
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_retrieveByKey ( RBTREE_HANDLE handle, RBTREE_KEY key, void ** ret_data );


/**
 @brief retrieves value from tree by key
 @param[in] handle tree handle
 @param[in] key unique reference to retrieve value by
 @param[out] ret_data value to be populated upon success
 @param[out] ret_key key of value ( only valid on success )
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_retrieveByIndex ( RBTREE_HANDLE handle, uint32_t index, void ** ret_data, RBTREE_KEY * ret_key );


/**
 @brief remove entry from tree by key
 @param[in] handle tree handle
 @param[in] key unique reference of entry to remove
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_deleteByKey ( RBTREE_HANDLE handle, RBTREE_KEY key );


/**
 @brief remove entry from tree by value
 @details all entries matching provided value will be removed. This may destroy more than one entry
 @param[in] handle tree handle
 @param[in] value all entries matching this will be removed
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_deleteByValue ( RBTREE_HANDLE handle, void * value );


/**
 @brief remove entry from tree by index
 @param[in] handle tree handle
 @param[in] index index of entry to remove ( where index=0 equals key with smallest value and so on )
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_deleteByIndex ( RBTREE_HANDLE handle, uint32_t index );


/**
 @brief search tree for entry
 @param[in] handle tree handle
 @param[in] cmp_fn comparator ( searching will stop when this returns #TRUE )
 @param[in] userdata data to pass into cmp_fn
 @param[out] ret_storevalue returned value
 @param[out] ret_key returned key
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_find ( RBTREE_HANDLE handle, rbtree_comparator_t cmp_fn, void * userdata, void ** ret_storevalue, RBTREE_KEY * ret_key );


/**
 @brief check if key exists
 @param[in] handle tree handle
 @param[in] storekey key to check for
 @param[out] doesExist populates if key exists or not
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_doesKeyExist ( RBTREE_HANDLE handle, RBTREE_KEY storekey, bool * doesExist );


/**
 @brief check if key exists
 @param[in] handle tree handle
 @param[in] storevalue value to check for
 @param[out] doesExist populates if value exists or not
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_doesValueExist ( RBTREE_HANDLE handle, void * storevalue, bool * doesExist );


/**
 @brief retrieve number of values stored in tree
 @param[in] handle tree handle
 @param[out] numberOfEntries populates number of values stored in tree
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_entryCount ( RBTREE_HANDLE handle, uint32_t * numberOfEntries );


/**
 @brief duplicate the values from copyInTree into handle tree
 @param[in] handle tree handle that will contain both sets of values
 @param[out] copyInTree tree handle to copy all values from
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_copyInTree ( RBTREE_HANDLE handle, RBTREE_HANDLE copyInTree );


/**
 @brief get the memory allocator functions passed into #rbtree_createTree
 @param[in] handle tree handle 
 @param[in] mem_alloc function pointer that will allocate this tree memory
 @param[in] mem_free function pointer that will free this tree memory
 @return returns #RBTREE_STATUS_OK on success
 */
RBTREE_STATUS rbtree_getMemoryAllocator ( RBTREE_HANDLE handle, rbtree_memalloc_t * mem_alloc, rbtree_memfree_t * mem_free );


#define RBTREE_VERSION 1.0f


#ifdef __cplusplus
}
#endif


#endif /* __RBTREE_H */
