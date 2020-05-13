#include <stdio.h>
#include "diff.h"

const int MAXCODELEN = 64000;
const int MAXFUNCS = 32;
ArrayVars GVar_array = {};
int datasize = 0;
int cur_datasize = 0;

struct  codeBuf {
    char* buffer;
    int size;
    int maxsize;
};

struct Func{
    char name[MAXLEN];
    int offset; 
}; 

struct Functable
{
    Func* func;
    int size;
    int maxsize;
};


void In_Function(codeBuf* codebuf, Node* cur_node, int key, Functable* functable);
void GetE(codeBuf* codebuf, Node* cur_node , int key,  Functable* functable);
void Walk_around_the_tree(codeBuf* codebuf, Node* cur_node, Functable* functable);
codeBuf* Create_codeBuf(int maxsize);
void Destruct_codeBuf(codeBuf* codebuf);
Functable* Make_Functable();
void Destruct_Functable(Functable* functable);
char* Make_elfHeader(int progsize, int offset_start);
void ADD_printf(codeBuf* codebuf, Functable* functable);
int Find_in_functable(Functable* functable, char* name);



int main(int argc, const char* argv[]) {

    char input[MAXLEN] = "../../../code/a.txt";
    char output[MAXLEN] = "../../../code/asmer.txt";

    if (argv[1]) {
        strcpy (input, argv[1]);
        Getnames(input, output, "ELF");
    }
    printf (">>>> %s: input \"%s\", output \"%s\"\n", argv[0], input, output);

    FILE* fltree = fopen(input,"r");
    Buf* buf = Create_Buf(fltree);
    Node* head_node = Create_Node_from_file(fltree, nullptr, &(buf -> len));
    //Treegraph(head_node);
    fclose(fltree);


    FILE* flout = fopen(output, "wb");
    Functable* functable = Make_Functable();
    codeBuf* codebuf = Create_codeBuf(MAXCODELEN);




    ADD_printf(codebuf, functable);
    Walk_around_the_tree(codebuf, head_node, functable);
    printf("sizecur:%d datasize:%d", cur_datasize, datasize);
    codebuf -> size = 0; 
    ADD_printf(codebuf, functable);
    Walk_around_the_tree(codebuf, head_node, functable);
    codebuf -> size += datasize; 



    char* elfHeader = Make_elfHeader(codebuf -> size, Find_in_functable(functable, "main"));


    fwrite(elfHeader, 1, 0x80, flout);
    fwrite(codebuf -> buffer, 1, codebuf -> size, flout);



    fclose(flout);
    Buf_destruct(buf);
    Deconstruct_Nodes(head_node);
    Destruct_Functable(functable);
    return 0;

}


#define WRITE(what, len) sprintf(codebuf -> buffer + codebuf -> size, what); \
                         codebuf -> size += len;  


Functable* Make_Functable(){
    Functable* functable = (Functable*) calloc (1, sizeof(Functable));
    functable -> func = (Func*) calloc (MAXFUNCS, sizeof(Func));
    functable -> size = 0;
    functable -> maxsize = MAXFUNCS;
    return functable;
}

void Destruct_Functable(Functable* functable){
    free(functable -> func);
    free(functable);
}



 char* Make_elfHeader(int progsize, int offset_start){
    char* elfhead = (char*) calloc (1, 0x80);
    char elf[] = {
     "\x7f""\x45""\x4c""\x46""\x02""\x01""\x01""\x00"
     "\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00"
     "\x02""\x00""\x3e""\x00""\x01""\x00""\x00""\x00"
     "\x5d""\x01""\x40""\x00""\x00""\x00""\x00""\x00"
     "\x40""\x00""\x00""\x00""\x00""\x00""\x00""\x00"
     "\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00"
     "\x00""\x00""\x00""\x00""\x40""\x00""\x38""\x00"
     "\x01""\x00""\x00""\x00""\x00""\x00""\x00""\x00"

     "\x01""\x00""\x00""\x00""\x05""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00" 
     "\x00""\x00""\x40""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x40""\x00""\x00""\x00""\x00""\x00" 
     "\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00"
     "\x00""\x00""\x20""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00""\x00"
    };
    //printf("offset_start %x",  );
    *(int*)(elf+ 0x18) = offset_start;
    *(int*)(elf+ 0x60) = *(int*)(elf+ 0x68) = 0x80 + progsize;
    memcpy(elfhead, elf, 0x80);
    return elfhead;
}


