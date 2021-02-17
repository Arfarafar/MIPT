
#include <limits.h>
#include <stdlib.h>

#include "types.h"

int Insert(AVL_Tree* tree, Elem_t Elem);
int Delete(AVL_Tree* tree, Elem_t Elem);
char Is_elem_here(AVL_Tree* tree, Elem_t elem);
int Foreach(Node* root, int(*callback)(Node* cur_node, Elem_t elem, void *data), void* data);

void Diigraph (AVL_Tree* tree, const char* name);

AVL_Tree* ConAVL_Tree();
void DeAVL_Tree(AVL_Tree* tree);

int Compare_int(int left, int right);

extern int (*Compare_function)(Elem_t Elem1, Elem_t Elem2);



//-----------------------------------------------------------------------------------------------------------








