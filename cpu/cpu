#include <stdio.h>

#include <enum.h>
#include <stdlib.h>
#include <Stack.h>

int Create_buf(FILE* flin, char** buf);

int main() {
    FILE *flin = fopen("../../../code/code.asmer", "rb");
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

    stek_t *stk1 = Stack_Construct(4, "stek");
    stek_t *stk_calls = Stack_Construct(4, "calls");

    if (!stk1 || !stk_calls) {
        printf("No possible to make this stack, memory error");
    }

    int Registers[4] = {};
    int PC = 0;
    char command = 0;
    int arg = 0;

#define DEF_CMD(Name, Num, code, syntax, arg) \
    case Num: code;  break;


    while (PC < length) {
        char command = *(buf + PC++);
        switch (command) {
#include <commands.h>
        }
       // printf("\n%d\n", Registers[0]);
        // Stack_Dump(stk1);
       // printf("\n%d\n", Registers[0]);
    }

    fclose(flin);
    return 0;
}

int Create_buf(FILE* flin, char** buf){
    fseek(flin, 0, SEEK_END);
    int length = ftell(flin) - (SIGNATURE_SIZE + VERSION_SIZE);
    fseek(flin, SIGNATURE_SIZE + VERSION_SIZE, SEEK_SET);
    *buf = (char *) calloc(length, sizeof(char));

    if (length != fread(*buf, sizeof(char), length, flin)) {
        printf("\nError. Length != fread()");
    }
    return length;
}
