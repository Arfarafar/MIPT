

enum errors{
    NORMAL_STATE = 0,
    IMPOSSIBLE_TO_INSERT,
    INVALID_TREE,
    TREE_IS_FULL

};

typedef int Elem_t;
#define Elem_t_print "%d"

typedef struct {
    Elem_t value;
    Node* link[2];
    char balance;
} Node;

typedef struct {
    Node* root;
    size_t size;
    int err;
} AVL_Tree;


int Insert(AVL_Tree* tree, Elem_t Elem);
int Delete(AVL_Tree* tree, Elem_t Elem);
int Find_Elem(AVL_Tree* tree, Elem_t Elem);
AVL_Tree* Merge(AVL_Tree* tree_1, AVL_Tree* tree_2);
int Split(AVL_Tree* scr, Elem_t Elem,  AVL_Tree* lesseq, AVL_Tree* more);

Node* Left_rotate(Node*);
Node* Right_rotate(Node*);

void AVL_Tree_Dump(AVL_Tree* tree);
void Eraise_Err(AVL_Tree* tree);
void Diigraph (AVL_Tree* tree);


AVL_Tree* ConAVL_Tree(Elem_t Elem);
void DeAVL_Tree(AVL_Tree*);

int Compare_int(int left, int right);


int (*Compare_function)(Elem_t Elem1, Elem_t Elem2) = Compare_int;



--------------------------------------------------------------------------------------------------------------------------------


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

    if (num * size > MAX_INT / 2)
        return NULL;
    return calloc(num, size);

}

Node* Create_Node(Elem_t Elem){
    Node* node = (Node*) avl_calloc(sizeof(Node), 1);

    if(node)
        node -> value = Elem;
    return node;
}

void Destruct_Node(Node* node){
    free(node);
}


AVL_Tree* ConAVL_Tree(Elem_t Elem){
    AVL_Tree* tree = (AVL_Tree*) avl_calloc(sizeof(AVL_Tree), 1);
    if(tree)
    {
        if(!tree -> root = Create_Node(Elem))
            return NULL;
        tree -> size = 1;
    }

    return tree;
}


Node* Find_Elem(AVL_Tree tree, Elem_t elem,  char* dir, Node** path, int* length){

    if(!tree)
        return NULL;

    int side = 0;

    for (Node* iterator = tree -> root; iterator != NULL; iterator = iterator -> link[side]){
         int cmp = Compare_function(iterator -> value, Elem_t);
         if (cmp == 0){
            return iterator;
        }
        dir[*length] = side = cmp > 0;
        path[*length] = iterator;
        (*length)++;
    }

    return NULL;
}



