
#include "tree.h"
#include <stdio.h>

const int MAX_AVL_height = 16;
const int MAX_AVL_SIZE = 65536;

struct Node {
    Elem_t value;
    struct Node* link[2];
    char balance;
};

struct AVL_Tree{
    Node_t* root;
    unsigned int size;
    int err;
};




static int Compare_int(int left, int right){
    if(right < left)
        return -1;
    return right > left;
}

int (*Compare_function)(Elem_t Elem1, Elem_t Elem2) = Compare_int;


static Node_t* Left_rotate(Node_t* root){

    Node_t* temp = root -> link[1];
    root -> link[1] = root -> link[1] -> link[0];
    temp -> link[0] = root;

    return temp;
}

static Node_t* Right_rotate(Node_t* root){

    Node_t* temp = root -> link[0];
    root -> link[0] = root -> link[0] -> link[1];
    temp -> link[1] = root;

    return temp;
}


static void* avl_calloc(size_t num, size_t size){

    if (num * size > INT_MAX / 4)
        return NULL;
    return calloc(num, size);

}

static Node_t* Create_Node(Elem_t Elem){
    Node_t* node_t = (Node_t*) avl_calloc(sizeof(Node_t), 1);

    if(node_t)
        node_t -> value = Elem;
    return node_t;
}



AVL_Tree_t* ConAVL_Tree(){
    AVL_Tree_t* tree = (AVL_Tree_t*) avl_calloc(sizeof(AVL_Tree_t), 1);
    if(tree)
    {
        tree -> root = NULL;  
        tree -> size = 0;
    }

    return tree;
}



static void Destruct_Node(Node_t* node_t){

    if(!node_t){
        return;
    }

    Destruct_Node(node_t -> link[0]);
    Destruct_Node(node_t -> link[1]);

    free(node_t);
}



int DeAVL_Tree(AVL_Tree_t* tree){

    if(!tree){
        return NO_TREE;
    }

    Destruct_Node(tree -> root);
    free(tree);
    return NORMAL_STATE;
}


int cmp (Node_t* cur_node, int elem, void *data){
    int max = *(int*)data;
    if (max < cur_node -> value){
        *(int*)data = cur_node -> value;
    }
    return max;
}


static int Foreach_Node(Node_t* root, int(*callback)(Node_t* cur_node, Elem_t elem, void *data), void* data){


    callback(root, root -> value, data);

    if(root -> link[0])
        Foreach_Node(root -> link[0], callback, data);
    if(root -> link[1])
        Foreach_Node(root -> link[1], callback, data);

    return NORMAL_STATE;
}


int Foreach(AVL_Tree_t* tree, int(*callback)(Node_t* cur_node, Elem_t elem, void *data), void* data){

    
    if (!tree){
        return NO_TREE;
    }
    if(!tree -> root){
        return EMPTY;
    }
    if (!callback)
        return NO_FUNC;
    if(!data)
        return NO_DATA;

    return Foreach_Node(tree->root, callback, data);
}

char Is_elem_here(AVL_Tree_t* tree, Elem_t elem){

    if(!tree)
        return 0;
    if (!(tree -> root))
        return 0;

    int side = 0;

    for (Node_t* iterator = tree -> root; iterator != NULL; iterator = iterator -> link[side]){
         int cmp = Compare_function(iterator -> value, elem);
         if (cmp == 0){
            return 1;
        }
        side = cmp > 0;
    }

    return 0;
}


