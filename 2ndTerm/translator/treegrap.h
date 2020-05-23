//
// Created by andrew on 17.11.2019.
//

#ifndef AKINATOR_TREEGRAP_H
#define AKINATOR_TREEGRAP_H
#define DOT_PATH  "C:/graphviz/release/bin/dot.exe"

#include "diff.h"
#include <stdlib.h>
template <typename T>
void Print_the_plot(FILE* flout, T* cur_node, char forlatex = 0){
        switch(cur_node -> node_type){
            case TYPE_VAR:
                fprintf(flout, "%s", Var_array.var[(int)cur_node -> value].name);
            break;
            case TYPE_FUNK:
                fprintf(flout, "$%s", Fun_array.var[(int)cur_node -> value].name);
                break;
            case TYPE_EQ:
                switch ((int)cur_node -> value){
                    case OP_MORE:
                        fprintf(flout, ">");   break;
                    case OP_MORE_EQ:
                        fprintf(flout, ">=");   break;
                    case OP_LESS:
                        fprintf(flout, "<");   break;
                    case OP_LESS_EQ:
                        fprintf(flout, "<=");   break;
                    case OP_EQUAL:
                        fprintf(flout, "==");   break;
                    case OP_NOT_EQUAL:
                        fprintf(flout, "!=");   break;
                    default:
                        printf("Unexpected operand");
                        break;
                }
                break;
            case TYPE_SYST :
                switch ((int)cur_node -> value){
                    case OP_ASSIGN:
                        fprintf(flout, "=");   break;
                    case OP_WHILE:
                        fprintf(flout, "while");   break;
                    case OP_IF:
                        fprintf(flout, "if");   break;
                    case OP_ELSE:
                        fprintf(flout, "else");   break;
                    case OP_ENDBLOCK:
                        fprintf(flout, "end");   break;
                    case OP_IFELSE:
                        fprintf(flout, "if-else");   break;
                    case OP_RETURN:
                        fprintf(flout, "ret");   break;
                    case OP_GETOP:
                        fprintf(flout, "op");   break;
                    case OP_GET:
                        fprintf(flout, "get");   break;
                    case OP_PUT:
                        fprintf(flout, "put");   break;
                    case OP_GETFUNK:
                        fprintf(flout, ";");   break;
                    case OP_SENDARGS:
                        fprintf(flout, ",");   break;
                    case OP_GLOBALEND:
                        fprintf(flout, "Global end"); break;

                    default:
                        printf("Unexpected operand");
                        break;
                }
                break;
            case TYPE_OPERATOR:
                switch ((int)cur_node -> value){
                    case OP_MINUS:
                        fprintf(flout, "-");   break;
                    case OP_PLUS:
                        fprintf(flout, "+");   break;
                    case OP_MUL:
                        fprintf(flout, forlatex ? "\\cdot " : "*");  break;
                    case OP_DIV:
                        if (forlatex)
                            break;
                        fprintf(flout, "/");  break;
                    case OP_SIN:
                        fprintf(flout, "sin");  break;
                    case OP_COS:
                        fprintf(flout, "cos");  break;
                    case OP_POW:
                        fprintf(flout, "^");  break;
                    case OP_LOGE:
                        fprintf(flout, "loge");  break;
                    case OP_D:
                        fprintf(flout, "deriv");   break;
                    case OP_OPENBRACKET:
                        fprintf(flout, "(");   break;
                    case OP_CLOSEBRACKET:
                        fprintf(flout, ")");   break;

                    default:
                        printf("Print the plot Unexpected operand %d %lf ",cur_node -> node_type, cur_node -> value);
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
};

template <typename T>
int Print_phrase(FILE* flout, T* cur_node, bool fordot = false) {
    if (fordot){
        switch(cur_node -> node_type) {
            case TYPE_VAR:
                fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#A5EFDE\", ", cur_node);
                break;
            case TYPE_NUM:
                fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#CDE1ED\", ", cur_node);
                break;
            case TYPE_OPERATOR:
                fprintf(flout, "\"%p\"[ shape=\"rectangle\", ", cur_node);
                break;
            case TYPE_SYST:
                fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#FFADAD\", ", cur_node);
                break;
            case TYPE_FUNK:
                fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#FF3455\", ", cur_node);
                break;
            case TYPE_EQ :
                fprintf(flout, "\"%p\"[shape=\"rectangle\", style=\"filled\", fillcolor=\"#A2A2FF\", ", cur_node);
                break;
            default:
                return 0;
        }
        //fprintf(flout, " label = \" %d | ", cur_node -> pos_instr);
        fprintf(flout, " label = \"");
    }
    Print_the_plot(flout, cur_node);
    if (fordot) {
        fprintf(flout, "\" ];\n");
    }
    return cur_node -> node_type;
}



void Print_graphnode(FILE* flout, Node* cur_node, int level){

    if(!cur_node){
        return;
    }
    Print_phrase(flout, cur_node, 1);

    fprintf(flout, " { rank = %d; \"%p\"}\n", level, cur_node);
    Print_graphnode(flout, cur_node->left, level + 1);
    Print_graphnode(flout, cur_node->right, level + 1);
    if (cur_node->left)
        fprintf(flout, "\"%p\" -> \"%p\" [ arrowhead=\"vee\" , label = \" l \"];\n", cur_node, cur_node-> left);
       // fprintf(flout, "\"%p\" -> \"%p\" [ arrowhead=\"vee\" ];\n", cur_node, cur_node-> left);

    if(cur_node->right)
        fprintf(flout, "\"%p\" -> \"%p\" [arrowhead=\"vee\",  label = \" r \"];\n", cur_node, cur_node-> right);
        //fprintf(flout, "\"%p\" -> \"%p\" [arrowhead=\"vee\"];\n", cur_node, cur_node-> right);

 if (cur_node -> parent)
     fprintf(flout, "\"%p\" -> \"%p\" [arrowhead=\"vee\"];\n", cur_node, cur_node-> parent);
}

void Treegraph(Node* tree){
    FILE* flout = fopen("tree.dot", "wt");
    fprintf(flout, "digraph G{\n"
                   "rankdir=TB; \n"
                   "node[color=\"goldenrod\",fontsize= 13];\n");

    Print_graphnode(flout, tree, 1);
    fprintf(flout, " }\n");
    fclose(flout);
    system("del tree.dot.png            && "
    DOT_PATH " -Tpng -O tree.dot && "
             "start /wait tree.dot.png    && "
             "pause");

}

void Print_tokennode(FILE* flout, Token* cur_token){

    Print_phrase(flout, cur_token, 1);
    if(cur_token -> value == OP_GLOBALEND && cur_token -> node_type == TYPE_SYST){
        return;
    }
    Print_tokennode(flout, (cur_token + 1));

        fprintf(flout, "\"%p\" -> \"%p\" [ arrowhead=\"vee\"];\n", cur_token, (cur_token+1));

}

void Tokengraph(Token* token){
    FILE* flout = fopen("token.dot", "wt");
    fprintf(flout, "digraph G{\n"
                   "rankdir=LR; \n"
                   "node[color=\"goldenrod\",fontsize= 13];\n");

    Print_tokennode(flout, token);
    fprintf(flout, " }\n");
    fclose(flout);
    system("del token.dot.png            && "
           DOT_PATH " -Tpng -O token.dot && "
           "start /wait token.dot.png    && "
           "pause");

}
#endif //AKINATOR_TREEGRAP_H
