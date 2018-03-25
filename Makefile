%.o: %.c
	gcc -Wall -c $< -o $@

bin_file := *.o

test-random: avl-tree.c test-random.c
	gcc -Wall $^ -o $@ -g

bin_file += test-random

test-illu-tree: avl-tree.c test-illu-tree.c
	gcc -Wall $^ -o $@ -g

bin_file += test-illu-tree

clean: 
	-rm $(bin_file)

.PHONY: clean
