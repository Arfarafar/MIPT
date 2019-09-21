#include <stdio.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <windows.h>

void sortByend(int stcount, struct string* index);
void sortBystart(int stcount, struct string* index);
void fprint(struct string* index, int, FILE* fl );
void indexFill( char **buf, FILE* flin, int* stcount, struct string** index);
bool CompareB(struct string index1, struct string index2);
bool ComparEnd(struct string index1, struct string index2);
void swap(struct string *a, struct string *b);
struct string{
    char *str;
    int len;
};

int main(void) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    //char flinName[] = "onegin.txt"; //onegin.txt

    char floutName[] = "sorted.txt"; //sorted.txt
    FILE *flin;
    if ((flin = fopen("../oneginrus.txt", "r" )) == NULL) {
        perror("fopen");
             printf("%s","cant open this file!") ;
        return 1;
    }
    FILE *flout = fopen(floutName,"w");

    int stcount=0; // starts counting with 0, if stcount = 2 it means 3 strings
    char *buf;
    struct string* index;

    indexFill(&buf,flin,&stcount, &index );
    //printf("%d",stcount);
   // printf("%d \n", index[0].len);
   //sortBystart( stcount, index );
   //fprint(index, stcount, flout);
   sortByend (stcount, index);
   fprint(index, stcount, flout);
    return 0;
}


//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  param[in] buf -  buffer
//!  param[in] flin -  input file
//!  param[out] stcount - amount of strings  if file
//!  param[out] index - string address
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

void fprint(struct string* index , int stcount, FILE* flout)
{
    for (int i = 0; i<= stcount; i++)
    {
        fputs(index[i].str,flout);
        putc('\n',flout);
    }
}

 void indexFill( char **buf, FILE* flin, int* stcount, struct string** index) {
    fseek(flin, 0, SEEK_END);
    int ftel = ftell(flin);
    *buf = (char *) calloc(ftel + 2, sizeof(char)) +1;
    fseek(flin, 0, SEEK_SET);
     ftel=fread(*buf, 1, ftel, flin);
    realloc(*buf,ftel);
    fseek(flin, 0, SEEK_SET);

    for (int i = 0; i < ftel; i++) {

        if (((*buf)[i] == '\n' || (*buf)[i] == '\0') ) {

            *((*buf) + i) = '\0';
            (*stcount)++;
        }
      //  printf("sp %d  s %c sc %d \n ",i, *(*buf + i) , (*stcount));
    }
    *((*buf)+ ftel) = '\0';
     *((*buf) -1) = '\0'; // \0 in first char
     (*stcount)++;
    *index = (struct string *) calloc(*stcount, sizeof(struct string));
     (*index)[0].str =(*buf);
    *stcount = 0;

    for (int i = 0; i < ftel; i++) {
        if (*(*buf + i) == '\0') {
            (*index)[++(*stcount)].str = (*buf) + i + 1;
            (*index)[(*stcount) - 1].len =  (*index)[*stcount].str -  (*index)[*stcount - 1].str;
          //  printf("%d \n",  (*index)[(*stcount) - 1].len);
        }
    }

     (*index)[*stcount].len = *buf + ftel -  (*index)[*stcount].str + 1;
   //  printf("%d \n",  (*index)[(*stcount) ].len);
}


bool CompareB(struct string index1, struct string index2) {
    // int m = min(index1.len,index2.len);
    int i = 0, j = 0;
    while (true) {
        switch (*(((char*)index1.str + i))) {
            case '<':
            case '(':
            case ')':
            case '\'':
            case '-':
            case '.':
            case '\"':
            case ',':
            case ' ':
                i++;
                continue;
            default:
                break;
        }
        switch (*((char*)index2.str + j)) {
            case '<':
            case '(':
            case ')':
            case '\'':
            case '-':
            case '.':
            case '\"':
            case ',':
            case ' ':
                j++;
                continue;
            default:
                break;
        }

        if ((*(index1.str + i) == '\0') || *(index2.str + j) =='\0'){
            if ((*(index1.str + i) == '\0') && *(index2.str + j) > '\0')
                return 0;
            else
                return 1;

        }
        if ((*(index1.str + i) == *(index2.str + j))) {

        } else {

            if (*(index1.str + i) >= *(index2.str + j))
                return 1;
            return 0;

        }
        i++;
        j++;
    }
}

