#include "stdio.h"
#include "tree.h"




void TreeTest(){

	AVL_Tree_t* tree = ConAVL_Tree();
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
	Foreach(tree, cmp, &max);
	
	Diigraph(tree, "tree52.dot");
	DeAVL_Tree(tree);

}

void TreeCriticalTest(){

	AVL_Tree_t* tree = ConAVL_Tree();

	for (int i = 0; i < 50; ++i)
	{
		Insert(tree, i);
	}

	for (int i = 0; i < 50; ++i)
	{
		Delete(tree, i);
	}


	for (int i = MAX_AVL_SIZE+2; i >= 0; --i)
	{
		Insert(tree, i);
	}


	Insert(tree, INT_MAX / 2);
	Insert(tree, INT_MAX / 2);
	Insert(NULL, 1);
	Insert(tree, INT_MAX / 2);

	Is_elem_here(NULL, 60);


	Delete(NULL, 0);
	Delete(tree, 0);

	Diigraph(NULL, "FF");

	DeAVL_Tree(NULL);

	int max = INT_MIN;
	Foreach(NULL, cmp, &max);
	Foreach(tree, NULL, &max);
	Foreach(tree, cmp, NULL);

	DeAVL_Tree(tree);

}

int main(int argc, char const *argv[])
{

	TreeTest();
	TreeCriticalTest();
	printf("Complete\n");

	return 0;
}