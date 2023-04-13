#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <math.h>


#define HOST_TCP_PORT 50000
#define CLIENT_UDP_PORT 50001


const double UPPER_LIMIT = 120000.0;
const double LOWER_LIMIT = 0.0;


typedef struct {
    double start;
    double end;
    double accuracy;
}  computing_task_t;


typedef struct {
	int fd;
	long rThr;
	int read;
	computing_task_t task;
} client_t;


static inline double func (double x){
	return sqrt(x) - x/1200;
}



void send_Broadcast(){

	int broadcast_trans = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in dest_addr = {.sin_family = AF_INET,
									.sin_port = htons(CLIENT_UDP_PORT),
									.sin_addr = htonl(INADDR_BROADCAST)
									};
    int enable = 1;
    setsockopt(broadcast_trans, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable));
	sendto (broadcast_trans, NULL, 0, 0, (struct sockaddr*) &dest_addr, sizeof(dest_addr));
	close(broadcast_trans);

}

void Enable_keepalive (int server)
{

	int seconds = 9;
	setsockopt(server, IPPROTO_TCP, TCP_KEEPIDLE, &seconds, sizeof(int));
	
	int interval = 1;
	setsockopt(server, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));
	
	int probes = 3;
	setsockopt(server, IPPROTO_TCP, TCP_KEEPCNT, &probes, sizeof(int));
	
}


int Create_server_socket(){

	int server =  socket(AF_INET, SOCK_STREAM, 0);

	int enable = 1;
	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	struct sockaddr_in bind_addr = {.sin_family = AF_INET,
									.sin_port = htons(HOST_TCP_PORT),
									.sin_addr = htonl(INADDR_ANY)
									};

	bind(server, (struct sockaddr*) &bind_addr, sizeof(bind_addr));

	Enable_keepalive(server);

	listen(server, SOMAXCONN);

	return server;
}


int accept_clients(int server, client_t* clients, int* allThreads, int requiredclients){

	struct timeval waiting_time = {
        .tv_sec = 0,
        .tv_usec = 1000000 /*1 sec*/
    };

    fd_set read_fds;
    *allThreads = 0;

    int ald_acpt = 0;
    int rd = 0;

    for(;;){
		
        FD_ZERO(&read_fds);
        FD_SET(server, &read_fds);
        for (int i = 0; i < ald_acpt; ++i)
        {
        	FD_SET(clients[i].fd, &read_fds);
        }

         
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, &waiting_time) <= 0)
            return -1;


		for (int i = 0; i < ald_acpt; ++i)
        {
        	if(FD_ISSET(clients[i].fd, &read_fds)){
        		long tmp;
        		if (read(clients[i].fd, &tmp, sizeof(long)) != sizeof(long))
        			return -1;
        		*allThreads += tmp;
        		clients[i].rThr = tmp;

        		rd++;
        		if(rd == requiredclients){
        			return 0;
        		}
        	}
        }

        if (FD_ISSET(server, &read_fds)){
        	clients[ald_acpt].fd = accept4(server, NULL, 0, SOCK_NONBLOCK);
        	clients[ald_acpt].read = 0;

        	int enable = 1;
        	setsockopt(clients[ald_acpt].fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(int));

        	ald_acpt = ald_acpt == requiredclients ? ald_acpt : ald_acpt+1 ;
        }

        waiting_time.tv_usec = 1000000;
    } 

}

int write_tasks(client_t* clients, int allThreads, int requiredclients){


	double delta  = (UPPER_LIMIT - LOWER_LIMIT) / allThreads;
	int thr_passed = 0;

	for(int i = 0; i < requiredclients; ++i){
		clients[i].task.accuracy = 0.00001;
		clients[i].task.start = LOWER_LIMIT + delta * thr_passed;
		clients[i].task.end = clients[i].task.start + delta * clients[i].rThr;
		thr_passed +=  clients[i].rThr;
	
		if( write(clients[i].fd, &clients[i].task, sizeof (clients[i].task)) != sizeof (clients[i].task))
			return -1;
	}

	return 0;
	
}


int receive_results (client_t* clients, int requiredclients, int allThreads, double* sum){

	int rd = 0;
	int failedwork = 0;
	int* id = (int*)malloc((requiredclients) * sizeof(int));
    fd_set read_fds;

	for(;;){
		
        FD_ZERO(&read_fds);
        
        for (int i = 0; i < requiredclients; ++i)
        {
        	if(!clients[i].read)
        		FD_SET(clients[i].fd, &read_fds);
        }

        if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0){
        	if (errno == EBADF){
        		struct stat st;
        		for (int i = 0; i < requiredclients; ++i)
			        {
			        	if(fstat(clients[i].fd, &st) == -1 && errno == EBADF){
			        		clients[i].read = 1;
			        		id[failedwork++] = i;
			        	}
			        }
        		rd++;
        		if(rd == requiredclients){
        			goto out;
        		}
        		continue;
        	}
            return -1;
        }


		for (int i = 0; i < requiredclients; ++i)
        {
        	if(FD_ISSET(clients[i].fd, &read_fds)){
        		double tmp = 0;
        		clients[i].read = 1;
        		rd++;
        		if (read(clients[i].fd, &tmp, sizeof(double)) != sizeof(double))
	        		id[failedwork++] = i;
        		
        		*sum += tmp;
        		
        		if(rd == requiredclients){
        			goto out;
        		}
        	}
        }
        
    } 

out: 
	for(failedwork--; failedwork >= 0; failedwork--){
		double acc = clients[id[failedwork]].task.accuracy;
		for (double x = clients[id[failedwork]].task.start, xend = clients[id[failedwork]].task.end; x < xend; x += acc){
			*sum += func(x)*acc;
		}
	}
	return 0;

}



int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Wrong number of parametrs\n");
		return 1;
	}


	send_Broadcast();
	
	int server = Create_server_socket();

	char* extstr;
	long requiredclients = strtol(argv[1], &extstr, 0);


    client_t* clients = (client_t*)malloc(requiredclients * sizeof(client_t));
    int allThreads;
   
	if (accept_clients(server, clients, &allThreads, requiredclients) == -1){
		printf("Not all clients are accepted\n");
		return 1;
	}

	if (write_tasks(clients, allThreads, requiredclients) == -1){
		printf("Not all tasks are given\n");
		return 1;
	}

	double sum;

	if (receive_results(clients, requiredclients, allThreads, &sum) == -1){
		printf("Not all results are received\n");
		return 1;
	}

	printf("%f\n", sum);
	free(clients);
	close(server);
	return 0;
}
