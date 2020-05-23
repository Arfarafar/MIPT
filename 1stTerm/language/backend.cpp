#include <stdio.h>
#include "diff.h"

void In_Function(FILE* flout, Node* cur_node, int key);
void GetE(FILE* flout, Node* cur_node , int key);
void Walk_around_the_tree(FILE* flout, Node* cur_node);

int Global_counter = 0;
ArrayVars GVar_array = {};

int main(int argc, const char* argv[]) {

    char input[MAXLEN] = "../../../code/a.txt";
    char output[MAXLEN] = "../../../code/asmer.txt";

    if (argv[1]) {
        strcpy (input, argv[1]);
        Getnames(input, output, "asmer.txt");
    }
    printf (">>>> %s: input \"%s\", output \"%s\"\n", argv[0], input, output);

    FILE* fltree = fopen(input,"r");
    Buf* buf = Create_Buf(fltree);
    Node* head_node = Create_Node_from_file(fltree, nullptr, &(buf -> len));
    Treegraph(head_node);
    fclose(fltree);
    FILE* flout = fopen(output, "wt");

    fprintf(flout, "PUSH %d\n", Var_array.size);
    fprintf(flout, "POP bp\n");
    fprintf(flout, "JMP main\n\n");

    Walk_around_the_tree(flout, head_node);



    fclose(flout);

    Deconstruct_Nodes(head_node);
    return 0;

}

#define NODETYPE(what) cur_node -> node_type == what
#define NODEVALUE(what) cur_node -> value == what
#define FUNVAR

void Get_SendArgs(FILE* flout, Node* cur_node, int key, unsigned var_num){
    if (NODEVALUE(OP_SENDARGS) && NODETYPE(TYPE_SYST)){
        GetE(flout, cur_node -> right, key);

        fprintf(flout, "PUSH %d\n", var_num);
        fprintf(flout, "PUSH bp\n");
        fprintf(flout, "ADD\n");
        fprintf(flout, "POP dx\n");
        fprintf(flout, "POP [dx] ; get arg\n");
        if (cur_node -> left){
            Get_SendArgs(flout, cur_node -> left, key, var_num + 1);
        }

    }

}

void Get_GetArgs(FILE* flout, Node* cur_node,  int key ){
    if (NODEVALUE(OP_SENDARGS) && NODETYPE(TYPE_SYST)){
         strcpy (Fun_array.var[key].vars.var[Fun_array.var[key].vars.size].name, Var_array.var[(int) cur_node -> right -> value].name);
         cur_node -> right -> value = Fun_array.var[key].vars.size++;
        if (cur_node -> left){
            Get_GetArgs(flout, cur_node -> left, key);
        }
    }
}

int Find_in_curFun(int key, char* name){
    for(int i = 0; i < Fun_array.var[key].vars.size; i++){
        if (strcmp(name, Fun_array.var[key].vars.var[i].name) == 0){
            return i;
        }
    }
    return -1;
}

int Find_in_GV(char* id){
    for(int i = 0; i < GVar_array.size; i++){
        if( strcmp(id , GVar_array.var[i].name) == 0  )
            return i;
    }
    return -1;
}


