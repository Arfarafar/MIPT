#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>
#include <malloc.h>
#include <forward_list> 
#include <math.h>

#define opINIT(n, sem, op, flg) sops[n].sem_num = sem;					\
						    	sops[n].sem_op = op;					\
						    	sops[n].sem_flg = flg;					


#define opSEM(sem, op, flg)		opINIT(0, sem, op, flg)					\
	    						semop(semid, sops, 1);


int semid = 0;


typedef struct CELL
{
    int distance_passed;
    int heuristic_func;
    int raw;
    int col;
    struct CELL* parent;
    char path;
    char isborder;
    char underconsider;
} cell_t;

cell_t* START;
cell_t* END;


struct 
 {
 	int width;
 	int height;
 	cell_t* origin;
 } FIELD;

#define GET_FIELD_ELEM(raw, col) (FIELD.origin + (raw) * FIELD.width + col)


int Heuristic(cell_t* curent_cell){

	return abs(curent_cell -> col - END -> col) + abs(curent_cell -> raw - END -> raw);
}

std::forward_list<cell_t*>::const_iterator Find_WheretoInsert(std::forward_list<cell_t*> *Regard, cell_t* Elem){ 

    
    std::forward_list<cell_t*>::iterator it = Regard -> begin();
    std::forward_list<cell_t*>::iterator preit = Regard -> before_begin();

    while (it != Regard -> end()
           && (*it) -> heuristic_func <= Elem -> heuristic_func)
    {  
    	if(*it == Elem)
    		return Regard -> end();

        it++;
        preit++;
        
    }
    
    return preit;
}


void Serve(cell_t* curent_cell, cell_t* neighbour, std::forward_list<cell_t*> *Regard){
	
	if (neighbour -> isborder)
		return;
	if (neighbour -> distance_passed != 0 && neighbour -> distance_passed <= curent_cell -> distance_passed + 1)
		return;

	neighbour -> distance_passed = curent_cell -> distance_passed + 1;
	neighbour -> parent = curent_cell;	
			
	struct sembuf sops[2];

	int func = Heuristic(neighbour) + curent_cell -> distance_passed + 1;

	neighbour -> heuristic_func = func;

	opSEM(0, -1, SEM_UNDO)
	std::forward_list<cell_t*>::const_iterator it = Find_WheretoInsert(Regard, neighbour);		
	if( it != Regard -> end()) {
		
		Regard -> insert_after(it, neighbour);
		opSEM(1, 1, 0) 
		
	}
	opSEM(0, 1, SEM_UNDO)

	//neighbour -> underconsider--;
}


void* routine(std::forward_list<cell_t*> *Regard){
	
	struct sembuf sops[2];

	
	while(1 == 1){
		opINIT(0, 1, -1, 0) 
		opINIT(1, 0, -1, SEM_UNDO)
		semop(semid, sops, 2);

		cell_t* curent_cell = Regard -> front();
		if(curent_cell == END){
			opSEM(1, 1, 0)
			opSEM(0, 1, SEM_UNDO)
			return NULL;
		}
		Regard -> pop_front();

		opSEM(0, 1, SEM_UNDO)

		if(curent_cell -> raw != 0)
			Serve(curent_cell, GET_FIELD_ELEM(curent_cell -> raw - 1, curent_cell -> col),  Regard);
		if(curent_cell -> raw != FIELD.height - 1)
			Serve(curent_cell, GET_FIELD_ELEM(curent_cell -> raw + 1, curent_cell -> col),  Regard);
		if(curent_cell -> col != 0)
			Serve(curent_cell, GET_FIELD_ELEM(curent_cell -> raw, curent_cell -> col - 1),  Regard);
		if(curent_cell -> col != FIELD.width - 1)
			Serve(curent_cell, GET_FIELD_ELEM(curent_cell -> raw, curent_cell -> col + 1),  Regard);
		
	}
	
}



void Load_field(){

	FILE* flin = fopen("field.txt", "rt");
	
	fscanf(flin, "%dx%d\n", &FIELD.width, &FIELD.height);

	FIELD.origin = (cell_t*)calloc(FIELD.width * FIELD.height, sizeof(cell_t));

	for (int i = 0; i < FIELD.height; ++i)
	{
		for (int j = 0; j < FIELD.width; ++j){
			GET_FIELD_ELEM(i, j) -> isborder = getc(flin) - '0';
			GET_FIELD_ELEM(i, j) -> raw = i;
			GET_FIELD_ELEM(i, j) -> col = j;
		}
		getc(flin);
	}

	fclose(flin);

}


long Initilize_given_points(char* argv[]){

	char* extstr;
	long requredThread = strtol(argv[1], &extstr, 0);

	long col = strtol(argv[2], &extstr, 0);
	long raw = strtol(argv[3], &extstr, 0);

	if (GET_FIELD_ELEM(raw, col) -> isborder)
	{	
		printf("Impossible START position\n");
		exit(1);
	}
	START = GET_FIELD_ELEM(raw, col);

	col = strtol(argv[4], &extstr, 0);
	raw = strtol(argv[5], &extstr, 0);

	if (GET_FIELD_ELEM(raw, col) -> isborder)
	{	
		printf("Impossible END position\n");
		exit(1);
	}
	END = GET_FIELD_ELEM(raw, col);


	START -> distance_passed = 1;
	START -> path = 1;
	START -> heuristic_func = Heuristic(START);

	semid = semget(ftok(argv[0], 11), 2, IPC_CREAT | 0666);
	if( semid == -1){
		perror("semid");
		exit(1);
	}
	semctl(semid, 0, SETVAL, 1);
	semctl(semid, 1, SETVAL, 1);

	return requredThread;
}


void Print_the_Path(){

	FILE* flin = fopen("pathfield.txt", "wt");

	cell_t* iterator = END;
	while (iterator != START){
		iterator -> path = 1;
		iterator = iterator -> parent;
	}
	

	for (int i = 0; i < FIELD.height; ++i)
	{
		for (int j = 0; j < FIELD.width; ++j){
			iterator = GET_FIELD_ELEM(i, j);
			if (iterator -> isborder)
				putc('1', flin);
			else if (iterator -> path)
				putc('*', flin);
			else
				putc('0', flin);
			
		}
		putc('\n', flin);
	}

	fclose(flin);

}


int main(int argc, char* argv[]){

	if (argc != 6){
		printf("Wrong number of parametrs\n");
		return 1;
	}


	Load_field();
	
	//Print_the_Path();

	long requredThread = Initilize_given_points(argv);

	std::forward_list<cell_t*> Regard {START};

	pthread_t* id = (pthread_t*)malloc((requredThread)* sizeof(pthread_t));

	for(long i = 0; i < requredThread ; i++){

		if(!pthread_create(id+i, NULL , (void* (*)(void*))routine, (void*)&Regard)){
			
		}

		else{
			printf("Failure to start a thread number %ld\n interrupt", i);
			for (int g = 0; g < i; ++g)
			{
				pthread_detach(*(id+g));
			}
			return 0;
		}
	}
	
	

	for(int i = 0; i < requredThread; i++){
		pthread_join(*(id+i), NULL);
	}

	Print_the_Path();

	free(id);
	free(FIELD.origin);
		return 0;
}
