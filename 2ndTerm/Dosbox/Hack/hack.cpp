#include <stdio.h>
#include <file.h>
#include <string.h>
#include <types.h>

#define MAXLEN 256

int main(int argc, const char* argv[]) {

    char input[MAXLEN] = "";
    char output[MAXLEN] = "";

    if (argv[1]) {
        strcpy (input, argv[1]);
        Getnames(input, output, "patched.com");
    }
    else {
        printf("ERROR: There is no input file");
        return 0;
    }

    FILE* flin = fopen(input, "rb");
    assert(flin);
    struct Buf* buf = Create_Buf(flin);
    fclose(flin);

    buf->buffer[56] = 116;

    FILE* flout = fopen(output, "wb");
    fwrite(buf->buffer, buf->len, 1, flout);
    fclose(flout);

    return 0;
}