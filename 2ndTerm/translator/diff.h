//
// Created by andrew on 21.11.2019.
//

#ifndef DIFF_DIFF_H
#define DIFF_DIFF_H


#include "types.h"
#include <ctype.h>

#include "file.h"
#include "latex.h"


void Print_tree_in_file(Node* cur_node, FILE* flout);
Node* Create_Node_from_file(FILE* flin, Node* Parent, int* length_remaining);
void Set_Type(char* fun, Node* cur_node);
void* Deconstruct_Nodes(Node* cur_node);
Node* Tree_Construct(FILE* flin);
Node* diff( Node* cur_node, char* variable);
Node* Optimize(Node *, char*);
Node* Create_new_Node(Node* left, Node* right, int type, double value);

#define PARENT cur_node -> parent
#define LEFT cur_node -> left
#define RIGHT cur_node -> right


Node* Execute (const char side, Node* cur_node){

    if (PARENT) {

        Node* direction = nullptr;
        //if (RIGHT -> parent != cur_node)
        if (side == 'l'){
            direction = LEFT;
            LEFT -> parent = PARENT;
            LEFT = nullptr;
        }
        else
        {
            direction = RIGHT;
            RIGHT -> parent = PARENT;
            RIGHT = nullptr;

        }
        PARENT = nullptr;
        Deconstruct_Nodes(cur_node);
        return direction;

    } else {
        Node* temp = side == 'l' ? LEFT : RIGHT;

        (side == 'l' ? LEFT : RIGHT) = nullptr;

        temp -> parent = nullptr;
        Deconstruct_Nodes(cur_node);
        return temp;
    }
}

Node* Change_by_Number(Node* cur_node, double num){
    assert(cur_node);

    Deconstruct_Nodes(LEFT);
    Deconstruct_Nodes(RIGHT);
    cur_node -> node_type = TYPE_NUM;
    cur_node -> value = -1;
    cur_node -> value = num;
    return cur_node;
}

void Print_parent(FILE* flout, Node* cur_node, int level){

    if(!cur_node){
        return;
    }
    Print_phrase(flout, cur_node, 1);

    fprintf(flout, " { rank = %d; \"%p\"}\n", level, cur_node);
    Print_parent(flout, cur_node->parent, level - 1);
    if (cur_node-> parent)
        fprintf(flout, "\"%p\" -> \"%p\" [ arrowhead=\"vee\"];\n", cur_node, cur_node-> parent);

}


void Print_way_out(Node* tree){
    FILE* flout = fopen("tree.dot", "wt");
    fprintf(flout, "digraph G{\n"
                   "rankdir=TB; \n"
                   "node[color=\"goldenrod\",fontsize= 13];\n");
    Print_parent(flout, tree, 1);
    fprintf(flout, " }\n");
    fclose(flout);
    system("del tree.dot.png            && "
    DOT_PATH " -Tpng -O tree.dot && "
             "start /wait tree.dot.png    && "
             "pause");
}

Node* Find_op_PlusMinus(Node* cur_node){
    if(!cur_node){
        return nullptr;
    }
    if (cur_node -> value == OP_PLUS || cur_node -> value == OP_MINUS)
        return cur_node;
    return Find_op_PlusMinus(PARENT);
}

Node* Find_headnote(Node* cur_node){
    if (PARENT)
        return Find_headnote(PARENT);
    return cur_node;
}



#define TYPE_LEFT(what) (LEFT -> node_type == what)
#define TYPE_RIGHT(what) (RIGHT -> node_type == what)

#define IS_LEFT(what) (TYPE_LEFT(TYPE_NUM) && LEFT -> value == what)
#define IS_RIGHT(what) (TYPE_RIGHT(TYPE_NUM) && RIGHT -> value == what)

#define BOTH_NUM (RIGHT -> node_type == TYPE_NUM && LEFT -> node_type == TYPE_NUM)
#define CHECK_PARENT(what) (PARENT != nullptr && PARENT -> node_type == TYPE_OPERATOR && PARENT -> value == what)

#define ANOTHER_PARENT_SIDE (PARENT -> left == cur_node ? PARENT -> right : PARENT -> left)
#define ANOTHER_PARENT_SIDE_IS_NUMBER  (ANOTHER_PARENT_SIDE -> node_type == TYPE_NUM)

#define OPERATION(T) LEFT -> value T RIGHT -> value