Node* Find_wheretoInsert( Elem_t Elem, char* dir,  Node** recent_nz, Node** p_recent_nz, char* side){

	Node* iterator, p_iterator;
    int length = 0;

    for(iterator = *recent_nz, p_iterator = *p_recent_nz; 
        iterator != NULL;
        p_iterator = iterator, iterator = iterator -> link[*side]) 
    {
        int cmp = Compare_function(iterator -> value, Elem_t);
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


int Apdate_balance(Node* iterator, Node* final, void* dir){

	 for (int length = 0; iterator != final; iterator = iterator -> link[dir[length++]])
    
        if (dir[length])
            iterator -> balance++;
        else
            iterator -> balance--;
}

Node* Left_at_x_Right_at_y(Node* X, Node* Y, int dir){
	
	Y -> link[dir] = Left_rotate(X);
    Node* new_root = Right_rotate(Y);
    if (new_root -> balance == −1) {
        X -> balance = 0;
        Y -> balance = 1;
    }
    else if (new_root -> balance == 0) {
        X -> balance = 0;
        Y -> balance = 0;
    }
    else {
        X -> balance = −1;
        Y -> balance = 0;
    }
    new_root -> balance = 0;

    return new_root;
}

Node* Right_at_x_Left_at_y (Node* X, Node* Y, int dir){

	Y -> link[dir] = Right_rotate(X);
    Node* new_root = Left_rotate(Y);
        if (new_root -> balance == −1) {
            X -> balance = 1;
            Y -> balance = 0;
        }
        else if (new_root -> balance == 0) {
            X -> balance = 0;
            Y -> balance = 0;
        }
        else {
            X -> balance = 0;
            Y -> balance = -1;
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


    Node* recent_nz = tree -> root, p_recent_nz = NULL;
    char dir[MAX_AVL_height+1];
    char side;


    Node* final = Find_wheretoInsert(Elem, dir, &recent_nz, &p_recent_nz, &side);

    if (if !final || !final -> link[side] = Create_Node(Elem))
        return IMPOSSIBLE_TO_INSERT;
    tree -> size++;

    Apdate_balance(recent_nz, final, dir)


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
            new_root = Left_at_x_Right_at_y(recent_nz -> link[0], recent_nz, 0); // x, y, dir
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
    Node* path[MAX_AVL_height] = {};
    char dir[MAX_AVL_height];
    int pos = 0;

    Node* final = Find_Elem(tree, Elem, dir, path, &pos);
    if (! tree -> size - 1){
        DeAVL_Tree(tree);
        return NO_TREE;
    }

    if (final -> link[1] == NULL){
        path[pos - 1] -> link [dir[pos - 1]] = final -> link[0];
    }
    else {
        Node* right = final -> link[1];
        if (right -> link[0] == NULL){
            right -> link[0] = final -> link[0];
            right -> balance = final -> balance;
            path[pos − 1] -> link[dir[pos − 1]] = right ;
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
            path[j − 1] -> link[dir[j − 1]] = successor;
            dir[j] = 1;
            path[j] = successor;

        }
    }

    Destruct_Node(final);

    assert (k > 0);
	while (−−pos > 0) {
		Node* cur_node = path[pos];
		if (dir[pos] == 0){ 
				cur_node -> balance++;

			if (y→avl balance == 1)
				break;
			else if (y→avl balance == 2){

				if (x →avl balance == −1){ 
					
					Node* new_node = Right_at_x_Left_at_y (cur_node -> link [1], cur_node, 1);
					path[pos − 1] -> link [dir[pos − 1]] = new_node ;

				}
				else { 
						y→avl link [1] = x →avl link [0];
						x →avl link [0] = y;
						pa[k − 1]→avl link [da[k − 1]] = x ;
						if (x →avl balance == 0) {
						x →avl balance = − 1;
						y→avl balance = 1;
						break;
					}

					else x →avl balance = y→avl balance = 0;
				}

					tree -> size−−;
					tree→avl generation++;
					return (void ∗) item;
			}

		}
		else { h Update y’s balance factor after right-side AVL deletion 179 i }
	}


}





#define Print_array(array)  \
for (int i = 0; i < tree -> maxsize; i++) {  \
array ;} \
putchar('\n');

void AVL_Tree_Dump(AVL_Tree* tree) {
    assert(tree);

    printf("       ");
    Print_array(printf("%5d", i))
    printf("Value: ");
    Print_array(Print_Word(stdout, *(char**)tree -> Nodes[i].value))
    printf("next:  ");
    Print_array(printf("%5d", tree->Nodes[i].next))
    printf("prev:  ");
    Print_array(printf("%5d", tree->Nodes[i].previous))

    printf("Head: %d\n", tree -> head);
    printf("Tail: %d\n", tree -> tail);

    int current = tree -> head;
    printf("Full path: %d ", current);
    for(int i = 0; i < tree -> size; i++)
        printf("-> %d ", current = tree->Nodes[current].next);
    printf("\nFree: %d\n", tree -> free);
    printf("Size: %d\n", tree -> size);
    printf("Max_Size: %d\n", tree -> maxsize);

}
#undef Print_array

void Print_tree(FILE* flin, AVL_Tree* tree){
        int current = 0;
        fprintf(flin, "\"%d\" [shape=record,label=\"  {%d | value\\n%final\\n " Elem_t_print " | {<prev> previous\\n%final\\n %d | <next> next\\n%final\\n %d} }\" ];\n",
        current, current, &(tree -> Nodes[current].value),
        *(char**)tree -> Nodes[current].value, &(tree -> Nodes[current].previous), tree -> Nodes[current].previous,
        &(tree -> Nodes[current].next), tree -> Nodes[current].next);
        fprintf(flin, " { rank = %d; \"%d\"}\n", 0, current);
        fprintf(flin, "%d:<prev> -> %d [arrowhead=\"vee\"];\n", current, tree -> Nodes[current].previous);
        fprintf(flin, "%d:<next> -> %d [arrowhead=\"vee\"];\n", current, tree -> Nodes[current].next);

        current = tree -> head;
        //int previous = tree -> Nodes[current].previous;
        for(int i = 0; i < tree -> size; i++){
            fprintf(flin, "\"%d\" [shape=record,label=\"  {%d | value\\n%final\\n ",  current, current, &(tree -> Nodes[current].value));
            (*Print_Function)(flin,  *(char**)tree -> Nodes[current].value);
            fprintf(flin, " | {<prev> previous\\n%final\\n %d | <next> next\\n%final\\n %d} }\" ];\n",
                    &(tree -> Nodes[current].previous), tree -> Nodes[current].previous,
                    &(tree -> Nodes[current].next), tree -> Nodes[current].next);
            fprintf(flin, " { rank = %d; \"%d\"}\n", 0, current);
            fprintf(flin, "%d:<prev> -> %d [arrowhead=\"vee\"];\n", current, tree -> Nodes[current].previous);
            fprintf(flin, "%d:<next> -> %d [arrowhead=\"vee\"];\n", current, tree -> Nodes[current].next);
            // previous = current;
            current = tree -> Nodes[current].next;
        }
};



void Diigraph(AVL_Tree* tree){
    FILE* flin = fopen("tree.dot", "wt");
    fprintf(flin, "digraph G{\n"
                  "rankdir=LR;\n"
                  "node[color=\"goldenrod\",fontsize=13];\n");
    Print_tree(flin, tree);
    fprintf(flin, " }\n"
    );
    fclose(flin);
    system("del tree.dot.png            && "
           DOT_PATH " -Tpng -O tree.dot && "
           "start /wait tree.dot.png    && "
           "pause");

}

void Eraise_Err(AVL_Tree* tree){
    AVL_Tree_Dump(tree);
    Diigraph(tree);
    abort();
}