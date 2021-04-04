#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>
#include <malloc.h>
#include <sys/mman.h>
#include <math.h>

#define BASE_PATH "/sys/devices/system/cpu/cpu"
#define TOPOLOGY_PATH_ONLINE "/sys/devices/system/cpu/online"
#define TOPOLOGY_PATH_CpuX_coreID "%d/topology/core_id"
#define MAX_CORE 128
#define MAX_CPU MAX_CORE*2
#define MAX_PATHLEN 128

long requredThread = 0;
const double UPPER_LIMIT = 15000.0;
const double LOWER_LIMIT = 0.0;
const double accuracy = 0.00001;


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



static inline double func (double x){
	return sqrt(x);
}

void integral(double* dest, long threadnum){

	*dest = 0.0;
	double delta  = (UPPER_LIMIT - LOWER_LIMIT) / requredThread;
	double a = delta * threadnum;
	double b = a + delta;

	for (double x = a; x < b; x += accuracy){
		*dest += func(x)*accuracy;
	}

}

void setAffinity (long threadnum){
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET((CORES.cores[threadnum % CORES.size].cpuX[threadnum / CORES.size % 2]) , &set);
	sched_setaffinity(0, sizeof(cpu_set_t), &set);
}

void* routine(void* threadnum){
	

	setAffinity((long)threadnum);

	double* mpage = (double*)memalign(4096, 4096);
	//mlock(mpage, 4096);

	integral(mpage, (long)threadnum);
	
	//munlock(mpage, 4096);
	return mpage;
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

void checkTopology(){

	for (int i = 0; i < CORES.size; i++){
		printf("core_id %d, cpu0 %d, cpu1 %d\n", CORES.cores[i].core_id, CORES.cores[i].cpuX[0],
										CORES.cores[i].cpuX[1]);
	}

}


int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Wrong number of parametrs\n");
		return 1;
	}

	
	Parse_topology();

	//checkTopology();

	char* extstr;
	requredThread = strtol(argv[1], &extstr, 0);
	int realthread = CORES.size > requredThread ? CORES.size : requredThread;

	//setAffinity(0);

	pthread_t* id = (pthread_t*)malloc((realthread)* sizeof(pthread_t));

	for(long i = 0; i < requredThread ; i++){

		if(!pthread_create(id+i, NULL , routine, (void*)i)){
			
		}

		else{
			printf("Failure to start a thread number %ld\n program interrupted", i);
			for (int g = 0; g < i; ++g)
			{
				pthread_detach(*(id+g));
			}
			return 0;
		}

	}

	for(long i = requredThread; i < realthread ; i++){

		if(!pthread_create(id+i, NULL , routine, (void*)i)){
			pthread_detach(*(id+i));
		}

		else{
			printf("Failure to start a thread number %ld\n", i);
			//return 0;
		}

	}

	double sum = 0;
	//integral(&sum, 0);

	for(int i = 0; i < requredThread; i++){
		double* part = NULL;
		
		pthread_join(*(id+i), (void**) &part);

		sum += *part;
	}


	free(id);
	printf("%f\n", sum);
		return 0;
}