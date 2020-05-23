#include <stdio.h>
#include "list.h"
#include "my_hash_functions.h"


const int Table_Size = 40000;
const int Max_word_size = 40;

struct Hashtable{
    struct List** table;
    unsigned int (*hashfunc)(const char*, int);
    int size;
};
struct Word{
	char word[Max_word_size];
	unsigned int length;
};


Hashtable* Hashtable_Construct(int , unsigned int (*hashfunc)(const char*, int));
void Hashtable_Deconstruct(Hashtable* hashtable);
void Make_First_Line(FILE* flin);
bool Fill_Hash_Line(FILE* flout,Hashtable* hashtable, const char*);
void Make_Hashtable(Hashtable* hashtable, Word* allwords);
bool Find_all_words(Hashtable* hashtable, Word* allwords);

int Fill_all_words(Word* allwords, FILE* flin);


#define MAKE_NEW_LINE(whatfunc, name)                                                                            \
hashtable = Hashtable_Construct(Table_Size, (unsigned int(*)(const char*, int)) whatfunc);           \
if (!hashtable) {                                                                                                     \
    printf("cant create hashtable " );                                                                                        \
    abort();                                                            \
}         \
printf("Begin to make hashtable %s\n", name);      \
Make_Hashtable(hashtable, flin);         \
printf("end to make hashtable\n");\
Hashtable_Deconstruct(hashtable); printf("created\n");






int main() {
   // SetConsoleCP(1251);
    //SetConsoleOutputCP(1251);

   
    FILE* flin = fopen("text.txt", "rt");
    Word allwords[45403] = {};
    Fill_all_words(allwords, flin);
    
	
   	Hashtable* hashtable = Hashtable_Construct(Table_Size, (unsigned int(*)(const char*, int)) Recur);
    Make_Hashtable(hashtable, allwords);
   

	printf("end to make hashtable\n");
   



    fclose(flin);
    
    return 0;
}


int getFileLength (FILE* flin){
    fseek(flin, 0, SEEK_END);
    int length = ftell(flin);
    fseek(flin, 0 , SEEK_SET);
    return  length;
}

char * Create_buf(int len){
    return  (char*)calloc(len, sizeof(char));
}

int Fill_all_words(Word* allwords, FILE* flin){

	int len = getFileLength(flin);
    char* buf = Create_buf(len);
    len = fread(buf, 1, len, flin);

    int word_counter = 0;
    int position = 0;

    char* cur_pos = buf;

    while(sscanf( cur_pos , " %s%n", allwords[word_counter].word, &position) && (cur_pos + position - buf < len) ){ //necessary \n after last line
       
        allwords[word_counter].length = strlen(allwords[word_counter].word);

        cur_pos += position;
        word_counter++;
    }
    free(buf);
    return word_counter;
}



void Make_Hashtable(Hashtable* hashtable, Word* allwords){
	

    for (int i = 0; i < 45402; i++){ //necessary \n after last line
        char* current_word = allwords[i].word;
      	List* list = hashtable -> table[(*(hashtable -> hashfunc))(current_word, allwords[i].length) % Table_Size];
       
        if(!Find_By_Value(list , current_word)) {
            Insert_Tail(list, current_word);
        }
     // printf("%d\n", ++word_counter);
    }
    for (int i = 0; i< 1000; i++){
    	if(!Find_all_words(hashtable, allwords)){
    		printf("NOT ALL WORDS");
    		return;
    	}
    }

}


bool Find_all_words(Hashtable* hashtable, Word* allwords){
	char word[Max_word_size] = {};
    int position = 0;
    int wordlen = 0;
  
    for (int i = 0; i < 45402; i++){ //necessary \n after last line
        
      	List* list = hashtable -> table[(*(hashtable -> hashfunc))(allwords[i].word, allwords[i].length) % Table_Size];
        
        //printf("%s\n",cur_pos - wordlen );
        
       // printf("%d %s\n",(*(hashtable -> hashfunc))(cur_pos - wordlen, wordlen) % Table_Size, word );
       //List_Dump(list);
   
       		
            Insert_Tail(list, allwords[i].word);
       
    }
    return true;

}


Hashtable* Hashtable_Construct(int size, unsigned int (*hashfunc)(const char*, int)){

    Hashtable *new_hashtable = (Hashtable *) calloc(1, sizeof(struct Hashtable));
    do{
        if(!new_hashtable)
            break;
        new_hashtable -> size = size;
        new_hashtable -> table = (List **) calloc(size,
                                                sizeof(List *)); //указатель на указатель на лист (создали массив указателей на списки)
        if(!new_hashtable -> table)
            break;
        for (int i = 0; i < size; i++) {
            new_hashtable -> table[i] = Create_List(5);
            if (!new_hashtable->table[i])
                break;
        }
        new_hashtable->hashfunc = hashfunc;
        return new_hashtable;
    } while (false);

    Hashtable_Deconstruct(new_hashtable);
    return nullptr;
}

void Hashtable_Deconstruct(Hashtable* hashtable){
    assert(hashtable);

    if(hashtable -> table)
        for (int i = 0; i < hashtable -> size; i++) {
            Deconstruct_List(hashtable->table[i]);
        }
    free(hashtable -> table);
    free(hashtable);
}