void ADD_printf(codeBuf* codebuf, Functable* functable){
    char print[] = { 
        "\x48" "\x31" "\xc9" "\x31" "\xd2" "\x66" "\x31" "\xf6" "\x83" "\xf8" "\x00" "\x7f" "\x07" "\xf7" "\xd0" "\xff" 
        "\xc0" "\x66" "\xff" "\xc6" "\xbb" "\xe8" "\x03" "\x00" "\x00" "\x48" "\xf7" "\xeb" "\x48" "\xc1" "\xe8" "\x0a" 
        "\x31" "\xd2" "\xbb" "\x0a" "\x00" "\x00" "\x00" "\xf7" "\xf3" "\x89" "\xd3" "\x83" "\xc3" "\x30" "\x66" "\xc1" 
        "\xe3" "\x08" "\xff" "\xc1" "\x66" "\x53" "\x48" "\xff" "\xc4" "\x83" "\xf9" "\x03" "\x74" "\x07" "\x83" "\xf8" 
        "\x00" "\x75" "\xdd" "\xeb" "\x11" "\x31" "\xdb" "\x83" "\xc3" "\x2e" "\xc1" "\xe3" "\x08" "\xff" "\xc1" "\x66"
        "\x53" "\x48" "\xff" "\xc4" "\xeb" "\xca" "\x66" "\x83" "\xfe" "\x01" "\x75" "\x0f" "\x31" "\xdb" "\x83" "\xc3" 
        "\x2d" "\xc1" "\xe3" "\x08" "\xff" "\xc1" "\x66" "\x53" "\x48" "\xff" "\xc4" "\x31" "\xdb" "\x83" "\xc3" "\x0d" 
        "\xc1" "\xe3" "\x08" "\xff" "\xc1" "\x66" "\x53" "\x48" "\xff" "\xc4" "\x31" "\xdb" "\x83" "\xc3" "\x0a" "\xc1" 
        "\xe3" "\x08" "\xff" "\xc1" "\x66" "\x53" "\x48" "\xff" "\xc4" "\x48" "\x89" "\xca" "\xb8" "\x01" "\x00" "\x00" 
        "\x00" "\xbf" "\x01" "\x00" "\x00" "\x00" "\x48" "\x89" "\xe6" "\x48" "\x01" "\xcc" "\x0f" "\x05" "\xc3" };
    char scan[] = {
        "\x48" "\x31" "\xd2" "\x66" "\x31" "\xff" "\x31" "\xc0" 
        "\xbb" "\x0a" "\x00" "\x00" "\x00" "\x80" "\x3e" "\x2d" 
        "\x75" "\x06" "\x48" "\xff" "\xc6" "\x66" "\xff" "\xc7" 
        "\x80" "\x3e" "\x3b" "\x74" "\x10" "\xf7" "\xe3" "\x8a" 
        "\x16" "\x01" "\xd0" "\x30" "\xd2" "\x83" "\xe8" "\x30"
        "\x48" "\xff" "\xc6" "\xeb" "\xeb" 
        "\x66" "\x83" "\xff" "\x01" "\x75" "\x04" "\xff" "\xc8" 
        "\xf7" "\xd0" "\xc1" "\xe0" "\x0a" "\x89" "\x01" "\xc3" };

 memcpy(codebuf -> buffer + codebuf -> size, print, sizeof(print) - 1 );
 codebuf -> size += (sizeof(print) - 1);
 strcpy(functable -> func[0].name, "printf");
 functable -> func[0].offset = 0x400080;

 strcpy(functable -> func[1].name, "scanf");
 functable -> func[1].offset = 0x400080 + codebuf -> size;
 memcpy(codebuf -> buffer + codebuf -> size, scan, sizeof(scan) - 1 );
 codebuf -> size += (sizeof(scan) - 1);

 functable->size += 2;
}


