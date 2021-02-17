#include "types.h"
#include "stdio.h"

void Print_tree(FILE* flout, Node* cur_node, int level){

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



void Diigraph(AVL_Tree* tree, const char* name){

    if(!tree){
        return;
    }

    FILE* flin = fopen(name, "wt");
    fprintf(flin, "digraph G{\n"
                  "rankdir=TB;\n"
                  "node[color=\"goldenrod\",fontsize=13];\n");
    Print_tree(flin, tree -> root, 1);
    fprintf(flin, " }\n");
    fclose(flin);
    

}