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
#include <sys/socket.h>
#include <netinet/in.h>


#define HOST_TCP_PORT 50000
#define CLIENT_UDP_PORT 50001


const double UPPER_LIMIT = 10000.0;
const double LOWER_LIMIT = 0.0;


struct {
    double start;
    double end;
    double accuracy = 0.00001;
}  computing_task;


typedef struct {
	int fd;
	long rThr;
} client_t;


void send_Broadcast(){

	int broadcast_trans = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in dest_addr = {.sin_family = AF_INET;
									.sin_port = htons(CLIENT_UDP_PORT);
									.sin_addr = htonl(INADDR_BROADCAST);
									};
    int enable = 1;
    setsockopt(broadcast_trans, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable));
	sentto (broadcast_trans, NULL, 0, 0, (struct sockaddr*) &dest_addr, sizeof(dest_addr));
	close(broadcast_trans);

}

void Enable_options (int server)
{
	/*
	int enable = 1;
	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	*/

	int seconds = 9;
	setsockopt(server, IPPROTO_TCP, TCP_KEEPIDLE, &seconds, sizeof(int));
	
	int interval = 1;
	setsockopt(server, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));
	
	int probes = 3;
	setsockopt(server, IPPROTO_TCP, TCP_KEEPCNT, &probes, sizeof(int));
	
}


int Create_server_socket(){

	int server =  socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in bind_addr = {.sin_family = AF_INET;
									.sin_port = htons(HOST_TCP_PORT);
									.sin_addr = htonl(INADDR_ANY);
									};

	bind(server, (struct sockaddr*) &bind_addr, sizeof(bind_addr));

	//Enable_options(server);

	listen(server, SOMAXCONN);

	return server;
}


int accept_clients(int server, int* clients, int* allThreads, int requredclients){

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
        		if(rd == requredclients){
        			return 0;
        		}
        	}
        }

        if (FD_ISSET(server, &read_fds)){
        	clients[ald_acpt].fd = accept4(server, NULL, 0, SOCK_NONBLOCK);

        	//int enable = 1;
        	//setsockopt(clients[ald_acpt].fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(int))

        	ald_acpt = ald_acpt == requredclients ? ald_acpt : ald_acpt+1 ;
        }

        waiting_time.tv_usec = 1000000;
    } 

}

int write_tasks(int* clients, int allThreads, int requredclients){


	double delta  = (UPPER_LIMIT - LOWER_LIMIT) / allThreads;
	int thr_passed = 0

	for(int i = 0; i < requredclients; ++i){

		computing_task.start = LOWER_LIMIT + delta * thr_passed;
		computing_task.end = computing_task.start + delta * clients[i].rThr;
		thr_passed +=  clients[i].rThr;
	
		if( write(clients[i].fd, &computing_task, sizeof (computing_task)) != sizeof (computing_task))
			return -1;
	}

	return 0;
	
}

int receive_results (int* clients, int allThreads, double* sum){

	int rd = 0;

	for(;;){
		
        FD_ZERO(&read_fds);
        
        for (int i = 0; i < allThreads; ++i)
        {
        	FD_SET(clients[i].fd, &read_fds);
        }

         
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, &waiting_time) <= 0)
            return -1;


		for (int i = 0; i < allThreads; ++i)
        {
        	if(FD_ISSET(clients[i].fd, &read_fds)){
        		double tmp;
        		if (read(clients[i].fd, &tmp, sizeof(double)) != sizeof(double))
        			return -1;
        		*sum += tmp;
        		rd++;
        		if(rd == requredclients){
        			return 0;
        		}
        	}
        }
        
    } 

}



int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Wrong number of parametrs\n");
		return 1;
	}


	send_Broadcast();
	
	int server = Create_server_socket();

	char* extstr;
	long requredclients = strtol(argv[1], &extstr, 0);


    client_t* clients = (client_t*)malloc(requredclients * sizeof(client_t));
    int allThreads;
   
	if (accept_clients(server, clients, &allThreads, requredclients) == -1){
		printf("Not all clients are accepted\n");
		return 1;
	}

	if (write_tasks(clients, allThreads, requredclients) == -1){
		printf("Not all tasks are given\n");
		return 1;
	}

	double sum;

	if (receive_results(clients, requredclients, &sum) == -1){
		printf("Not all results are received\n");
		return 1;
	}

	printf("%f\n", sum);
	free(clients);
	close(server);
	return 0;
}
