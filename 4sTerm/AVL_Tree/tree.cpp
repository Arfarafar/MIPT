

enum errors{
    NORMAL_STATE = 0,
    IMPOSSIBLE_TO_INSERT,

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



Node* Find_Elem( Elem_t Elem, void* dir,  Node** recent_nz, Node** p_recent_nz){

	Node* iterator, p_iterator;

    for(iterator = recent_nz, p_iterator = *p_recent_nz; 
        iterator != NULL;
        p_iterator = iterator, iterator = iterator -> link[side]) 
    {
        int cmp = Compare_function(iterator -> value, Elem_t);
        if (cmp == 0)
            return -1;
        if (iterator -> balance != 0){
            p_recent_nz = p_iterator;
            recent_nz = iterator;
            length = 0;
        }
        dir[length++] = side = cmp > 0;

    }

    return p_iterator;

}


int Apdate_balance(Node* iterator, Node* final, void* dir){

	 for (int length = 0; p != final; iterator = iterator -> link[dir[length++]])
    
        if (dir[length])
            iterator -> balance++;
        else
            iterator -> balance--;
}


int Insert(AVL_Tree* tree, Elem_t Elem){

    if(!tree)
        return -2;

    Node* recent_nz = tree -> root, p_recent_nz = NULL;
    Node* final;
    char dir[MAX_AVL_height];
    int length;
    char side;


    final = Find_Elem()

    if (!final -> link[side] = Create_Node(Elem))
        return IMPOSSIBLE_TO_INSERT;
    tree -> size++;

   Apdate_balance(recent_nz, p_iterator -> link[side], dir)


    Node* new_root = NULL;

    if(recent_nz -> balance == -2){
        if(recent_nz -> link[0] -> balance == -1)
        {
            new_root = Right_rotate(recent_nz);
            recent_nz -> balance = 0;
        }

        else
        {
            recent_nz -> link[0] = Left_rotate(recent_nz -> link[0]);
            new_root = Right_rotate(recent_nz);
            if (new_root -> balance == −1) {
                recent_nz -> link[0] -> balance = 0;
                recent_nz -> balance = 1;
            }
            else if (new_root -> balance == 0) {
                recent_nz -> link[0] -> balance = 0;
                recent_nz -> balance = 0;
            }
            else {
                recent_nz -> link[0] -> balance = −1;
                recent_nz -> balance = 0;
            }
           
       }


    }
    else if (recent_nz -> balance == 2){
        if(recent_nz -> link[1] -> balance == 1)
        {
            new_root = Left_rotate(recent_nz);
            recent_nz -> balance = 0;
        }

        else
        {
            recent_nz -> link[1] = Right_rotate(recent_nz -> link[1]);
            new_root = Left_rotate(recent_nz);
            if (new_root -> balance == −1) {
                recent_nz -> link[1] -> balance = 1;
                recent_nz -> balance = 0;
            }
            else if (new_root -> balance == 0) {
                recent_nz -> link[1] -> balance = 0;
                recent_nz -> balance = 0;
            }
            else {
                recent_nz -> link[1] -> balance = 0;
                recent_nz -> balance = -1;
            }
           
       }

        
    }
    else return 0;

    new_root -> balance = 0;
        

    if(p_recent_nz)
        p_recent_nz -> link[recent_nz != p_recent_nz -> link[0]] = new_root;
    else
        tree -> root = new_root;
    return 0;

}


int Delete(AVL_Tree* tree, Elem_t Elem){

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
        fprintf(flin, "\"%d\" [shape=record,label=\"  {%d | value\\n%p\\n " Elem_t_print " | {<prev> previous\\n%p\\n %d | <next> next\\n%p\\n %d} }\" ];\n",
        current, current, &(tree -> Nodes[current].value),
        *(char**)tree -> Nodes[current].value, &(tree -> Nodes[current].previous), tree -> Nodes[current].previous,
        &(tree -> Nodes[current].next), tree -> Nodes[current].next);
        fprintf(flin, " { rank = %d; \"%d\"}\n", 0, current);
        fprintf(flin, "%d:<prev> -> %d [arrowhead=\"vee\"];\n", current, tree -> Nodes[current].previous);
        fprintf(flin, "%d:<next> -> %d [arrowhead=\"vee\"];\n", current, tree -> Nodes[current].next);

        current = tree -> head;
        //int previous = tree -> Nodes[current].previous;
        for(int i = 0; i < tree -> size; i++){
            fprintf(flin, "\"%d\" [shape=record,label=\"  {%d | value\\n%p\\n ",  current, current, &(tree -> Nodes[current].value));
            (*Print_Function)(flin,  *(char**)tree -> Nodes[current].value);
            fprintf(flin, " | {<prev> previous\\n%p\\n %d | <next> next\\n%p\\n %d} }\" ];\n",
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