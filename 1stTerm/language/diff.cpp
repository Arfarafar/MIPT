#include <stdio.h>
#include <diff.h>
#include <math.h>
#include <treegrap.h>
#include <assert.h>




int main() {


    FILE* fltree = fopen("diff.txt", "r+");

    Node* head_node = Tree_Construct(fltree);
    fclose(fltree);
   // Treegraph(head_node);
    char file_name[MAXLEN] = "latex.tex";
    Create_Latex(file_name);
    Append_Message(file_name, "Вот исходная функция");
    Add_Formula(head_node, file_name);

    Append_Message(file_name, "Возмем первую производную");
    Node* After_diff1 = diff(head_node, "x");
    Add_Formula(After_diff1, file_name);
    Treegraph(After_diff1);
    After_diff1 = Optimize(After_diff1, file_name);
    Treegraph(After_diff1);

    Append_Message(file_name, "Возмем вторую производную");
    Node* After_diff2 = diff(After_diff1, "x");
    Add_Formula(After_diff2, file_name);
    Treegraph(After_diff2);
    After_diff2 = Optimize(After_diff2, file_name);
    Treegraph(After_diff2);

    Close_Latex(file_name);

    fltree = fopen("diff.txt", "w");

    Print_tree_in_file(After_diff2, fltree);
    fclose(fltree);

    Deconstruct_Nodes(After_diff1);
    Deconstruct_Nodes(After_diff2);
    Deconstruct_Nodes(head_node);

    return 0;
}






