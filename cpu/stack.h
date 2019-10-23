//
// Created by andrew on 07.10.2019.
//

#ifndef STEK_STACK_HEADER_H
#define STEK_STACK_HEADER_H


const float INCREACE = 1.5;
const int POISON = -666;
#define MAXLENGTH(stk) ((stk) -> MAX_size)
#define POSITION __FILE__ , __LINE__ , __PRETTY_FUNCTION__
#define DUMPCALL(arg) Raise_Error((stk), arg, POSITION); assert(!"OK"); break;

#define STACK_Check(stk) if(not Stack_OK(stk)) {\
switch ((stk) -> err)\
{\
    case WRONGdataPTR: DUMPCALL("WRONG dataPTR")\
    case WRONGcanaryPTR: DUMPCALL("WRONG dataCANARY_PTR")\
    case WRONGCANARY: DUMPCALL("WRONG CANARY")\
    case WRONGdataCANARY: DUMPCALL("WRONG dataCANARY")\
    case WRONGHASH: DUMPCALL("WRONG HASH")\
    case WRONGSIZE: DUMPCALL("WRONG SIZE")\
    case WRONGPOISON: DUMPCALL("WRONG POISON")\
    case EUNDERFLOW: DUMPCALL("UNDERFLOW")\
    default: DUMPCALL("UNKNOWNERROR")\
        break;\
}}

#define HASHWITHOUTDATA(stk)  (stk) -> hash = 0; \
        (stk) -> hash = Stack_Hash ((const void*) (stk), sizeof(stek_t))


#define HASH(stk)  HASHWITHOUTDATA (stk) + Stack_Hash ((const void*) (stk) -> canary_data_1, sizeof(Canary_t) * 2 + sizeof(Elem_t) * MAXLENGTH(stk));

enum {  NORMAL = 0 ,
        WRONGdataPTR = 1,
        WRONGcanaryPTR = 2,
        WRONGCANARY = 10,
        WRONGdataCANARY = 11 ,
        WRONGHASH = 100,
        WRONGSIZE = 115,
        WRONGPOISON = 120,
        EUNDERFLOW = -1 };

typedef int Elem_t;
#define ELEMT "%d"

typedef long long int Canary_t;
#define CANARYT     "%lld"
#define CANARYVALUE  0xBEDABEDABEDADEDll
#define CANARYdataVALUE  0xBEDABEDABEDA278ll


struct stek_t{
    char name[16];
    Canary_t canary1;
    int size;
    unsigned int MAX_size;
    char err;
    unsigned int hash;
    Canary_t *canary_data_1;
    Elem_t *data ;
    Canary_t *canary_data_2;
    Canary_t canary2;
};




#endif //STEK_STACK_HEADER_H

#include <cstdio>
#include <cassert>
#include <cstdlib>

unsigned int Stack_Hash( const void* key, int len, unsigned int seed = 0);

void Stack_Dump(stek_t* stk);

void Raise_Error(stek_t* stk, const char* cause, const char* file, int line, const char*  function );

stek_t* Stack_Construct (int stackSize, const char* name);

bool Memory_allocation_data(stek_t* stk);

bool Stack_Push (stek_t* stk, Elem_t value);

Elem_t Stack_Pop(stek_t* stk);

bool Stack_OK(stek_t* stk);

/*
bool Test(stek_t* stk){

    stk -> data[stk -> size++] = 9;
    HASH(stk)
    StackPush(stk, 15);
    return 1;
}


int main() {
    stek_t* stk1 = Stack_Construct(2, "stk1");
    if ( !stk1 ){
        printf("No possible to make this stack, memory error");
    }
    // if (Test(&stk1)){
    //     printf("IM IN");
    // }
    printf("%p", stk1);
    StackPush (stk1, 17);
    StackPush (stk1, 19);
    StackPush (stk1, 20);
    StackPush (stk1, 20);
    stk1 -> size = -1;


    printf("%d", MAXLENGTH(stk1));


    return 0;
} */

