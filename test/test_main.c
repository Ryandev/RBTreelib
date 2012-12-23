

#include <stdio.h>
#include "test_rbtree.h"

int main(int argc, const char * argv[])
{
    int status = test_rbtree() ? 0 : 1;
    printf("RBTree test: %s\n", ( status ? "failed" : "passed" ) );
    return status;
}

