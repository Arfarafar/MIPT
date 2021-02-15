#include "stdio.h"
#include "tree.h"


int cmp (Node* cur_node, int elem, void *data){
	int max = *(int*)data;
	if (max < cur_node -> value){
		*(int*)data = cur_node -> value;
	}
	return max;
}

int main(int argc, char const *argv[])
{

	AVL_Tree* tree = ConAVL_Tree(10);
	if(!tree){
		printf("дерево не создалось\n");
		return 0;
	}

	for (int i = 0; i < 100; ++i)
	{
		int err = Insert(tree, ((i+1)*189797) % 100);
		switch (err){
			case IMPOSSIBLE_TO_INSERT:
				break;
			case INVALID_TREE:
				break;
			case TREE_IS_FULL: printf("TREE_IS_FULL\n");
				break;
			default:
				break;
		}
		
	}

	int max = INT_MIN;
	Foreach(tree -> root, cmp, &max);
	printf("%d\n", max);

	Diigraph(tree, "tree51.dot");
	Delete(tree, 11);
	//return 0;
	Diigraph(tree, "tree52.dot");
	Delete(tree, 31);
	Diigraph(tree, "tree53.dot");

	DeAVL_Tree(tree);

	return 0;
}