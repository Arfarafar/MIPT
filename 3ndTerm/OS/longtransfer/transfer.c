#define _GNU_SOURCE 
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>        
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>




#define UNIQFIFONAME "process."
#define LENGTH_UNIQFIFONAME (sizeof(UNIQFIFONAME) + 10)


#define OPEN_FIFO_NONBLOCK(n, inout, flags)   \
    snprintf(startpoint, LENGTH_UNIQFIFONAME, "%d", n);         \
    Make_fifo(uniqfifo);                                        \
    inout = Openfd(uniqfifo, flags  | O_NONBLOCK, #inout);      



#define	OPEN_FIFO_BLOCK(n, inout, flags)									\
    OPEN_FIFO_NONBLOCK(n, inout, flags)      \
	DisableNONBLOCK(inout);



typedef struct {
	int in;
    int out;
	char* buf;
    int buf_size;
    int cur_pos;
    int last_pos;
    char isfull;
} Transfer;




void Make_fifo(const char* PATH){
	umask(0); 
    int   ret = 0;                       
	if ((ret = mkfifo(PATH, 0666)) == -1 && errno != EEXIST){
        printf("cannot make FIFO %s !!\n", PATH);
    	exit(1);
	}
  
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



void CloseParent_sFd(Transfer* transfer, int ch){
    Closefd(transfer[0].out, "");
    for (int i = 1; i <= ch; ++i)
    {
       Closefd(transfer[i].in, "");
       Closefd(transfer[i].out, "");
    }
}


int main(int argc, char* argv[]){

	if (argc > 3 || argc == 1 || argc == 2){
		printf("Invalid argc\n");
		return 0;
	}


    

	char* extstr;
	int nch = (int)strtol(argv[1], &extstr, 0);

    int reallength = 0;


	char uniqfifo[LENGTH_UNIQFIFONAME] = UNIQFIFONAME;


	int in = 0;
	int out = 0;
    Transfer* transfer = (Transfer*) calloc(sizeof(Transfer), nch + 1);

	char* startpoint = uniqfifo + sizeof(UNIQFIFONAME) - 1 ;


    OPEN_FIFO_NONBLOCK(1, transfer[0].out, O_RDONLY)

	pid_t child_id = fork();


	if (child_id == 0){	

        CloseParent_sFd(transfer, 0);

		OPEN_FIFO_BLOCK(1, out, O_WRONLY)
		
		in = open(argv[2], O_RDONLY);

		if(lseek(in, 0, SEEK_END) < 0)
			exit(2);

		lseek(in, 0, SEEK_SET);

		while(reallength = splice(in, NULL, out, NULL, PIPE_BUF, 0)) {}; 
      
        Closefd(out, "");
        Closefd(in, "");

		exit(0);                    
	}                               


	
	for (int i = 1; i < nch; ++i){

        OPEN_FIFO_NONBLOCK(2*i,     transfer[i].in,  O_RDWR)
        OPEN_FIFO_NONBLOCK(2*i+1,   transfer[i].out, O_RDONLY)

		child_id = fork();

		if (child_id == 0){         
            
            CloseParent_sFd(transfer, i);
			
			OPEN_FIFO_BLOCK(2*i, in, O_RDONLY)
			OPEN_FIFO_BLOCK(2*i+1, out, O_WRONLY)

            while(reallength = splice(in, NULL, out, NULL, PIPE_BUF, 0)) {};  
          
            Closefd(out, "");
            Closefd(in, "");    
            exit(0);     
		}


    }
    
    snprintf(startpoint, LENGTH_UNIQFIFONAME, "%d", 2*nch);                                                  \
    transfer[nch].in = STDOUT_FILENO;

    for (int i = 0; i < nch; ++i){
        transfer[i].buf_size = (int) pow (3, nch-i)*1024;
        transfer[i].buf = (char*) calloc (transfer[i].buf_size, 1);
    }



    fd_set rd_set;
    fd_set wr_set;
    
    int endrd = 0;
    int endwr = 1;

    while (endrd + endwr <= 2*nch){

        FD_ZERO(&rd_set);
        FD_ZERO(&wr_set);
        int nfds = 0;

        for (int i = endrd; i < nch; ++i)
        {
          
            FD_SET(transfer[i].out, &rd_set);

            if(transfer[i].out >= nfds)
                nfds = transfer[i].out + 1;
        }

        for (int i = endwr; i <= nch; ++i)
        {

            FD_SET(transfer[i].in, &wr_set);

            if(transfer[i].in >= nfds)
                nfds = transfer[i].in + 1;
        }


        int readytoread = select(nfds, &rd_set, NULL, NULL, NULL);

      

        for (int i = endrd; readytoread > 0; ++i)
        {
           
            if(FD_ISSET(transfer[i].out, &rd_set)){
                readytoread--;
                reallength = 1;
               
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

                if(!reallength){
                        if (i != endrd)
                            exit(0);

                        Closefd(transfer[i].out, "");
                        transfer[i].out = -666;
                        endrd++;
                    }
            }

        }

        int readytowrite = select(nfds, NULL, &wr_set, NULL, NULL);


        for (int i = endwr - 1; readytowrite > 0; ++i)
        {
           
            if(FD_ISSET(transfer[i+1].in, &wr_set)){
                readytowrite--;


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
                    transfer[i+1].in = -666;
                    endwr++;
                }
                
            }

        }

    }

    return 0;
}