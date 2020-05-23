//
// Created by andrew on 17.11.2019.
//

#ifndef DIFF_FILE_H
#define DIFF_FILE_H
#include <stdio.h>
#include <cstdlib>
#include <errno.h>

struct  Buf {
    char* buffer;
    int len;
    int cur_pos_in_file;
    int cur_line;
    int cur_pos_in_line;
};

int getFileLength (FILE* flin){
    fseek(flin, 0, SEEK_END);
    int length = ftell(flin);
    fseek(flin, 0 , SEEK_SET);
    return  length;
}

Buf* Create_Buf(FILE* flin){

    int len = getFileLength(flin);
    if (!len){
        return nullptr;
    }
    Buf* buf = (Buf*)calloc(1, sizeof(Buf));
    buf -> buffer = (char*)calloc(len, sizeof(char));
    buf -> len = fread(buf -> buffer, sizeof(char), len, flin);
    buf -> cur_pos_in_line = 1;
    buf -> cur_line = 1;
    fseek(flin, 0 , SEEK_SET);
    return buf;
}

void Buf_destruct(Buf* buf) {
    free(buf -> buffer);
    free(buf);
}
#endif //DIFF_FILE_H