Node* Optimize_Node(Node* cur_node, int* counter){
    if(!cur_node)
        return nullptr ;

    switch (cur_node -> node_type){
        case TYPE_NUM:
            return cur_node;
        case TYPE_VAR:
            return cur_node;
        case TYPE_OPERATOR:
            switch ((int)cur_node -> value){

                case OP_MUL:
                    if (IS_LEFT(0) || IS_RIGHT(0)){
                        (*counter)++;
                        //Treegraph(Find_headnote(cur_node));
                        return  Change_by_Number(cur_node, 0);
                    }
                    else if IS_LEFT(1){
                        (*counter)++;
                        // Treegraph(cur_node->parent->parent);
                        return Execute('r' , cur_node);
                    }
                    else if IS_RIGHT(1){
                        (*counter)++;
                        // Treegraph(cur_node->parent->parent);
                        return Execute('l', cur_node);
                    }
                    else if BOTH_NUM{
                        (*counter)++;
                        return  Change_by_Number(cur_node,  OPERATION(*));
                    }
                    else if (CHECK_PARENT(OP_MUL)  && (TYPE_RIGHT(TYPE_NUM) || TYPE_LEFT(TYPE_NUM)) && ANOTHER_PARENT_SIDE_IS_NUMBER) {
                        if (TYPE_RIGHT(TYPE_NUM)){
                            ANOTHER_PARENT_SIDE -> value *= RIGHT -> value;
                            return Execute('l', cur_node);
                        }
                        else {
                            ANOTHER_PARENT_SIDE -> value *= LEFT -> value;
                            return Execute('r', cur_node);
                        }
                    }
                    break;
                case OP_PLUS:
                    if IS_LEFT(0){
                        (*counter)++;
                        //Treegraph(cur_node->parent->parent);
                        return Execute('r' , cur_node);
                    }
                    else if IS_RIGHT(0){
                        (*counter)++;
                        //Treegraph(cur_node->parent->parent);
                        return Execute('l', cur_node);
                    }
                    else if BOTH_NUM{
                        (*counter)++;

                        return  Change_by_Number(cur_node, OPERATION(+));
                    }
                    break;
                case OP_MINUS:
                    if IS_RIGHT(0){
                        (*counter)++;
                        //Treegraph(cur_node->parent->parent);
                        return Execute('l', cur_node);
                    }
                    else if IS_LEFT(0){
                        if (PARENT) {
                            Node *temp = Find_op_PlusMinus(cur_node->parent);
                            if (temp) {
                                temp->value == OP_PLUS ? temp->value = OP_MINUS : temp->value = OP_PLUS;
                            } else {
                                temp = Find_headnote(cur_node);
                                temp->parent = Create_new_Node(Create_new_Node(NULL, NULL, TYPE_NUM, 0), temp,
                                                               TYPE_OPERATOR, OP_MINUS);
                            }
                            (*counter)++;
                            return Execute('r', cur_node);
                        }
                    }
                    else if BOTH_NUM{
                        (*counter)++;
                        return  Change_by_Number(cur_node,  OPERATION(-));
                    }
                    break;
                case OP_POW:
                    if IS_RIGHT(0){
                        (*counter)++;
                        return Change_by_Number(cur_node, 1);
                    }
                    else if IS_RIGHT(1){
                        (*counter)++;
                        return Execute('l', cur_node);
                    }
                    else if (CHECK_PARENT(OP_POW) && ANOTHER_PARENT_SIDE_IS_NUMBER && TYPE_RIGHT(TYPE_NUM)){
                        (*counter)++;
                        PARENT -> right -> value *= RIGHT -> value;
                        return Execute('l', cur_node);
                    }
                    break;
                case OP_DIV:
                    if (IS_LEFT(0) ){
                        (*counter)++;
                        // Treegraph(cur_node->parent->parent);
                        return  Change_by_Number(cur_node, 0);
                    }
                    else if BOTH_NUM{
                        (*counter)++;
                        return  Change_by_Number(cur_node,  OPERATION(/));
                    }
                    break;
                default:
                    break;
            }
            RIGHT = Optimize_Node(RIGHT, counter);
            LEFT = Optimize_Node(LEFT,  counter);
            return cur_node;


        default:
            printf("error Optimize_Node");
            return nullptr;
    }
}

void Add_Random_Message(char* file_name){
    int i = rand() % Common_phrase_number;
    Append_Message(file_name, common_phrases[i]);
}


