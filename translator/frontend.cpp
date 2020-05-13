#include <stdio.h>
#include <cctype>
#include <strings.h>
#include "diff.h"
#include "treegrap.h"


Node* GetN();
Node* GetT();
Node* GetV();
Node* GetF();
Node* GetE();
Node* GetP();
Node* GetG(const Token* token);
Node* GetOP();
Node* GetWH();
Node* GetFunkCall();
Token* Lexical_analysis(Buf* buf);

const  Token* Global__cur_token = {};


int main(int argc, const char* argv[]) {

    char input[MAXLEN] = "code.txt";
    char output[MAXLEN] = "tree.txt";

    if (argv[1]) {
        strcpy (input, argv[1]);
        Getnames(input, output, "tree.txt");
    }
    printf (">>>> %s: input \"%s\", output \"%s\"\n", argv[0], input, output);

    FILE* flin = fopen(input, "r");
    assert(flin);
    Buf* buf = Create_Buf(flin);
    fclose(flin);

    Token* first_token = Lexical_analysis(buf);
    Tokengraph(first_token);
    Node* head_node = GetG(first_token);

    Treegraph(head_node);
    Buf_destruct(buf);

    FILE* fltree = fopen(output, "w");
    Print_tree_in_file(head_node, fltree);
    fclose(fltree);

    Deconstruct_Nodes(head_node);


    return 0;
}





#define NODETYPE(type)  Global__cur_token -> node_type == type
#define CHECKVAL(val)  Global__cur_token->value == val
#define RAISEERROR(funk)  printf( #funk" Lexical error in line %d pos %d\nPlease, check your code\n", Global__cur_token -> str_num, Global__cur_token -> pos_instr);

Node* GetN(){

    if (Global__cur_token -> node_type == TYPE_NUM) {
        Node* new_Node = CREATE_NODE_NUM(0);
        new_Node-> value = Global__cur_token->value;
        Global__cur_token++;
        return new_Node;
    }

    RAISEERROR(GetN)
    return nullptr;
}

Node* GetV(){
    if (Global__cur_token -> node_type == TYPE_VAR) {
        Node *new_Node = Create_new_Node(nullptr, nullptr, TYPE_VAR, Global__cur_token->value);
        Global__cur_token++;
        return new_Node;
    }

    printf("GETV Lexical error in line %d pos %d\nPlease, check your code/n", Global__cur_token -> str_num, Global__cur_token -> pos_instr);
    return nullptr;
}

Node* GetF(){
    if (Global__cur_token -> node_type == TYPE_OPERATOR) {
        Node *new_Node = Create_new_Node(nullptr,
                                         nullptr,
                                         TYPE_OPERATOR, Global__cur_token->value);
        Global__cur_token++;
        new_Node -> right = GetP();
        new_Node -> right -> parent = new_Node;
        return new_Node;
    }

    printf("GET F Lexical error in line %d pos %d\nPlease, check your code \n", Global__cur_token -> str_num, Global__cur_token -> pos_instr);
    return nullptr;
}


Node* GetP(){
    Node* val = nullptr;
    if (Global__cur_token -> node_type == TYPE_OPERATOR &&
        Global__cur_token -> value == OP_OPENBRACKET){
        Global__cur_token++;
        val = GetE();
        if(Global__cur_token -> node_type == TYPE_OPERATOR &&
           Global__cur_token -> value == OP_CLOSEBRACKET)
             Global__cur_token++;
        else
            RAISEERROR(GetP)
    } else if (Global__cur_token -> node_type == TYPE_NUM){
        val = GetN();
    }
    else if (Global__cur_token -> node_type == TYPE_VAR){
        val = GetV();
    }
    else if(Global__cur_token -> node_type == TYPE_OPERATOR &&  Global__cur_token -> value < 14 ){
        val = GetF();
    }
    else if (NODETYPE(TYPE_FUNK)){
        val = GetFunkCall();
    }
    else {
       RAISEERROR(GetP)
    }

    return val;
}

Node* GetPOW(){
    Node* val = GetP();
    while( Global__cur_token -> node_type == TYPE_OPERATOR && Global__cur_token -> value == OP_POW){
        Global__cur_token++;
        val = Create_new_Node(val, GetP(), TYPE_OPERATOR,  OP_POW);

    }
    return val;
}

Node* GetT(){
    Node* val = GetPOW();
    while( Global__cur_token -> node_type == TYPE_OPERATOR ){
        switch ((int)Global__cur_token++ -> value){
            case OP_DIV:
                val = Create_new_Node(val, GetPOW(), TYPE_OPERATOR,  OP_DIV);
                break;
            case OP_MUL:
                val = Create_new_Node(val, GetPOW(), TYPE_OPERATOR, OP_MUL) ;
                break;
            default:
                Global__cur_token--;
                return val;
        }

    }
    return val;
}

