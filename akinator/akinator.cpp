#include <cstdio>
#include <list.h>
#include <Stack.h>
#include <TXlib/TX/TXLib.h>  // x ^=~ 1  ^=!    (0)^(0)
#define MAXLEN 256

struct Node{
    Elem_t data;
    Node* parent;

    Node* left;
    Node* right;

};

struct Tree{
    Node* head_node;
    List* final_elems;
};


Node* Create_Node(FILE* fltree, List *finalelems, Node* Parent, int *length_remaining);
Node* Create_new_branch(FILE* fltree, List* finalelems, Node* cur_node, int* length_remaining );
Tree* Tree_Construct(FILE* flin);
void Tree_Destruct(Tree* tree);
void Print_tree_in_file(Node* cur_node, FILE* flout);
void Treegraph(Tree* tree);
void Play(Node* cur_node, List* answers);
void Print_character(List* list, char* player_answer);
void Comp_Character(List* list, char* first_note, char* second_note);

int main() {
  //  SetConsoleCP(1251);
//    SetConsoleOutputCP(1251);
    char str[256] = "";

    FILE* fltree = fopen("tree.txt", "r+");


    Tree* tree = Tree_Construct(fltree);
    if (!tree){
        printf("Empty file");
    }


    char* first_note = (char*)calloc(1, MAXLEN);
    char* second_note = (char*)calloc(1, MAXLEN);
    int action = 0;
    while (true){
        printf("Выбирай: 1. Играть 2. Вывести характеристику 3. Сравнить 2 характеристики\n\t 4. Увидеть дерево 5. Увидеть все ответы 6. Закончить\n");
        scanf("%d", &action);
        switch (action){
            case 1: Play(tree->head_node, tree->final_elems);
                break;
            case 2:
                printf("Кого именно ты хочешь выбрать? \n");
                scanf("%s", first_note);
                Print_character(tree -> final_elems, first_note);
                break;
            case 3:
                printf("Кого именно ты хочешь выбрать? \n");
                scanf("%s %s", first_note, second_note);
                Comp_Character(tree -> final_elems, first_note, second_note);
                break;
            case 4: Treegraph(tree);
                break;
            case 5: Diigraph(tree -> final_elems);
                break;
            case 6: fseek(fltree, 0, SEEK_SET);
                    Print_tree_in_file(tree -> head_node, fltree);
                    fclose(fltree);
                    Tree_Destruct(tree);
                    free(first_note);
                    free(second_note);
                return 0;
            default: printf("Нет такого варианта давай еще раз\n");
                break;
        }
    }


    return 0;
}

void Fill_Stek(stek_t* stek, Node* cur_node, char* escape_sequence){
    while (cur_node -> parent){
        if (cur_node -> parent -> left == cur_node)
            *(escape_sequence + stek -> size) = 'l';
        else  *(escape_sequence + stek -> size) = 'r';

        Stack_Push(stek, cur_node -> data);
        cur_node = cur_node -> parent;
    }
    Stack_Push(stek, cur_node -> data);
}

stek_t* Create_Stack_for_Character(List* list, char* player_answer, char* escape_seq){
    int ans = Find_By_Value(list, player_answer);
    if (!ans){
        printf("Нет такого варианта давай еще раз\n");
        return nullptr;
    }
    Node* cur_node = (Node*)(list -> cells[ans].value);
    stek_t* stek = Stack_Construct(32, "answers");
    Fill_Stek(stek, cur_node, escape_seq);
    return  stek;
}

void Print_Stek(stek_t* stek, char* escape_sequence, int kol){
    while (stek->size > 1 && kol > 0){
        if (*(escape_sequence + stek->size - 2) ==  'r'){
            printf("NE ");
        }
        fprintf(stdout, "%s", Stack_Pop(stek));
        printf("\b. ");
        kol--;
    }
}

