#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ioctl.h>


#define CONNECTIVE_FIFO_PATH "fifos/connection"
#define UNIQFIFONAME "fifos/process.%ld"
#define LENGTH_UNIQFIFONAME (sizeof(UNIQFIFONAME) + 25)
#define WAITSECONDS 100

const int SIZEOFCHAR = sizeof(char);
const int SIZEOFPID_T = sizeof(pid_t);

void Closefd(int fd, const char* msg){
	if (close(fd) == -1){ 
		perror(msg) ;
		exit(1);
	}
}

int Openfd(const char* name, int flags, const char* msg){

	int Fd = open(name, flags);
	if (Fd == -1){
	    perror(msg);
	    exit(1);
	}
	return Fd;
}

void Make_fifo(const char* PATH){
	umask(0);                          
	if (mkfifo(PATH, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        printf("open FIFO %s", PATH);
    	exit(1);
	}
}

void DisableNONBLOCK(int fd){
	int flags;

    flags = fcntl(fd, F_GETFL);
	flags &= ~O_NONBLOCK;             
	fcntl(fd, F_SETFL, flags);

}



void File_Translator(char* filename){

	FILE* flin = fopen(filename, "rb");

	int connectFd = Openfd(CONNECTIVE_FIFO_PATH, O_RDWR, "connectFd open");
          

    char buf[PIPE_BUF] = "";
    char uniqfifo[LENGTH_UNIQFIFONAME] = "";
    
    int reallength = 0;

    read(connectFd, buf, SIZEOFPID_T);`


    pid_t pid = *(pid_t*)buf;

    snprintf(uniqfifo, LENGTH_UNIQFIFONAME, UNIQFIFONAME, (long)pid);

    int filetranslatorFd = Openfd(uniqfifo, O_WRONLY | O_NONBLOCK, "file translatorFd open");
    DisableNONBLOCK(filetranslatorFd);


	while((reallength = fread(buf + 1, SIZEOFCHAR,  PIPE_BUF - 1, flin)) == PIPE_BUF - 1 ){
		//sleep(1);
		buf[0] = 0;
		write(filetranslatorFd, buf, PIPE_BUF);
	}
	buf[0] = 1;
	write(filetranslatorFd, buf, reallength + 1);
	
	Closefd(connectFd,			"connectFd close");
	Closefd(filetranslatorFd,   "filetranslatorFd close");
	
}



void File_Receiver(){

	int connectFd = Openfd(CONNECTIVE_FIFO_PATH, O_RDWR, "contactFd open");

	
	char buf[PIPE_BUF] = "";
    int indicator = 0;
    int reallength = PIPE_BUF;
    
    char uniqfifo[LENGTH_UNIQFIFONAME] = "";
    pid_t pid = getpid();

    snprintf(uniqfifo, LENGTH_UNIQFIFONAME, UNIQFIFONAME, (long)pid);
    Make_fifo(uniqfifo);
    

    int filereceiverFd = Openfd(uniqfifo, O_RDONLY | O_NONBLOCK, "file receiverFd open");
    DisableNONBLOCK(filereceiverFd);
    
    write(connectFd, &pid, SIZEOFPID_T);
 
 	
    	

	while(reallength == PIPE_BUF && !buf[0] ) {	
		int i = 0;
 		while(i < WAITSECONDS)
	 	{
	 		ioctl(filereceiverFd, FIONREAD, &indicator);
		    if (indicator){
		    	break;
		    }
		    i++;
		    sleep(1);
	 	}

	    if (i == WAITSECONDS){
	    	printf("uniqfifo writer is dead or going too slow\n");
	    	exit(1);
	    }
		reallength = read(filereceiverFd, buf,  PIPE_BUF);
		write(STDOUT_FILENO, buf + 1 , reallength - 1);	

	}

	Closefd(filereceiverFd, "rewriteFd close");
	Closefd(connectFd, "contactFd close");
						
}



int main(int argc, char* argv[]){
	if (argc > 2){
		printf("Invalid argc\n");
		return 0;
	}

	Make_fifo(CONNECTIVE_FIFO_PATH);

	
		switch (argc) {
			case 1:
				File_Receiver();
				break;
			case 2:
				File_Translator(argv[1]);
				break;
			default:
				break;
			}
	
	return 0;
}