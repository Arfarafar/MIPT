#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>



#define opINIT(n, sem, op, flg) sops[n].sem_num = sem;					\
						    	sops[n].sem_op = op;					\
						    	sops[n].sem_flg = flg;					


#define opSEM(sem, op, flg)		opINIT(0, sem, op, flg)					\
	    						semop(semid, sops, 1);



#define PAGE_SIZE 320
const char SIZEOFINT = 4;
const char SIZEOFCHAR = 1;



int initSem(int semId, int semNum, int initVal){
    return semctl(semId, semNum, SETVAL, initVal);
}


void File_Translator(char* filename, int semid, void* addr){
	
	FILE* flin = fopen(filename, "rb");

	struct sembuf sops[4];


	opINIT(0, 0, 0, 0) //register yourself if no one else is here
	opINIT(1, 0, 1, SEM_UNDO)
	semop(semid, sops, 2);


	opINIT(0, 4, 0, 0); //to be sure there is no pair from previous translator
	semop(semid, sops, 1);

	
	initSem(semid, 3, 1);


	opINIT(0, 0, 1, SEM_UNDO); //before register you are alive
	opINIT(1, 3, -1, SEM_UNDO) //if you die always realease read
	semop(semid, sops, 2);
	

	opINIT(0, 1, -2, 0) //check your pair at the same step
	opINIT(1, 1, 2, 0)
	opINIT(2, 4, 1, SEM_UNDO); //register that you are alive
	semop(semid, sops, 3);
	

    int reallength = PAGE_SIZE - SIZEOFINT;
    
	while(reallength == PAGE_SIZE - SIZEOFINT){
		
		//reserve write
		opSEM(2, -1, 0)

		if(semctl(semid, 4, GETVAL) != 2){

			break;
		}
		
		reallength = fread(addr + SIZEOFINT, SIZEOFCHAR,  PAGE_SIZE - SIZEOFINT, flin);
		*(int*)addr = reallength;

		//realease read
		opSEM(3, 1, 0)

	}

	
}



void File_Receiver(int semid, void* addr){

	struct sembuf sops[4];

	opINIT(0, 1, 0, 0) //register yourself if no one else is here
	opINIT(1, 1, 1, SEM_UNDO)
	semop(semid, sops, 2);
	
	opINIT(0, 4, 0, 0); //to be sure there is no pair from previous receiver
	semop(semid, sops, 1);


	initSem(semid, 2, 2);

	opINIT(0, 1, 1, SEM_UNDO); //before register you are alive
	opINIT(1, 2, -1, SEM_UNDO) //if you die always realease write
	semop(semid, sops, 2);
	

	opINIT(0, 0, -2, 0) //check your pair has registered to
	opINIT(1, 0, 2, 0)
	opINIT(2, 4, 1, SEM_UNDO); //register that you are alive
	semop(semid, sops, 3);
	


	
	int indicator = PAGE_SIZE - SIZEOFINT;

	while(indicator == PAGE_SIZE - SIZEOFINT) {	
	
		//reserve read

		opSEM(3, -1, 0)
		
		if(semctl(semid, 4, GETVAL) != 2){
			if (*(int*)addr == PAGE_SIZE - SIZEOFINT)
				break;
		}

		indicator = *(int*)addr;
		write(STDOUT_FILENO, addr + SIZEOFINT , indicator);	

		//realease write
		opSEM(2, 1, 0)

	}


						
}



int main(int argc, char* argv[]){
	if (argc > 2){
		printf("Invalid argc\n");
		return 0;
	}

	int semid = semget(ftok(argv[0], 10), 5, IPC_CREAT | 0666);
	if( semid == -1){
		perror("semid");
		exit(1);
	}
	//0-file translator
	//1-file receiver
	//2-shmwrite
	//3-shmread
	//4-synchronyze

	int shmid = shmget(ftok(argv[0], 10), PAGE_SIZE, IPC_CREAT | 0666);
	if( shmid == -1){
		perror("shmid");
		exit(1);
	}
	void* addr = shmat(shmid, NULL, 0);


		switch (argc) {
			case 1:
				File_Receiver(semid, addr);
				break;
			case 2:
				File_Translator(argv[1], semid, addr);
				break;
			default:
				break;
			}
	
	return 0;
}
