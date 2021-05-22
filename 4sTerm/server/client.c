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
#include <sys/socket.h>
#include <netinet/in.h>

#define HOST_TCP_PORT 50000
#define CLIENT_UDP_PORT 50001

#define BASE_PATH "/sys/devices/system/cpu/cpu"
#define TOPOLOGY_PATH_ONLINE "/sys/devices/system/cpu/online"
#define TOPOLOGY_PATH_CpuX_coreID "%d/topology/core_id"
#define MAX_CORE 128
#define MAX_CPU MAX_CORE*2
#define MAX_PATHLEN 128

long requredThread = 0;

struct {
    double LOWER_LIMIT;
    double UPPER_LIMIT;
    double accuracy;
} computing_task;

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
	return sqrt(x) - x/120;
}

void integral(double* dest, long threadnum){

	*dest = 0.0;
	double delta  = ( computing_task.UPPER_LIMIT - computing_task.LOWER_LIMIT) / requredThread;
	double a =  computing_task.LOWER_LIMIT + delta * threadnum;
	double b = a + delta;

	for (double x = a; x < b; x += computing_task.accuracy){
		*dest += func(x)*computing_task.accuracy;
	}

}

void setAffinity (long threadnum){
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(CORES.cores[threadnum % CORES.size].cpuX[threadnum / CORES.size % 2], &set);
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



void Parse_topology(int *realthread){

	FILE* flin = fopen(TOPOLOGY_PATH_ONLINE, "rt");

	int begin, end, argc, core_id;
	char path[MAX_PATHLEN] = BASE_PATH;
	int baselen = sizeof(BASE_PATH) - 1 ;

	while ((argc = fscanf(flin, " %d-%d", &begin, &end))!= EOF){
		getc(flin);
		if (argc == 1){
			end = begin;
		}
		*realthread += end - begin + 1;
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



double calculate(int realthread){

	pthread_t* id = (pthread_t*)malloc((realthread)* sizeof(pthread_t));

	for(long i = 0; i < requredThread ; i++){

		if(!pthread_create(id+i, NULL , routine, (void*)i)){
			
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

	for(long i = requredThread; i < realthread ; i++){

		if(!pthread_create(id+i, NULL , routine, (void*)i)){
			pthread_detach(*(id+i));
		}

	}

	double sum = 0;

	for(int i = 0; i < requredThread; i++){
		double* part = NULL;
		
		pthread_join(*(id+i), (void**) &part);

		sum += *part;
	}


	free(id);

	return sum;

}



int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Wrong number of parametrs\n");
		return 1;
	}

	int realthread = 0;
	Parse_topology(&realthread);
	char* extstr;
	requredThread = strtol(argv[1], &extstr, 0);
	realthread = realthread > requredThread ? realthread : requredThread;

	//checkTopology();

	int broadcast_receiver = socket(AF_INET, SOCK_DGRAM, 0);
	int client = socket(AF_INET, SOCK_STREAM, 0);

	char buf[MAX_PATHLEN] = "";

	struct sockaddr_in src_addr;
	int addrlen = sizeof(src_addr);

	int enable = 1;
	setsockopt(broadcast_receiver, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	

	struct sockaddr_in bind_addr = {.sin_family = AF_INET,
									.sin_port = htons(CLIENT_UDP_PORT),
									.sin_addr = htonl(INADDR_ANY)
									};
	bind(broadcast_receiver, (struct sockaddr*) &bind_addr, sizeof(bind_addr));
	recvfrom(broadcast_receiver, buf, MAX_PATHLEN, 0, (struct sockaddr*) &src_addr, &addrlen);


	src_addr.sin_port = htons(HOST_TCP_PORT);
	if (connect(client, (struct sockaddr*) &src_addr, sizeof(src_addr)) == -1){
		printf("Fails to connect\n");
		return 1;
	}

	write(client, &requredThread, sizeof(long));

	if (read(client, &computing_task, sizeof(computing_task)) != sizeof(computing_task)){
		printf("server is dead\n");
		return 1;
	}
	

	double sum = calculate(realthread);
	
	if (write(client, &sum, sizeof(sum)) != sizeof(sum)){
		printf("Fails to write\n");
		return 1;
	}

	close(broadcast_receiver);
	close(client);
		return 0;
}
