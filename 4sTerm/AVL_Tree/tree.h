
#include <limits.h>
#include <stdlib.h>

enum errors{
    NORMAL_STATE = 0,
    IMPOSSIBLE_TO_INSERT,
    INVALID_TREE,
    TREE_IS_FULL,
    NO_SUCH_ELEMENT,
    NO_TREE
};

typedef int Elem_t;
#define Elem_t_print "%d"

const int MAX_AVL_height = 16;
const int MAX_AVL_SIZE = 65536;

typedef struct Node{
    Elem_t value;
    struct Node* link[2];
    char balance;
} Node;

typedef struct {
    Node* root;
    size_t size;
    int err;
} AVL_Tree;


int Insert(AVL_Tree* tree, Elem_t Elem);
int Delete(AVL_Tree* tree, Elem_t Elem);
Node* Find_Elem(AVL_Tree* tree, Elem_t elem,  char* dir, Node** path, int* length);
void Foreach(Node* root, int(*callback)(Node* cur_node, Elem_t elem, void *data), void* data);

Node* Find_wheretoInsert( Elem_t Elem, char* dir,  Node** recent_nz, Node** p_recent_nz, char* side);

Node* Left_rotate(Node*);
Node* Right_rotate(Node*);


void Diigraph (AVL_Tree* tree, const char* name);


AVL_Tree* ConAVL_Tree(Elem_t Elem);
void DeAVL_Tree(AVL_Tree* tree);

int Compare_int(int left, int right);


int (*Compare_function)(Elem_t Elem1, Elem_t Elem2) = Compare_int;



//-----------------------------------------------------------------------------------------------------------


int Compare_int(int left, int right){
    if(right < left)
        return -1;
    return right > left;
}


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



AVL_Tree* ConAVL_Tree(Elem_t Elem){
    AVL_Tree* tree = (AVL_Tree*) avl_calloc(sizeof(AVL_Tree), 1);
    if(tree)
    {
        if(!(tree -> root = Create_Node(Elem)))
            return NULL;
        tree -> size = 1;
    }

    return tree;
}

void Destruct_Node(Node* node){

    if(node -> link[0])
        Destruct_Node(node -> link[0]);
    if(node -> link[1])
        Destruct_Node(node -> link[1]);

    free(node);
}



void DeAVL_Tree(AVL_Tree* tree){

    Destruct_Node(tree -> root);

    free(tree);
}


void Foreach(Node* root, int(*callback)(Node* cur_node, Elem_t elem, void *data), void* data){

    callback(root, root -> value, data);

    if(root -> link[0])
        Foreach(root -> link[0], callback, data);
    if(root -> link[1])
        Foreach(root -> link[1], callback, data);
}


Node* Find_Elem(AVL_Tree* tree, Elem_t elem,  char* dir, Node** path, int* length){

    if(!tree)
        return NULL;

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


    if (!final || !(final -> link[side] = Create_Node(Elem)))
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

        else
        {
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

        else
        {
            new_root = Right_at_x_Left_at_y(recent_nz -> link[1], recent_nz, 1);
           
       }

        
    }
    else return 0;

        

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

    if (!(tree -> size - 1)){
        DeAVL_Tree(tree);
        return NO_TREE;
    }

   
    path[0] = final;
    dir[0] = 0;
    

    AVL_Tree treeee;
    

    if (final -> link[1] == NULL){
        path[pos - 1] -> link [dir[pos - 1]] = final -> link[0];
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
    }

    if(final == tree -> root){
        tree -> root = path[1];
    }    

    final -> link[0] = final -> link[1] = NULL;

    Destruct_Node(final);
    


	while (--pos > 0) {
		Node* cur_node = path[pos];
		if (dir[pos] == 0){ 

			cur_node -> balance++;
			if (cur_node -> balance == 1)
				break;
			else if (cur_node -> balance == 2){

				if ((cur_node -> link [1]) -> balance == -1){ 
					
					Node* new_node = Right_at_x_Left_at_y (cur_node -> link [1], cur_node, 1);
					path[pos - 1] -> link [dir[pos - 1]] = new_node;
				}
				else { 
					cur_node -> link [1] = cur_node -> link [1] -> link [0];
					cur_node -> link [1] -> link [0] = cur_node;
					path[pos - 1] -> link [dir[pos - 1]] = cur_node -> link [1];
					if ((cur_node -> link [1])-> balance == 0) {
						(cur_node -> link [1]) -> balance = -1;
						cur_node -> balance = 1;
						break;
					}

				    else
                        (cur_node -> link [1]) -> balance = cur_node -> balance = 0;
				}

				
			}

		}
		else {
            cur_node -> balance--;
            if (cur_node -> balance == -1)
                break;
            else if (cur_node -> balance == -2) {
                
                if (cur_node -> link[0] -> balance == 1) {
                    
                    Node* new_node = Left_at_x_Right_at_y(cur_node -> link[0], cur_node, 0);
                    path[pos - 1] -> link [dir[pos - 1]] = new_node;
                } 
                else {
                    cur_node -> link[0] = (cur_node -> link[0]) -> link[1];
                   
                    (cur_node -> link[0]) -> link[1] = cur_node;
                    path[pos - 1] -> link [dir[pos - 1]] = cur_node -> link [0];

                    if ((cur_node -> link)[0] -> balance == 0) {
                        (cur_node -> link[0]) -> balance = 1;
                        cur_node -> balance = -1;
                        break;
                    }
                    else 
                        (cur_node -> link[0]) -> balance = cur_node -> balance = 0;
                }
            }

        }
	}

    tree -> size--;
    return 0;


}





void Print_tree(FILE* flout, Node* cur_node, int level){

    if(!cur_node){
        return;
    }

    if(cur_node -> balance > 0){
        fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#FA1005\", label = \"", cur_node);
        fprintf(flout, Elem_t_print " +", cur_node -> value);
    }
    else if(cur_node -> balance < 0){
        fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#0FB8F0\", label = \"", cur_node);
        fprintf(flout, Elem_t_print " -", cur_node -> value);
    }
    else{
        fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#EFEFEF\", label = \"", cur_node);
        fprintf(flout, Elem_t_print " o", cur_node -> value);
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



void Diigraph(AVL_Tree* tree, const char* name){
    FILE* flin = fopen(name, "wt");
    fprintf(flin, "digraph G{\n"
                  "rankdir=TB;\n"
                  "node[color=\"goldenrod\",fontsize=13];\n");
    Print_tree(flin, tree -> root, 1);
    fprintf(flin, " }\n");
    fclose(flin);
    

}