unsigned int Stack_Hash(const void * key, int len, unsigned int seed)
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const unsigned int m = 0xc6a4a793;
    const int r = 16;

    // Initialize the hash to a 'random' value

    unsigned int h = seed ^ (len * m);

    // Mix 4 bytes at a time into the hash

    const unsigned char * data = (const unsigned char *)key;

    while(len >= 4)
    {
        h += *(unsigned int *)data;
        h *= m;
        h ^= h >> r;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch(len)
    {
        case 3:
            h += data[2] << 16;
        case 2:
            h += data[1] << 8;
        case 1:
            h += data[0];
            h *= m;
            h ^= h >> r;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h *= m;
    h ^= h >> 10;
    h *= m;
    h ^= h >> 17;

    return h;

}


#define CHECK(obj, errT) if ( obj ){ stk -> err = errT; return 0;}
bool Stack_OK(stek_t* stk) {
    assert(stk);

    if (stk -> err != NORMAL)
        return 0;

    CHECK (stk -> canary2 != CANARYVALUE || stk -> canary1 != CANARYVALUE ,                       WRONGCANARY) //10
    CHECK (*stk -> canary_data_2 != CANARYdataVALUE || *stk -> canary_data_1 != CANARYdataVALUE,  WRONGdataCANARY) // 11
    CHECK (stk -> data == nullptr,                                                                WRONGdataPTR) //1
    CHECK (stk -> canary_data_1 == nullptr || stk -> canary_data_2 == nullptr ,                   WRONGcanaryPTR) //2
    CHECK (stk -> size > (stk) -> MAX_size || stk -> size < 0,                                    WRONGSIZE)

    unsigned int i =  stk -> size;
    while( i < (stk) -> MAX_size  && stk -> data[i++] == POISON){}
    if (i != (stk) -> MAX_size){
        stk -> err = WRONGPOISON;
        return 0;
    }

    i = stk -> hash;
    HASH(stk)
    if (i == stk -> hash)
        return 1;

    stk -> err != WRONGHASH;
    stk -> hash = i;
    return 0;
}
#undef CHECK


void Raise_Error(stek_t* stk, const char* cause, const char* file, int line, const char*  function ){

    printf("\nDump %s from %s\t line: %d\n%s", cause, file, line, function);
    Stack_Dump(stk);
}

void Stack_Dump(stek_t* stk) {

    printf(" \n\n stack_t \"%s\" [%p]\n\t{\n ", (stk)->name, stk);
    printf("\t canary1 = "  CANARYT "\t normally is " CANARYT "\n", (stk)->canary1, CANARYVALUE);
    printf("\t size = %d\n", (stk)->size);
    printf("\t canary_data_1 = "  CANARYT "\t normally is " CANARYT "\n", *(stk)->canary_data_1, CANARYdataVALUE);
    printf("\t data[%d][%p]\n", (stk)->MAX_size, (stk)->data);
    printf("\t\t{\n");
    if ((stk)->err != WRONGSIZE)
        for (int i = 0; i < (stk)->size; i++) {
            printf("\t\t *[%d] = " ELEMT "\n", i, (stk)->data[i]);
        }
    for (int i = (stk)->size > 0 ? (stk)->size : 0; i < (stk)->MAX_size; i++) {
        printf("\t\t  [%d] = " ELEMT "\n", i, (stk)->data[i]);
    }
    printf("\t\t}\n");
    printf("\t canary_data_2 = "  CANARYT "\t normally is " CANARYT "\n", *(stk)->canary_data_2, CANARYdataVALUE);
    printf("\t error number = %d\n", (stk)->err);
    printf("\t hash = %u\n", (stk)->hash);
    printf("\t canary2 = "  CANARYT "\t normally is " CANARYT "\n", (stk)->canary2, CANARYVALUE);
    printf("\t}\n");
}


bool Memory_allocation_data(stek_t* stk){
    assert(stk);

    if (stk -> data){
        /* int  i = stk -> hash;
         HASHWITHOUTDATA(stk);
         if (i != stk -> hash)
             Raise_Error((stk), "SOMEONE HAS CHANGED SIZE OF STACK", POSITION);*/
        stk -> canary_data_1 = (Canary_t *) realloc((void *) (stk -> canary_data_1), sizeof(Canary_t) * 2 + sizeof(Elem_t) * (stk) -> MAX_size);
    }
    else
        stk -> canary_data_1 = (Canary_t *) calloc(sizeof(Canary_t) * 2 + sizeof(Elem_t) * (stk) -> MAX_size, 1);

    if (stk -> canary_data_1 == nullptr)
        return    false;

    stk -> data = (Elem_t*)(stk -> canary_data_1 + 1);
    for (size_t i = stk -> size; i < (stk) -> MAX_size; i++)
        stk -> data[i] = POISON;

    stk -> canary_data_2 = (Canary_t*)(stk -> data + (stk) -> MAX_size);

    *stk -> canary_data_1 = CANARYdataVALUE;
    *stk -> canary_data_2 = CANARYdataVALUE;

    HASH(stk)
    STACK_Check(stk)
    return true;
}

stek_t* Stack_Construct (int stackSize, const char name[]){
    stek_t* stk1 = (stek_t*) calloc(sizeof(stek_t), 1);
    strncpy( stk1 -> name, name, 16) ;
    stk1 -> canary1 = CANARYVALUE;
    stk1 -> canary2 = CANARYVALUE;
    stk1 -> size = 0;
    stk1 -> data = nullptr;
    stk1 -> err = NORMAL;
    (stk1) -> MAX_size = stackSize ;
    if (!Memory_allocation_data(stk1)){
        return nullptr;
    }
    HASH(stk1)
    return stk1;
}

void Stack_Distruct(stek_t* stk){
    assert(stk);

    free(stk -> canary_data_1);
    stk -> size = 0;
    stk -> canary_data_1 = nullptr;
    stk -> canary_data_2 = nullptr;
    stk -> data = nullptr;
}

bool Stack_Push (stek_t* stk, Elem_t value){
    assert(stk);
    STACK_Check(stk)

    if (stk -> size >= (stk) -> MAX_size){
        (stk) -> MAX_size = (int) ((stk) -> MAX_size * INCREACE) + 1;
        HASH(stk)
        if (!Memory_allocation_data(stk))
            return false;

    }

    stk -> data[stk -> size ++] = value;
    HASH(stk)

    STACK_Check(stk)
    return true;
}

Elem_t Stack_Pop(stek_t* stk){
    assert(stk);
    STACK_Check(stk)

    if (stk -> size <= 0){
        stk -> err = EUNDERFLOW;
        STACK_Check(stk)
        return 0;
    }

    Elem_t y = stk -> data[ -- stk -> size];
    stk ->  data[stk -> size] = POISON;

    if (stk -> size * INCREACE * INCREACE + 2 < (stk) -> MAX_size) {
        (stk) -> MAX_size = (int) ((stk) -> MAX_size / INCREACE);
        HASH(stk)
        Memory_allocation_data(stk);
    }
     HASH(stk)


    STACK_Check(stk)
    return y;
}