void GetE(FILE* flout, Node* cur_node , int key){
    if(!cur_node){
        return;
    }
    int temp = 0;

    switch(cur_node -> node_type){
        case TYPE_VAR:


            if ((temp =  Find_in_curFun(key, Var_array.var[(int) cur_node->value].name)) + 1) {
                fprintf(flout, "\nPUSH %d\n", temp);
                fprintf(flout, "PUSH bp\n");
                fprintf(flout, "ADD\n");
            } else if ((temp =  Find_in_GV( Var_array.var[(int) cur_node->value].name)) + 1) {
                fprintf(flout, "PUSH %d\n", temp);
            }


            fprintf(flout, "POP dx\n");
            fprintf(flout, "PUSH [dx]; push Var\n\n\n");

            break;
        case TYPE_FUNK:
            Get_SendArgs(flout, cur_node -> left, key, Fun_array.var[key].vars.size);

            fprintf(flout, "PUSH bp\n");
            fprintf(flout, "PUSH %d\n", Fun_array.var[key].vars.size);
            fprintf(flout, "ADD\n");
            fprintf(flout, "POP bp; shift bp+\n\n");

            fprintf(flout, "CALL %s\n\n", Fun_array.var[(int)cur_node -> value].name);

            fprintf(flout, "PUSH ax\n");
            fprintf(flout, "PUSH bp\n");
            fprintf(flout, "PUSH %d\n", Fun_array.var[key].vars.size);
            fprintf(flout, "SUB\n");
            fprintf(flout, "POP bp; shift bp-\n\n");\

            break;

        case TYPE_OPERATOR:
            switch ((int)cur_node -> value){
                case OP_MINUS:
                    GetE(flout, cur_node -> left, key);
                    GetE(flout, cur_node -> right, key);
                    fprintf(flout, "SUB ; op sub\n");
                    break;

                case OP_PLUS:
                    GetE(flout, cur_node -> left, key);
                    GetE(flout, cur_node -> right, key);
                    fprintf(flout, "ADD; op add\n");
                    break;

                case OP_MUL:
                    GetE(flout, cur_node -> left, key);
                    GetE(flout, cur_node -> right, key);

                    fprintf(flout, "MUL; op mul\n");

                    break;

                case OP_DIV:
                    GetE(flout, cur_node -> left, key);
                    GetE(flout, cur_node -> right, key);

                    fprintf(flout, "DIV; op div\n");
                    break;
                case OP_SIN:

                    GetE(flout, cur_node -> right, key);
                    fprintf(flout, "SIN\n");

                    break;
                case OP_COS:

                    GetE(flout, cur_node -> right, key);
                    fprintf(flout, "COS\n");

                    break;
                case OP_POW:
                    GetE(flout, cur_node -> left, key);
                    GetE(flout, cur_node -> right, key);
                    fprintf(flout, "POW\n");
                    break;
                case OP_LOGE:
                    GetE(flout, cur_node -> right, key);
                    fprintf(flout, "LOG_E\n");

                    break;

                default:
                    printf("GetE Unexpected operand %d %lf ",cur_node -> node_type, cur_node -> value);
                    break;
            }

            break;
        case TYPE_NUM:
            if (cur_node-> value == (int) cur_node -> value)
                fprintf(flout, "PUSH %d; num\n", (int) cur_node -> value);
            else
                fprintf(flout, "PUSH %.3lf; num\n", cur_node -> value);
            break;
        default:
            break;
    }

}

void GetEq(FILE* flout, Node* cur_node, int key, char* label_name){

    if (!cur_node)
        return;
    if (cur_node -> node_type == TYPE_EQ){ // inversive
        switch ((int)cur_node -> value){
            case OP_MORE:
                GetE(flout, cur_node -> left, key);
                GetE(flout, cur_node -> right, key);
                fprintf(flout, "JBE %s\n", label_name);
                break;
            case OP_MORE_EQ:
                GetE(flout, cur_node -> left, key);
                GetE(flout, cur_node -> right, key);
                fprintf(flout, "JB %s\n", label_name);
                break;
            case OP_LESS:
                GetE(flout, cur_node -> left, key);
                GetE(flout, cur_node -> right, key);
                fprintf(flout, "JAE %s\n", label_name);
                break;
            case OP_LESS_EQ:
                GetE(flout, cur_node -> left, key);
                GetE(flout, cur_node -> right, key);
                fprintf(flout, "JA %s\n", label_name);
                break;
            case OP_EQUAL:
                GetE(flout, cur_node -> left, key);
                GetE(flout, cur_node -> right, key);
                fprintf(flout, "JNE %s\n", label_name);
                break;
            case OP_NOT_EQUAL:
                GetE(flout, cur_node -> left, key);
                GetE(flout, cur_node -> right, key);
                fprintf(flout, "JE %s\n", label_name);
                break;
            default:
                printf("Unexpected operand");
                break;
        }
    }
    else
        printf("Wrong type of node");
}

void GetIF(FILE* flout, Node* cur_node, int key){
    char label_st[MAXLEN] = "";
    char label_end[MAXLEN] = "";
    int level = Global_counter++;

    sprintf(label_st, "ELSE_%d_%s", level, Fun_array.var[key].name);
    GetEq(flout, cur_node->left, key, label_st);

    cur_node = cur_node->right;



    In_Function(flout, cur_node->left, key);
    sprintf(label_end, "IFEND_%d_%s", level, Fun_array.var[key].name);
    fprintf(flout, "JMP %s\n\n", label_end);
    fprintf(flout, "%s:\n\n", label_st);

    if (cur_node->right) {
        In_Function(flout, cur_node->right, key);
    }

    fprintf(flout, "%s:\n\n", label_end);

}

