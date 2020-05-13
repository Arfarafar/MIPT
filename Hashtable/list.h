//
// Created by andrew on 30.10.2019.
//

#ifndef LIST_LIST_H
#define LIST_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define DOT_PATH  "C:/graphviz/release/bin/dot.exe"

enum errors{
    NORMAL_STATE = 0,
    ELEMENTARY_LOOP = 1
};

typedef char* Elem_t;
#define Elem_t_print "%s "

const char Pois[] = "-666";
const Elem_t Poison = (char*)Pois;
const int Increase = 2;

struct cell{
    Elem_t value;
    int next;
    int previous;
};

struct List{
    struct cell* cells;
    int head;
    int tail;
    int free;
    int size;
    int maxsize;
    int err;
    bool sorted = 0;
};

int Insert_Head(List* list, Elem_t Elem);
//int Insert_Tail(List* list, Elem_t Elem);
int Insert_Before(List* list, int position, Elem_t Elem);
int Insert_After(List* list, int position, Elem_t Elem);
int Delete_This(List* list, int position);
extern int Find_By_Value(List* list, Elem_t Elem);
int Find_By_Logic_Number(List* list, int num);
void List_Dump(List* list);
void Eraise_Err(List* list);
struct List* Create_List(int size);
void Diigraph (List* list);
//extern int List_is_empty(List* list);



//bool List_is_empty(List* list){
//    return !(list -> size); 
//}

void Clear_Cells(List* new_list, int position){
    for(int i = position; i < new_list -> maxsize - 1; i++) {
        new_list -> cells[i].next = i + 1;
        new_list -> cells[i].previous = -1;
    }
    new_list -> cells[new_list -> maxsize - 1].next = 0;
    new_list -> cells[new_list -> maxsize - 1].previous = -1;

}

void Clear_First(List* new_list) {
    new_list->cells[0].value = Poison;
    new_list->cells[0].next = 0;
    new_list->cells[0].previous = 0;
}


void Clear_List(List* new_list){
    new_list -> head = 1;
    new_list -> tail = 1;
    new_list -> size = 0;

    Clear_First(new_list);

    Clear_Cells(new_list, 1);

    new_list -> free = 1;
    new_list -> err = NORMAL_STATE;
}

void Memory_Reallocation(List* list, int new_size){
    list -> cells = (cell*)realloc((void*)(list -> cells), new_size * sizeof(cell));
    assert( list -> cells );

    list -> maxsize = new_size;
    Clear_Cells(list, list -> size + 1);
    list -> cells[list -> free].next = list -> size + 1;
}

struct List* Create_List(int size){
    List* new_list = (List*)calloc(1, sizeof(struct List));
    new_list -> maxsize = size + 1;
    new_list -> cells = (struct cell*)calloc(size + 1, sizeof(struct cell));
    Clear_List(new_list);
    return new_list;
}

void Deconstruct_List(List* list){
    if (list) {
        free(list->cells);
        free(list);
    }
}



bool List_OK(struct List* list){
    assert(list);

    int current = list -> head;
    for(int i = 0; i < list -> size - 1; i++){
        current = list -> cells[current].next;
    }
    if (list -> cells[current].next || current != list -> tail)
        list -> err = ELEMENTARY_LOOP;

    if (list -> err != NORMAL_STATE)
        Eraise_Err(list);
    return 1;
}

int Insert_Head(List* list, Elem_t Elem){
    assert(list);

    list -> size += 1;
    if (list -> size == list -> maxsize - 1)
        Memory_Reallocation(list, list -> maxsize * Increase);

    list -> cells[list -> head].previous = list -> free;
    int new_free = list -> cells[list -> free].next;
    list -> cells[list -> free].next = list -> size - 1 ? list -> head : 0 ;
    list -> head = list -> free;
    list -> free = new_free;
    list -> cells[list -> head].value = Elem;
    list -> cells[list -> head].previous = 0;

    return list -> head;
}

 extern int Insert_Tail(List* list, Elem_t Elem);
 /*{
    //assert(list);

    list -> size += 1;
    if (list -> size == list -> maxsize - 1)
        Memory_Reallocation(list, list -> maxsize * Increase);

    int new_free = list -> cells[list -> free].next;
    list -> cells[list -> tail].next = list -> free;

    list -> cells[list -> free].previous =  list -> size - 1 ? list -> tail : 0 ;
    list -> tail = list -> free;
    list -> free = new_free;
    list -> cells[list -> tail].value = Elem;
    //printf("%s\n", Elem);
    list -> cells[list -> tail].next = 0;

    return list -> tail; */
