#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys\stat.h>
#include <assert.h>
#include <windows.h>
#include <io.h>


int SymbolCount(char **buf,int ftel, char desired);

void bufprint(char* buf, int count, FILE* flout);

void sortByend(int stcount, struct string* index);

void sortBystart(int stcount, struct string* index);

int particionStart(int stcount,struct string* index);

int particionEnd(int stcount,struct string* index);

void fprint(struct string* index, int, FILE* fl );

void indexFill( char **buf, FILE* flin, int* stcount, struct string** index);

int CompareB(struct string index1, struct string index2);

int ComparEnd(struct string index1, struct string index2);

void swap(struct string *a, struct string *b);

struct string {
    char *str;
    int len;
};

int main(void) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    char floutName[] = "sorted.txt"; //sorted.txt
    FILE *flin = fopen("../oneginrus.txt", "r" );
    if (flin == NULL) {
        printf("cant open file oneginrus.txt ! ") ;
        return 1;
    }
    FILE *flout = fopen(floutName, "w" );
    if (flout == NULL) {
        printf("cant open %s file!",floutName) ;
        return 1;
    }

    int stcount = 0; // starts counting with 0, if stcount = 2 it means 3 strings
    char *buf = NULL;
    struct string* index = NULL;
    indexFill( &buf, flin, &stcount, &index ); // creates massive of struct string

    printf("Would you like to sort by beginings of the lines? (Y/N) \n");
    char c = 'Y';
    scanf ("%c",&c);
    if (c == 'Y') {
        sortBystart(stcount, index);
        fprint(index, stcount, flout);
    }
    else {
        sortByend (stcount, index);
        fprint(index, stcount, flout);
    }
    bufprint(buf,stcount,flout);


    fclose(flin);
    fclose(flout);
    free(buf-1); // cause 1 symbol was before the pointer
    free(index);


    printf("Done \n");
    return 0;
}


//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  prints all index.str one by one
//!
//!
//!  @param[in] flout - pointer on output file
//!  @param[in] stcount - amount of strings  in file
//!  @param[out] index - pointer on array of struct string
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐


