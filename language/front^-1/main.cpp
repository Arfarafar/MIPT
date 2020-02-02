#include <stdio.h>
#include <diff.h>

void Get_GetArgs(FILE* flout, Node* cur_node );
void Restore_Code(FILE* flout, Node* cur_node, char level);

int main(int argc, const char* argv[]) {

    char input[MAXLEN] = "../../../code/tree.txt";
    char output[MAXLEN] = "../../../code/restored_code.txt";

    if (argv[1]) {
        strcpy (input, argv[1]);
        Getnames(input, output, "restored_code.txt");
    }
    printf (">>>> %s: input \"%s\", output \"%s\"\n", argv[0], input, output);

    FILE* fltree = fopen(input, "r");
    Buf* buf = Create_Buf(fltree);
    Node* headnode = Create_Node_from_file(fltree, nullptr, &(buf -> len));
    Treegraph(headnode);
    fclose(fltree);
    FILE* flout = fopen(output, "wt");

    Restore_Code(flout, headnode, 0);
    return 0;
}

#define NODETYPE(what) cur_node -> node_type == what
#define NODEVALUE(what) cur_node -> value == what
void GetE(FILE* flout, Node* cur_node ){
    if(!cur_node){
        return;
    }

    switch(cur_node -> node_type){
        case TYPE_VAR:
            fprintf(flout, "%s", Var_array.var[(int)cur_node -> value].name);

            break;
        case TYPE_FUNK:
            fprintf(flout, "%s (", Fun_array.var[(int)cur_node -> value].name);
            Get_GetArgs(flout, cur_node -> left);
            fprintf(flout, ")");
            break;

        case TYPE_OPERATOR:
            switch ((int)cur_node -> value){
                case OP_MINUS:
                    GetE(flout, cur_node -> left);
                    fprintf(flout, " %s ", operators[OP_MINUS]);
                    GetE(flout, cur_node -> right);
                    break;

                case OP_PLUS:
                    GetE(flout, cur_node -> left);
                    fprintf(flout, " %s ", operators[OP_PLUS]);
                    GetE(flout, cur_node -> right);
                    break;

                case OP_MUL:
                    if(cur_node -> left -> node_type == TYPE_OPERATOR &&
                       (cur_node -> left -> value == OP_PLUS || cur_node -> left -> value == OP_MINUS) ){
                        fprintf(flout, "(");
                        GetE(flout, cur_node -> left);
                        fprintf(flout, ")");
                    }
                    else  GetE(flout, cur_node -> left);

                    fprintf(flout, " %s ", operators[OP_MUL]);

                    if(cur_node -> right -> node_type == TYPE_OPERATOR &&
                       (cur_node -> right -> value == OP_PLUS || cur_node -> right -> value == OP_MINUS) ){
                        fprintf(flout, "(");
                        GetE(flout, cur_node -> right);
                        fprintf(flout, ")");
                    }
                    else  GetE(flout, cur_node -> right);

                    break;

                case OP_DIV:
                    if (cur_node -> left->node_type != TYPE_VAR && cur_node -> left->node_type != TYPE_NUM ){
                        fprintf(flout, "(");
                        GetE(flout, cur_node -> left);
                        fprintf(flout, ")");
                    }
                    else GetE(flout, cur_node -> left);

                    fprintf(flout, " %s ", operators[OP_DIV]);

                    if (cur_node -> left->node_type != TYPE_VAR && cur_node -> left->node_type != TYPE_NUM ){
                        fprintf(flout, "(");
                        GetE(flout, cur_node -> right);
                        fprintf(flout, ")");
                    }
                    else GetE(flout, cur_node -> right);

                    break;
                case OP_SIN:
                    fprintf(flout, "sin (");
                    GetE(flout, cur_node -> right);
                    fprintf(flout, ")");

                    break;
                case OP_COS:
                    fprintf(flout, "cos (");
                    GetE(flout, cur_node -> right);
                    fprintf(flout, ")");

                    break;
                case OP_POW:
                    if (cur_node -> left->node_type != TYPE_VAR && cur_node -> left->node_type != TYPE_NUM ){
                        fprintf(flout, "(");
                        GetE(flout, cur_node -> left);
                        fprintf(flout, ")");
                    }
                    else GetE(flout, cur_node -> left);

                    fprintf(flout, " %s ", operators[OP_POW]);

                    if (cur_node -> left->node_type != TYPE_VAR && cur_node -> left->node_type != TYPE_NUM ){
                        fprintf(flout, "(");
                        GetE(flout, cur_node -> right);
                        fprintf(flout, ")");
                    }
                    else GetE(flout, cur_node -> right);

                    break;
                case OP_LOGE:
                    fprintf(flout, "loge (");
                    GetE(flout, cur_node -> right);
                    fprintf(flout, ")");

                    break;
                case OP_D:
                    fprintf(flout, "%s(", operators[OP_D]);
                    GetE(flout, cur_node -> left);
                    fprintf(flout, ") ");
                    fprintf(flout, "(");
                    GetE(flout, cur_node -> right);
                    fprintf(flout, ")");
                    break;
                default:
                    printf("GetE Unexpected operand %d %lf ",cur_node -> node_type, cur_node -> value);
                    break;
            }

            break;
        case TYPE_NUM:
            if (cur_node-> value == (int) cur_node -> value)
                fprintf(flout, "%d", (int) cur_node -> value);
            else
                fprintf(flout, "%.3lf", cur_node -> value);
            break;
        default:
            break;
    }

}

