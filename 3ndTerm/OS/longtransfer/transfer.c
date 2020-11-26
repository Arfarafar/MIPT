#define _GNU_SOURCE 
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>        
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>



#define UNIQFIFONAME "process."
#define LENGTH_UNIQFIFONAME (sizeof(UNIQFIFONAME) + 10)


#define OPEN_FIFO_NONBLOCK(n, inout, flags)   \
    snprintf(startpoint, LENGTH_UNIQFIFONAME, "%d", n);         \
    Make_fifo(uniqfifo);                                        \
    inout = Openfd(uniqfifo, flags  | O_NONBLOCK, #inout);      



#define	OPEN_FIFO_BLOCK(n, inout, flags)									\
    OPEN_FIFO_NONBLOCK(n, inout, flags)      \
	DisableNONBLOCK(inout);


#define opINIT(n, sem, op, flg) sops[n].sem_num = sem;                  \
                                sops[n].sem_op = op;                    \
                                sops[n].sem_flg = flg;                  


#define opSEM(sem, op, flg)     opINIT(0, sem, op, flg)                 \
                                semop(semid, sops, 1);


int initSem(int semId, int semNum, int initVal){
    return semctl(semId, semNum, SETVAL, initVal);
}


typedef struct {
	int in;
    int out;
	char* buf;
    int buf_size;
    int cur_pos;
    int last_pos;
    char isfull;
} Transfer;


void TransferDump(Transfer* transfer, int i, char* name){

    printf("операция %s\n", name);
    printf("структура номер %d\n", i);
    printf("чтение из %d\n", transfer[i].in);
    printf("запись в %d\n", transfer[i].out);
    printf("размер буфера %d\n", transfer[i].buf_size);
    printf("текущая позиция %d\n", transfer[i].cur_pos);
    printf("последняя позиция %d\n", transfer[i].last_pos);
    printf("заполнена ли труба? %d\n\n\n", transfer[i].isfull);
    fflush(0);
}



void Make_fifo(const char* PATH){
	//umask(0);                          
	if (mkfifo(PATH, 0666) == -1 && errno != EEXIST){
        printf("not make FIFO %s !!\n", PATH);
    	exit(1);
	}
    printf("make FIFO %s\n", PATH);
    fflush(0);
}

void Closefd(int fd, const char* msg){
	if (close(fd) == -1){ 
		perror(msg) ;
		exit(1);
	}
}

int Openfd(const char* name, int flags, const char* msg){

	int Fd = open(name, flags);
	if (Fd == -1){
        if(errno == ENXIO){
            printf("ENXIO\n");
            fflush(0);
        }
	    perror(name);
	    exit(1);
	}
	return Fd;
}

void DisableNONBLOCK(int fd){
	int flags;

    flags = fcntl(fd, F_GETFL);
	flags &= ~O_NONBLOCK;             
	fcntl(fd, F_SETFL, flags);

}


void child(int sig){
	exit(0);
}




int main(int argc, char* argv[]){

	if (argc > 3 || argc == 1 || argc == 2){
		printf("Invalid argc\n");
		return 0;
	}

    int semid = semget(ftok(argv[0], 10), 2, IPC_CREAT | 0666);
    if( semid == -1){
        perror("semid");
        exit(1);
    }

    initSem(semid, 1, 0);
    initSem(semid, 0, 0);

	char* extstr;
	int nch = (int)strtol(argv[1], &extstr, 0);
    printf("nch %d \n", nch);   

    int reallength = 0;


	char uniqfifo[LENGTH_UNIQFIFONAME] = UNIQFIFONAME;

	struct sigaction act = {};
	act.sa_handler = child;
	sigaction(SIGCHLD, &act, NULL);

	int in = 0;
	int out = 0;

	char* startpoint = uniqfifo + sizeof(UNIQFIFONAME) - 1 ;

    struct sembuf sops[1];

	pid_t child_id = fork();

	if (child_id == 0){	

		prctl(PR_SET_PDEATHSIG, SIGCHLD);
		if ( getppid() == 1)
			exit(1);

        printf("child %d out\n", 0);  
             fflush(0);

		OPEN_FIFO_BLOCK(1, out, O_RDWR)
		
		in = open(argv[2], O_RDONLY);

		if(lseek(in, 0, SEEK_END) <= 0)
			exit(2);

		lseek(in, 0, SEEK_SET);

        
        opSEM(0, -1, 0)
        printf("начал печать %d\n", 0 );

		while(reallength = splice(in, NULL, out, NULL, PIPE_BUF, 0)) {
          printf("я %d перекинул %d байт\n", 0, reallength);
          fflush(0);
        };   //крит секция зеркальная начальной родительской
         printf("Сплайс закрылся %d\n", 0);
        Closefd(out, "");
        Closefd(in, "");


        opSEM(1, -1, 0)

		exit(0);                      // КРИТ секция, родитель с ребенком гоняются
	}                                 // за то произойдет передача или нет одновременное выполнение 
                                      // этой строчки с работающей передачей прервет передачу.



	
	for (int i = 1; i < nch; ++i){

		child_id = fork();

		if (child_id == 0){          // крит секция с форка до семопа for what??
            
			prctl(PR_SET_PDEATHSIG, SIGCHLD);
			if ( getppid() == 1)
				exit(1);

            printf("child %d in\n", i);   
            fflush(0);
			OPEN_FIFO_BLOCK(2*i, in, O_RDONLY)
            printf("child %d out\n", i);  
             fflush(0);
			OPEN_FIFO_BLOCK(2*i+1, out, O_RDWR)

            opSEM(0, -1, 0) //начало крит секции чтобы родитель успел открыть все фд до нее

            printf("начал печать %d\n", i );

            while(splice(in, NULL, out, NULL, PIPE_BUF, 0)) {
             printf("я %d перекинул %d байт\n", i, reallength);
            fflush(0);
            };   //конец крит секции чтобы сплайс не вернул 0 изза того что конца на запись нет

            printf("Сплайс закрылся %d\n", i );
            Closefd(out, "");
            Closefd(in, "");


            opSEM(1, -1, 0)

            exit(0);     
		}


    }
    
    Transfer* transfer = (Transfer*) calloc(sizeof(Transfer), nch + 1);

    OPEN_FIFO_NONBLOCK(1, transfer[0].out, O_RDONLY)

    transfer[0].buf_size = (int) pow (3, nch)*1024;
    transfer[0].buf = (char*) calloc (transfer[0].buf_size, 1);

	for (int i = 1; i < nch; ++i){
        
		OPEN_FIFO_NONBLOCK(2*i,     transfer[0].in,  O_WRONLY)
        OPEN_FIFO_NONBLOCK(2*i+1,   transfer[0].out, O_RDONLY)
    
        transfer[i].buf_size = (int) pow (3, nch-i)*1024;
		transfer[i].buf = (char*) calloc (transfer[i].buf_size, 1);

	}
    snprintf(startpoint, LENGTH_UNIQFIFONAME, "%d", 2*nch);                                                  \
    transfer[nch].in = STDOUT_FILENO;


     // для род процесса начиная с начала и до этого семопа     
     // крит секция чтобы успеть открыть все файл-дескрипторы                 
    opSEM(0, nch, 0)
    printf("Я открыл всем детям семафоры\n");

    fd_set rd_set;
    fd_set wr_set;
    
    int endrd = 0;
    int endwr = 1;

    while (endrd + endwr < 2*nch){

        FD_ZERO(&rd_set);
        FD_ZERO(&wr_set);
        int nfds = 0;

        for (int i = endrd; i < nch; ++i)
        {
          
            FD_SET(transfer[i].out, &rd_set);

            if(transfer[i].out >= nfds)
                nfds = transfer[i].out + 1;
        }

        for (int i = endwr; i < nch; ++i)
        {

            FD_SET(transfer[i].in, &wr_set);

            if(transfer[i].in >= nfds)
                nfds = transfer[i].in + 1;
        }

        sleep(1);
        int cnt = -22;
       
        ioctl(transfer[0].out, FIONREAD, &cnt);
        printf("я в сете: %d во мне %d байт \n", FD_ISSET(transfer[0].out, &rd_set), cnt);
        fflush(0);

        int readytoread = select(nfds, &rd_set, NULL, NULL, NULL);

        printf("readytoread %d\n", readytoread);

        for (int i = endrd; readytoread > 0; ++i)
        {
           
            if(FD_ISSET(transfer[i].out, &rd_set)){
                readytoread--;
                reallength = 1;
                TransferDump(transfer, i, "read");
                int distance = transfer[i].last_pos - transfer[i].cur_pos;
                if(distance > 0)
                {
                    reallength = read(transfer[i].out,
                                     transfer[i].buf + transfer[i].last_pos, 
                                     transfer[i].buf_size - transfer[i].last_pos);

                    transfer[i].last_pos += reallength;
                    if(! (transfer[i].last_pos %= transfer[i].buf_size))
                        transfer[i].isfull = transfer[i].cur_pos ? 0 : 1;
                }
                else if(distance < 0){
                    reallength = read(transfer[i].out,
                                                 transfer[i].buf + transfer[i].last_pos, 
                                                 transfer[i].cur_pos - transfer[i].last_pos);
                    transfer[i].last_pos += reallength;
                    transfer[i].isfull = transfer[i].cur_pos == transfer[i].last_pos ? 1 : 0;
                }
                else if(!transfer[i].isfull)
                {
                    reallength = read(transfer[i].out,
                                         transfer[i].buf, 
                                         transfer[i].buf_size);
                    transfer[i].last_pos = reallength;
                    transfer[i].cur_pos = 0;

                    if( reallength && (! (transfer[i].last_pos %= transfer[i].buf_size )) )
                        transfer[i].isfull = 1;
                }
                TransferDump(transfer, i, "read");

                if(!reallength){
                        Closefd(transfer[i].out, "");
                        transfer[i].out = -666;
                        endrd++;
                    }
            }

        }

        int readytowrite = select(nfds, NULL, &wr_set, NULL, NULL) + 1;

        printf("readytowrite %d\n", readytoread);

        for (int i = endwr - 1; readytowrite > 0; ++i)
        {
           
            if(FD_ISSET(transfer[i+1].in, &rd_set) || i == nch-1){
                readytowrite--;

                TransferDump(transfer, i, "write");

                int distance = transfer[i].last_pos - transfer[i].cur_pos;
                if(distance > 0)
                {
                    transfer[i].cur_pos += write(transfer[i+1].in,
                                                 transfer[i].buf + transfer[i].cur_pos, 
                                                 transfer[i].last_pos - transfer[i].cur_pos);
                   
                    transfer[i].isfull = 0;
                }
                else if(distance < 0){
                    transfer[i].cur_pos += write(transfer[i+1].in,
                                                 transfer[i].buf + transfer[i].cur_pos, 
                                                 transfer[i].buf_size - transfer[i].cur_pos);
                    transfer[i].cur_pos %= transfer[i].buf_size;
                    transfer[i].isfull = 0;
                }
                else if(transfer[i].isfull)
                {
                    transfer[i].cur_pos += write(transfer[i+1].in,
                                             transfer[i].buf + transfer[i].cur_pos, 
                                             transfer[i].buf_size - transfer[i].cur_pos);

                    transfer[i].cur_pos %= transfer[i].buf_size;
                    transfer[i].isfull = 0;
                }
                else if (transfer[i].out == -666)
                {
                    Closefd(transfer[i+1].in, "");
                    endwr++;
                }
                TransferDump(transfer, i, "write");
            }

        }

    }

    opSEM(1, nch, 0)

    return 0;
}