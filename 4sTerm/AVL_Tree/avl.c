#include "stdio.h"
#include "tree.h"


int cmp (Node* cur_node, int elem, void *data){
	int max = *(int*)data;
	if (max < cur_node -> value){
		*(int*)data = cur_node -> value;
	}
	return max;
}

void TreeTest(){

	AVL_Tree* tree = ConAVL_Tree();
	for (int i = 0; i < 50; ++i)
	{
		Insert(tree, (i*18397) % 57);
	}
	Is_elem_here(tree, 58);
	Is_elem_here(tree, 0);

	
	for (int i = 0; i < 50; ++i)
	{
		Delete(tree, (i*18397) % 57);
	}

	for (int i = 0; i < 5000; ++i)
	{
		Insert(tree, 2 + rand() % 5700);
	}

	int max = INT_MIN;
	Foreach(tree -> root, cmp, &max);
	
	Diigraph(tree, "tree52.dot");
	DeAVL_Tree(tree);

}

void TreeCriticalTest(){

	AVL_Tree* tree = ConAVL_Tree();

	for (int i = 0; i < 50; ++i)
	{
		Insert(tree, i);
	}

	for (int i = 0; i < 50; ++i)
	{
		Delete(tree, i);
	}


	for (int i = 100; i > 50; --i)
	{
		Insert(tree, i);
	}


	Insert(tree, INT_MAX / 2);
	Insert(tree, INT_MAX / 2);
	Insert(NULL, 1);
	Insert(tree, INT_MAX / 2);

	tree -> size = MAX_AVL_SIZE;
	Insert(tree, 60);
	Is_elem_here(NULL, 60);



	Delete(NULL, 0);
	Delete(tree, 0);

	Diigraph(NULL, "FF");

	DeAVL_Tree(NULL);

	int max = INT_MIN;
	Foreach(NULL, cmp, &max);
	Foreach(tree -> root, NULL, &max);
	Foreach(tree -> root, cmp, NULL);

	DeAVL_Tree(tree);

}

int main(int argc, char const *argv[])
{

	TreeTest();
	TreeCriticalTest();
	printf("Complete\n");

	return 0;
}