Node* GetPM(){
    Node* val = GetT();
    while( NODETYPE(TYPE_OPERATOR ) ){
        switch ((int)Global__cur_token++ -> value){
            case OP_PLUS:
                val = Create_new_Node(val, GetT(), TYPE_OPERATOR,  OP_PLUS);
                break;
            case OP_MINUS:
                val = Create_new_Node(val, GetT(), TYPE_OPERATOR, OP_MINUS) ;
                break;
            default:
                Global__cur_token--;
                return val;
        }

    }
    return val;
}

Node* GetE(){
    Node* val = nullptr;
    if (NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_D)){
        Global__cur_token++;
        if (NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_OPENBRACKET)) {
            Global__cur_token++;
            val = GetV();
            if (NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_CLOSEBRACKET)) {
                Global__cur_token++;
                return Create_new_Node(val, GetPM(), TYPE_OPERATOR, OP_D);
            }
            else
                RAISEERROR(GetE)
        } else
            RAISEERROR(GeetE)
            abort();
    }

        return GetPM();

}


Node* GetArgs(){
    Node* val = nullptr;
    if ( !(NODETYPE(TYPE_OPERATOR)) && !(CHECKVAL(OP_CLOSEBRACKET)) ){
        val = GetE();
        return Create_new_Node(GetArgs(), val, TYPE_SYST, OP_SENDARGS);
    }
    return val;
}

Node* GetFunkCall(){
    Node* val = nullptr;
    if (NODETYPE(TYPE_FUNK)) {
        double temp = Global__cur_token->value;
        Global__cur_token++;

        if (NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_OPENBRACKET)) {
            Global__cur_token++;
            val = GetArgs();
            if (NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_CLOSEBRACKET)) {
                Global__cur_token++;
                return Create_new_Node(val, nullptr, TYPE_FUNK, temp);
            }
        }
    }
    RAISEERROR(GetAssign)
    return val;

}


Node* GetAssign(){
    Node* val = GetV();
    if (NODETYPE(TYPE_SYST) && CHECKVAL(OP_ASSIGN)){
            Global__cur_token++;
            if (NODETYPE(TYPE_FUNK)) {
                return Create_new_Node(val, GetFunkCall(), TYPE_SYST, OP_ASSIGN);
            }
            else {
                return Create_new_Node(val, GetE(), TYPE_SYST, OP_ASSIGN);
            }
    }

    RAISEERROR(GetAssign)
    free(val);
    return nullptr;

}

Node* GetEQ(){
    Node* val = GetE();

     if (NODETYPE(TYPE_EQ)){
         double temp = Global__cur_token -> value;
         Global__cur_token++;
         return Create_new_Node(val, GetE(), TYPE_EQ, temp);
     }


        return val;

}


Node* GetWH(){
    Node* val = nullptr;
    if (NODETYPE(TYPE_SYST) && CHECKVAL(OP_WHILE)){
        Global__cur_token++;
    } else {
        RAISEERROR(GetWH)
        return val;
    }

    if (NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_OPENBRACKET)) {
        Global__cur_token++;
        val = GetEQ();
        if (NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_CLOSEBRACKET)) {

            Global__cur_token++;
            return Create_new_Node(val, GetOP(), TYPE_SYST, OP_WHILE);
        }
    }

        RAISEERROR(GetWH)
        return val;


}

Node* GetIFElse(){
    Node* val = GetOP();
    Node* val2 = nullptr;
    if (NODETYPE(TYPE_SYST) && CHECKVAL(OP_ELSE)){
        Global__cur_token++;
        val2 = GetOP();
    }
    return Create_new_Node(val, val2, TYPE_SYST, OP_IFELSE);
}


Node* GetIF(){
    Node* val = nullptr;
    if (NODETYPE(TYPE_SYST) && CHECKVAL(OP_IF)){
        Global__cur_token++;
    } else {
        RAISEERROR(GetIF)
        return val;
    }
     if (NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_OPENBRACKET)) {
         Global__cur_token++;
         val = GetEQ();
         if (NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_CLOSEBRACKET)) {

             Global__cur_token++;
             return Create_new_Node(val, GetIFElse(), TYPE_SYST, OP_IF);
         }
     }
         RAISEERROR(GetIF)
         return val;

}