void Get_GetArgs(FILE* flout, Node* cur_node ){
    if (NODEVALUE(OP_SENDARGS) && NODETYPE(TYPE_SYST)){
        GetE(flout, cur_node -> right);
        if (cur_node -> left){
           fprintf(flout, ", ");
           Get_GetArgs(flout, cur_node -> left);
        }
    }
}

/*
void Get_GetArgs(FILE* flout, Node* cur_node ){
    if (NODETYPE(TYPE_VAR)){
        GetE(flout, cur_node);
        if (cur_node -> left){
            fprintf(flout, ", ");
            Get_GetArgs(flout, cur_node -> left);
        }
    }
}*/

void GetEq(FILE* flout, Node* cur_node){

    if (!cur_node)
        return;
    if (cur_node -> node_type == TYPE_EQ){
        switch ((int)cur_node -> value){
            case OP_MORE:
                GetE(flout, cur_node -> left);
                fprintf(flout, " %s ", equals[OP_MORE]);
                GetE(flout, cur_node -> right);
                break;
            case OP_MORE_EQ:
                GetE(flout, cur_node -> left);
                fprintf(flout, " %s ", equals[OP_MORE_EQ]);
                GetE(flout, cur_node -> right);
                break;
            case OP_LESS:
                GetE(flout, cur_node -> left);
                fprintf(flout, " %s ", equals[OP_LESS]);
                GetE(flout, cur_node -> right);
                break;
            case OP_LESS_EQ:
                GetE(flout, cur_node -> left);
                fprintf(flout, " %s ", equals[OP_LESS_EQ]);
                GetE(flout, cur_node -> right);
                break;
            case OP_EQUAL:
                GetE(flout, cur_node -> left);
                fprintf(flout, " %s ", equals[OP_EQUAL]);
                GetE(flout, cur_node -> right);
                break;
            case OP_NOT_EQUAL:
                GetE(flout, cur_node -> left);
                fprintf(flout, " %s ", equals[OP_NOT_EQUAL]);
                GetE(flout, cur_node -> right);
                break;
            default:
                printf("Unexpected operand");
                break;
        }
    }
    else
        printf("Wrong type of node");
}

void Print_tabs(FILE* flout, char temp){
    while (temp-- > 0){
        fprintf(flout, "    ");
    }
}

void Restore_Code(FILE* flout, Node* cur_node, char level){
    if (!cur_node)
        return;



    if (cur_node -> node_type ==  TYPE_SYST ){
            switch ((int)cur_node -> value){
                case OP_ASSIGN:
                    Print_tabs(flout, level);
                    GetE(flout, cur_node -> left);
                    fprintf(flout, " %s ", sys_lexems[OP_ASSIGN]);
                    GetE(flout, cur_node -> right);
                    fprintf(flout, ";\n");

                    break;
                case OP_WHILE:
                    Print_tabs(flout, level);
                    fprintf(flout, "%s (", sys_lexems[OP_WHILE]);
                    GetEq(flout, cur_node -> left);
                    fprintf(flout, ")\n");
                    Restore_Code(flout, cur_node -> right, level+1);
                    Print_tabs(flout, level);
                    fprintf(flout, "%s\n\n", sys_lexems[OP_ENDBLOCK]);

                    break;
                case OP_IF:
                    Print_tabs(flout, level);
                    fprintf(flout, "%s (", sys_lexems[OP_IF]);
                    GetEq(flout, cur_node -> left);
                    fprintf(flout, ")\n");
                    Restore_Code(flout, cur_node -> right, level);
                    break;

                case OP_IFELSE:
                    Restore_Code(flout, cur_node -> left, level+1);
                    Print_tabs(flout, level);
                    fprintf(flout, "%s\n\n", sys_lexems[OP_ENDBLOCK]);

                    if (cur_node -> right){
                        Print_tabs(flout, level);
                        fprintf(flout, "%s\n", sys_lexems[OP_ELSE]);
                        Restore_Code(flout, cur_node -> right, level+1);
                        Print_tabs(flout, level);
                        fprintf(flout, "%s\n\n", sys_lexems[OP_ENDBLOCK]);
                    }
                    break;

                case OP_RETURN:
                    Print_tabs(flout, level);
                    fprintf(flout, "%s ", sys_lexems[OP_RETURN]);
                    GetE(flout, cur_node -> left);
                    fprintf(flout, ";\n");
                    break;

                case OP_GETOP:
                    Restore_Code(flout, cur_node -> left, level);
                    Restore_Code(flout, cur_node -> right, level);
                    break;
                case OP_GET:
                    Print_tabs(flout, level);
                    fprintf(flout, "%s ", sys_lexems[OP_GET]);
                    GetE(flout, cur_node -> left);
                    fprintf(flout, ";\n");
                    break;
                case OP_PUT:
                    Print_tabs(flout, level);
                    fprintf(flout, "%s ", sys_lexems[OP_PUT]);
                    GetE(flout, cur_node -> left);
                    fprintf(flout, ";\n");
                    break;
                case OP_GETFUNK:

                    Restore_Code(flout, cur_node -> left, level);
                    Restore_Code(flout, cur_node -> right, level);
                    break;


                default:
                    printf("Unexpected operand");
                    break;
            }


    } else if (cur_node -> node_type == TYPE_FUNK){
        fprintf(flout, "%s (", Fun_array.var[(int)cur_node -> value].name);
        if(cur_node->left)
            Get_GetArgs(flout, cur_node -> left);

        fprintf(flout, ")\n");

        Restore_Code(flout, cur_node -> right, level+1);
        Print_tabs(flout, level);
        fprintf(flout, "%s\n\n", sys_lexems[OP_ENDBLOCK]);
    }


}

