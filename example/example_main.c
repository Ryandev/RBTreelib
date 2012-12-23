

#include <stdio.h>
#include "rbtree.h"

int main(int argc, const char * argv[])
{
    RBTREE_HANDLE handle = RBTREE_HANDLE_INVALID;

    rbtree_createTree ( &handle, NULL, NULL );
    
    RBTREE_KEY keyA = RBTREE_KEY_INVALID;
    RBTREE_KEY keyB = RBTREE_KEY_INVALID;
    RBTREE_KEY keyC = RBTREE_KEY_INVALID;
    
    rbtree_insert ( handle, (void *)'A', &keyA );
    rbtree_insert ( handle, (void *)'B', &keyB );
    rbtree_insert ( handle, (void *)'C', &keyC );
    
    void *valueA = NULL;
    void *valueB = NULL;
    void *valueC = NULL;
    
    rbtree_retrieveByKey ( handle, keyA, &valueA );
    rbtree_retrieveByKey ( handle, keyB, &valueB );
    rbtree_retrieveByKey ( handle, keyC, &valueC );
    
    printf("keyA value: %c\n",(char)valueA);
    printf("keyB value: %c\n",(char)valueB);
    printf("keyC value: %c\n",(char)valueC);

    rbtree_destroyTree ( handle );
    
    return 0;
}

