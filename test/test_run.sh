gcc -std=c99 test_main.c test_rbtree.c ../src/rbtree.c ../src/rbtree_checks.c ../src/rbtree_common.c -I ../inc -I ../src -o rbtree_test
./rbtree_test
