
#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>
#include <time.h>
#include <string.h>

#include <sys/sysinfo.h>
#include <sched.h>
#include <sys/mman.h>



#define BASE_PATH "/sys/devices/system/cpu/cpu"
#define TOPOLOGY_PATH_ONLINE "/sys/devices/system/cpu/online"
#define TOPOLOGY_PATH_CpuX_coreID "%d/topology/core_id"
#define MAX_CORE 128
#define MAX_CPU MAX_CORE*2
#define MAX_PATHLEN 128

#define MATRIXSIZE 2048 
//2048*2048*4 = 16MB

#define PAGESIZE 4096
#define TYPE long 
#define D 32

//D: 32*32*sizeof(long int) = 4096




typedef struct
{
	int threadnum;
	int maxTreads;
	TYPE* A;
	TYPE* B;
	TYPE* SUM;
} arg_t;



void init(int size, TYPE* A){  //init matrix with random value

	
	for(int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			A[i*size + j] = rand()%1000;
		}
	} 

}


void SetMatrix(TYPE** A, TYPE** B, TYPE** SUM, int size){

	*A = (TYPE*) memalign( PAGESIZE, size * size * sizeof(TYPE) );
	*B = (TYPE*) memalign( PAGESIZE, size * size * sizeof(TYPE) );

	init(size, *A);
	init(size, *B);


	*SUM = (TYPE*) memalign( PAGESIZE, size*size*sizeof(TYPE)) ;
	memset(*SUM, 0, size*size*sizeof(TYPE)); 

}



void naive_one(){

	FILE* fin = fopen("naive_one.txt", "w");
	if (fin == NULL) {
		return;
	}


	for (int matr = 32; matr <= MATRIXSIZE; matr += 32){ 

		TYPE* A = NULL, *B = NULL, *SUM = NULL;
		SetMatrix(&A, &B, &SUM, matr);

		struct timespec t0;
		struct timespec t1;

		clock_gettime (CLOCK_REALTIME, &t0);


		//перемножение по стандартному тройному циклу 
		for(int i = 0; i < matr; i++)
			for (int k = 0; k < matr; k++)
				for (int j = 0; j < matr; j++)
					SUM[i* matr + k] += A[i*matr + j]*B[j*matr + k];
			

		

		clock_gettime (CLOCK_REALTIME, &t1);

		free(A);
		free(B);
		free(SUM);

		printf("потоков 1, размер матрицы %d, время умножения %lf\n", matr, t1.tv_sec-t0.tv_sec + ((t1.tv_nsec-t0.tv_nsec) / 1000) * 0.000001);
		fprintf(fin, "%d,%lf\n", matr, t1.tv_sec-t0.tv_sec + ((t1.tv_nsec-t0.tv_nsec) * 0.000000001));
	}
	fclose(fin);
}


//---------------------------------------------------------------------------------------------------------------------------------------------





void* routineNaive(void* arg){

	int delta = (MATRIXSIZE - 1)/ ((arg_t*)arg) -> maxTreads + 1;
	int start = ((arg_t*)arg)->threadnum * delta;
	int end = (start + delta) > MATRIXSIZE ? MATRIXSIZE : (start + delta);
	
	for(int i = start; i < end; i++)
		for (int k = 0; k < MATRIXSIZE; k++)
			for (int j = 0; j < MATRIXSIZE; j++)
					((arg_t*)arg)->SUM[i* MATRIXSIZE + k] += ((arg_t*)arg)->A[i*MATRIXSIZE + j]*((arg_t*)arg)->B[j*MATRIXSIZE + k];
			
	free(arg);
	return NULL;
}



