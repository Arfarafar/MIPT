#include <stdio.h>
#include <cmath>
#include "enum.h"
#include <stdlib.h>
#include "stack.h"

int Create_buf(FILE* flin, char** buf);


#include "types.h"
int main(int argc, const char* argv[]) {

    char input[MAXLEN] = "../../../code/code.asmer";
    char output[MAXLEN] = "";

    if (argv[1]) {
        strcpy (input, argv[1]);
        Getnames(input, output, "");
    }
    printf (">>>> %s: input \"%s\", output \"%s\"\n", argv[0], input, output);

    FILE *flin = fopen(input, "rb");
    if (flin == nullptr) {
        printf("cannot open code.asmer \n");
        abort();
    }

    if (!Check_Signature_Version(flin)) {
        printf("old or cracked file \n");
        abort();
    }

    char* buf = nullptr;
    int length = Create_buf(flin, &buf);

    stek_t *stk1 = Stack_Construct (4, "stek");
    stek_t *stk_calls = Stack_Construct (4, "calls");

    if (!stk1 || !stk_calls) {
        printf ("No possible to make this stack, memory error");
    }


    int Operative[MAXVARS*100] = {};
    int Registers[5] = {};
    int PC = 0;
    char command = 0;
    int arg = 0;

#define DEF_CMD(Name, Num, code, syntax, arg) \
    case Num: code;  break;


    while (PC < length && PC >= 0) {
        command = *(buf + PC++);
        //printf("before %d %d\n", PC++, command);
        switch (command) {
#include "commands.h"
        }
        /*printf("after %d %d\n", PC, command);
        for(int i = 4; i < 15 ; i++){
            printf("%d: %d ", i, Operative[i]);
        }

        printf("\n0r: %d ", Registers[0]);
        printf("1r: %d ", Registers[1]);
        printf("3r: %d ", Registers[3]);
        printf("4r: %d\n", Registers[4]);
        printf("\n"); */

     //    Stack_Dump(stk1);
        //printf("\n%d\n", Registers[4]);
    }
   // printf("\n%d\n", Registers[0]);
 //  printf("\n%d\n", Registers[1]);
    if (PC < 0)
        printf("Неправильный прыжок команда %d", command);
   // printf(" %d %d", PC, command);
    fclose(flin);
    return 0;
}


int Create_buf(FILE* flin, char** buf){
    assert(flin);
    assert(buf);

    fseek(flin, 0, SEEK_END);
    int length = ftell(flin) - (SIGNATURE_SIZE + VERSION_SIZE);
    fseek(flin, SIGNATURE_SIZE + VERSION_SIZE, SEEK_SET);
    *buf = (char *) calloc(length, sizeof(char));

    if (length != fread(*buf, sizeof(char), length, flin)) {
        printf("\nError. Length != fread()");
    }
    return length;
}