Node* Optimize(Node* head_note, char* file_name){
    int counter  = 1;
    while (counter){
        counter = 0;
        Add_Random_Message(file_name);
        head_note = Find_headnote(Optimize_Node(head_note, &counter));
        Add_Formula(head_note, file_name);
        // Treegraph(head_note);
    }
    Append_Message(file_name, "К сожалению больше нечего оптимизирвоать, текущий шаг закончен\n\n");
    return head_note;
}

Node* Copy(Node* what, Node* parent){
    if(!what)
        return nullptr;
    Node* newNode = (Node*)calloc(sizeof(Node), 1);
    newNode -> node_type = what ->node_type;
    newNode -> left = Copy(what->left, newNode);
    newNode -> right = Copy(what->right, newNode);
    newNode ->  parent = parent;
    newNode ->  value = what-> value;
    return newNode;
}

bool Find_var_in_tree(Node* cur_node, char* variable){
    if (!cur_node)
        return 0;
    if ( cur_node -> node_type == TYPE_VAR  && strcmp(Var_array.var[(int)cur_node -> value].name , variable) == 0) {
        return 1;
    }
    if ( LEFT || RIGHT ) {
        return Find_var_in_tree(LEFT, variable) ? 1 : Find_var_in_tree(RIGHT, variable);
    }
    return 0;
}

Node* Create_new_Node(Node* left, Node* right, int type, double value){
    Node* newNode = (Node*)calloc(sizeof(Node), 1);
    newNode -> node_type = type;
    newNode -> left = left;
    newNode -> right = right;
    if (left){
        left -> parent = newNode;
    }
    if (right){
        right -> parent = newNode;
    }

    newNode -> value = value;

    return newNode;
}

#define MUL(left, right)    Create_new_Node(left, right, TYPE_OPERATOR,  OP_MUL  )
#define DIV(left, right)    Create_new_Node(left, right, TYPE_OPERATOR,  OP_DIV  )
#define POW(left, right)    Create_new_Node(left, right, TYPE_OPERATOR,  OP_POW  )
#define PLUS(left, right)   Create_new_Node(left, right, TYPE_OPERATOR, OP_PLUS )
#define MINUS(left, right)  Create_new_Node(left, right, TYPE_OPERATOR, OP_MINUS)
#define CREATE_NODE_NUM(what) Create_new_Node(nullptr, nullptr, TYPE_NUM,  what )

#define cL Copy(LEFT, nullptr)
#define cR Copy(RIGHT, nullptr)
#define dL diff(LEFT, variable)
#define dR diff(RIGHT, variable)


Node* diff(Node* cur_node,  char* variable){

    switch (cur_node ->node_type){
        case TYPE_NUM:
            return  CREATE_NODE_NUM(0);
        case TYPE_VAR:
            if (strcmp(Var_array.var[(int)cur_node -> value].name, variable) == 0) {
                return CREATE_NODE_NUM(1);
            }
            else
                return CREATE_NODE_NUM(0);

        case TYPE_OPERATOR:
            switch ((int)cur_node -> value){
                case OP_MINUS:
                    return MINUS( dL, dR);
                case OP_PLUS:
                    return PLUS( dL, dR);
                case OP_MUL:
                    return PLUS(MUL(dL, cR ), MUL( cL, dR));
                case OP_DIV:
                    return DIV(MINUS(MUL(dL, cR),
                                     MUL( cL , dR)),
                               POW( cR , CREATE_NODE_NUM(2)));
                case OP_SIN:
                    return MUL( LEFT ? dL : dR,
                               Create_new_Node(nullptr, LEFT ? cL : cR , TYPE_OPERATOR,  OP_COS)) ;
                case OP_COS:
                    return MUL(LEFT ? dL : dR,
                               MINUS(CREATE_NODE_NUM(0),
                                     Create_new_Node(nullptr, LEFT ? cL : cR , TYPE_OPERATOR, OP_SIN))) ;
                case OP_POW:
                    return  Find_var_in_tree(RIGHT, variable)  ?
                            MUL(POW(Copy(LEFT, nullptr), Copy(RIGHT, nullptr)),
                                PLUS(MUL(diff(Create_new_Node( cL, nullptr, TYPE_OPERATOR,  OP_LOGE), variable), cR),
                                     MUL(Create_new_Node( cL , nullptr, TYPE_OPERATOR,  OP_LOGE), dR)))
                                         :
                            MUL(  dL , MUL( cR, POW( cL, MINUS( cR, CREATE_NODE_NUM(1)))));

                case OP_LOGE:
                    return Find_var_in_tree(cur_node, variable) ? MUL(DIV( CREATE_NODE_NUM(1), LEFT ? cL : cR ),
                                                                      LEFT ? dL : dR)
                                                                : CREATE_NODE_NUM(0);
                case OP_EXP:
                    return MUL(Copy(cur_node, nullptr), LEFT ? dL : dR);
                default:
                    printf("Unexpected operand\n");
                    return nullptr;
            }
    }
    return nullptr;
}
#undef MUL
#undef PLUS
#undef MINUS
#undef DIV
#undef POW