void naive_many(){

	FILE* fin = fopen("naive_many.txt", "w");
	if (fin == NULL) {
		return;
	}


	for (int numThread = 1; numThread < 33; numThread++)
	{	

		TYPE* A = NULL, *B = NULL, *SUM = NULL;
		SetMatrix(&A,&B,&SUM, MATRIXSIZE);

		pthread_t* id = (pthread_t*)malloc((numThread)* sizeof(pthread_t));

		struct timespec t0;
		struct timespec t1;

		clock_gettime (CLOCK_REALTIME, &t0);
		
		for(long i = 0; i < numThread ; i++){

			arg_t* arg = (arg_t*) malloc (sizeof(arg_t));
			arg -> threadnum = i, arg -> maxTreads = numThread, arg -> A = A, arg -> B = B, arg -> SUM = SUM;

			if(!pthread_create(id+i, NULL , routineNaive, (void*)arg)){
				
			}

			else{
				printf("Failure to start a thread number %ld,\n interrupt\n", i);
				for (int g = 0; g < i; ++g)
				{
					pthread_detach(*(id+g));
				}
				
			}

		}


		for(int i = 0; i < numThread; i++){
			
			pthread_join(*(id+i), NULL);

		}

		clock_gettime (CLOCK_REALTIME, &t1);

		free(id);
		free(A);
		free(B);
		free(SUM);

		printf("потоков %d, время умножения %lf\n", numThread, t1.tv_sec-t0.tv_sec + ((t1.tv_nsec-t0.tv_nsec) / 1000) * 0.000001);
		fprintf(fin, "%d,%lf\n", numThread, t1.tv_sec-t0.tv_sec + ((t1.tv_nsec-t0.tv_nsec) * 0.000000001));
	}

	fclose(fin);
}


//--------------------------------------------------------------------------------------------------------------------------------------------------


void block_one(){

	FILE* fin = fopen("block_one.txt", "w");
	if (fin == NULL) {
		return;
	}

	for (int matr = D; matr <= MATRIXSIZE; matr += D){ 

			TYPE* A = NULL, *B = NULL, *SUM = NULL;
			SetMatrix(&A,&B,&SUM,matr); //матрицы хранятся поблочно, блоками по 32х32
			

			struct timespec t0;
			struct timespec t1;

			clock_gettime (CLOCK_REALTIME, &t0);


			
			for (int di = 0; di < matr; di += D)
				for (int dk = 0; dk < matr; dk += D){
                    int dj = 0;
					for (; dj < matr-D; dj += D){
                        __builtin_prefetch(SUM + di*matr + (dj+D)*D, 0, 2);
                        __builtin_prefetch(B + dk*matr + (dj+D)*D, 0, 2);
						for (int i = 0; i < D; i++)
						for (int k = 0; k < D; k++)
						for (int j = 0; j < D; j++)
							SUM[ di*matr + dj*D + i*D + j] += A[ di*matr + dk*D + i*D + k] * B[ dk*matr + dj*D + k*D + j];
			        }
                    for (int i = 0; i < D; i++)
						for (int k = 0; k < D; k++)
						for (int j = 0; j < D; j++)
							SUM[ di*matr + dj*D + i*D + j] += A[ di*matr + dk*D + i*D + k] * B[ dk*matr + dj*D + k*D + j];
                }

			clock_gettime (CLOCK_REALTIME, &t1);

			free(A);
			free(B);
			free(SUM);

		    printf("потоков 1, размер матрицы %d, время умножения %lf\n", matr, t1.tv_sec-t0.tv_sec + ((t1.tv_nsec-t0.tv_nsec) / 1000) * 0.000001);
			fprintf(fin, "%d,%lf\n", matr, t1.tv_sec-t0.tv_sec + ((t1.tv_nsec-t0.tv_nsec) * 0.000000001));
		}

		fclose(fin);
}



//---------------------------------------------------------------------------------------------------------------------------------------------------


typedef struct
{
	int core_id;
	int cpuX[2];
} core_t;

struct 
{
	core_t cores[MAX_CORE];
	int size;
	
} CORES = {};


void setAffinity (long threadnum){
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(CORES.cores[threadnum % CORES.size].cpuX[threadnum / CORES.size % 2], &set);
	sched_setaffinity(0, sizeof(cpu_set_t), &set);
}


