gcc -std=c99 example_main.c ../src/rbtree.c ../src/rbtree_checks.c ../src/rbtree_common.c -I ../inc -I ../src -o rbtree_example
./rbtree_example
