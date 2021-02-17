
#include "tree.h"

const int MAX_AVL_height = 16;
const int MAX_AVL_SIZE = 65536;




int Compare_int(int left, int right){
    if(right < left)
        return -1;
    return right > left;
}

int (*Compare_function)(Elem_t Elem1, Elem_t Elem2) = Compare_int;


Node* Left_rotate(Node* root){

    Node* temp = root -> link[1];
    root -> link[1] = root -> link[1] -> link[0];
    temp -> link[0] = root;

    return temp;
}

Node* Right_rotate(Node* root){

    Node* temp = root -> link[0];
    root -> link[0] = root -> link[0] -> link[1];
    temp -> link[1] = root;

    return temp;
}


void* avl_calloc(size_t num, size_t size){

    if (num * size > INT_MAX / 4)
        return NULL;
    return calloc(num, size);

}

Node* Create_Node(Elem_t Elem){
    Node* node = (Node*) avl_calloc(sizeof(Node), 1);

    if(node)
        node -> value = Elem;
    return node;
}



AVL_Tree* ConAVL_Tree(){
    AVL_Tree* tree = (AVL_Tree*) avl_calloc(sizeof(AVL_Tree), 1);
    if(tree)
    {
        tree -> root = NULL;  
        tree -> size = 1;
    }

    return tree;
}

void Destruct_Node(Node* node){

    if(!node){
        return;
    }

    Destruct_Node(node -> link[0]);
    Destruct_Node(node -> link[1]);

    free(node);
}



void DeAVL_Tree(AVL_Tree* tree){

    if(!tree){
        return;
    }

    Destruct_Node(tree -> root);
    free(tree);
}





int Foreach(Node* root, int(*callback)(Node* cur_node, Elem_t elem, void *data), void* data){

    
    if (!root){
        return NO_TREE;
    }
    if (!callback)
        return NO_FUNC;
    if(!data)
        return NO_DATA;


    callback(root, root -> value, data);

    
    Foreach(root -> link[0], callback, data);
    Foreach(root -> link[1], callback, data);

    return 0;
}

char Is_elem_here(AVL_Tree* tree, Elem_t elem){

    if(!tree)
        return 0;

    int side = 0;

    for (Node* iterator = tree -> root; iterator != NULL; iterator = iterator -> link[side]){
         int cmp = Compare_function(iterator -> value, elem);
         if (cmp == 0){
            return 1;
        }
    }

    return 0;
}


Node* Find_Elem(AVL_Tree* tree, Elem_t elem,  char* dir, Node** path, int* length){

    int side = 0;

    for (Node* iterator = tree -> root; iterator != NULL; iterator = iterator -> link[side]){
         int cmp = Compare_function(iterator -> value, elem);
         if (cmp == 0){
            return iterator;
        }
        dir[*length] = side = cmp > 0;
        path[*length] = iterator;
        (*length)++;
    }

    return NULL;
}



Node* Find_wheretoInsert( Elem_t elem, char* dir,  Node** recent_nz, Node** p_recent_nz, char* side){

	Node* iterator, *p_iterator;
    int length = 0;

    for(iterator = *recent_nz, p_iterator = *p_recent_nz; 
        iterator != NULL;
        p_iterator = iterator, iterator = iterator -> link[*side]) 
    {
        int cmp = Compare_function(iterator -> value, elem);
        if (cmp == 0){
            return NULL;
        }
        if (iterator -> balance != 0){
            *p_recent_nz = p_iterator;
            *recent_nz = iterator;
            length = 0;
        }
        dir[length++] = *side = cmp > 0;

    }

    return p_iterator;

}


void Apdate_balance(Node* iterator, Node* final, char* dir){

	for (int length = 0; iterator != final; iterator = iterator -> link[dir[length++]])
    
        if (dir[length])
            iterator -> balance++;
        else
            iterator -> balance--;
}


Node* Left_at_x_Right_at_y(Node* X, Node* cur_node, int dir){
	
	cur_node -> link[dir] = Left_rotate(X);
    Node* new_root = Right_rotate(cur_node);
    if (new_root -> balance == -1) {
        X -> balance = 0;
        cur_node -> balance = 1;
    }
    else if (new_root -> balance == 0) {
        X -> balance = 0;
        cur_node -> balance = 0;
    }
    else {
        X -> balance = -1;
        cur_node -> balance = 0;
    }
    new_root -> balance = 0;

    return new_root;
}

Node* Right_at_x_Left_at_y (Node* X, Node* cur_node, int dir){

	cur_node -> link[dir] = Right_rotate(X);
    Node* new_root = Left_rotate(cur_node);
        if (new_root -> balance == -1) {
            X -> balance = 1;
            cur_node -> balance = 0;
        }
        else if (new_root -> balance == 0) {
            X -> balance = 0;
            cur_node -> balance = 0;
        }
        else {
            X -> balance = 0;
            cur_node -> balance = -1;
        }
        new_root -> balance = 0;

        return new_root;
}




