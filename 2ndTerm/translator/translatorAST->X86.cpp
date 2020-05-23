#include <stdio.h>
#include "diff.h"

const int MAXCODELEN = 64000;
const int MAXFUNCS = 32;
const int BASE_LOAD_ADDRES = 0x400000;
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
int Find_in_functable(Functable* functable, const char* name);



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
    // Treegraph(head_node);
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


void CopyInBuf(codeBuf* codebuf, const char* sourse, unsigned int size){
    memcpy(codebuf -> buffer + codebuf -> size, sourse, size);
    codebuf -> size += size;
}

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
    // printf("offset_start %x",  );
    *(int*)(elf+ 0x18) = offset_start;
    *(int*)(elf+ 0x60) = *(int*)(elf+ 0x68) = 0x80 + progsize;
    memcpy(elfhead, elf, 0x80);
    return elfhead;
}




void ADD_printf(codeBuf* codebuf, Functable* functable){
    char print[] =  
        "\x48" "\x31" "\xc9" "\x31" "\xd2" "\x66" "\x31" "\xf6" "\x83" "\xf8" "\x00" "\x7f" "\x07" "\xf7" "\xd0" "\xff" 
        "\xc0" "\x66" "\xff" "\xc6" "\xbb" "\xe8" "\x03" "\x00" "\x00" "\x48" "\xf7" "\xeb" "\x48" "\xc1" "\xe8" "\x0a" 
        "\x31" "\xd2" "\xbb" "\x0a" "\x00" "\x00" "\x00" "\xf7" "\xf3" "\x89" "\xd3" "\x83" "\xc3" "\x30" "\x66" "\xc1" 
        "\xe3" "\x08" "\xff" "\xc1" "\x66" "\x53" "\x48" "\xff" "\xc4" "\x83" "\xf9" "\x03" "\x74" "\x07" "\x83" "\xf8" 
        "\x00" "\x75" "\xdd" "\xeb" "\x11" "\x31" "\xdb" "\x83" "\xc3" "\x2e" "\xc1" "\xe3" "\x08" "\xff" "\xc1" "\x66"
        "\x53" "\x48" "\xff" "\xc4" "\xeb" "\xca" "\x66" "\x83" "\xfe" "\x01" "\x75" "\x0f" "\x31" "\xdb" "\x83" "\xc3" 
        "\x2d" "\xc1" "\xe3" "\x08" "\xff" "\xc1" "\x66" "\x53" "\x48" "\xff" "\xc4" "\x31" "\xdb" "\x83" "\xc3" "\x0d" 
        "\xc1" "\xe3" "\x08" "\xff" "\xc1" "\x66" "\x53" "\x48" "\xff" "\xc4" "\x31" "\xdb" "\x83" "\xc3" "\x0a" "\xc1" 
        "\xe3" "\x08" "\xff" "\xc1" "\x66" "\x53" "\x48" "\xff" "\xc4" "\x48" "\x89" "\xca" "\xb8" "\x01" "\x00" "\x00" 
        "\x00" "\xbf" "\x01" "\x00" "\x00" "\x00" "\x48" "\x89" "\xe6" "\x48" "\x01" "\xcc" "\x0f" "\x05" "\xc3";
    char scan[] = 
        "\x48" "\x31" "\xd2" "\x66" "\x31" "\xff" "\x31" "\xc0" 
        "\xbb" "\x0a" "\x00" "\x00" "\x00" "\x80" "\x3e" "\x2d" 
        "\x75" "\x06" "\x48" "\xff" "\xc6" "\x66" "\xff" "\xc7" 
        "\x80" "\x3e" "\x0a" "\x74" "\x10" "\xf7" "\xe3" "\x8a" 
        "\x16" "\x01" "\xd0" "\x30" "\xd2" "\x83" "\xe8" "\x30"
        "\x48" "\xff" "\xc6" "\xeb" "\xeb" 
        "\x66" "\x83" "\xff" "\x01" "\x75" "\x04" "\xff" "\xc8" 
        "\xf7" "\xd0" "\xc1" "\xe0" "\x0a" "\x89" "\x01" "\xc3";

 CopyInBuf(codebuf, print,  sizeof(print) - 1);
 strcpy(functable -> func[0].name, "printf");
 functable -> func[0].offset = BASE_LOAD_ADDRES + 0x80;

 strcpy(functable -> func[1].name, "scanf");
 functable -> func[1].offset = BASE_LOAD_ADDRES + 0x80 + codebuf -> size;
 CopyInBuf(codebuf, scan,  sizeof(scan) - 1);

 functable->size = 2;
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

         CopyInBuf(codebuf, "\x49\x89\x3e" 		"\x49\x83\xc6\x08", 7); 
        					 // mov [r14], rdi		add r14, 8 
        cur_datasize += 8; 
        if(cur_datasize > datasize) 
                            datasize += 8;  
        GetE(codebuf, cur_node -> right, key, functable);    // rax
        cur_datasize -= 8; 
        CopyInBuf(codebuf, "\x49\x83\xee\x08"	"\x49\x8b\x3e"	, 7); 
        					// sub r14, 8 		 mov rdi, [r14]
        

        CopyInBuf(codebuf, "\x48\x83\xef\x04"	"\x89\x07", 		6);    
                   		   // sub rdi ,4 		mov  [rdi], eax
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

int Find_in_functable(Functable* functable, const char* name){
    for(int i = 0; i < functable -> size; i++ ){
        if (strcmp(name, functable -> func[i].name) == 0){
            return functable -> func[i].offset;
        }
    }
    return 0;
}

#define  RbxLax     GetE(codebuf, cur_node -> right, key, functable) ; \
                    CopyInBuf(codebuf, "\x49\x89\x06"	"\x49\x83\xc6\x08", 7); \
                    cur_datasize += 8; \
                    if(cur_datasize > datasize) \
                            datasize += 8; \
                    GetE(codebuf, cur_node -> left, key, functable);    \
                    cur_datasize -= 8; \
                    CopyInBuf(codebuf, "\x49\x83\xee\x08"	"\x49\x8b\x1e", 7); 
// rax  
                    // mov [r14], rax 
                    // add r14, 8
// rax   
                    // sub r14, 8
                    // mov rbx, [r14]
 

void GetE(codeBuf* codebuf, Node* cur_node , int key, Functable* functable){
    if(!cur_node){
        return;
    }
    

    switch(cur_node -> node_type){
        case TYPE_VAR:
        {
            int temp = 0;
            // переменная ищется в текущем стековом фрейме и кладется в rax 
            if (temp =  Find_in_curFun(key, Var_array.var[(int) cur_node->value].name) + 1) {
                temp *= 4;
                char var[] = 
                "\x48\x89\xe9"						// mov rcx, rbp
                "\x48\x81\xe9""\x00\x00\x00\x00" 	// sub rcx, temp 
                "\x8b\x01"							// mov eax, [rcx] 
                "\x48\x63\xc0";						// movsx rax, eax
                *(int*)(var+6) = temp; 
                CopyInBuf(codebuf, var,  15);
                                                 
            }

            break;
        }
        case TYPE_FUNK:
        {
            int temp = Fun_array.var[key].vars.size * 4 + 16;
            char frame[] = 
            "\x48\x89\xef"						// mov rdi, rbp
            "\x48\x81\xef""\x00\x00\x00\x00"; 	// sub rdi, temp
            *(int*)(frame+6) = temp; 
            CopyInBuf(codebuf, frame,  10);

            Get_SendArgs(codebuf, cur_node -> left, key, Fun_array.var[key].vars.size, functable);

            int offset = Find_in_functable(functable, Fun_array.var[(int)cur_node -> value].name);
            char call[] =  
            "\x48\x81\xed\x00\x00\x00\x00" 		// sub rbp, Fun_array.var[key].vars.size * 4 + 16
            "\x48\x89\xec" 						// mov rsp,rbp
            "\x48\x83\xc4\x08" 					// add rsp,8
            "\xeb\x14"							// jmp call 
			"\x48\x83\xc4\x08" 					// add rsp, 8 <------------+
			"\x48\xc7\x04\x24\x00\x00\x00\x00" 	// mov qword[rsp], 0	   |
            "\xc7\x04\x24\x00\x00\x00\x00"		// mov dword[rsp], offset  |
            "\xc3"								// ret                     |
            "\xe8\xe7\xff\xff\xff" 				// call -------------------+
            "\x48\x81\xc5\x00\x00\x00\x00"; 	// add rbp, temp
                     *(int*)(call+3) = temp;
                     *(int*)(call+44) = temp;
                     *(int*)(call+31) = offset;

           CopyInBuf(codebuf, call,  48);
            
            break;
        }

        case TYPE_OPERATOR:                             
            switch ((int)cur_node -> value){
                case OP_MINUS:
                    RbxLax              
                    CopyInBuf(codebuf, "\x48\x29\xd8", 3);    // sub rax, rbx  

                    break;

                case OP_PLUS:
                    RbxLax
                    CopyInBuf(codebuf, "\x48\x01\xd8", 3);    // add rax, rbx
                    break;

                case OP_MUL:
                    RbxLax
                    CopyInBuf(codebuf, "\x48\xf7\xeb" "\x48\xc1\xf8\x0a", 7);
                                                
                                             // imul rbx
                                             // sar rax, 10
                    break;

                case OP_DIV:
                    {
                        RbxLax

                    char div[] =  
                    "\x48\xc1\xe0\x0a" 				// shl rax, 10
                    "\x48\x83\xf8\x00" 				// cmp rax, 0
                    "\x7f\x09" 						// jg g
                    "\x48\xc7\xc2\xff\xff\xff\xff" 	// mov rdx, -1
                    "\xeb\x02" 						// jmp i
                    "\x31\xd2" 						// g:  xor edx, edx
                    "\x48\xf7\xfb"; 				// i:  idiv rbx

                         CopyInBuf(codebuf, div,  sizeof(div)-1);
 
                        break;
                    }

                case OP_POW:
                    {
                        GetE(codebuf, cur_node -> left, key, functable);

                        int temp = Fun_array.var[key].vars.size * 4 + 8;
                         char pow[] = 
                            "\x48\x89\xe9" 					// mov rcx, rbp
                            "\x48\x81\xe9\x00\x00\x00\x00" 	// sub rcx, temp
                            "\x48\xc1\xe0\x0a" 				// shl rax, 10
                            "\x48\x89\x01" 					// mov [rcx], rax
                            "\xdf\x29" 						// fild  qword [rcx]
                            "\xd9\xfa" 						// fsqrt
                            "\x9b"  						// fwait
                            "\xdf\x39" 						// fistp qword[rcx]
                            "\x48\x8b\x01";  				// mov rax, [rcx]
                            *(int*)(pow + 9) = temp;
                            CopyInBuf(codebuf, pow,  27); 
                   
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
                  char num[] = 
                  "\xb8\x00\x00\x00\x00" // mov eax, temp
                  "\x48\x63\xc0";		// movsx rax, eax

                  *(int*)(num + 1) = temp;
                  CopyInBuf(codebuf, num,  8); 
                    

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
                    CopyInBuf(codebuf, "\x48\x39\xd8\x0f" "\x8e", 5); // cmp rax, rbx 
                    												 // jng 
   
                break;
            case OP_MORE_EQ:
                    RbxLax
                    CopyInBuf(codebuf, "\x48\x39\xd8\x0f" "\x8c", 5); // cmp rax, rbx  
                    												// jnge 
                break;
            case OP_LESS:
                    RbxLax
                    CopyInBuf(codebuf, "\x48\x39\xd8\x0f" "\x8d", 5); // cmp rax, rbx 
                    												// jnl
                break;
            case OP_LESS_EQ:
                    RbxLax
                    CopyInBuf(codebuf, "\x48\x39\xd8\x0f" "\x8f", 5); // cmp rax, rbx 
                    												// jnle  
                break;
            case OP_EQUAL:
                    RbxLax
                    CopyInBuf(codebuf, "\x48\x39\xd8\x0f" "\x85", 5); // cmp rax, rbx 
                    												// jne
                break;
            case OP_NOT_EQUAL:
                    RbxLax
                    CopyInBuf(codebuf, "\x48\x39\xd8\x0f" "\x84", 5); // cmp rax, rbx 
                    												// je 
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
    char IF[] = "\xe9\x00\x00\x00\x00";   // jmp                                        
    *(int*)(IF + 1) = temp;
    CopyInBuf(codeIF, IF,  5); 

  
  // вставить в конец блока condition прыжок на конец блока if 
    temp = codeIF -> size;
    CopyInBuf(codeCondition, (char*)&temp,  4);  



    CopyInBuf(codebuf   , codeCondition -> buffer  , codeCondition -> size);
    CopyInBuf(codebuf   , codeIF -> buffer         , codeIF -> size);
    CopyInBuf(codebuf   , codeELSE -> buffer       , codeELSE  -> size);

    Destruct_codeBuf(codeELSE);
    Destruct_codeBuf(codeIF);
    Destruct_codeBuf(codeCondition);
}

void GetWH(codeBuf* codebuf, Node* cur_node, int key, Functable* functable){

    codeBuf* codeBody = Create_codeBuf(MAXCODELEN / 4);
    codeBuf* codeCondition = Create_codeBuf(MAXCODELEN / 4);
    
    GetEq(codeCondition, cur_node->left, key, functable);
    In_Function(codeBody, cur_node->right, key, functable);

 // jmp на проверку условия из конца цикла
    int temp = -(codeCondition -> size + 4 + codeBody -> size + 5);
    char IF[] = "\xe9\x00\x00\x00\x00";           // jmp                                
    *(int*)(IF + 1) = temp;
    CopyInBuf(codeBody, IF,  5);

    temp = codeBody -> size; // если условие не верное перепрыгни цикл
    CopyInBuf(codeCondition, (char*)&temp,  4);  
 

    CopyInBuf(codebuf   , codeCondition -> buffer   , codeCondition -> size);
    CopyInBuf(codebuf   , codeBody -> buffer        , codeBody -> size);     

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
                char var[] = 
                        "\x48\x89\xe9" 					// mov rcx, rbp 
                        "\x48\x81\xe9\x00\x00\x00\x00" 	// sub rcx, temp
                        "\x89\x01"; 					// mov [rcx], eax
                 *(int*)(var + 6) = temp;
                  CopyInBuf(codebuf, var,  12);  
                
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
                CopyInBuf(codebuf,  "\x48\x89\xec" 	"\xc3", 4);
                				  // mov rsp, rbp 	// ret
                
                
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

                
                char get[] =    "\x48\x89\xee" 					// mov rsi, rbp
                                "\x48\x81\xee\x00\x00\x00\x00"  // sub rsi, temp
                                "\x48\x83\xee\x30" 				// sub rsi, 48  
                                "\x48\x31\xc0"					// xor rax, rax
                                "\x48\x31\xff" 					// xor rdi, rdi                 
                                "\x48\x31\xd2\xb2\x20" 			// mov rdx, 32
                                "\x0f\x05" 						// syscall
                                "\x48\x89\xf1" 					// mov rcx, rsi
                                "\x48\x83\xc1\x30"  			// add rcx, 48
                                "\x48\x89\xcc" ;    			// mov rsp, rcx
                *(int*)(get + 6) = temp;
                CopyInBuf(codebuf, get,  37);  
                
                
                              
                
                
                
                
                
                
                

                    int offset = Find_in_functable(functable, "scanf");
                    
                    char call[] =  
                    "\xeb\x14" 							// jmp call
                    "\x48\x83\xec\x08"					// sub rsp, 8 <------------+
                    "\xc7\x44\x24\x04\x00\x00\x00\x00" 	// mov dword[rsp+4], 0     |
                    "\xc7\x04\x24\x00\x00\x00\x00" 		// mov dword[rsp], offset  |
                    "\xc3" 								// ret                     |
                    "\xe8\xe7\xff\xff\xff"; 			// call -------------------+
                     *(int*)(call + 17) = offset;

                    CopyInBuf(codebuf, call,  27);
                    
                    
                    
                    
                    
                    
                    
                break;
            }

            case OP_PUT:
                {
                    GetE(codebuf, cur_node -> left, key, functable);

                    int temp = Fun_array.var[key].vars.size * 4;
                    char put[] =  "\x48\x89\xec"  					// mov rsp, rbp
                    			  "\x48\x81\xec\x00\x00\x00\x00"; 	// sub rsp, temp
                    *(int*)(put + 6) = temp;
                    CopyInBuf(codebuf, put,  10);

                    
                    int offset = Find_in_functable(functable, "printf");
                    
                    char call[] =  
                    "\xeb\x14" 							// jmp call
                    "\x48\x83\xec\x08"					// sub rsp, 8 <------------+
                    "\xc7\x44\x24\x04\x00\x00\x00\x00" 	// mov dword[rsp+4], 0     |
                    "\xc7\x04\x24\x00\x00\x00\x00" 		// mov dword[rsp], offset  |
                    "\xc3" 								// ret                     |
                    "\xe8\xe7\xff\xff\xff"; 			// call -------------------+
                     *(int*)(call + 17) = offset;

                    CopyInBuf(codebuf, call,  27);


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
            functable -> func[functable->size++].offset = codebuf -> size + BASE_LOAD_ADDRES + 0x80;
            if(strcmp(Fun_array.var[(int) cur_node->value].name, "main") == 0){
                int temp = datasize;
                
                char main[] =  	"\x48\x81\xec\x00\x00\x00\x00" 	// sub rsp, datasize
                				"\x48\x89\xe5" 					// mov rbp, rsp
                				"\x49\x89\xe6" 					// mov r14, rsp
                				"\x9b\xdb\xe3"; 				// finit
                     *(int*)(main+3) = temp;

                    CopyInBuf(codebuf, main,  16);
                
            }
        }

        if (cur_node->left)
            Get_GetArgs(codebuf, cur_node->left, (int) cur_node->value);

        In_Function(codebuf, cur_node->right, (int) cur_node->value, functable);
        char end[] = "\xb8\x3c\x00\x00\x00" 
        			"\x48\x31\xff" 
        			"\x0f\x05";
        CopyInBuf(codebuf, end,  10);
        }

       
    }