void GetWH(FILE* flout, Node* cur_node, int key){
    char label_st[MAXLEN] = "";
    char label_end[MAXLEN] = "";
    int level = Global_counter++;


    sprintf(label_st, "STARTWH_%d_%s", level, Fun_array.var[key].name);
    fprintf(flout, "%s:\n\n", label_st);
    sprintf(label_end, "ENDWH_%d_%s", level, Fun_array.var[key].name);
    GetEq(flout, cur_node->left, key, label_end);

    In_Function(flout, cur_node->right, key);


    fprintf(flout, "JMP %s\n\n", label_st);
    fprintf(flout, "%s:\n\n", label_end);
}

void In_Function(FILE* flout, Node* cur_node, int key){
    if (!cur_node)
        return;


    int temp = 0;


    if (cur_node->node_type == TYPE_SYST) {
        switch ((int) cur_node->value) {
            case OP_ASSIGN:
                fprintf(flout, "PUSH bx\n");
                GetE(flout, cur_node -> right  , key);

                if ((temp =  Find_in_curFun(key, Var_array.var[(int) cur_node->left->value].name)) + 1) {
                    fprintf(flout, "PUSH %d\n", temp);
                }
                else {
                    strcpy(Fun_array.var[key].vars.var[Fun_array.var[key].vars.size].name, Var_array.var[(int) cur_node->left->value].name);
                    fprintf(flout, "PUSH %d\n", Fun_array.var[key].vars.size++);
                }

                fprintf(flout, "PUSH bp\n");
                fprintf(flout, "ADD\n");
                fprintf(flout, "POP bx\n");
                fprintf(flout, "POP [bx]\n");
                fprintf(flout, "POP bx; get assign\n\n\n");
                break;

            case OP_WHILE:
                GetWH(flout, cur_node, key);
                break;

            case OP_IF:
                GetIF(flout, cur_node, key);
                break;


            case OP_RETURN:

                GetE(flout, cur_node->left, key);
                fprintf(flout, "POP ax\n");
                fprintf(flout, "RET\n\n");
                break;

            case OP_GETOP:
                In_Function(flout, cur_node->left, key);
                In_Function(flout, cur_node->right, key);
                break;

            case OP_GET:
                if ((temp =  Find_in_curFun(key, Var_array.var[(int) cur_node->left->value].name)) + 1) {
                    fprintf(flout, "PUSH %d\n", temp);
                }
                else {
                    strcpy(Fun_array.var[key].vars.var[Fun_array.var[key].vars.size].name, Var_array.var[(int) cur_node->left->value].name);
                    fprintf(flout, "PUSH %d\n", Fun_array.var[key].vars.size++);
                }

                fprintf(flout, "PUSH bp\n");
                fprintf(flout, "ADD\n");
                fprintf(flout, "POP bx\n");
                fprintf(flout, "IN [bx]\n\n\n");
                break;
            case OP_PUT:

                    GetE(flout, cur_node -> left, key);
                    fprintf(flout, "OUT\n");


                break;

            default:
                printf("Unexpected operand");
                break;
        }
    }
}


void Walk_around_the_tree(FILE* flout, Node* cur_node) {
    if (!cur_node)
        return;

    int temp = 0;

    if (cur_node->node_type == TYPE_SYST) {
        switch ((int) cur_node->value) {
            case OP_ASSIGN:
                fprintf(flout, "PUSH bx\n");
                if (cur_node -> right -> value == (int) cur_node -> value)
                    fprintf(flout, "PUSH %d; num\n", (int) cur_node -> value);
                else
                    fprintf(flout, "PUSH %.3lf; num\n", cur_node -> value);

                if ((temp =  Find_in_GV( Var_array.var[(int) cur_node->value].name)) + 1) {
                    fprintf(flout, "PUSH %d\n", temp);
                }
                else {
                    strcpy(GVar_array.var[GVar_array.size].name, Var_array.var[(int) cur_node->left->value].name);
                    fprintf(flout, "PUSH %d\n", GVar_array.size++);
                }


                fprintf(flout, "PUSH bp\n");

                fprintf(flout, "ADD\n");
                fprintf(flout, "POP bx\n");
                fprintf(flout, "POP [bx]\n");
                fprintf(flout, "POP bx\n\n\n");
                break; 

            case OP_GETFUNK:

                Walk_around_the_tree(flout, cur_node->left) ;
                Walk_around_the_tree(flout, cur_node->right);
                break;


            default:
                printf("Unexpected operand");
                break;
        }


    } else if (cur_node->node_type == TYPE_FUNK) {
        fprintf(flout, "%s:\n", Fun_array.var[(int) cur_node->value].name);
        if (cur_node->left)
            Get_GetArgs(flout, cur_node->left, (int) cur_node->value);
        In_Function(flout, cur_node->right, (int) cur_node->value);
        fprintf(flout, "END\n");
        }

       fprintf(flout, "\n");
    }

