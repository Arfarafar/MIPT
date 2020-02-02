//
// Created by andrew on 15.10.2019.
//

#ifndef KOMPIL_ENUM_H
#define KOMPIL_ENUM_H

#include <assert.h>
#include <stdlib.h>
#include <cstring>

const int MAXTEXTSIZE = 16384;
const int MAXARGSIZE = 512;

#define MAX
#define VERSION 47

#define ACCURACY 1000

#define SIGNATURE_SIZE 3
#define VERSION_SIZE 1
#define PUT_SIGNATURE assem[PC++] = 'M'; assem[PC++] = 'A'; assem[PC++] = 'S';
#define PUT_VERSION assem[PC++] = VERSION;
#define MAX_LABLES 256
#define MAXLEN 256

enum REGISTERS {
    REG_NUM_ax = 0,
    REG_NUM_bx = 1,
    REG_NUM_cx = 2,
    REG_NUM_dx = 3,
    REG_NUM_bp
};

#define DEF_CMD(Name, Num, ...)\
CMD_##Name = Num,

enum COMMANDS {
#include <commands.h>

};
#undef DEF_CMD


#define PRINT_digit(what) printf("\t%.3f\n", what);

bool Check_Signature_Version(FILE* flin) {
    assert(flin);

    char signature[SIGNATURE_SIZE + VERSION_SIZE + 1] = {};
    fgets(signature, SIGNATURE_SIZE + VERSION_SIZE + 1, flin);
    fseek(flin, 0, SEEK_SET);

    if ( signature [0] != 'M'  ||
         signature [1] != 'A'  ||
         signature [2] != 'S'  ||
         signature [3] != VERSION)
        return 0;
    return 1;
}

#define X ((float)Stack_Pop(stk1) / ACCURACY)
#define Y X

#define $(what) Stack_Push(stk1, what);

#define OPERATION(sign)\
float y = Y; \
float x = X;  \
$((Elem_t)((x sign y) * ACCURACY ));



struct mark{
    int address;
    char label_name[MAXLEN];
};

#define LABEL   sscanf(current_line + position, "%s", arg);             \
                int pos = Find_Label(label, arg);                       \
                if (pos != -2) {                                        \
                    *(int*) (assem + PC) = pos; PC += 4;                \
                }                                                       \
                else                                                    \
                    ERR

#define JUMPTO_IF(sign) \
                        float y = X; \
                        float x = Y;\
                        /*printf("%f"#sign"%f\n", x,y);*/\
                        if (x sign y)       \
                            PC = *(int*)(buf + PC) - SIGNATURE_SIZE - VERSION_SIZE; \
                        else PC += 4;


#define FPUTARG arg = *(int*)(buf + PC); PC += 4;   fprintf(flout, " %d:\n", arg);

#define REGISTER_NUM(arg, name, cmdtype) \
(stricmp(arg, #name) == 0) { \
assem[PC-1] = cmdtype; \
assem[PC++] = REG_NUM_##name; }


#define CHOSE_REG(command)                                                  \
                             if REGISTER_NUM(arg, ax, command )        \
                        else if REGISTER_NUM(arg, bx, command )        \
                        else if REGISTER_NUM(arg, cx, command )        \
                        else if REGISTER_NUM(arg, dx, command )  \
                        else if REGISTER_NUM(arg, bp, command )

int Find_Label(struct mark label[], char arg[]){

    if (arg[0] == '\0')
        return -2;

    for (int i = 0; i < MAX_LABLES; i++){
        if (strcmp(label[i].label_name, arg) == 0)
            return label[i].address;
    }
    return -1;
}





#endif //KOMPIL_ENUM_H
