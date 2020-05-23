#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <cstring>
#include <stdlib.h>
#include "onegin.h"
#include "enum.h"

bool Check_and_fill_jump_target(char* current_line, char* ch, mark* label, int* label_counter, int PC);
void Compil(struct string *index, int stcount, int* PC, char assem[], struct mark* label, int* label_counter);
void Free_Args(char* letter, char* arg);
bool Comment_or_Jump(char* current_line, struct mark* label, int* label_counter, int* PC);

#include "types.h"

int main(int argc, const char* argv[]) {

    char input[MAXLEN] = "../../../code/a.txt";
    char output[MAXLEN] = "../../../code/code.asmer";

    if (argv[1]) {
        strcpy (input, argv[1]);
        Getnames(input, output, "code.asmer");
    }
    printf (">>>> %s: input \"%s\", output \"%s\"\n", argv[0], input, output);

    //FILE *flin = fopen("../../../code/quad.txt", "r");
    //FILE *flin = fopen("../../../code/Factorial.txt", "r");
    FILE *flin = fopen(input, "r");
    FILE *flout = fopen(output, "wb");

    int stcount = 0; // starts counting with 0, if stcount = 2 it means 3 strings
    char *buf = NULL;
    struct string *index = indexFill(&buf, flin, &stcount);

    char assem[MAXTEXTSIZE] = {};
    int PC = 0;

    mark label[MAX_LABLES] = {};
    int label_counter = 0;

    PUT_SIGNATURE;
    PUT_VERSION;

    for (int pass_remaining = 2; pass_remaining > 0; pass_remaining--) {
        PC = SIGNATURE_SIZE + VERSION_SIZE;
        Compil(index, stcount, &PC, assem, label, &label_counter);
    }

    fwrite(assem, sizeof(char), PC, flout);

    fclose(flin);
    fclose(flout);
    return 0;
}

bool Check_and_fill_jump_target(char *current_line, char *ch, struct mark *label, int *label_counter, int PC) {
    int position = 0;
    *ch = '\0';
    sscanf(current_line, "%s%n", label[*label_counter].label_name, &position);
    if (position + current_line - ch) {
        printf("in label %s syntax error\n", label[*label_counter].label_name);
        abort();
    }
    label[(*label_counter)++].address = PC;

    return 0;
}



void Compil(struct string *index, int stcount, int* IP, char assem[], struct mark* label, int* label_counter) {

    int PC = *IP;
    char letter[MAXARGSIZE] = {'\0'};
    int position = 0;
    char arg[MAXARGSIZE] = {'\0'};
    int num = 0;

    char *current_line = nullptr;

#define BEGIN_DEF_CMD  if (false) {}

#define ERR  printf("in line %d syntax error\n", i+1);
#define DEF_CMD(Name, Num, code, syntax, decode)       \
                                                        else if(strcasecmp(letter,#Name) == 0){ \
                                                        assem[PC++] = Num;  \
                                                        syntax;}

    for (int i = 0; i <= stcount; i++) {

        current_line = index[i].str;

        if (Comment_or_Jump(current_line, label, label_counter, &PC)){
            continue;
        }

        sscanf(current_line, " %s%n", letter, &position);

        if (letter[0] == '\0') // if string was empty
            continue;

        BEGIN_DEF_CMD

#include "commands.h"

      Free_Args(letter, arg);
    }

#undef ERR
#undef DEF_CMD
    *IP = PC;
}

void Free_Args(char* letter, char* arg){
    for (int freeargs = 0; freeargs < MAXARGSIZE; freeargs++)
        arg[freeargs] = letter[freeargs] = '\0';
}

bool Comment_or_Jump(char* current_line, struct mark* label, int* label_counter, int* PC){
    char *ch = nullptr;
    if (ch = strchr(current_line, ';'))
        *ch = '\0';

    if (ch = strchr(current_line, ':')) {
        Check_and_fill_jump_target(current_line, ch, label, label_counter, *PC);
        return 1;
    }
    return 0;
}