static Node_t* Find_Elem(AVL_Tree_t* tree, Elem_t elem,  char* dir, Node_t** path, int* length){

    int side = 0;

    for (Node_t* iterator = tree -> root; iterator != NULL; iterator = iterator -> link[side]){
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



static Node_t* Find_wheretoInsert( Elem_t elem, char* dir,  Node_t** recent_nz, Node_t** p_recent_nz, char* side){

	Node_t* iterator, *p_iterator;
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


static void Apdate_balance(Node_t* iterator, Node_t* final, char* dir){

	for (int length = 0; iterator != final; iterator = iterator -> link[dir[length++]])
    
        if (dir[length])
            iterator -> balance++;
        else
            iterator -> balance--;
}


static Node_t* Left_at_x_Right_at_y(Node_t* X, Node_t* cur_node, int dir){
	
	cur_node -> link[dir] = Left_rotate(X);
    Node_t* new_root = Right_rotate(cur_node);
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

static Node_t* Right_at_x_Left_at_y (Node_t* X, Node_t* cur_node, int dir){

	cur_node -> link[dir] = Right_rotate(X);
    Node_t* new_root = Left_rotate(cur_node);
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




int Insert(AVL_Tree_t* tree, Elem_t Elem){

    if(!tree)
        return NO_TREE;
    if(tree -> size == MAX_AVL_SIZE){
        return TREE_IS_FULL;
    }


    Node_t* recent_nz = tree -> root;
    Node_t* p_recent_nz = NULL;
    char dir[MAX_AVL_height+1];
    char side;


    Node_t* final = Find_wheretoInsert(Elem, dir, &recent_nz, &p_recent_nz, &side);



    if (!tree -> root){
        if(!(tree -> root = Create_Node(Elem)))
            return IMPOSSIBLE_TO_INSERT;
        tree -> size ++;
        return 0;
    }
    else if (!final) 
        return IMPOSSIBLE_TO_INSERT;

    else if(!(final -> link[side] = Create_Node(Elem)))
        return IMPOSSIBLE_TO_INSERT;

    tree -> size++;

    Apdate_balance(recent_nz, final -> link[side], dir);
  


    Node_t* new_root = NULL;

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


static int Rebalance(int sign, Node_t* cur_node, Node_t** root){
            cur_node -> balance += sign;

            int dir = sign == 1;
            int indir = sign == -1;

            if (cur_node -> balance == sign*1)
                return 1;
            else if (cur_node -> balance == sign*2) {
                Node_t* left = cur_node -> link[dir];
                if (left -> balance == -sign) {
                    
                    if( sign == -1)
                        *root = Left_at_x_Right_at_y(left, cur_node, 0);
                    else
                        *root = Right_at_x_Left_at_y(left, cur_node, 0);

        
                } 
                else {
                    cur_node -> link[dir] = left -> link[indir];
                    left -> link[indir] = cur_node;
                    *root = left;

                    if (left -> balance == 0) {
                        left -> balance = -sign;
                        cur_node -> balance = sign;
                        return 1;
                    }
                    else 
                        left -> balance = cur_node -> balance = 0;
                }
               
            }
            return 0;

}



int Delete(AVL_Tree_t* tree, Elem_t Elem){

    if (!tree)
        return NO_TREE;

    Node_t* path[MAX_AVL_height];
    char dir[MAX_AVL_height];
    int pos = 1;

    Node_t* final = Find_Elem(tree, Elem, dir, path, &pos);

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
        Node_t* right = final -> link[1];
        if (right -> link[0] == NULL){


            right -> link[0] = final -> link[0];
            right -> balance = final -> balance;
            path[pos - 1] -> link[dir[pos - 1]] = right ;
            dir[pos] = 1;
            path[pos++] = right ;

        }
        else {
            Node_t* successor;
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
    

	while (--pos > 1) {
		Node_t* cur_node = path[pos];
        Node_t** new_root = &(path[pos - 1] -> link [dir[pos - 1]]);
		if (dir[pos] == 0){ 
            if(Rebalance(1, cur_node, new_root))
                break;
		}
		else{ 
            if(Rebalance(-1, cur_node, new_root))
                break;
        }
	}


    if (pos == 1){
        Node_t* cur_node = path[pos];
        Node_t** new_root = &(tree -> root);
        if (dir[pos] == 0)
            Rebalance(1, cur_node, new_root);
        else  
            Rebalance(-1, cur_node, new_root);
    }


    tree -> size--;
    return 0;


}


static void Print_tree(FILE* flout, Node_t* cur_node, int level){

    if(!cur_node){
        return;
    }

    if(cur_node -> balance > 0){
        fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#FA1005\", label = \"", cur_node);
        fprintf(flout, Elem_t_print " %p", cur_node -> value, cur_node);
    }
    else if(cur_node -> balance < 0){
        fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#0FB8F0\", label = \"", cur_node);
        fprintf(flout, Elem_t_print " %p", cur_node -> value, cur_node);
    }
    else{
        fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#EFEFEF\", label = \"", cur_node);
        fprintf(flout, Elem_t_print " %p", cur_node -> value, cur_node);
    }
    fprintf(flout, "\" ];\n");


    fprintf(flout, " { rank = %d; \"%p\"}\n", level, cur_node);
    Print_tree(flout, cur_node-> link[0], level + 1);
    Print_tree(flout, cur_node-> link[1], level + 1);
    if (cur_node-> link[0])
        fprintf(flout, "\"%p\" -> \"%p\" [ arrowhead=\"vee\" , label = \" l \"];\n", cur_node, cur_node->  link[0]);
       // fprintf(flout, "\"%p\" -> \"%p\" [ arrowhead=\"vee\" ];\n", cur_node, cur_node->  link[0]);

    if(cur_node-> link[1])
        fprintf(flout, "\"%p\" -> \"%p\" [arrowhead=\"vee\",  label = \" r \"];\n", cur_node, cur_node->  link[1]);
        //fprintf(flout, "\"%p\" -> \"%p\" [arrowhead=\"vee\"];\n", cur_node, cur_node->  link[1]);

   
}



int Diigraph(AVL_Tree_t* tree, const char* name){

    if(!tree){
        return NO_TREE;
    }

    FILE* flin = fopen(name, "wt");
    fprintf(flin, "digraph G{\n"
                  "rankdir=TB;\n"
                  "node[color=\"goldenrod\",fontsize=13];\n");
    Print_tree(flin, tree -> root, 1);
    fprintf(flin, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#EFEFEF\", label = \"", tree);
    fprintf(flin,"%d %d \" ];\n", tree -> size, MAX_AVL_SIZE);
    fprintf(flin, " }\n");
    fclose(flin);
    
    return NORMAL_STATE;

}

