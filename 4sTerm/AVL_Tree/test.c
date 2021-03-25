#include "stdio.h"
#include "tree.h"
#include "assert.h"


int cmp (Node_t* cur_node, int elem, void *data){
    int max = *(int*)data;
    if (max < Get_value(cur_node)){
        *(int*)data = Get_value(cur_node);
    }
    return max;
}


void TreeTest(){

	AVL_Tree_t* tree = ConAVL_Tree();
	assert(tree);

	for (int i = 0; i < 50; ++i)
	{
		assert( Insert(tree, (i*18397) % 57) == NORMAL_STATE);
	}
	
	for (int i = 0; i < 50; ++i)
	{
		assert(Delete(tree, (i*18397) % 57) == NORMAL_STATE);
	}


	for (int i = 0; i < 5000; ++i)
	{
		Insert(tree, 2 + rand() % 5700);
	}

	int max = INT_MIN;
	assert(Foreach(tree, cmp, &max) == NORMAL_STATE);
	
	assert(Diigraph(tree, "tree52.dot")== NORMAL_STATE );
	assert(DeAVL_Tree(tree) == NORMAL_STATE);

}

void TreeCriticalTest(){

	AVL_Tree_t* tree = ConAVL_Tree();
	assert(tree);

	for (int i = 0; i < 50; i+=2)
	{
		assert(Insert(tree, i) == NORMAL_STATE);
	}
	
	assert(Is_elem_here(tree, 30) == 1);
	assert(Is_elem_here(tree, 4) == 1);
	assert(Is_elem_here(tree, 45) == 0);

	for (int i = 0; i < 50; i+=2)
	{
		assert(Delete(tree, i) == NORMAL_STATE);
	}

	assert(Is_elem_here(NULL, 60) == 0);
	assert(Is_elem_here(tree, 60) == 0);

	int max = INT_MIN;
	assert(Foreach(tree, cmp, &max) == EMPTY);

	for (int i = MAX_AVL_SIZE; i > 1; --i)
	{
		Insert(tree, i);
	}


	assert(Insert(tree, MAX_AVL_SIZE / 2) == IMPOSSIBLE_TO_INSERT);
	assert(Insert(tree, 1) == NORMAL_STATE);
	assert(Insert(NULL, 1) == NO_TREE);
	assert(Insert(tree, INT_MAX / 2) == TREE_IS_FULL);


	assert(Delete(NULL, 0) == NO_TREE);
	assert(Delete(tree, 0) == NO_SUCH_ELEMENT);

	assert(Diigraph(NULL, "FF") == NO_TREE);

	assert(DeAVL_Tree(NULL) == NO_TREE);

	
	assert(Foreach(NULL, cmp, &max) == NO_TREE);
	assert(Foreach(tree, NULL, &max) == NO_FUNC);
	assert(Foreach(tree, cmp, NULL) == NO_DATA);

	assert(DeAVL_Tree(tree) == NORMAL_STATE);

}

int main() 
{

	TreeTest();
	TreeCriticalTest();
	printf("Complete\n");

	return 0;
}