void  Comp_Character(List* list, char* first_note, char* second_note){
    printf("%s и %s похожи тем, что оба ", first_note, second_note );
    char first_escape_sequence[MAXLEN] = {};
    char second_escape_sequence[MAXLEN] = {};
    stek_t* first_st = Create_Stack_for_Character(list, first_note, first_escape_sequence);
    stek_t* second_st = Create_Stack_for_Character(list, second_note, second_escape_sequence);

    while (Compare_Stack_First_Elements(first_st, second_st) && Compare_Stack_Second_Elements(first_st, second_st)){
        Print_Stek(first_st, first_escape_sequence, 1);
        Stack_Pop(second_st);
    }
    printf(" и различны тем, что %s ", first_note );
    Print_Stek(first_st, first_escape_sequence, first_st->size);
    printf(" а %s ", second_note );
    Print_Stek(second_st, second_escape_sequence, second_st->size);
    putchar('\n');
}

void Print_character(List* list, char* player_answer){
    stek_t* stek = Create_Stack_for_Character(list, player_answer, player_answer);
    if(!stek){
        return;
    }
    Print_Stek(stek, player_answer, stek -> size); // player_answer is already an escape_sequence
     putchar('\n');
     Stack_Distruct(stek);
}


void questionnode(Node* cur_node, List* answers){
    putchar('\n');
    char player_answer[MAXLEN] = {};

    scanf("%s", player_answer);

    if (stricmp(player_answer, "yes") == 0){
        Play(cur_node->left, answers);
    }
    else if (stricmp(player_answer, "no") == 0){
        Play(cur_node->right, answers);
    }
    else {
        printf("Попробуйте еще раз\n");
        Play(cur_node, answers);
    }
}


Node* Create_new_Node(char* buf, Node* parent){
    Node* newNode = (Node*)calloc(sizeof(Node), 1);
    newNode -> parent = parent;
    newNode -> data = buf;
    return newNode;
}

void Add_New_Node(Node* cur_node, List* answers){
    char player_answer[MAXLEN] = {};
    char newquest[MAXLEN] ={};

    printf("Что это было за слово?\n");
    scanf("%s", player_answer);
    if (Find_By_Value( answers, player_answer)){
        printf("Такое слово уже было вот его характеристика:\n");
        Print_character(answers, player_answer);
        return;
    }
    fflush(stdin);
    int pos = 0;
    printf("Чем %s отличается от %s?\n", player_answer, cur_node -> data);
    scanf("%[^\n]%n", newquest, &pos );
    *(newquest + pos) = '?';

        Delete_This(answers, Find_By_Value(answers, cur_node->data));

        cur_node -> left = Create_new_Node(strdup(player_answer), cur_node);

        cur_node -> right = Create_new_Node(cur_node -> data, cur_node);

        cur_node -> data = strdup(newquest);

}

void answernote(Node* cur_node, List* answers){
    printf("?\n");
    char player_answer[MAXLEN] = "";

    scanf("%s", player_answer);
    if (stricmp(player_answer, "yes") == 0){
        printf("Слово отгадано\n");
    }
    else if (stricmp(player_answer, "no") == 0){
        Add_New_Node(cur_node, answers);
    }
    else {
        printf("Попробуйте еще раз\n");
        Play(cur_node, answers);
    }
}

bool isitquestoin(Node* cur_node){
    if (!cur_node -> left)
        return false;
    return true;
}

void Play(Node* cur_node, List* answers){

    printf("%s", cur_node->data);
    isitquestoin(cur_node) ? questionnode(cur_node, answers) : answernote(cur_node, answers);

}

int getFileLength (FILE* flin){
    fseek(flin, 0, SEEK_END);
    int length = ftell(flin);
    fseek(flin, 0 , SEEK_SET);
    return  length;
}


Tree* Tree_Construct(FILE* flin) {

    Tree *tree = (Tree *) calloc(1, sizeof(Tree));

    tree->final_elems = Create_List(2);

    int length_remaining = getFileLength(flin);

    tree->head_node = Create_Node(flin, tree->final_elems, nullptr, &length_remaining);
    if(!length_remaining){
        return tree;
    } else{
        Tree_Destruct(tree);
        return nullptr;
    }

}

void Deconstruct_Nodes(Node* cur_node){
    if(!cur_node){
        return;
    }
    free(cur_node -> data);
    Deconstruct_Nodes(cur_node->left);
    Deconstruct_Nodes(cur_node->right);
    free(cur_node);
}

