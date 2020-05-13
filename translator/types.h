//
// Created by andrew on 04.12.2019.
//

#ifndef FRONTEND_TYPES_H
#define FRONTEND_TYPES_H
#include <math.h>
#include <assert.h>
#include <cstring>

enum node_types{
    TYPE_NUM = 0,
    TYPE_OPERATOR = 1,
    TYPE_VAR = 2,
    TYPE_FUNK = 3,
    TYPE_SYST = 4,
    TYPE_EQ = 5
};

#define MAXLEN 256

enum equal{
    OP_EQUAL = 0,
    OP_NOT_EQUAL,
    OP_MORE ,
    OP_LESS  ,
    OP_LESS_EQ ,
    OP_MORE_EQ
};

char equals[][MAXLEN] = //my
        {"smels_like", "doesnt_smells_like", "yummier", "more_disgusting", "prettier", "not_so_pleasant_like"};
char equals_to_res[][MAXLEN] = //standart
        {"==", "!=", ">", "<", ">=", "<="};

int num_equals = sizeof(equals) / MAXLEN;
int num_equals_to_res = sizeof(equals_to_res) / MAXLEN;



enum system {
                            OP_ASSIGN = 0, OP_GET, OP_PUT, OP_WHILE , OP_IF, OP_FOR, OP_ELSE, OP_RETURN, OP_ENDBLOCK, OP_IFELSE, OP_GETOP, OP_SENDARGS, OP_GETFUNK, OP_GLOBALEND
};
char sys_lexems[][MAXLEN] = { "meow",      "eat" , "kkxxr_on_carpet",  "sleep_while", "snuf-snuf",   "for",  "door_is_open_but_we_are_standing_in_front_of_it",  "bring_prey",   "run_like_tornado"};
char sys_lexems_to_res[][MAXLEN] = { "=",  "get" ,  "put",  "while",   "if",   "for",  "else",  "ret",   "end",     "if-else",  "op"};
int num_sys_lexems = sizeof(sys_lexems) / MAXLEN;
int num_sys_lexems_to_res = sizeof(sys_lexems_to_res) / MAXLEN;


enum fun_names{
    /*OP_PLUS = '+',
    OP_MINUS = '-',
    OP_MUL = '*',
    OP_DIV = '/',
    OP_POW = '^', */
    OP_OPENBRACKET = '(',
    OP_CLOSEBRACKET = ')',
    OP_D  = 0,
    OP_COS,
    OP_SIN,
    OP_TAN,
    OP_COT,
    OP_ACOS,
    OP_ASIN,
    OP_ATAN,
    OP_COSH,
    OP_SINH,
    OP_TANH,
    OP_EXP,
    OP_LOGE,
    OP_LOG10,
    OP_PLUS,
    OP_MINUS ,
    OP_MUL ,
    OP_DIV ,
    OP_POW,

};


char operators[][MAXLEN] = //my
        {"pet", "cos", "sin", "tan", "cot", "acos", "asin", "atan", "cosh", "sinh", "tanh", "exp", "loge", "log10", "mew", "mrr", "murr", "mauu", "MEEW"};
char operators_to_res[][MAXLEN] = //standart
        {"deriv", "cos", "sin", "tan", "cot", "acos", "asin", "atan", "cosh", "sinh", "tanh", "exp", "loge", "log10", "+", "-", "*", "/", "^"};

int num_operators = sizeof(operators) / MAXLEN;
int num_operators_to_res = sizeof(operators_to_res) / MAXLEN;


struct Node{
    double value;

    Node* parent;
    Node* left;
    Node* right;
    //int pos_instr;
    char node_type;
};

struct Token{
    char node_type;
    int str_num;
    int pos_instr;
    double value;
};

const int MAXVARS = 256;
struct  Vars {
    double val;
    char name[MAXLEN];
};

struct  ArrayVars {
    Vars var[MAXVARS];
    unsigned size;
} Var_array = {};


struct  Funs {
    //double val;
    char name[MAXLEN];
    ArrayVars vars;
};

struct  ArrayFuns {
    Funs var[MAXVARS];
    unsigned size;
} Fun_array = {};

void Getnames(char* input, char* output, const char* name){

strcpy (output, input);

char* output_name1 = strrchr (output, '.') - 1;
char* output_name2 = strrchr (output, '/');
char* output_name  = (output_name1 > output_name2) ? output_name1 : output_name2;

strcpy ((output_name? output_name+1 : output), name);


}

#endif //FRONTEND_TYPES_H