#define NODETYPE(what) cur_node -> node_type == what
#define NODEVALUE(what) cur_node -> value == what


codeBuf* Create_codeBuf(int maxsize){
    codeBuf* buf = (codeBuf*)calloc(1, sizeof(codeBuf));
    buf -> buffer = (char*)calloc(maxsize, sizeof(char));
    buf -> size = 0;
    buf -> maxsize = maxsize;
    return buf;
}

void Destruct_codeBuf(codeBuf* codebuf){
    free(codebuf -> buffer);
    free(codebuf);
}



void Get_SendArgs(codeBuf* codebuf, Node* cur_node, int key,    
                      unsigned var_num, Functable* functable){

    if (NODEVALUE(OP_SENDARGS) && NODETYPE(TYPE_SYST)){

         WRITE("\x49\x89\x3e\x49\x83\xc6\x08", 7) 
         //mov [r14], rdi
         // add r14, 8
        cur_datasize += 8; 
        if(cur_datasize > datasize) 
                            datasize += 8;  
        GetE(codebuf, cur_node -> right, key, functable);    //rax
        cur_datasize -= 8; 
        WRITE("\x49\x83\xee\x08\x49\x8b\x3e", 7) 
        //sub r14, 8
        //mov rdi, [r14]
        

        WRITE("\x48\x83\xef\x04\x89\x07", 6)    // sub rdi ,4 
                                                //mov  [rdi], eax
        if (cur_node -> left){
            Get_SendArgs(codebuf, cur_node -> left, key, var_num + 1, functable);
        }

    }

}

void Get_GetArgs(codeBuf* codebuf, Node* cur_node,  int key ){
    if (NODEVALUE(OP_SENDARGS) && NODETYPE(TYPE_SYST)){
         strcpy (Fun_array.var[key].vars.var[Fun_array.var[key].vars.size].name, Var_array.var[(int) cur_node -> right -> value].name);
         cur_node -> right -> value = Fun_array.var[key].vars.size++;
        if (cur_node -> left){
            Get_GetArgs(codebuf, cur_node -> left, key);
        }
    }
}

int Find_in_curFun(int key, char* name){
    for(int i = 0; i < Fun_array.var[key].vars.size; i++){
        if (strcmp(name, Fun_array.var[key].vars.var[i].name) == 0){
            return i;
        }
    }
    return -1;
}

int Find_in_GV(char* id){
    for(int i = 0; i < GVar_array.size; i++){
        if( strcmp(id , GVar_array.var[i].name) == 0  )
            return i;
    }
    return -1;
}

int Find_in_functable(Functable* functable, char* name){
    for(int i = 0; i < functable -> size; i++ ){
        if (strcmp(name, functable -> func[i].name) == 0){
            return functable -> func[i].offset;
        }
    }
    return 0;
}

#define  RbxLax     GetE(codebuf, cur_node -> right, key, functable) ; \
                    WRITE("\x49\x89\x06\x49\x83\xc6\x08", 7) \
                    cur_datasize += 8; \
                    if(cur_datasize > datasize) \
                            datasize += 8; \
                    GetE(codebuf, cur_node -> left, key, functable);    \
                    cur_datasize -= 8; \
                    WRITE("\x49\x83\xee\x08\x49\x8b\x1e", 7) 
//rax  
                    //mov [r14], rax 
                    //add r14, 8
