

#include "test_rbtree.h"
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>


/* range is inclusive. Meaning returned value can be equal to x or y (hence:+1) */
#define RANDOM_UINT32_RANGE(x,y) (uint32_t) ( (rand() % (((y)+1)-(x))) + (x) )


bool test_rbtree_nullParam ( void )
{
    bool didPass = false;
    
    if ( rbtree_createTree(NULL,NULL,NULL) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);
    }
    else if ( rbtree_destroyTree(RBTREE_HANDLE_INVALID) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_insert(RBTREE_HANDLE_INVALID, (void *)1U, (void *)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);                
    }
    else if ( rbtree_insert((void *)1U, (void *)1U, NULL) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);                
    }
    else if ( rbtree_deleteByKey(RBTREE_HANDLE_INVALID, (RBTREE_KEY)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_deleteByKey((void*)1U, RBTREE_KEY_INVALID) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_deleteByValue(RBTREE_HANDLE_INVALID, (void*)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_deleteByIndex(RBTREE_HANDLE_INVALID, 1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_doesKeyExist(RBTREE_HANDLE_INVALID, (RBTREE_KEY)1U, (bool *)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_doesKeyExist((void*)1U, (RBTREE_KEY)1U, (bool *)NULL) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_doesValueExist(RBTREE_HANDLE_INVALID, (RBTREE_KEY*)1U, (bool *)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);
    }
    else if ( rbtree_doesValueExist((void*)1U, (RBTREE_KEY*)1U, (bool *)NULL) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);
    }
    else if ( rbtree_find(RBTREE_HANDLE_INVALID, (void*)1, 0U, (void*)1U, (void*)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_find((void*)1, NULL, 0U, (void*)1U, (void*)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_find((void*)1, (void*)1U, 0U, NULL, (void*)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_find((void*)1, (void*)1U, 0U, (void*)1U, NULL) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_retrieveByKey (RBTREE_HANDLE_INVALID, (RBTREE_KEY)1U, (void **)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_retrieveByKey ((RBTREE_HANDLE)1U, RBTREE_KEY_INVALID, (void **)1U) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else if ( rbtree_retrieveByKey ((RBTREE_HANDLE)1U, (RBTREE_KEY)1U, (void **)NULL) == RBTREE_STATUS_OK )
    {
        printf("NULL param accepted %s:%d",__FILE__,__LINE__);        
    }
    else
    {
        didPass = true;
    }

    return didPass;
}


static bool test_customMemoryManagers_malloc_triggered = false;
static bool test_customMemoryManagers_free_triggered = false;


void * test_rbtree_customMemoryManagers_malloc ( size_t allocSize )
{
    test_customMemoryManagers_malloc_triggered = true;
    return malloc(allocSize);
}

void test_rbtree_customMemoryManagers_free ( void * ptr )
{
    test_customMemoryManagers_free_triggered = true;
    free(ptr);
}

bool test_rbtree_customMemoryManagers ( void )
{
    RBTREE_KEY key = RBTREE_KEY_INVALID;
    RBTREE_HANDLE handle = RBTREE_HANDLE_INVALID;
    
    test_customMemoryManagers_malloc_triggered = false;
    test_customMemoryManagers_free_triggered = false;
    
    if ( rbtree_createTree(&handle, test_rbtree_customMemoryManagers_malloc, test_rbtree_customMemoryManagers_free) != RBTREE_STATUS_OK )
    {
        printf("Failed to create tree\n");
        return false;
    }
    
    if ( test_customMemoryManagers_malloc_triggered == false )
    {
        printf("my malloc not used to create tree\n");
        return false;
    }
    
    test_customMemoryManagers_malloc_triggered = false;
        
    if ( rbtree_insert(handle, (void *)1, &key) != RBTREE_STATUS_OK )
    {
        printf("Failed to insert: %d\n",1);
        return false;
    }

    if ( test_customMemoryManagers_malloc_triggered == false )
    {
        printf("my malloc not used to create tree entry\n");
        return false;
    }

    if ( rbtree_deleteByIndex(handle, 0U) != RBTREE_STATUS_OK )
    {
        printf("Failed to delete at index: %d\n",0);
        return false;
    }
    
    if ( test_customMemoryManagers_free_triggered == false )
    {
        printf("my free not used to free tree entry\n");
        return false;
    }
    
    test_customMemoryManagers_free_triggered = false;
    
    if ( rbtree_destroyTree(handle) != RBTREE_STATUS_OK )
    {
        printf("Failed to delete tree\n");
        return false;
    }
    
    if ( test_customMemoryManagers_free_triggered == false )
    {
        printf("my free not used to free tree entry\n");
        return false;
    }
    
    return true;
}

bool test_rbtree_indexApi_Size ( uint32_t count )
{
    RBTREE_HANDLE handle = RBTREE_HANDLE_INVALID;
    RBTREE_KEY keys[count];
    
    if ( rbtree_createTree(&handle, NULL, NULL) != RBTREE_STATUS_OK )
    {
        printf("Failed to create tree\n");
        return false;
    }
    
    for ( uint32_t i = 0U; i<count; i++ )
    {
        uint32_t newCount = 0U;

        if ( rbtree_insert(handle, (void *)i, &keys[i]) != RBTREE_STATUS_OK )
        {
            printf("Failed to insert: %d\n",i);
            return false;
        }
        else if ( rbtree_entryCount(handle, &newCount) != RBTREE_STATUS_OK )
        {
            printf("Failed to get entry count %d\n",i);
            return false;
        }
        else if ( newCount != i+1 )
        {
            printf("Expected %d got %d\n",i+1,newCount);
            return false;
        }
    }

    for ( uint32_t i = 0U; i<count; i++ )
    {
        uint32_t newCount = 0U;
        
        if ( rbtree_deleteByIndex(handle, 0U) != RBTREE_STATUS_OK )
        {
            printf("Failed to delete at index: %d\n",0U);
            return false;
        }
        else if ( rbtree_entryCount(handle, &newCount) != RBTREE_STATUS_OK )
        {
            printf("Failed to get entry count %d\n",i);
            return false;
        }
        else if ( newCount != count-i-1U )
        {
            printf("Expected %d got %d\n",count-i-1,newCount);
            return false;
        }
    }
        
    if ( rbtree_destroyTree(handle) != RBTREE_STATUS_OK )
    {
        printf("Failed to delete tree\n");
        return false;
    }
    
    return true;
}

bool test_rbtree_indexApi ( void )
{
    bool didPass = true;;
    
    for ( uint32_t i=1U; i<300U; i++ )
    {
        printf("testing index size: %u\n",i);

        if ( ! test_rbtree_indexApi_Size(i) )
        {
            printf("index size: %u failed\n",i);
            didPass = false;
            break;
        }
    }
    
    return didPass;
}
bool test_rbtree_mergeTrees ( void )
{
    bool didPass = false;
    
    RBTREE_HANDLE treeHandleA = RBTREE_HANDLE_INVALID;
    RBTREE_HANDLE treeHandleB = RBTREE_HANDLE_INVALID;
    RBTREE_KEY keyA = RBTREE_KEY_INVALID;
    RBTREE_KEY keyB = RBTREE_KEY_INVALID;
    void * valueA = (void*)1U;
    void * valueB = (void*)2U;
    bool doesExist = false;
    uint32_t entrySize = 0U;
    
    /* create tree A */
    if ( rbtree_createTree(&treeHandleA,NULL,NULL) != RBTREE_STATUS_OK )
    {
        printf("create treeA failed\n");
    }
    else if ( rbtree_createTree(&treeHandleB,NULL,NULL) != RBTREE_STATUS_OK )
    {
        printf("create treeB failed\n");
    }
    else if ( treeHandleA == treeHandleB )
    {
        printf("tree handles are equal ?!\n");
    }
    else if ( rbtree_insert(treeHandleA, valueA, &keyA) != RBTREE_STATUS_OK )
    {
        printf("insert keyA failed\n");
    }
    else if ( rbtree_insert(treeHandleB, valueB, &keyB) != RBTREE_STATUS_OK )
    {
        printf("insert keyB failed\n");
    }
    else if ( rbtree_copyInTree(treeHandleA, treeHandleB) != RBTREE_STATUS_OK )
    {
        printf("tree merge failed\n");
    }
    else if ( rbtree_entryCount(treeHandleA, &entrySize) != RBTREE_STATUS_OK )
    {
        printf("failed to get entry count\n");
    }
    else if ( entrySize != 2U )
    {
        printf("entry size of treeA is not 2\n");
    }
    /* size of treeB should not change */
    else if ( rbtree_entryCount(treeHandleB, &entrySize) != RBTREE_STATUS_OK )
    {
        printf("failed to get entry count\n");
    }
    else if ( entrySize != 1U )
    {
        printf("entry size of treeB is not 1\n");
    }
    else if ( rbtree_doesValueExist(treeHandleA, valueA, &doesExist) != RBTREE_STATUS_OK )
    {
        printf("failed to retrieve valueA from treeHandleA\n");
    }
    else if ( doesExist == false )
    {
        printf("key does not exist: valueA\n");
    }
    else if ( rbtree_doesValueExist(treeHandleB, valueB, &doesExist) != RBTREE_STATUS_OK )
    {
        printf("failed to retrieve valueB from treeHandleA\n");
    }
    else if ( doesExist == false )
    {
        printf("key does not exist: valueB\n");
    }
    else if ( rbtree_destroyTree(treeHandleA) != RBTREE_STATUS_OK )
    {
        printf("failed to destroy treeHandleA\n");
    }
    else if ( rbtree_destroyTree(treeHandleB) != RBTREE_STATUS_OK )
    {
        printf("failed to destroy treeHandleB\n");
    }
    else
    {
        didPass = true;
    }
    
    return didPass;
}

bool test_rbtree_createEmpty ( void )
{
    bool didPass = false;
    uint32_t count = 999U;
    
    RBTREE_HANDLE handle = RBTREE_HANDLE_INVALID;
    
    if ( rbtree_createTree(&handle,NULL,NULL) != RBTREE_STATUS_OK )
    {
        printf("create tree failed\n");
    }
    else if ( rbtree_entryCount(handle, &count) != RBTREE_STATUS_OK )
    {
        printf("get entry count failed\n");
    }
    else if ( count != 0U )
    {
        printf("entry count != 0 (%d) \n",count);
    }
    else if ( rbtree_destroyTree(handle) != RBTREE_STATUS_OK )
    {
        printf("failed to destroy tree\n");
    }
    else
    {
        didPass = true;
    }
    
    return didPass;
}

bool test_rbtree_linearInsertRandomDeletionSize ( uint32_t size )
{
    RBTREE_KEY keys[size];
    RBTREE_HANDLE handle = RBTREE_HANDLE_INVALID;

    if ( rbtree_createTree(&handle,NULL,NULL) != RBTREE_STATUS_OK )
    {
        printf("create tree failed\n");
        return false;
    }
    
    /* insert */
    for ( uint32_t i=0U; i<size; i++ )
    {
        uint32_t entryCount = 0U;
        void * retVal = NULL;
        bool doesKeyExist = false;
        
        if ( rbtree_insert(handle, (void *)i, &keys[i]) != RBTREE_STATUS_OK )
        {
            printf("insert %d failed\n",i);
            return false;
        }
        else if ( rbtree_entryCount(handle, &entryCount) != RBTREE_STATUS_OK )
        {
            printf("retrieve entry count failed %d\n",i);
            return false;
        }
        else if ( entryCount != i+1 )
        {
            printf("entry count ins mismatch %d != %d \n",i,entryCount);
            return false;
        }
        else if ( rbtree_retrieveByKey(handle, keys[i], &retVal) != RBTREE_STATUS_OK )
        {
            printf("retrieve failed %d\n",i);
            return false;
        }
        else if ( retVal != (void*)i )
        {
            printf("retrieve value mismatch %d != %d\n",i,(int32_t)retVal);
            return false;
        }
        else if ( rbtree_doesKeyExist(handle, keys[i], &doesKeyExist) != RBTREE_STATUS_OK )
        {
            printf("rbtree_doesKeyExist failed\n");
            return false;
        }
        else if ( doesKeyExist == false )
        {
            printf("key does not exist %u\n",keys[i]);
            return false;
        }
        else
        {
            printf("Inserted key: %d\n",keys[i]);
        }
    }
    
    /* delete */
    for ( uint32_t i=0; i<size; i++ )
    {
        uint32_t entryCount = 0U;
        void * retVal = NULL;
        bool doesKeyExist = false;
        RBTREE_KEY key = RBTREE_KEY_INVALID;
        uint32_t index_delete = 0U;
        
        if ( rbtree_entryCount(handle, &entryCount) != RBTREE_STATUS_OK )
        {
            printf("rbtree_entryCount failed idx:%u \n",i);
            return false;
        }
        else if ( rbtree_retrieveByIndex(handle, (index_delete=RANDOM_UINT32_RANGE(0,entryCount-1)), &retVal, &key)
                 != RBTREE_STATUS_OK )
        {
            printf("retrieve by index:%u \n",index_delete);
            return false;
        }
        else if ( rbtree_deleteByKey(handle, key) != RBTREE_STATUS_OK )
        {
            printf("delete failed key:%u \n",key);
            return false;
        }
        else if ( rbtree_entryCount(handle, &entryCount) != RBTREE_STATUS_OK )
        {
            printf("retrieve entry count failed %d\n",i);
            return false;
        }
        else if ( entryCount != (size-i-1) )
        {
            printf("entry count del mismatch %d != %d \n",entryCount,(size-i-1));
            return false;
        }
        else if ( rbtree_retrieveByKey(handle, key, &retVal) == RBTREE_STATUS_OK )
        {
            printf("retrieve deleted key success ? %d\n",i);
            return false;
        }
        else if ( rbtree_doesKeyExist(handle, key, &doesKeyExist) != RBTREE_STATUS_OK )
        {
            printf("rbtree_doesKeyExist failed\n");
            return false;
        }
        else if ( doesKeyExist == true )
        {
            printf("key does exist ? %u\n",keys[i]);
            return false;
        }
        else
        {
            printf("Deleted key:%u\n",keys[i]);
        }
    }
    
    if ( rbtree_destroyTree(handle) != RBTREE_STATUS_OK )
    {
        printf("destroy tree failed\n");
        return false;
    }

    return true;
}

bool test_rbtree_linearInsertRandomDeletion ( void )
{
    bool didPass = true;
    
    for ( uint32_t i=0U; i<300U; i++ )
    {
        if ( ! test_rbtree_linearInsertRandomDeletionSize(i) )
        {
            printf("random deletion size:%d failed\n",i);
            didPass = false;
            break;
        }
    }
    
    return didPass;
}

bool test_rbtree_singleCycle ( void )
{
    bool didPass = false;
    
    RBTREE_HANDLE handle = RBTREE_HANDLE_INVALID;
    RBTREE_KEY key = RBTREE_KEY_INVALID;
    uint32_t count = 0U;
    void * storevalue = NULL;
    
    if ( rbtree_createTree(&handle,NULL,NULL) != RBTREE_STATUS_OK )
    {
        printf("create tree failed\n");
    }
    else if ( rbtree_entryCount(handle, &count) != RBTREE_STATUS_OK )
    {
        printf("get entry count failed\n");
    }
    else if ( count != 0U )
    {
        printf("entry count != 0 (%d) \n",count);
    }
    else if ( rbtree_insert(handle, storevalue, &key) != RBTREE_STATUS_OK )
    {
        printf("failed to insert key\n");
    }
    else if ( rbtree_entryCount(handle, &count) != RBTREE_STATUS_OK )
    {
        printf("get entry count failed\n");
    }
    else if ( count != 1U )
    {
        printf("entry count != 1 (%d) \n",count);
    }
    else if ( rbtree_deleteByKey(handle, key) != RBTREE_STATUS_OK )
    {
        printf("failed to delete key: %d\n",key);
    }
    else if ( rbtree_entryCount(handle, &count) != RBTREE_STATUS_OK )
    {
        printf("get entry count failed\n");
    }
    else if ( count != 0U )
    {
        printf("entry count != 0 (%d) \n",count);
    }
    else if ( rbtree_destroyTree(handle) != RBTREE_STATUS_OK )
    {
        printf("failed to destroy tree\n");
    }
    else
    {
        didPass = true;
    }

    return didPass;
}

bool _test_rbtree_linearInsertDeleteSize ( uint32_t size )
{
    RBTREE_KEY keys[size];
    RBTREE_HANDLE handle = RBTREE_HANDLE_INVALID;
    
    for ( uint32_t i=0U; i<size; i++ )
    {
        keys[i] = RBTREE_KEY_INVALID;
    }
    
    if ( rbtree_createTree(&handle,NULL,NULL) != RBTREE_STATUS_OK )
    {
        printf("create tree failed\n");
        return false;
    }
    
    /* insert */
    for ( uint32_t i=0U; i<size; i++ )
    {
        uint32_t entryCount = 0U;
        void * retVal = NULL;
        bool doesKeyExist = false;

        if ( rbtree_insert(handle, (void *)i, &keys[i]) != RBTREE_STATUS_OK )
        {
            printf("insert %d failed\n",i);
            return false;
        }
        else if ( rbtree_entryCount(handle, &entryCount) != RBTREE_STATUS_OK )
        {
            printf("retrieve entry count failed %d\n",i);
            return false;
        }
        else if ( entryCount != i+1 )
        {
            printf("entry count ins mismatch %d != %d \n",i,entryCount);
            return false;
        }
        else if ( rbtree_retrieveByKey(handle, keys[i], &retVal) != RBTREE_STATUS_OK )
        {
            printf("retrieve failed %d\n",i);
            return false;
        }
        else if ( retVal != (void*)i )
        {
            printf("retrieve value mismatch %d != %d\n",i,(int32_t)retVal);
            return false;
        }
        else if ( rbtree_doesKeyExist(handle, keys[i], &doesKeyExist) != RBTREE_STATUS_OK )
        {
            printf("rbtree_doesKeyExist failed\n");
            return false;
        }
        else if ( doesKeyExist == false )
        {
            printf("key does not exist %u\n",keys[i]);
            return false;
        }
        else
        {
            printf("Inserted key: %d\n",keys[i]);
        }
    }
    
    /* delete */
    for ( uint32_t i=0; i<size; i++ )
    {
        uint32_t entryCount = 0U;
        void * retVal = NULL;
        bool doesKeyExist = false;
        
        if ( rbtree_deleteByKey(handle, keys[i]) != RBTREE_STATUS_OK )
        {
            printf("delete failed key:%u \n",keys[i]);
            return false;            
        }
        else if ( rbtree_entryCount(handle, &entryCount) != RBTREE_STATUS_OK )
        {
            printf("retrieve entry count failed %d\n",i);
            return false;
        }
        else if ( entryCount != (size-i-1) )
        {
            printf("entry count del mismatch %d != %d \n",entryCount,(size-i-1));
            return false;
        }
        else if ( rbtree_retrieveByKey(handle, keys[i], &retVal) == RBTREE_STATUS_OK )
        {
            printf("retrieve deleted key success ? %d\n",i);
            return false;
        }
        else if ( rbtree_doesKeyExist(handle, keys[i], &doesKeyExist) != RBTREE_STATUS_OK )
        {
            printf("rbtree_doesKeyExist failed\n");
            return false;
        }
        else if ( doesKeyExist == true )
        {
            printf("key does exist ? %u\n",keys[i]);
            return false;
        }
        else
        {
            printf("Deleted key:%u\n",keys[i]);
        }
    }
    
    if ( rbtree_destroyTree(handle) != RBTREE_STATUS_OK )
    {
        printf("destroy tree failed\n");
        return false;
    }
    
    return true;
}


bool test_rbtree_linearInsertDeletion ( void )
{
    for ( uint32_t i=1U; i<300; i++ )
    {
        printf("testing size: %u\n",i);

        if ( ! _test_rbtree_linearInsertDeleteSize(i) )
        {
            printf("linear insertion size:%d failed \n",i);
            return false;
        }
    }
    
    return true;
}

bool test_rbtree ( void )
{
    bool didPass = false;
    
    if ( ! test_rbtree_nullParam() )
    {
        printf("test_rbtree_nullParam() failed\n");
    }
    else if ( ! test_rbtree_createEmpty() )
    {
        printf("test_rbtree_linearInsertDeletion() failed\n");
    }
    else if ( ! test_rbtree_singleCycle() )
    {
        printf("test_rbtree_singleCycle() failed\n");
    }
    else if ( ! test_rbtree_linearInsertDeletion() )
    {
        printf("test_rbtree_linearInsertDeletion() failed\n");
    }
    else if ( ! test_rbtree_linearInsertRandomDeletion() )
    {
        printf("test_rbtree_linearInsertRandomDeletion() failed\n");
    }
    else if ( ! test_rbtree_indexApi() )
    {
        printf("test_rbtree_linearInsertRandomDeletion() failed\n");
    }
    else if ( ! test_rbtree_customMemoryManagers() )
    {
        printf("test_rbtree_customMemoryManagers() failed\n");
        
    }
    else if ( ! test_rbtree_mergeTrees() )
    {
        printf("test_rbtree_mergeTrees() failed\n");
    }
    else
    {
        printf("test_rbtree passed\n");
        didPass = true;
    }
    
    return didPass;
}
