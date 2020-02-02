#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <cstring>
#include <stdlib.h>
#include <onegin.h>
#include <enum.h>

bool Check_and_fill_jump_target(char* current_line, char* ch, mark* label, int* label_counter, int PC);

int main() {
    FILE *flin = fopen("../../../code/Factorial.txt", "r");
    FILE *flout = fopen("../../../code/code.asmer", "wb");

    int stcount = 0; // starts counting with 0, if stcount = 2 it means 3 strings
    char *buf = NULL;
    struct string *index = indexFill(&buf, flin, &stcount);

    char assem[8192] = {};
    int PC = 0;

    char letter[64] = {'\0'};
    char *ch = nullptr;
    char *current_line = nullptr;
    char arg[64] = {'\0'};
    int num = 0;
    int position = 0;

// struct (jumpname jumpto)
    mark label[MAX_LABLES] = {} ;
    int label_counter = 0;


    PUT_SIGNATURE;
    PUT_VERSION;


#define ERR  printf("in line %d syntax error\n", i+1);
#define DEF_CMD(Name, Num, code, syntax, decode)       \
                                                        else if(stricmp(letter,#Name) == 0){          \
                                                        assem[PC++] = Num;  \
                                                        syntax;}
// 2х проходная компиляция
for (int j = 2; j > 0 ; j--) {
    PC = SIGNATURE_SIZE + VERSION_SIZE;
    for (int i = 0; i <= stcount; i++) {
    // берем строку, проверяем ее на комментарий и метку (затираем : и ; \0 -ем)
        current_line = index[i].str;
        if (ch = strchr(current_line, ';'))
            *ch = '\0';

        if (ch = strchr(current_line, ':')) {
             Check_and_fill_jump_target(current_line, ch, label, &label_counter, PC);
                continue;
        }
        
        // считываем команду в леттер, затем аргумент в арг
        sscanf(current_line, "%s%n", letter, &position);

        if (letter[0] == '\0') // if string was empty
            continue;

        if (0) {}

#include <commands.h>

        for (int j = 0; j < 64; j++) // чистим строку
            arg[j] = letter[j] = '\0';
    }

}
#undef ERR
#undef DEF_CMD

    fwrite(assem, sizeof(char), PC, flout); // пишем буфер в файл

    fclose(flin);
    fclose(flout);
    return 0;
}

bool Check_and_fill_jump_target(char* current_line, char* ch, mark* label, int* label_counter, int PC) {

        int position = 0;
        *ch = '\0';
        sscanf(current_line, "%s%n", label[*label_counter].label_name, &position);
        if (position + current_line - ch) {
            printf("in label %s syntax error\n", label[*label_counter].label_name );
            abort();
        }
        label[(*label_counter)++].address = PC;
    return 0;
}