//rax   
                    //sub r14, 8
                    //mov rbx, [r14]
 

void GetE(codeBuf* codebuf, Node* cur_node , int key, Functable* functable){
    if(!cur_node){
        return;
    }
    

    switch(cur_node -> node_type){
        case TYPE_VAR:
        {
            int temp = 0;
            //переменная ищется в текущем стековом фрейме и кладется в rax 
            if (temp =  Find_in_curFun(key, Var_array.var[(int) cur_node->value].name) + 1) {
                temp *= 4;
                sprintf(codebuf -> buffer + codebuf -> size, "\x48\x89\xe9\x48\x81\xe9%c%c%c%c"
                                                                "\x8b\x01"
                                                                "\x48\x63\xc0",
                                                             *(char*)&temp, *((char*)&temp + 1),
                                                             *((char*)&temp + 2), *((char*)&temp + 3)); 
                         codebuf -> size += 15;  

                                                    // mov rcx, rbp
                                                    // sub rcx, temp 
                                                    // mov eax, [rcx] 
                         // movsx rax, eax
            }

            break;
        }
        case TYPE_FUNK:
        {
            int temp = Fun_array.var[key].vars.size * 4 + 16;
            sprintf(codebuf -> buffer + codebuf -> size, "\x48\x89\xef\x48\x81\xef%c%c%c%c",
                                                     *(char*)&temp, *((char*)&temp + 1),
                                                     *((char*)&temp + 2), *((char*)&temp + 3)); 
            codebuf -> size += 10; 
            
                    //mov rdi, rbp
                    //sub rdi, temp

            Get_SendArgs(codebuf, cur_node -> left, key, Fun_array.var[key].vars.size, functable);
            char zero = 0;
            int offset = Find_in_functable(functable, Fun_array.var[(int)cur_node -> value].name);
            sprintf(codebuf -> buffer + codebuf -> size, "\x48\x81\xed%c%c%c%c"
                    "\x48\x89\xec\x48\x83\xc4\x08"
                    "\xeb\x14\x48\x83\xc4\x08"
                    "\x48\xc7\x04\x24%c%c%c%c"
                    "\xc7\x04\x24%c%c%c%c"
                    "\xc3\xe8\xe7\xff\xff\xff\x48\x81\xc5%c%c%c%c",
                     *(char*)&temp, *((char*)&temp + 1),
                     *((char*)&temp + 2), *((char*)&temp + 3),

                     zero, zero, zero, zero,

                      *(char*)&offset, *((char*)&offset + 1),
                     *((char*)&offset + 2), *((char*)&offset + 3),

                     *(char*)&temp, *((char*)&temp + 1),
                    *((char*)&temp + 2), *((char*)&temp + 3)); 

            codebuf -> size += 48; 
            // sub rbp, Fun_array.var[key].vars.size * 4 + 16
            // mov rsp,rbp
            // add rsp,8
            // jmp call 
            // add rsp, 8 <------------+
            // mov qword[rsp], offset  |
            // ret                     |
            // call -------------------+
            // возвращаемое значение в rax    
            // возврат 
            // add rbp, temp

           

            break;
        }

        case TYPE_OPERATOR:                             
            switch ((int)cur_node -> value){
                case OP_MINUS:
                    RbxLax              
                    WRITE("\x48\x29\xd8", 3)    // sub rax, rbx  

                    break;

                case OP_PLUS:
                    RbxLax
                    WRITE("\x48\x01\xd8", 3)    // add rax, rbx
                    break;

                case OP_MUL:
                    RbxLax
                    WRITE("\x48\xf7\xeb\x48\xc1\xf8\x0a", 7)
                                                
                                             //imul rbx
                                             // sar rax, 10
                    break;

                case OP_DIV:
                    {
                        RbxLax
                    
                  /*shl rax, 10
                    cmp rax, 0
                    jg g
                    mov rdx, -1
                    jmp i
                g:  xor edx, edx
                i:  idiv rbx
                   */

                    char div[] = { 
              "\x48" "\xc1" "\xe0" "\x0a" "\x48" "\x83" "\xf8" "\x00" "\x7f" "\x09" "\x48"
               "\xc7" "\xc2" "\xff" "\xff" "\xff" 
                 "\xff" "\xeb" "\x02" "\x31" "\xd2" 
                 "\x48" "\xf7" "\xfb"  };

                         memcpy(codebuf -> buffer + codebuf -> size, div, sizeof(div) - 1 );
                         codebuf -> size += (sizeof(div) - 1);
 
                        break;
                    }

                case OP_POW:
                    {
                        GetE(codebuf, cur_node -> left, key, functable);

                        int temp = Fun_array.var[key].vars.size * 4 + 8;
                        sprintf(codebuf -> buffer + codebuf -> size, 
                            "\x9b\xdb\xe3\x48\x89\xe9\x48\x81\xe9%c%c%c%c"
                            "\x48\xc1\xe0\x0a\x48\x89\x01\xdf\x29"
                            "\xd9\xfa\x9b\xdf\x39\x48\x8b\x01",
                                                         *(char*)&temp, *((char*)&temp + 1),
                                                         *((char*)&temp + 2), *((char*)&temp + 3)); 
                         codebuf -> size += 30; 
                         // finit
                         // mov rcx, rbp
                         // sub rcx, temp
                         // shl rax, 10
                         // mov [rcx], rax
                         // fild  qword [rcx]
                         // fsqrt
                         // fwait
                         // fistp qword[rcx]
                         // mov rax, [rcx]
                        break;
                    }    
           
                default:
                    printf("GetE Unexpected operand %d %lf ",cur_node -> node_type, cur_node -> value);
                    break;
            }

            break;
        case TYPE_NUM: // пишем все в инт*1024
                {
                  int temp = (int) (cur_node -> value * 1024);
                  sprintf(codebuf -> buffer + codebuf -> size, "\xb8%c%c%c%c\x48\x63\xc0",
                                                        *(char*)&temp, *((char*)&temp + 1),
                                                        *((char*)&temp + 2), *((char*)&temp + 3)); 
                  codebuf -> size += 8; 
                    //mov eax, temp
                    //movsx rax, eax
                }
            break;
        default:
            break;
    }

}

