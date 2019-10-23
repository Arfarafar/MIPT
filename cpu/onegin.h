//
// Created by andrew on 11.10.2019.
//

#ifndef ONEGIN_ONEGIN_H
#define ONEGIN_ONEGIN_H



struct string {
    char *str;
    int len;
};



int SymbolCount(char **buf,int ftel, char desired)
{
    assert(buf);
    assert(*buf);

    int i = 0; //counter
    for (char* s = (char*) memchr(*buf, desired, ftel);
         s;
         s = (char*) memchr(s+1, desired, ftel - (s-*buf)))
    {
        i++;
    }
    return i;
}

int Createbuf(char** buf, int ftel, FILE* flin){
    assert(buf);
    assert(flin);

    *buf = (char *) calloc(ftel + 2, sizeof(char)) + 1; // ftel+2 because of element in front of,  1 behind
    ftel = fread(*buf, 1, ftel, flin);
    *buf = (char*) realloc(*buf-1, ftel+2) + 1;

    return ftel;
}

int File_Length(FILE * flin){
    assert(flin);
    fseek(flin, 0, SEEK_END);
    int ftel = ftell(flin);
    fseek(flin, 0, SEEK_SET);
    return ftel;
}

struct string* indexFill( char **buf, FILE* flin, int* stcount) {
    assert(buf);
    assert(stcount);
    assert(flin);

    int buf_length = Createbuf(buf, File_Length(flin), flin );
    *stcount = SymbolCount(buf, buf_length, '\n');

    *((*buf) + buf_length) = '\0';
    *((*buf) - 1) = '\0'; // \0 in first char
    (*stcount)++;

    struct string *index = (struct string *) calloc(*stcount, sizeof(struct string));
    (index)[0].str =(*buf);
    *stcount = 0;

    for (char* s = (char*)memchr(*buf, '\n', buf_length);
         s;
         s = (char*)memchr(s + 1, '\n', buf_length - (s - *buf) - 1))
    {   // fills index
        /* if (*(s-1) == '\0'){
             (*stcount)--;
             continue;
         }*/
        (index)[++(*stcount)]    .str = s + 1;
        (index)[  (*stcount) - 1].len = (index)[*stcount    ].str -
                                        (index)[*stcount - 1].str;
        *s = '\0';
    }

    (index)[*stcount].len = *buf + buf_length - (index)[*stcount].str + 1;//last line
    return index;
}

void Index_print(struct string* index , int stcount, FILE* flout)
{
    assert(flout);
    assert(index);

    for (int i = 0; i<= stcount; i++)
    {
        fputs(index[i].str, flout);
        putc('\n', flout);
    }
}

#endif //ONEGIN_ONEGIN_H