void swap(struct string *a, struct string *b){
     int k;
     char *c;
     k = (*a).len;
     c = (*a).str;
    (*a).len = (*b).len;
    (*a).str =(*b).str;
    (*b).len = k;
    (*b).str = c;


 }

int divide(int stcount,struct string* index){
   // printf("%d \n", index[0].len);
    int k = (stcount) / 2;
    int l = 0;
    int r = stcount;
    struct string c = index[k];
   // char *c1 = index[l].str;
    //char *c2 = index[r].str;
    while (true) {

        while (!CompareB(index[l],c)){
            l++;
           // c1 = index[l].str;
        }
        while (r>0 && CompareB(index[r], c)){
            r--;
           // c2 = index[r].str;
        }//  1 если 1ый больше  или равен
        if (l>=r){
            if (l== 0 && (r == 0))
                swap(&index[k],&index[0]);
            return r+1;
        }

        swap(&index[l++],&index[r--]);

    }
 }




void sortBystart(int stcount, struct string* index)
{
   // printf("%d \n", index[0].len);
        if ( stcount == 0 || stcount == -1)
            return;
        int k = divide(stcount,index);
        sortBystart(k-1,index);
        sortBystart(stcount-k,index+k);
}


bool ComparEnd(struct string index1, struct string index2) {
    // int m = min(index1.len,index2.len);
    int i = 0, j = 0;
    while (true) {
        switch (*(((char*)index1.str +index1.len - i-2))) {
            case '<':
            case '(':
            case ')':
            case ':':
            case '!':
            case '?':
            case ';':
            case '\'':
            case '-':
            case '.':
            case '\"':
            case ',':
            case ' ':
                i++;
                continue;
            default:
                break;
        }
        switch (*((char*)index2.str + index2.len - j-2)) {
            case '<':
            case '(':
            case ')':
            case ':':
            case '!':
            case '?':
            case ';':
            case '\'':
            case '-':
            case '.':
            case '\"':
            case ',':
            case ' ':
                j++;
                continue;
            default:
                break;
        }
        char c1 , c2;
        c1 = *(index1.str +index1.len - i-2);//длина включает \0 пустая трока даст \0 предыдущей строки.
        c2 =*(index2.str + index2.len - j-2);
        if ((*(index1.str +index1.len - i-2) == '\0') || *(index2.str + index2.len - j-2) =='\0'){
            if ((*(index1.str +index1.len - i-2) == '\0') && *(index2.str + index2.len - j-2) > '\0')
                return 0;
            else
                return 1;

        }
        if ((*(index1.str +index1.len - i-2) == *(index2.str + index2.len - j-2))) {

        } else {

            if (*(index1.str +index1.len - i-2) >= *(index2.str + index2.len - j-2))
                return 1;
            return 0;

        }
        i++;
        j++;
    }
}

int dividEnd(int stcount,struct string* index){
    // printf("%d \n", index[0].len);
    int k = (stcount) / 2;
    int l = 0;
    int r = stcount;
    struct string c = index[k];
    // char *c1 = index[l].str;
    //char *c2 = index[r].str;
    while (true) {

        while (!ComparEnd(index[l],c)){
            l++;
            // c1 = index[l].str;
        }
        while (r>0 && ComparEnd(index[r], c)){
            r--;
            // c2 = index[r].str;
        }//  1 если 1ый больше  или равен
        if (l>=r){
            if (l== 0 && (r == 0))
                swap(&index[k],&index[0]);
            return r+1;
        }

        swap(&index[l++],&index[r--]);

    }
}

void sortByend(int stcount, struct string* index)
{
    // printf("%d \n", index[0].len);
    if ( stcount == 0 || stcount == -1)
        return;
    int k = dividEnd(stcount,index);
    sortByend(k-1,index);
    sortByend(stcount-k,index+k);
}