void GetEq(codeBuf* codebuf, Node* cur_node, int key, Functable* functable){

    if (!cur_node)
        return;
    if (cur_node -> node_type == TYPE_EQ){ // inversive
        switch ((int)cur_node -> value){
            case OP_MORE:
                    RbxLax
                    WRITE("\x48\x39\xd8\x0f\x8e", 5) //cmp rax, rbx 
                    //jng 
   
                break;
            case OP_MORE_EQ:
                    RbxLax
                    WRITE("\x48\x39\xd8\x0f\x8c", 5) //cmp rax, rbx  
                    //jnge 
                break;
            case OP_LESS:
                    RbxLax
                    WRITE("\x48\x39\xd8\x0f\x8d", 5) //cmp rax, rbx 
                    //jnl
                break;
            case OP_LESS_EQ:
                    RbxLax
                    WRITE("\x48\x39\xd8\x0f\x8f", 5) //cmp rax, rbx 
                    //jnle  
                break;
            case OP_EQUAL:
                    RbxLax
                    WRITE("\x48\x39\xd8\x0f\x85", 5) //cmp rax, rbx 
                    //jne
                break;
            case OP_NOT_EQUAL:
                    RbxLax
                    WRITE("\x48\x39\xd8\x0f\x84", 5) //cmp rax, rbx 
                    //je 
                break;
            default:
                printf("Unexpected operand");
                break;
        }
    }
    else
        printf("Wrong type of node");
}

