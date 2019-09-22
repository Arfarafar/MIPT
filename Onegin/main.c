#include <stdio.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <assert.h>
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

    char floutName[] = "sorted.txt"; //sorted.txt
    FILE *flin;
    if ((flin = fopen("../oneginrus.txt", "r" )) == NULL) {
        perror("fopen");
             printf("%s","cant open this file!") ;
        return 1;
    }
    FILE *flout;
    if ((flout = fopen(floutName, "w" )) == NULL) {
        perror("fopen");
        printf("%s","cant open this file!") ;
        return 1;
    }

    int stcount=0; // starts counting with 0, if stcount = 2 it means 3 strings
    char *buf;
    struct string* index;
    indexFill(&buf,flin,&stcount, &index ); // creates massive of struct string

    printf("Would you like to sort by beginings of the lines? (Y/N) \n");
    char c;
    scanf ("%c",&c);
    if (c =='Y') {
       sortBystart( stcount, index );
       fprint(index, stcount, flout);
    }
    else {
       sortByend (stcount, index);
       fprint(index, stcount, flout);
    }
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
    assert(flout != 0);
    for (int i = 0; i<= stcount; i++)
    {
        fputs(index[i].str,flout);
        putc('\n',flout);
    }
}


//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐
//!  fills in the index array with beginnings of the strings' address and length and buf array with chars from file,
//!
//!  @param[out] buf -  pointer on pointer on buffer array of chars
//!  @param[in]  flin -  pointer on input file
//!  @param[out] stcount - pointer on amount of strings  in file
//!  @param[out] index - pointer on pointer on array of structures string
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

 void indexFill( char **buf, FILE* flin, int* stcount, struct string** index) {
     assert(buf != 0);
     assert(*buf != 0);
     assert(index != 0);
     assert(*index != 0);
     assert(stcount != 0);
     assert(flin != 0);

    fseek(flin, 0, SEEK_END);
    int ftel = ftell(flin);
    *buf = (char *) calloc(ftel + 2, sizeof(char)) +1; // ftel+2 because of element in front of,  1 behind
    fseek(flin, 0, SEEK_SET);
    ftel=fread(*buf, 1, ftel, flin);
    realloc(*buf,ftel);
    fseek(flin, 0, SEEK_SET);

    for (int i = 0; i < ftel; i++) {

        if (((*buf)[i] == '\n' || (*buf)[i] == '\0') ) {

            *((*buf) + i) = '\0';
            (*stcount)++;
        }

    }
    *((*buf)+ ftel) = '\0'; // \0 in last char
     *((*buf) -1) = '\0'; // \0 in first char
     (*stcount)++;
    *index = (struct string *) calloc(*stcount, sizeof(struct string));
     (*index)[0].str =(*buf);
    *stcount = 0;

    for (int i = 0; i < ftel; i++) {// fills index
        if (*(*buf + i) == '\0') {
            (*index)[++(*stcount)].str = (*buf) + i + 1;
            (*index)[(*stcount) - 1].len =  (*index)[*stcount].str -  (*index)[*stcount - 1].str;

        }
    }

     (*index)[*stcount].len = *buf + ftel -  (*index)[*stcount].str + 1;//last line

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

bool CompareB(struct string index1, struct string index2) {
    assert(index1.str != 0);
    assert(index2.str != 0);
    int i = 0, j = 0;
    while (true) {
        switch (*((index1.str + i))) {

            case '<':
            case '(':
            case ')':
            case -105: //for windows letter —
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
        switch (*(index2.str + j)) {
            case '<':
            case '(':
            case ')':
            case -105: //for windows letter —
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
        char c1 = *(index1.str+i), c2 = *(index2.str+j);

        if ((c1 == '\0') || c2 =='\0'){
            if ((c1 == '\0') && c2 > '\0')
                return 0;
            else
                return 1;

        }
        if ((c1 == c2)) {

        } else {

            if (c1 >= c2)
                return 1;
            return 0;

        }
        i++;
        j++;
    }
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
    assert(a != 0);
    assert(b != 0);
     int k;
     char *c;
     k = (*a).len;
     c = (*a).str;
    (*a).len = (*b).len;
    (*a).str =(*b).str;
    (*b).len = k;
    (*b).str = c;


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

int divide(int stcount,struct string* index){
    assert(index != 0);
    assert(stcount == -1);
    int base = (stcount) / 2;
    int l = 0;
    int r = stcount;
    struct string c = index[base];

    while (true) {

        while (!CompareB(index[l],c)){ // find element that >= index[base]
            l++;

        }
        while (r>0 && CompareB(index[r], c)){ // find element that < index[base]
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
//!  sorts struct array with Tony hoar quick sort
//!
//!
//!  @param[in] stcount - amount of strings  in file started with 0;
//!  @param[out] index[]  - array of structures string
//!
//‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐‐

void sortBystart(int stcount, struct string* index)
{
        assert(index != 0);
        if ( stcount == 0 || stcount == -1)
            return;
        int k = divide(stcount,index);
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

bool ComparEnd(struct string index1, struct string index2) {
    assert(index1.str != 0);
    assert(index2.str != 0);
    int i = 0, j = 0;
    while (true) {
        switch (*((index1.str +index1.len - i-2))) {
            case '>':
            case '(':
            case ')':
            case -105: //for windows letter —
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
        switch (*(index2.str + index2.len - j-2)) {
            case '>':
            case '(':
            case ')':
            case -105: //for windows letter —
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
        char c1 = *(index1.str +index1.len - i-2), c2 = *(index2.str + index2.len - j-2);

        if ((c1 == '\0') || c2 =='\0'){
            if ((c1 == '\0') && c2 > '\0')
                return 0;
            else
                return 1;

        }
        if (c1 == c2) {

        } else {

            if (c1 >= c2)
                return 1;
            return 0;

        }
        i++;
        j++;
    }
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

int dividEnd(int stcount,struct string* index){
    assert(index != 0);
    assert(stcount == -1);
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
    assert(index != 0);
    if ( stcount == 0 || stcount == -1)
        return;
    int k = dividEnd(stcount,index);
    sortByend(k-1,index);
    sortByend(stcount-k,index+k);
}