__asm__(R"(
        .globl  _Z11Insert_TailP4ListPc
    .type   _Z11Insert_TailP4ListPc, @function
_Z11Insert_TailP4ListPc:

    .cfi_startproc
    pushq   %rbp
    
    pushq   %rbx
    
    movq    %rsi, %rbp
    
    
    incl    20(%rdi)        #size++
    movl    24(%rdi), %esi  #maxsize    
    leal    -1(%rsi), %eax
    movl    20(%rdi), %ecx  #list -> size
    cmpl    %eax, %ecx #if (list -> size == list -> maxsize - 1)
    jne  ..L47

    addl    %esi, %esi
    call    _Z19Memory_ReallocationP4Listi
    movl    20(%rdi), %ecx  #list -> size

..L47:
    movslq  16(%rdi), %rdx  #rdx = list -> free
    movq    (%rdi), %r8 #r8 = &firstcell
    movslq  12(%rdi), %rbx  #rbx = list -> tail

    movq    %rdx, %rax #rax = list -> free
    salq    $4, %rdx
    addq    %r8, %rdx       #rdx = &freecell
    movl    8(%rdx), %r9d   #r9 = tailcell.next

    movq    %rbx, %rsi      #rsi = list -> tail
    salq    $4, %rbx
    movl    %eax, 8(%r8,%rbx) #list -> cells[list -> tail].next = list -> free;
    
    cmpl    $1, %ecx        #list -> size - 1 ? :
    movl    $0, %ecx
    cmove   %ecx, %esi
    movl    %esi, 12(%rdx)  #list -> cells[list -> free].previous = list -> tail / / 0

   
  
    movl    %eax, 12(%rdi)  #list -> tail = list -> free;
    movl    %r9d, 16(%rdi)  #list -> free = list -> cells[list -> free].next;
    movq    %rbp, (%rdx)    #list -> cells[list -> tail].value = Elem;
    movl    $0, 8(%rdx)     #list -> cells[list -> tail].next = 0;
    
    
    popq    %rbx
    
    popq    %rbp
    
    ret
    
  
  
    .cfi_endproc
    .size   _Z11Insert_TailP4ListPc, .-_Z11Insert_TailP4ListPc
    )"); 



int Insert_Before(List* list, int position, Elem_t Elem){
    assert(list);

    list -> size += 1;
    if (list -> size == list -> maxsize - 1)
        Memory_Reallocation(list, list -> maxsize * Increase);

    int new_free = list -> cells[list -> free].next;
    int previous = list -> cells[position].previous;

    list -> cells[previous].next = list -> cells[position].previous = list -> free;

    list -> cells[list -> free].previous = previous;
    list -> cells[list -> free].next = position;
    list -> cells[list -> free].value = Elem;

    list -> free = new_free;

    return list -> cells[position].previous;
}

int Insert_After(List* list, int position, Elem_t Elem){
    assert(list);

    list -> size += 1;
    if (list -> size == list -> maxsize - 1)
        Memory_Reallocation(list, list -> maxsize * Increase);

    int new_free = list -> cells[list -> free].next;
    int next = list -> cells[position].next;

    list -> cells[next].previous = list -> cells[position].next = list -> free;

    list -> cells[list -> free].previous = position;
    list -> cells[list -> free].next = next;
    list -> cells[list -> free].value = Elem;

    list -> free = new_free;

    return list -> cells[position].next;
}

int Delete_This(List* list, int position){
    assert(list);

    int next = list -> cells[position].next;
    int previous = list -> cells[position].previous;

    if(list->tail != position)
        list -> cells[next].previous = previous;
    else list->tail = previous;
    if(list->head != position)
        list -> cells[previous].next = next;
    else list->head = next;

    list -> cells[position].next = list -> free;
    list -> cells[position].previous = -1;
    list -> free = position;

    list -> size -=1;
    return position;

}