void GetIF(codeBuf* codebuf, Node* cur_node, int key, Functable* functable){
    codeBuf* codeIF = Create_codeBuf(MAXCODELEN / 4);
    codeBuf* codeELSE = Create_codeBuf(MAXCODELEN / 4);
    codeBuf* codeCondition = Create_codeBuf(MAXCODELEN / 4);
    
    GetEq(codeCondition, cur_node->left, key, functable);

    cur_node = cur_node->right;

    In_Function(codeIF, cur_node->left, key, functable);

    if (cur_node->right) {
        In_Function(codeELSE, cur_node->right, key, functable);
    }
  // вставить в конец блока if прыжок на конец блока else 
    int temp = codeELSE -> size;
    sprintf(codeIF -> buffer + codeIF -> size, "\xe9%c%c%c%c",
                                                *(char*)&temp, *((char*)&temp + 1),
                                                *((char*)&temp + 2), *((char*)&temp + 3)); 
    codeIF -> size += 5; 

 // вставить в конец блока condition прыжок на конец блока if 
    temp = codeIF -> size;
    sprintf(codeCondition -> buffer + codeCondition -> size, "%c%c%c%c",
                                                *(char*)&temp, *((char*)&temp + 1),
                                                *((char*)&temp + 2), *((char*)&temp + 3)); 
    codeCondition -> size += 4; 


    memcpy(codebuf -> buffer + codebuf -> size, codeCondition -> buffer, codeCondition -> size);
    codebuf -> size += codeCondition -> size;
    memcpy(codebuf -> buffer + codebuf -> size, codeIF -> buffer, codeIF -> size);
    codebuf -> size += codeIF -> size;                                                      
    memcpy(codebuf -> buffer + codebuf -> size, codeELSE -> buffer, codeELSE  -> size);
    codebuf -> size += codeELSE -> size;

    Destruct_codeBuf(codeELSE);
    Destruct_codeBuf(codeIF);
    Destruct_codeBuf(codeCondition);
}

void GetWH(codeBuf* codebuf, Node* cur_node, int key, Functable* functable){

    codeBuf* codeBody = Create_codeBuf(MAXCODELEN / 4);
    codeBuf* codeCondition = Create_codeBuf(MAXCODELEN / 4);
    
    GetEq(codeCondition, cur_node->left, key, functable);

    In_Function(codeBody, cur_node->right, key, functable);

 // jmp на проверку условия
    int temp = -(codeCondition -> size + 4 + codeBody -> size + 5);
    sprintf(codeBody -> buffer + codeBody -> size, "\xe9%c%c%c%c",
                                                *(char*)&temp, *((char*)&temp + 1),
                                                *((char*)&temp + 2), *((char*)&temp + 3)); 
    codeBody -> size += 5; 

    temp = codeBody -> size; // если условие не верное прыгни в конец
    sprintf(codeCondition -> buffer + codeCondition -> size, "%c%c%c%c",
                                                *(char*)&temp, *((char*)&temp + 1),
                                                *((char*)&temp + 2), *((char*)&temp + 3)); 
    codeCondition -> size += 4; 

    memcpy(codebuf -> buffer + codebuf -> size, codeCondition -> buffer, codeCondition -> size);
    codebuf -> size += codeCondition -> size;
    memcpy(codebuf -> buffer + codebuf -> size, codeBody -> buffer, codeBody -> size);
    codebuf -> size += codeBody -> size;     

    Destruct_codeBuf(codeBody);
    Destruct_codeBuf(codeCondition);
}

