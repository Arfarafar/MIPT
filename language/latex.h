//
// Created by andrew on 29.11.2019.
//

#ifndef DIFF_LATEX_H
#define DIFF_LATEX_H

#define TEX_PATH "C:/Users/andrew/AppData/Local/Programs/MiKTeX 2.9/miktex/bin/x64/miktex-texworks.exe"

#include <treegrap.h>
#include <diff.h>

char common_phrases[][MAXLEN] = {"Легко видеть, что", "Без лишних слов понятно, что",
                         "Если вам не очевиден следущий переход, встаньте выйдете из кабинета",
                        "Далее, путем нехитрых преобразований, получаем", "Очевидно, что",
                        "Положим что звезда Проксима-Центавра b недавно завершила свой цикл и вспыхнула, тогда",
                        "Думаю, без лишних объяснений ясно, что это преобразуется в следущий вид",
                        "Чисто для понимания и без сотрудников Кафедры Матанализа, что это равно",
                        "Как вы уже догадались это преобразуется в",
                        "Не нужно быть семи пядей во лбу чтобы понять переход к",
                        "Путем нехитрого проецирования всей видимой вселенной на плоскость альфа заметим, что это равно",
                        "Следущий переход не вызывает вопросов",
                        "Я вижу ваше негодование от столь подробных пояснений, но я хочу показать все детально",
                        "Думаю, что образ прообраза образа вектора в н-мерное пространство и обратно без труда позволит увидеть, что",
                        "Итак, получаем следущее"};
const int Common_phrase_number = sizeof(common_phrases) / (sizeof(char)*MAXLEN) ;


void Latex_dump( FILE*, Node*);

int Create_Latex(const char* file_name) {
    assert(file_name);

    FILE* flout = fopen(file_name, "w");

    fprintf(flout, "\\documentclass[12pt]{article}\n"
                   "\\usepackage[utf8]{inputenc}\n"
                   "\\usepackage[russian]{babel}\n"
                   "\\usepackage{setspace,amsmath}\n"
                   "\\begin{document}\n\n\n"
                   " {\\bf Взятие производных \n}\n\n");


    fclose(flout);
    return 1;
}

void Close_Latex(const char* file_name){


    FILE* flout = fopen(file_name, "a");
    assert(flout);

    fprintf(flout, "\n\n\n\\end{document}\n");
    fclose(flout);
    //system( "start " TEX_PATH );
   /* system("rm latex.log");
    system("rm latex.aux"); */

}

int Add_Formula(Node* cur_node, const char* file_name) {
    assert(cur_node);
    assert(file_name);

    FILE* flout = fopen(file_name, "a") ;

    fprintf(flout, "$$");
    if (cur_node -> node_type == TYPE_OPERATOR && cur_node -> value == OP_MINUS && cur_node -> left -> node_type == TYPE_NUM && cur_node -> left -> value == 0){
        Print_the_plot(flout, cur_node, 1);
        cur_node = cur_node -> right;
    }
    Latex_dump(flout, cur_node);
    fprintf(flout, "$$\n");

    fclose(flout);
    return 1;
}

int Append_Message(const char* file_name, const char* message) {
    assert(message);
    assert(file_name);

    FILE* output = fopen(file_name, "a");

    fprintf(output, "%s\n", message);

    fclose(output);
    return 1;
}


void Latex_dump( FILE* flout, Node* cur_node) {
    assert(cur_node);
    assert(flout);

    if (cur_node->left != nullptr) {
        if (cur_node -> value == OP_MUL &&
            (cur_node -> left -> value == OP_PLUS || cur_node->left->value == OP_MINUS)) {
            fprintf(flout, "(");
            Latex_dump(flout, cur_node->left);
            fprintf(flout, ")");
        } else if (cur_node-> value == OP_POW && (cur_node->left->node_type == TYPE_OPERATOR) ) {
            fprintf(flout, "(");
            Latex_dump(flout, cur_node->left);
            fprintf(flout, ")");
        } else if (cur_node-> value == OP_DIV) {
            fprintf(flout, "\\frac{");
            Latex_dump(flout, cur_node->left);
            fprintf(flout, "}");
        } else
            Latex_dump(flout, cur_node->left);
    }

    Print_the_plot(flout, cur_node, 1);

    if (cur_node->right != nullptr) {
        if (cur_node-> value == OP_MUL &&
            (cur_node->right->  value == OP_PLUS || cur_node->right-> value == OP_MINUS)) {
            fprintf(flout, "(");
            Latex_dump(flout, cur_node->right);
            fprintf(flout, ")");

        } else if (cur_node-> value == OP_POW ) {
            fprintf(flout, "{");
            Latex_dump(flout, cur_node->right);
            fprintf(flout, "}");

        } else if (cur_node->value == OP_DIV) {
            fprintf(flout, "{");
            Latex_dump(flout, cur_node->right);
            fprintf(flout, "}");
        } else if ((cur_node->node_type == TYPE_OPERATOR) && (cur_node -> value < 20)) {
            fprintf(flout, "(");
            Latex_dump(flout, cur_node -> right);
            fprintf(flout, ")");
        } else
            Latex_dump(flout, cur_node->right);
    }
}
#endif //DIFF_LATEX_H