int Wordcmp(const char* word1, const char* word2){
    int counter = 0;
    while ((*(word1 + counter) == *(word2 + counter)) && (isalpha(*(word1 + counter)))){
        counter++;
    }
    return  (!isalpha(*(word2 + counter))) && (!isalpha(*(word1 + counter))) ? 0 : 1;

}


/*int Find_By_Value(List* list, Elem_t Elem){ // Elem на вх
    //assert(list);

    int counter = 0;
    int current = list -> head;

    //while (counter < list -> size && Wordcmp(list -> cells[current].value, Elem )){  //возвращает 0 если слова равны
   //     counter++;
   //     current = list -> cells[current].next;
   // }

   // if (!isalpha(*Elem))
     //   return 1; 

    while (counter < list -> size && strcmp(list -> cells[current].value, Elem )){  //возвращает 0 если слова равны
        counter++;
        current = list -> cells[current].next;
    }

    if (counter == list -> size)
        return 0;
    return 1;
}*/

int Find_By_Logic_Number(List* list, int num){
    assert(list);

    if (list -> sorted)
        return num;
    int current = list -> head;
    for(int i = 0; i < num - 1; i++){
        current = list -> cells[current].next;
    }
    return current;
}



#define Print_array(array)  \
for (int i = 0; i < list -> maxsize; i++) {  \
array ;} \
putchar('\n');

void List_Dump(List* list) {
    assert(list);

    printf("       ");
    Print_array(printf("%5d", i))
    printf("Value: ");
    Print_array(printf(Elem_t_print, list->cells[i].value))
    printf("next:  ");
    Print_array(printf("%5d", list->cells[i].next))
    printf("prev:  ");
    Print_array(printf("%5d", list->cells[i].previous))

    printf("Head: %d\n", list -> head);
    printf("Tail: %d\n", list -> tail);

    int current = list -> head;
    printf("Full path: %d ", current);
    for(int i = 0; i < list -> size; i++)
        printf("-> %d ", current = list->cells[current].next);
    printf("\nFree: %d\n", list -> free);
    printf("Size: %d\n", list -> size);
    printf("Max_Size: %d\n", list -> maxsize);

}
#undef Print_array

void Diigraph(List* list){
    FILE* flin = fopen("list.dot", "wt");
    fprintf(flin, "digraph G{\n"
                  "rankdir=LR;\n"
                  "node[color=\"goldenrod\",fontsize=13];\n");
    int current = 0;
    fprintf(flin, "%d [shape=record,label=\"  {%d | value\\n%p\\n " Elem_t_print " | {<prev> previous\\n%p\\n %d | <next> next\\n%p\\n %d} }\" ];\n",
            current, current, &(list -> cells[current].value),
            list -> cells[current].value, &(list -> cells[current].previous), list -> cells[current].previous,
            &(list -> cells[current].next), list -> cells[current].next);
    fprintf(flin, "%d:<prev> -> %d [arrowhead=\"vee\"];\n", current, list -> cells[current].previous);
    fprintf(flin, "%d:<next> -> %d [arrowhead=\"vee\"];\n", current, list -> cells[current].next);

    current = list -> head;
    //int previous = list -> cells[current].previous;
    for(int i = 0; i < list -> size; i++){
        fprintf(flin, "%d [shape=record,label=\"  {%d | value\\n%p\\n " Elem_t_print " | {<prev> previous\\n%p\\n %d | <next> next\\n%p\\n %d} }\" ];\n",
                current, current, &(list -> cells[current].value),
                list -> cells[current].value, &(list -> cells[current].previous), list -> cells[current].previous,
                &(list -> cells[current].next), list -> cells[current].next);
        fprintf(flin, "%d:<prev> -> %d [arrowhead=\"vee\"];\n", current, list -> cells[current].previous);
        fprintf(flin, "%d:<next> -> %d [arrowhead=\"vee\"];\n", current, list -> cells[current].next);
        // previous = current;
        current = list -> cells[current].next;
    }
    fprintf(flin, " }\n"
    );
    fclose(flin);
    system("del list.dot.png            && "
           DOT_PATH " -Tpng -O list.dot && "
           "start /wait list.dot.png    && "
           "pause");

}

void Eraise_Err(List* list){
    List_Dump(list);
    Diigraph(list);
    abort();
}
#endif //LIST_LIST_H
