#include <stdio.h>
#include <enum.h>
#include <stdlib.h>


int main() {
    FILE* flout = fopen("../../../code/Excode.txt","wt");
    if (flout == nullptr) {
        printf("cannot create new file \n");
        abort();
    }

    FILE* flin = fopen("../../../code/code.asmer","rb");
    if (flin == nullptr) {
        printf("cannot open code.asmer \n");
        abort();
    }

    if (!Check_Signature_Version(flin)) {
        printf("cracked file \n");
        abort();
    }

    fseek(flin, 0, SEEK_END);
    int length = ftell(flin) - (SIGNATURE_SIZE + VERSION_SIZE);
    fseek(flin, SIGNATURE_SIZE + VERSION_SIZE, SEEK_SET);
    char *buf = (char *) calloc(length, sizeof(char));

    if (length != fread(buf, sizeof(char), length, flin)) {
        printf("\nError");
    }

    int PC = 0;
    char command = 0;
    int arg = 0;

#define DEF_CMD(Name, Num, code, syntax, decode) \
    case Num: fputs(#Name " ", flout)   ;    decode                                                      \
    break;
    
while (PC < length) {
    char command = *(buf + PC++);
    switch (command) {

#include <commands.h>
    }
}
    
    fclose(flin);
    fclose(flout);
    return 0;
}