void Tree_Destruct(Tree* tree){
    Deconstruct_Nodes(tree->head_node);
    Deconstruct_List(tree -> final_elems);
    free(tree);
}

int Skip_spaces(FILE* flin){
    int position = 0;
    fscanf(flin, "%*[^{}\"n]%n", &position);
    return position;
}


Node* Create_new_branch(FILE* flin, List* finalelems, Node* cur_node, int* length_remaining ){
    Node* side = nullptr;

    *length_remaining -= Skip_spaces(flin);
        if (getc(flin) == '{') {
            fseek(flin, -1, SEEK_CUR);
            side = Create_Node(flin, finalelems, cur_node, length_remaining);

        } else  printf("syntax error in char %ld", ftell(flin));

    return side;
}

Node* Create_Node(FILE* flin, List* finalelems, Node* Parent, int* length_remaining){
    *length_remaining -= Skip_spaces(flin);

    if (*length_remaining <= 0){
        return nullptr;
    }

    if (getc(flin) != '{'){
        printf("syntax error in char %ld", ftell(flin));
        return nullptr;
    }

    *length_remaining -= Skip_spaces(flin) + 1; // +1 ot getc

    Node* cur_node = (Node*)calloc(1, sizeof(Node));
    int position = 0;
    cur_node -> parent = Parent;

    char string[MAXLEN] = "";

    fscanf(flin, "\"%[^\"]\"%n", string, &position);
    if(position > MAXLEN){
        printf("Строка слишком большая! проверьте код");
        free(cur_node);
        return nullptr;
    }

    cur_node -> data = strdup(string);

    *length_remaining -= Skip_spaces(flin) + position;

    if (getc(flin) == '}'){
        cur_node->right = nullptr;
        cur_node->left = nullptr;
        *length_remaining -= 1;
       // if(!Find_By_Value(finalelems, cur_node -> data))
            Insert_Tail(finalelems, ((char*)cur_node));
        return cur_node;
    }
    fseek(flin, -1, SEEK_CUR);
    cur_node -> left = Create_new_branch(flin, finalelems, cur_node, length_remaining);
    cur_node -> right = Create_new_branch(flin, finalelems, cur_node, length_remaining);

    *length_remaining -= Skip_spaces(flin);
    if (getc(flin) == '}'){
        *length_remaining -= 1;
        return cur_node;
    }
    else  printf("syntax error in char %ld", ftell(flin));
    return cur_node;
}

void Print_tree_in_file(Node* cur_node, FILE* flout){
    if (!cur_node)
        return;
    fputc( '{', flout);
    fputc( '\"', flout);
    fprintf(flout,"%s", cur_node -> data);
    fputc( '\"', flout);
    Print_tree_in_file(cur_node->left, flout);
    Print_tree_in_file(cur_node->right, flout);
    fputc( '}', flout);
}

void Print_graphnode(FILE* flout, Node* cur_node, int level){

    if(!cur_node){
        return;
    }
    fprintf(flout, "\"%p\" [label = \"",  cur_node);
    fprintf(flout,"%s", cur_node -> data);
    fprintf(flout, "\" ];\n");
    fprintf(flout, " { rank = %d; \"%p\"}\n", level, cur_node);
    Print_graphnode(flout, cur_node->left, level + 1);
    Print_graphnode(flout, cur_node->right, level + 1);
    if (cur_node->left)
        fprintf(flout, "\"%p\" -> \"%p\" [label = \"yes\"; arrowhead=\"vee\"];\n", cur_node, cur_node-> left);

    if(cur_node->right)
        fprintf(flout, "\"%p\" -> \"%p\" [label = \"no\"; arrowhead=\"vee\"];\n", cur_node, cur_node-> right);

}

void Treegraph(Tree* tree){
    FILE* flout = fopen("tree.dot", "wt");
    fprintf(flout, "digraph G{\n"
                   "rankdir=TB; \n"
                   "node[color=\"goldenrod\",fontsize= 13];\n");
    Print_graphnode(flout, tree->head_node, 1);
    fprintf(flout, " }\n");
    fclose(flout);
    system("del tree.dot.png            && "
           DOT_PATH " -Tpng -O tree.dot && "
           "start /wait tree.dot.png    && "
           "pause");

}
