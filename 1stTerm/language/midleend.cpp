#include <stdio.h>
#include <diff.h>

void Finddiff(Node* cur_node, char* fname);

int main(int argc, const char* argv[]) {

    char input[MAXLEN] = R"(C:\Users\andrew\Desktop\prog\tree.txt)";
    char output[MAXLEN] = "../../../code/a.txt";

    if (argv[1]) {
        strcpy (input, argv[1]);
        Getnames(input, output, "a.txt");
    }
    printf (">>>> %s: input \"%s\", output \"%s\"\n", argv[0], input, output);

    FILE* fltree = fopen(input ,"r");
    Buf* buf = Create_Buf(fltree);
    Node* head_node = Create_Node_from_file(fltree, nullptr, &(buf -> len));
    //Treegraph(head_node);
    fclose(fltree);
    FILE* flout = fopen(output, "wt");

    char file_name[MAXLEN] = R"(C:\Users\andrew\Desktop\prog\latex.tex)";
    Create_Latex(file_name);
    Finddiff(head_node, file_name);
    Close_Latex(file_name);
    Treegraph(head_node);

    Print_tree_in_file(head_node, flout);
    fclose(flout);
    Deconstruct_Nodes(head_node);
    return 0;

}


void Finddiff(Node* cur_node, char* fname){
    if (!cur_node)
        return;

    if (cur_node -> node_type == TYPE_OPERATOR && cur_node->value == OP_D){
        Append_Message(fname, "Вот исходная функция");
        Add_Formula(cur_node -> right, fname);
        Append_Message(fname, "Возмем производную");
        Node* new_node = diff(cur_node -> right, Var_array.var[(int)cur_node->left->value].name ) ;
        new_node = Optimize(new_node, fname);

        Node* temp = cur_node -> parent;
        Deconstruct_Nodes(cur_node);
        if(!temp -> right){
            temp->right = new_node;
        } else if(!temp -> left)
            temp -> left = new_node;
        else {
            printf("ERRRORRR!");
            abort();
        }
        return;
    }
    Finddiff(cur_node -> right, fname);
    Finddiff(cur_node -> left, fname);
}