Node* Tree_Construct(FILE* flin) {

    int length_remaining = getFileLength(flin);
    Node* head_node = Create_Node_from_file(flin, nullptr, &length_remaining);
    if(!length_remaining){
        return head_node;
    } else {
        Deconstruct_Nodes(head_node);
        return nullptr;
    }

}

void* Deconstruct_Nodes(Node* cur_node){
    if(!cur_node){
        return nullptr;
    }
    if (PARENT){
        if (cur_node->parent->left == cur_node) {
            cur_node->parent->left = nullptr;
        } else if (cur_node->parent->right == cur_node) {
            cur_node->parent->right = nullptr;
        }
      /*  else {
            Treegraph(PARENT);
            Treegraph(cur_node);
            Print_way_out(cur_node);
        } */
    }
    Deconstruct_Nodes(cur_node->left);
    Deconstruct_Nodes(cur_node->right);
    free(cur_node);
    return nullptr;
}



int Skip_spaces(FILE* flin){
    int position = 0;
    fscanf(flin, "%*[ \n\t]%n", &position);
    return position;
}




int Find_inVars(char* id, int from_pos = 0){
    for(int i = from_pos; i < Var_array.size; i++){
        if( strcmp(id , Var_array.var[i].name) == 0  )
            return i;
    }
    return -1;
}

int Find_inFunk(char* id){
    for(int i =0; i < Fun_array.size; i++){
        if( strcmp(id , Fun_array.var[i].name) == 0 )
            return i;
    }
    return -1;
}

#define FIND(what) \
for (int i = 0; i < num_##what; i++){\
if (strcasecmp(fun, what [i]) == 0){\
return i;\
}}

int Find_inSystem(char* fun, char to_res = 0){
    if (!to_res)
        FIND(sys_lexems)
    else
        FIND(sys_lexems_to_res)
    return -1;
}

int Find_inEq(char* fun, char to_res = 0){
    if (!to_res)
        FIND(equals)
    else
        FIND(equals_to_res)
    return -1;
}



int Find_inOperands(char* fun, char to_res = 0){
    switch (*fun) {
        case '+':
            return OP_PLUS;
        case '-':
            return OP_MINUS;
        case '*':
            return OP_MUL;
        case '/':
            return OP_DIV;
        case '^':
            return OP_POW;
        case '(':
            return OP_OPENBRACKET;
        case ')':
            return OP_CLOSEBRACKET;
        default:
            if (!to_res)
                FIND(operators)
            else
                FIND(operators_to_res)
            return -1;
    }

}