void In_Function(codeBuf* codebuf, Node* cur_node, int key, Functable* functable){
    if (!cur_node)
        return;


    if (cur_node->node_type == TYPE_SYST) {
        switch ((int) cur_node->value) {
            case OP_ASSIGN:
            {  
                GetE(codebuf, cur_node -> right  , key, functable);
                int temp = 0;
                if ((temp =  Find_in_curFun(key, Var_array.var[(int) cur_node->left->value].name)) + 1) {
                
                }
                else {
                    strcpy(Fun_array.var[key].vars.var[Fun_array.var[key].vars.size].name, Var_array.var[(int) cur_node->left->value].name);
                    temp = Fun_array.var[key].vars.size++;
                }
                temp = temp * 4 + 4;
                sprintf(codebuf -> buffer + codebuf -> size, 
                        "\x48\x89\xe9\x48\x81\xe9%c%c%c%c"
                        "\x89\x01",
                        *(char*)&temp, *((char*)&temp + 1),
                        *((char*)&temp + 2), *((char*)&temp + 3)); 
                codebuf -> size += 12; 
                
                //mov rcx, rbp 
                //sub rcx, temp
                // 
                // mov [rcx], eax
                break;
            }

            case OP_WHILE:
                GetWH(codebuf, cur_node, key, functable);
                break;

            case OP_IF:
                GetIF(codebuf, cur_node, key, functable);
                break;


            case OP_RETURN:

                GetE(codebuf, cur_node->left, key, functable);// rax
                WRITE("\x48\x89\xec\xc3", 4)
                //mov rsp, rbp
                 //ret
                break;

            case OP_GETOP:
                In_Function(codebuf, cur_node->left, key, functable);
                In_Function(codebuf, cur_node->right, key, functable);
                break;

            case OP_GET:
            {
                
                int temp = 0;

                if ((temp =  Find_in_curFun(key, Var_array.var[(int) cur_node->left->value].name)) + 1) {
                
                }
                else {
                    strcpy(Fun_array.var[key].vars.var[Fun_array.var[key].vars.size].name, Var_array.var[(int) cur_node->left->value].name);
                    temp = Fun_array.var[key].vars.size++;
                }
                temp = temp * 4 + 4;

                
                sprintf(codebuf -> buffer + codebuf -> size,
                "\x48\x89\xee\x48\x81\xee%c%c%c%c" 
                "\x48\x83\xee\x30"   
                        "\x48\x31\xc0\x48\x31\xff"                  
                        "\x48\x31\xd2\xb2\x20\x0f"
                        "\x05\x48\x89\xf1\x48\x83\xc1\x30"
                        "\x48\x89\xcc",
                        *(char*)&temp, *((char*)&temp + 1),
                        *((char*)&temp + 2), *((char*)&temp + 3)); 
                codebuf -> size += 37; 
                // mov rsi, rbp
                // sub rsi, temp
                // sub rsi, 48                
                // mov rax, 0
                // mov rdi, 0
                // mov rdx, 32
                // syscall
                // mov rcx, rsi
                // add rcx, 48
                // mov rsp, rcx

                    char zero = 0;
                    int offset = Find_in_functable(functable, "scanf");
                    sprintf(codebuf -> buffer + codebuf -> size, 
                    "\xeb\x14\x48\x83\xec\x08"
                    "\xc7\x44\x24\x04%c%c%c%c"
                    "\xc7\x04\x24%c%c%c%c"
                    "\xc3\xe8\xe7\xff\xff\xff",

                     zero, zero, zero, zero,

                      *(char*)&offset, *((char*)&offset + 1),
                     *((char*)&offset + 2), *((char*)&offset + 3)); 

                    codebuf -> size += 27; 

                    // jmp call
                    // sub rsp, 8 <------------+
                    // mov dword[rsp+4], 0     |
                    // mov dword[rsp], offset  |
                    // ret                     |
                    // call -------------------+
                    
                break;
            }

            case OP_PUT:
                {
                    GetE(codebuf, cur_node -> left, key, functable);

                     /*mov rsp, rbp
                    sub rsp, temp
                     */
                     
                    int temp = Fun_array.var[key].vars.size * 4;
                    sprintf(codebuf -> buffer + codebuf -> size, 
                        "\x48\x89\xec\x48\x81\xec%c%c%c%c",
                        *(char*)&temp, *((char*)&temp + 1),
                        *((char*)&temp + 2), *((char*)&temp + 3)); 
                     codebuf -> size += 10; 


                    char zero = 0;
                    int offset = Find_in_functable(functable, "printf");
                    sprintf(codebuf -> buffer + codebuf -> size, 
                    "\xeb\x14\x48\x83\xec\x08"
                    "\xc7\x44\x24\x04%c%c%c%c"
                    "\xc7\x04\x24%c%c%c%c"
                    "\xc3\xe8\xe7\xff\xff\xff",

                     zero, zero, zero, zero,

                      *(char*)&offset, *((char*)&offset + 1),
                     *((char*)&offset + 2), *((char*)&offset + 3)); 

                    codebuf -> size += 27; 

                    // jmp call
                    // sub rsp, 8 <------------+
                    // mov dword[rsp+4], 0     |
                    // mov dword[rsp], offset  |
                    // ret                     |
                    // call -------------------+


                break;
                }

            default:
                printf("Unexpected operand");
                break;
        }
    }
}