void Parse_topology(){

	FILE* flin = fopen(TOPOLOGY_PATH_ONLINE, "rt");

	int begin, end, argc, core_id;
	char path[MAX_PATHLEN] = BASE_PATH;
	int baselen = sizeof(BASE_PATH) - 1 ;

	while ((argc = fscanf(flin, " %d-%d", &begin, &end))!= EOF){
		getc(flin);
		if (argc == 1){
			end = begin;
		}
		
		for (int cpu_n = begin; cpu_n <= end; ++cpu_n){

			sprintf((path+baselen), TOPOLOGY_PATH_CpuX_coreID, cpu_n);
			FILE* fcore = fopen(path, "rt");
			
			fscanf(fcore, "%d", &core_id);

			int i = 0;// CORES.last_notfillied;
			for( ; i < CORES.size; i++){
				if(CORES.cores[i].core_id == core_id){
					CORES.cores[i].cpuX[1] = cpu_n;
					break;
				}
			}
			if (i == CORES.size){
				
				CORES.size++;
				CORES.cores[i].core_id = core_id;
				CORES.cores[i].cpuX[0] = cpu_n;
				CORES.cores[i].cpuX[1] = cpu_n;
			}
			fclose(fcore);
		}
		
	}

	fclose(flin);

}

TYPE* A = NULL, *B = NULL, *SUM = NULL;
int numThread = 1;

void* routineBlock(void* threadnum){

	setAffinity((long)threadnum);

	int delta = (MATRIXSIZE / D - 1) / numThread + 1;
	int start = (long)threadnum * delta;
	int end = (start + delta)*D > MATRIXSIZE ? MATRIXSIZE : (start + delta)*D;
	
	for (int di = start*D; di < end; di += D)
		for (int dk = 0; dk < MATRIXSIZE; dk += D){
            int dj = 0;
			for (; dj < MATRIXSIZE; dj += D){
                //__builtin_prefetch(SUM + di*MATRIXSIZE + (dj+D)*D, 0, 2);
                //__builtin_prefetch(B + dk*MATRIXSIZE + (dj+D)*D, 0, 2);
				for (int i = 0; i < D; i++)
				for (int k = 0; k < D; k++)
				for (int j = 0; j < D; j++)
					SUM[ di*MATRIXSIZE + dj*D + i*D + j] += A[ di*MATRIXSIZE + dk*D + i*D + k] * B[ dk*MATRIXSIZE + dj*D + k*D + j];
            }
       }

	return NULL;
}



void block_many(){

	FILE* fin = fopen("block_many.txt", "w");
	if (fin == NULL) {
		return;
	}

	for (; numThread < 33; numThread++)
	{	

		
		SetMatrix(&A,&B,&SUM, MATRIXSIZE);
		Parse_topology();

		pthread_t* id = (pthread_t*)malloc((numThread)* sizeof(pthread_t));

		struct timespec t0;
		struct timespec t1;

		clock_gettime (CLOCK_REALTIME, &t0);
		
		for(long i = 0; i < numThread ; i++){
		

			if(!pthread_create(id+i, NULL , routineBlock, (void*)i)){
				
			}

			else{
				printf("Failure to start a thread number %ld,\n interrupt\n", i);
				for (int g = 0; g < i; ++g)
				{
					pthread_detach(*(id+g));
				}
				
			}

		}


		for(int i = 0; i < numThread; i++){
			
			pthread_join(*(id+i), NULL);

		}

		clock_gettime (CLOCK_REALTIME, &t1);

		free(id);
		free(A);
		free(B);
		free(SUM);

		fprintf(fin, "%d,%lf\n", numThread, t1.tv_sec-t0.tv_sec + ((t1.tv_nsec-t0.tv_nsec) * 0.000000001));
		printf("потоков %d, размер матрицы %d, время умножения %lf\n", numThread, MATRIXSIZE, t1.tv_sec-t0.tv_sec + ((t1.tv_nsec-t0.tv_nsec) / 1000) * 0.000001);
	}

	fclose(fin);
}






int main(int argc, char** argv)
{

	block_many();
	block_one();
    naive_many();
	
    naive_one();

	return 0;
		
}