void Set_Type(char* fun, Node* cur_node){
    double data = strtod(fun, nullptr);
    int temp = 0;
    if  (data == 0 && *fun != '0'){
        if ((temp = Find_inOperands(fun, 1)) + 1) {
            cur_node -> node_type = TYPE_OPERATOR;
            cur_node -> value = temp;
        }
        else if ((temp = Find_inSystem(fun, 1)) + 1){
            cur_node -> node_type = TYPE_SYST;
            cur_node -> value = temp;
        }
        else if (*fun == '$'){
            cur_node->node_type = TYPE_FUNK;
            int key = Find_inFunk(fun+1);
            if(!(key+1)) {
                strcpy(Fun_array.var[Fun_array.size].name, fun + 1 );
                key = Fun_array.size;
                cur_node -> value = Fun_array.size++;

            } else {
                cur_node -> value = key;
            }

            
        }
        else if (isalpha(*fun)) {
            cur_node->node_type = TYPE_VAR;
            int key = Find_inVars(fun);
            if (!(key + 1)) {
                strcpy(Var_array.var[Var_array.size].name, fun);
                cur_node->value = Var_array.size++;
            } else {
                cur_node->value = key;
            }
        }
        else {
            switch (*fun) {
                case ';':
                    cur_node -> node_type = TYPE_SYST;
                    cur_node -> value = OP_GETFUNK;
                    break;
                case ',':
                    cur_node -> node_type = TYPE_SYST;
                    cur_node -> value = OP_SENDARGS;
                    break;
                case '=':
                    if (*(fun + 1) == '='){
                        cur_node -> node_type = TYPE_EQ;
                        cur_node -> value = OP_EQUAL;
                    }
                    else {
                      printf("Wrong, equal");
                    }
                    break;
                case '!':
                    if (*(fun + 1) == '='){
                        cur_node -> node_type = TYPE_EQ;
                        cur_node -> value = OP_NOT_EQUAL;
                    }
                    else {
                        printf("Wrong, equal");
                    }
                    break;
                case '>':
                    if (*(fun + 1) == '='){
                        cur_node -> node_type = TYPE_EQ;
                        cur_node -> value = OP_MORE_EQ;
                    }
                    else{
                        cur_node -> node_type = TYPE_EQ;
                        cur_node -> value = OP_MORE;
                    }
                    break;
                case '<':
                    if (*(fun + 1) == '='){
                        cur_node -> node_type = TYPE_EQ;
                        cur_node -> value = OP_LESS_EQ;
                    }
                    else{
                        cur_node -> node_type = TYPE_EQ;
                        cur_node -> value = OP_LESS;
                    }
                    break;
                default:

                    break;
            }
        }


    }
    else {
        cur_node ->node_type = TYPE_NUM;
        cur_node->value = data;
    }
}


void Print_tree_in_file(Node* cur_node, FILE* flout){
    if (!cur_node) {
        fputc( '@', flout);
        return;
    }
    fputc( '{', flout);
    Print_the_plot(flout, cur_node);
    if( cur_node -> left || cur_node -> right) {
        Print_tree_in_file(cur_node->left, flout);
        Print_tree_in_file(cur_node->right, flout);
    }
    fputc( '}', flout);
}

Node* Create_new_branch( FILE* flin, Node* cur_node, int* length_remaining ){
    Node* side = nullptr;
    char c = 0;
    *length_remaining -= Skip_spaces(flin);
    if ((c = getc(flin)) == '{') {
        fseek(flin, -1, SEEK_CUR);
        side = Create_Node_from_file(flin, cur_node, length_remaining);

    } else if (c == '@'){

    }
    else
        fseek(flin, -1, SEEK_CUR);

    return side;
}

Node* Create_Node_from_file(FILE* flin, Node* Parent, int* length_remaining){
    *length_remaining -= Skip_spaces(flin);
    if (*length_remaining <= 0 || fgetc(flin) == '}' ){
        fseek(flin, -1, SEEK_CUR);
        return nullptr;
    }
    fseek(flin, -1, SEEK_CUR);

    if (fgetc(flin) != '{'){
        fseek(flin, -1, SEEK_CUR);
        printf("syntax error in char %ld", ftell(flin));
        return nullptr;
    }
    *length_remaining -= 1;

    Node* cur_node = (Node*)calloc(1, sizeof(Node));
    int position = 0;
    PARENT = Parent;




    *length_remaining -= Skip_spaces(flin);

    char fun[MAXLEN] = "";
    if (!fscanf(flin, "%[^{}@]%n", fun, &position)){
        fseek(flin, -1, SEEK_CUR);
        printf("syntax error in char %ld %c", ftell(flin), fgetc(flin));
    }

    if(position > MAXLEN){
        printf("Строка слишком большая! проверьте код");
        free(cur_node);
        return nullptr;
    }
    *length_remaining -= Skip_spaces(flin) + position;

    Set_Type(fun, cur_node);

    if (fgetc(flin) == '}'){
        *length_remaining -= 1;
        return cur_node;
    }
    fseek(flin, -1, SEEK_CUR);

    LEFT = Create_new_branch( flin, cur_node, length_remaining);
    RIGHT = Create_new_branch(flin, cur_node, length_remaining);

    *length_remaining -= Skip_spaces(flin);

    if (fgetc(flin) == '}'){
        *length_remaining -= 1;
        return cur_node;
    }
    else printf("syntax error");
    fseek(flin, -1, SEEK_CUR);
    return cur_node;
}

#endif //DIFF_DIFF_H