int Insert(AVL_Tree* tree, Elem_t Elem){

    if(!tree)
        return INVALID_TREE;
    if(tree -> size == MAX_AVL_SIZE){
        return TREE_IS_FULL;
    }


    Node* recent_nz = tree -> root;
    Node* p_recent_nz = NULL;
    char dir[MAX_AVL_height+1];
    char side;


    Node* final = Find_wheretoInsert(Elem, dir, &recent_nz, &p_recent_nz, &side);



    if (!tree -> root){
        if(!(tree -> root = Create_Node(Elem)))
            return IMPOSSIBLE_TO_INSERT;
        return 0;
    }
    else if (!final) 
        return IMPOSSIBLE_TO_INSERT;

    else if(!(final -> link[side] = Create_Node(Elem)))
        return IMPOSSIBLE_TO_INSERT;

    tree -> size++;

    Apdate_balance(recent_nz, final -> link[side], dir);
  


    Node* new_root = NULL;

    if(recent_nz -> balance == -2){
        if(recent_nz -> link[0] -> balance == -1)
        {
            new_root = Right_rotate(recent_nz);
            recent_nz -> balance = 0;
            new_root -> balance = 0;
        }

        else {
            new_root = Left_at_x_Right_at_y(recent_nz -> link[0], recent_nz, 0); // x, cur_node, dir
        }


    }
    else if (recent_nz -> balance == 2){
        if(recent_nz -> link[1] -> balance == 1)
        {
            new_root = Left_rotate(recent_nz);
            recent_nz -> balance = 0;
            new_root -> balance = 0;
        }

        else {
            new_root = Right_at_x_Left_at_y(recent_nz -> link[1], recent_nz, 1);
           
        }
        
    }
    else 
        return 0;

        

    if(p_recent_nz)
        p_recent_nz -> link[recent_nz != p_recent_nz -> link[0]] = new_root;
    else
        tree -> root = new_root;
    return 0;

}




int Delete(AVL_Tree* tree, Elem_t Elem){

    if (!tree)
        return INVALID_TREE;

    Node* path[MAX_AVL_height];
    char dir[MAX_AVL_height];
    int pos = 1;

    Node* final = Find_Elem(tree, Elem, dir, path, &pos);

    if(final == NULL)
        return NO_SUCH_ELEMENT;

   
    path[0] = tree -> root;
    dir[0] = 0;
    
    

    if (final -> link[1] == NULL){
        path[pos - 1] -> link [dir[pos - 1]] = final -> link[0];
        if(final == tree -> root)
            tree -> root = final -> link[0];
    }
    else {
        Node* right = final -> link[1];
        if (right -> link[0] == NULL){


            right -> link[0] = final -> link[0];
            right -> balance = final -> balance;
            path[pos - 1] -> link[dir[pos - 1]] = right ;
            dir[pos] = 1;
            path[pos++] = right ;

        }
        else {
            struct Node* successor;
            int j = pos++;
            for (;;) {
                dir[pos] = 0;
                path[pos++] = right ;
                successor = right -> link[0];
                if (successor -> link[0] == NULL)
                break;
                right = successor;
            }
            
            successor -> link[0] = final -> link[0];
            right -> link[0] = successor -> link[1];
            successor -> link[1] = final -> link[1];
            successor -> balance = final -> balance;
            path[j - 1] -> link[dir[j - 1]] = successor;
            dir[j] = 1;
            path[j] = successor;

        }

        if(final == tree -> root){
            tree -> root = path[1];
        }    

    }

    

    final -> link[0] = final -> link[1] = NULL;
    Destruct_Node(final);
    

	while (--pos > 0) {
		Node* cur_node = path[pos];
		if (dir[pos] == 0){ 

			cur_node -> balance ++;
			if (cur_node -> balance == 1)
				break;
			else if (cur_node -> balance == 2){
                Node* right = cur_node -> link [1] ;
				if (right -> balance == -1){ 
					
					Node* new_node = Right_at_x_Left_at_y (right, cur_node, 1);
					if(pos != 1)
                        path[pos - 1] -> link [dir[pos - 1]] = new_node;
                    else 
                        tree-> root = new_node;
                   
				}
				else { 
                   
					cur_node -> link [1] = right -> link [0];
					right -> link [0] = cur_node;
					path[pos - 1] -> link [dir[pos - 1]] = right;
					if (right -> balance == 0) {
						right -> balance = -1;
						cur_node -> balance = 1;
						break;
					}

				    else
                        right -> balance = cur_node -> balance = 0;
				}
                 
				
			}
		}

		else { // -
            cur_node -> balance--;

            if (cur_node -> balance == -1)
                break;
            else if (cur_node -> balance == -2) {
                Node* left = cur_node -> link[0];
                if (left -> balance == 1) {
                    
                    Node* new_node = Left_at_x_Right_at_y(left, cur_node, 0);
                    path[pos - 1] -> link [dir[pos - 1]] = new_node;
                } 
                else {
                    cur_node -> link[0] = left -> link[1];
                    left -> link[1] = cur_node;
                    path[pos - 1] -> link [dir[pos - 1]] = left;

                    if (left -> balance == 0) {
                        left -> balance = 1;
                        cur_node -> balance = -1;
                        break;
                    }
                    else 
                        left -> balance = cur_node -> balance = 0;
                }
               
            }

        }
	}

    tree -> size--;
    return 0;


}


void Rebalance(int sign, Node* cur_node, Node** root)
{
            cur_node -> balance += sign;

            int dir = sign == 1;

            if (cur_node -> balance == sign*1)
                break;
            else if (cur_node -> balance == sign*2) {
                Node* left = cur_node -> link[dir];
                if (left -> balance == -sign) {
                    
                    Node* new_node = Left_at_x_Right_at_y(left, cur_node, 0);
                    *root = new_node;
                } 
                else {
                    cur_node -> link[0] = left -> link[1];
                    left -> link[1] = cur_node;
                    *root = left;

                    if (left -> balance == 0) {
                        left -> balance = -sign;
                        cur_node -> balance = sign;
                        break;
                    }
                    else 
                        left -> balance = cur_node -> balance = 0;
                }
               
            }

        }