void Walk_around_the_tree(codeBuf* codebuf, Node* cur_node, Functable* functable) {
    if (!cur_node)
        return;

    int temp = 0;

    if (cur_node->node_type == TYPE_SYST) {
        switch ((int) cur_node->value) {
            /*case OP_ASSIGN:
                fprintf(flout, "PUSH bx\n");
                if (cur_node -> right -> value == (int) cur_node -> value)
                    fprintf(flout, "PUSH %d; num\n", (int) cur_node -> value);
                else
                    fprintf(flout, "PUSH %.3lf; num\n", cur_node -> value);

                if ((temp =  Find_in_GV( Var_array.var[(int) cur_node->value].name)) + 1) {
                    fprintf(flout, "PUSH %d\n", temp);
                }
                else {
                    strcpy(GVar_array.var[GVar_array.size].name, Var_array.var[(int) cur_node->left->value].name);
                    fprintf(flout, "PUSH %d\n", GVar_array.size++);
                }


                fprintf(flout, "PUSH bp\n");

                fprintf(flout, "ADD\n");
                fprintf(flout, "POP bx\n");
                fprintf(flout, "POP [bx]\n");
                fprintf(flout, "POP bx\n\n\n");
                break; */

            case OP_GETFUNK:

                Walk_around_the_tree(codebuf, cur_node->left, functable) ;
                Walk_around_the_tree(codebuf, cur_node->right, functable);
                break;


            default:
                printf("Unexpected operand");
                break;
        }


    } else if (cur_node->node_type == TYPE_FUNK) {

        if (functable -> size < functable -> maxsize){
            strcpy(functable -> func[functable->size].name, Fun_array.var[(int) cur_node->value].name);
            functable -> func[functable->size++].offset = codebuf -> size + 0x400080;
            if(strcmp(Fun_array.var[(int) cur_node->value].name, "main") == 0){
                int temp = datasize;
                sprintf(codebuf -> buffer + codebuf -> size, 
                        "\x48\x81\xec%c%c%c%c"
                        "\x48\x89\xe5"
                        "\x49\x89\xe6", 
                        *(char*)&temp, *((char*)&temp + 1),
                        *((char*)&temp + 2), *((char*)&temp + 3)); 
                codebuf -> size += 13;
                //sub rsp, datasize
                //mov rbp, rsp
                //mov r14, rsp
            }
        }

        if (cur_node->left)
            Get_GetArgs(codebuf, cur_node->left, (int) cur_node->value);

        In_Function(codebuf, cur_node->right, (int) cur_node->value, functable);
        char end[] = {"\xb8" "\x3c" "\x00" "\x00" "\x00" "\x48" "\x31" "\xff" "\x0f" "\x05"};
        memcpy(codebuf -> buffer + codebuf -> size, end, 10);
        codebuf -> size += 10;
        }

       
    }

