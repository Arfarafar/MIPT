
#include <limits.h>
#include <stdlib.h>

#include "types.h" 

int Insert(AVL_Tree_t* tree, Elem_t Elem);
int Delete(AVL_Tree_t* tree, Elem_t Elem);
char Is_elem_here(AVL_Tree_t* tree, Elem_t elem);
int Foreach(AVL_Tree_t* tree, int(*callback)(Node_t* cur_node, Elem_t elem, void *data), void* data);
Elem_t Get_value(Node_t* node);
int Diigraph (AVL_Tree_t* tree, const char* name);

AVL_Tree_t* ConAVL_Tree();
int DeAVL_Tree(AVL_Tree_t* tree);

extern int (*Compare_function)(Elem_t Elem1, Elem_t Elem2);



//-----------------------------------------------------------------------------------------------------------