Node* GetRet(){
    Node* val = nullptr;
    if(NODETYPE(TYPE_SYST) && CHECKVAL(OP_RETURN)){
        Global__cur_token++;
        val = GetE();
        return Create_new_Node(val, nullptr, TYPE_SYST, OP_RETURN);
    }
}

Node* GetIN(){
    Node* val = nullptr;
      if(NODETYPE(TYPE_SYST) && CHECKVAL(OP_GET  )){
          Global__cur_token++;
          val = GetV();
          return Create_new_Node(val, nullptr,   TYPE_SYST, OP_GET);
      }
      RAISEERROR(GetIN);
    return val;
}

Node* GetOUT(){
    Node* val = nullptr;
    if(NODETYPE(TYPE_SYST) && CHECKVAL(OP_PUT )){
        Global__cur_token++;
        val = GetP();
        return Create_new_Node(val, nullptr,   TYPE_SYST, OP_PUT);
    }
    RAISEERROR(GetOUT);
    return val;
}


Node* GetOP(){
    Node* val = nullptr;
    if (NODETYPE(TYPE_SYST) && CHECKVAL(OP_ENDBLOCK)){
        Global__cur_token++;
        return val;
    }
    if (NODETYPE(TYPE_SYST)) {
        switch ((int)Global__cur_token -> value){
            case OP_IF:
                val = GetIF();
                break;
            case OP_WHILE:
                val = GetWH();
                break;

            case OP_RETURN:
                val = GetRet();
                break;
            case OP_GET:
                val = GetIN();
                break;
            case OP_PUT:
                val = GetOUT();
                break;

            case OP_FOR:
            default:
                RAISEERROR(GetOP)
                return val;
        }
        return  Create_new_Node(val, GetOP(), TYPE_SYST, OP_GETOP);
    } else if (NODETYPE(TYPE_VAR)) {
        val = GetAssign();
        return Create_new_Node(val, GetOP(), TYPE_SYST, OP_GETOP);
    }

        RAISEERROR(GetOP)
        return val;


}


/*Node* GetArgs() {

    if (NODETYPE(TYPE_VAR)) {
        double temp = Global__cur_token++ -> value;
        return Create_new_Node(GetArgs(), nullptr, TYPE_VAR,  temp);
    }
    return nullptr;
}
*/

Node* GetGF (){
    Node* val = nullptr;
    if (Global__cur_token->value == OP_GLOBALEND && Global__cur_token -> node_type == TYPE_SYST){
        return val;
    }
    if (NODETYPE(TYPE_VAR)) {
        val = GetAssign();
    }
    else {
        if (NODETYPE(TYPE_FUNK)){
            double temp = Global__cur_token -> value;
            Global__cur_token++;
            if(NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_OPENBRACKET)) {
                Global__cur_token++;
                val = GetArgs();
                if(NODETYPE(TYPE_OPERATOR) && CHECKVAL(OP_CLOSEBRACKET)) {
                    Global__cur_token++;
                    val = Create_new_Node(val, GetOP(), TYPE_FUNK, temp);
                } else {
                    RAISEERROR(GetGF)
                    abort();
                }
            }
            else{
                RAISEERROR(GetGF)
                abort();
            }
        } else {
            RAISEERROR(GetGF)
            abort();
        }
    }
    return  Create_new_Node(val, GetGF(), TYPE_SYST, OP_GETFUNK);

}

Node* GetG(const Token* token){
    Global__cur_token = token;
    Node* val = GetGF();
    if (Global__cur_token->value != OP_GLOBALEND && Global__cur_token -> node_type != TYPE_OPERATOR)
        printf("Last value isnt Global-end value, do smth\n");
    return val;
}

#define SHIFT(pos)  buf -> cur_pos_in_file += pos;  buf -> cur_pos_in_line += pos;

int Count_spaces(char* buf){
    int pos = 0;
    if ( (sscanf(buf, "%*[ \t;.,]%n", &pos), pos > 0)){
        return pos;
    }
    else if ( (sscanf(buf, "%*[{]%n", &pos) , pos > 0 )){
        sscanf(buf, "%*[^}]%n", &pos);
        return pos + 1;
    }
    return  0;
}

Buf* Make_nextline(Buf* buf){
    buf -> cur_line += 1;
    buf -> cur_pos_in_file += 1;
    buf -> cur_pos_in_line = 1;
}

Buf* White_spaces(Buf* buf){
    int pos = 0;
    while ( buf -> cur_pos_in_file < buf -> len) {
        pos = Count_spaces(buf->buffer + buf->cur_pos_in_file);
        if (pos){
        SHIFT(pos)
        }
        else if ((sscanf(buf-> buffer + buf -> cur_pos_in_file, "%*[\n]%n", &pos) , pos > 0 )){
           buf = Make_nextline(buf);
        }
        else
            return buf;
        pos = 0;
    }
    return buf;
}