void fprint(struct string* index , int stcount, FILE* flout)
{
    assert(flout);
    assert(index);
    for (int i = 0; i<= stcount; i++)
    {
        fputs(index[i].str,flout);
        putc('\n',flout);
}
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  prints buffer
//!
//!
//!  @param[in] flout - pointer on output file
//!  @param[in] count - amount of strings  in file
//!  @param[in] buf - pointer on array of char
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

void bufprint(char* buf, int count, FILE* flout){
    char* cur = buf;
    char str[] = "\n\nUNSORTED\n\n";
    fputs(str,flout);
      for( int i = 0; i<= count; i++){
          fputs(cur,flout);
          putc('\n',flout);
          cur = (char*) strchr(cur, '\0')+1;
      }
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  Count desired symbols
//!
//!  @param[in] buf -  pointer on pointer on buffer array of chars
//!  @param[in] ftel - bufsize
//! @param[in] desired - symbol to find
//!
//! @return strings count;
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

int SymbolCount(char **buf,int ftel, char desired)
{
    assert(buf);
    assert(*buf);
    int i = 0;
    for (char* s = (char*) memchr(*buf, desired, ftel);
         s;
         s = (char*) memchr(s+1, desired, ftel - (s-*buf)))
    {
        i++;
    }
    return i;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  File Length
//!
//!  @param[in]  flin -  pointer on input file
//!
//!  set seek at the begining of file
//! @return file length
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

int File_Length(FILE * flin){
    fseek(flin, 0, SEEK_END);
    int ftel = ftell(flin);
    fseek(flin, 0, SEEK_SET);
    return ftel;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  creates buffer and return it length;
//!
//!  @param[out] buf -  pointer on pointer on char (buffer array of chars)
//!  @param[in]  ftel - file length
//!  @param[in]  flin -  pointer on input file
//!
//! @return buf length
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

int Createbuf(char** buf, int ftel, FILE* flin){
    *buf = (char *) calloc(ftel + 2, sizeof(char)) +1; // ftel+2 because of element in front of,  1 behind
    ftel=fread(*buf, 1, ftel, flin);
    *buf =(char*) realloc(*buf-1,ftel+2)+1;
    return ftel;
}


//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  fills in the index array with beginnings of the strings' address and length and buf array with chars from file,
//!
//!  @param[out] buf -  pointer on pointer on buffer array of chars
//!  @param[in]  flin -  pointer on input file
//!  @param[out] stcount - pointer on amount of strings  in file
//!  @param[out] index - pointer on pointer on array of structures string
//!
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

void indexFill( char **buf, FILE* flin, int* stcount, struct string** index) {
    assert(buf);
    assert(index);
    assert(stcount);
    assert(flin);


    int buf_length = Createbuf(buf, File_Length(flin), flin );
    *stcount = SymbolCount(buf, buf_length, '\n');

    *((*buf) + buf_length) = '\n';
    *((*buf) -1) = '\0'; // \0 in first char
    (*stcount)++;

    *index = (struct string *) calloc(*stcount, sizeof(struct string));
    (*index)[0].str =(*buf);
    *stcount = 0;

    for (char* s = (char*)memchr(*buf, '\n', buf_length);
        s;
        s = (char*)memchr(s + 1, '\n', buf_length - (s - *buf) - 1))
    {   // fills index
        (*index)[++(*stcount)].str = s + 1;
        (*index)[(*stcount) - 1].len =  (*index)[*stcount].str -  (*index)[*stcount - 1].str;
        *s = '\0';
    }


    (*index)[*stcount].len = *buf + buf_length - (*index)[*stcount].str + 1;//last line

}

int goodletter(char letter){
    switch (letter) {
        case -85: //'«'
        case -69:
        case '>':
        case '(':
        case ')':
        case -105: //for windows letter —
        case ':':
        case '<':
        case '!':
        case '?':
        case ';':
        case '\'':
        case '-':
        case '.':
        case '\"':
        case ',':
        case ' ':
        case '\n':
            return 0;
        default:
            return 1;
    }
}


//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  Compare index1.str vs index2.str
//!  Starting with begin of the line
//!
//!
//!  @param[in] index1 - structure string
//!  @param[in] index2 - structure string
//!
//!  @returns 1 if first >= second
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

int CompareB(struct string index1, struct string index2) {
    assert(index1.str);
    assert(index2.str);
    int i = 0, j = 0;
    while (i < index1.len) {
        while (!goodletter(*(index1.str+i))){ i++; }
        while (!goodletter(*(index2.str+j))){ j++; }

        char c1 = *(index1.str+i++), c2 = *(index2.str+j++);

        if (c1 == c2) {}
        else {
            return tolower(c1)-tolower(c2); // if <0 -> 2>1
        }
    }
    return 0;
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  swap the elements
//!
//!
//!  @param[out] a  - structure string
//!  @param[out] b  - structure string
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

void swap(struct string *a, struct string *b){
    assert(a);
    assert(b);
    struct string tmp = *a;
    *a = *b;
    *b= tmp;
}


//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  divide the array of struct
//! all elements right are >= base element
//!
//!
//!  @param[in]  stcount - amount of strings to divide
//!  @param[out] index - pointer on first structure string
//!
//!  @returns r - all elements with position  > r are >= index[base] , all positions < r are < index[base]
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

int particionStart(int stcount,struct string* index){
    assert(index);
    assert(stcount+1);
    int base = (stcount) / 2;
    int l = 0; //left
    int r = stcount; // right
    struct string pivot = index[base];

    while (true) {

        while (CompareB(index[l], pivot) < 0){ // find element that >= pivot
            l++;
        }

        while (r>0 && (CompareB(index[r], pivot) >= 0)){ // find element that < index[base]
            r--;
        }

        if (l>=r){
            if (l== 0 && r == 0) // it means that base element is smallest one
                swap(&index[base],&index[0]);
            return r+1;
        }

        swap(&index[l++],&index[r--]);

    }
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  sorts struct array with Tony Hoar quick sort
//!
//!
//!  @param[in] stcount - amount of strings  in file started with 0;
//!  @param[out] index[]  - array of structures string
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

void sortBystart(int stcount, struct string* index)
{
    assert(index);
    if ( stcount == 0 || stcount == -1)
        return;
    int k = particionStart(stcount,index);
    sortBystart(k-1,index);
    sortBystart(stcount-k,index+k);
}


//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  Compare index1.str vs index2.str
//!
//!  @Starting with end of the line
//!
//!
//!  @param[in] index1 - structure string
//!  @param[in] index2 - structure string
//!
//!  @returns 1 if first >= second
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

int ComparEnd(struct string index1, struct string index2) {
    assert(index1.str);
    assert(index2.str);
    int i = 0, j = 0;
    while (i < index1.len) {
        while (!goodletter(*(index1.str + index1.len - i -2))){ i++; }
        while (!goodletter(*(index2.str + index2.len - j -2))){ j++; }

        char c1 = *(index1.str +index1.len - i++ -2), c2 = *(index2.str + index2.len - j++ -2);

        if (c1 == c2) {}
        else {
            return tolower(c1)-tolower(c2); // if <0 -> 2>1
        }
    }
    return 0;
}



//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  divide the array of struct
//! all elements right are >= base element
//!
//!
//!  @param[in]  stcount - amount of strings to divide
//!  @param[out] index - pointer on first structure string
//!
//!  @returns r - all elements with position  > r are >= index[base] , all positions < r are < index[base]
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

int particionEnd(int stcount,struct string* index){
    assert(index);
    assert (stcount+1);
    int base = (stcount) / 2;
    int l = 0;
    int r = stcount;
    struct string c = index[base];

    while (true) {

        while (!ComparEnd(index[l],c)){ // find element that >= index[base]
            l++;
        }

        while (r>0 && ComparEnd(index[r], c)){ // find element that < index[base]
            r--;
        }
        if (l>=r){
            if (l== 0 && (r == 0)) // it means that base element is smallest one
                swap(&index[base],&index[0]);
            return r+1;
        }

        swap(&index[l++],&index[r--]);

    }
}

//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  sorts struct array with Tony Hoar quick sort
//!
//!
//!  @param[in] stcount - amount of strings  in file started with 0;
//!  @param[out] index[]  - array of structures string
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
void sortByend(int stcount, struct string* index)
{
    assert(index);
    if ( stcount == 0 || stcount == -1)
        return;
    int k = particionEnd(stcount, index);
    sortByend(k-1, index);
    sortByend(stcount-k, index+k);
}