#define PRINTERROR printf("Lexical error in line %d pos %d in token %d\nPlease, check your code\n", buf -> cur_line, buf -> cur_pos_in_line, token_counter)
#define FILLTOKEN(type, val)  token_array[token_counter].value = val; \
token_array[token_counter].node_type = type;\
token_array[token_counter].str_num = buf -> cur_line;\
token_array[token_counter++].pos_instr = buf -> cur_pos_in_line;




Token* Lexical_analysis (Buf* buf){
    assert(buf);

    int maxTokens = 256;
    int token_counter = 0;
    int temp = 0;

    Token* token_array = (Token*)calloc(maxTokens, sizeof(Token));

    char fun[MAXLEN]  = "";
    int pos = 0;

    buf = White_spaces(buf);
    while ( buf -> cur_pos_in_file < buf -> len){

        char* nextchar = (buf -> buffer + buf -> cur_pos_in_file);

        if (token_counter >= maxTokens - 1) {
            maxTokens = maxTokens * 2;
            token_array = (Token*)realloc(token_array, maxTokens+1);
        }

        if (isdigit(*nextchar)){
            char* end = nextchar;
            double data = strtod(nextchar, &end);
            int shift = end - nextchar;
            if (!shift){
                PRINTERROR;
                abort();
            }


            FILLTOKEN(TYPE_NUM, data)

            SHIFT(shift);
        }
        else if (isalpha(*nextchar)){

            if (!sscanf(buf -> buffer + buf -> cur_pos_in_file, "%[^()+/*#?.\\/|,><!%&`~=; \n\t^{}:\"\']%n", fun, &pos)){
                PRINTERROR;
                abort();
            }

            if ((temp = Find_inOperands(fun)) + 1){
                FILLTOKEN(TYPE_OPERATOR, temp)
            }
            else if ((temp = Find_inSystem(fun)) + 1){
                FILLTOKEN(TYPE_SYST, temp)
            }
            else if ((temp = Find_inEq(fun)) + 1){
                FILLTOKEN(TYPE_EQ, temp)
            }
            else if (*(buf -> buffer + buf -> cur_pos_in_file + pos + Count_spaces(buf -> buffer + buf -> cur_pos_in_file + pos)) == '(') {
                temp = Find_inFunk(fun);

                if (!(temp + 1)) {
                    strcpy(Fun_array.var[Fun_array.size].name, fun);
                    FILLTOKEN(TYPE_FUNK, Fun_array.size++)
                } else {
                    FILLTOKEN(TYPE_FUNK, temp)
                }

            }



            else{
               temp = Find_inVars(fun);
                if(!(temp+1)) {
                    strcpy(Var_array.var[Var_array.size].name, fun);
                    FILLTOKEN(TYPE_VAR, Var_array.size++)
                } else {
                    FILLTOKEN(TYPE_VAR, temp)
                }
            }
            SHIFT(pos)
        }
        else{
            switch (*nextchar) {
                case '=':
                    if (*(nextchar + 1) == '='){
                        FILLTOKEN(TYPE_EQ, OP_EQUAL)
                       SHIFT(2)
                    }
                    else {
                        FILLTOKEN(TYPE_SYST, OP_ASSIGN)
                        SHIFT(1)
                    }
                    break;
                case '>':
                    if (*(nextchar + 1) == '='){
                        FILLTOKEN(TYPE_EQ, OP_MORE_EQ)
                        SHIFT(2)
                    }
                    else{
                        FILLTOKEN(TYPE_EQ, OP_MORE)
                        SHIFT(1)
                    }
                    break;
                case '<':
                    if (*(nextchar + 1) == '='){

                        FILLTOKEN(TYPE_EQ, OP_LESS_EQ)
                        SHIFT(2)
                    }
                    else{
                        FILLTOKEN(TYPE_EQ, OP_LESS)
                        SHIFT(1)
                    }
                    break;
                case '!':
                    if (*(nextchar + 1) == '='){
                        FILLTOKEN(TYPE_EQ, OP_NOT_EQUAL)
                        SHIFT(2)
                    } else PRINTERROR;
                    break;
                default:

                    if ((temp = Find_inOperands(nextchar)) + 1) {
                        FILLTOKEN(TYPE_OPERATOR, temp)
                        SHIFT(1);
                    } else {
                        PRINTERROR;
                    }
                    break;
            }


        }
        buf = White_spaces(buf);
    }
    FILLTOKEN(TYPE_SYST, OP_GLOBALEND)
    return token_array;
}