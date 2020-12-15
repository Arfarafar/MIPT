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
#include <sys/ioctl.h>



enum {
    RD = 0,
    WR = 1
};



typedef struct {
	int in[2];
    int out[2];
	char* buf;
    int buf_size;
    int cur_pos;
    int last_pos;
    char isfull;
} Transfer;



void Closefd(int fd, const char* msg){
	if (close(fd) == -1){ 
		perror(msg) ;
		exit(1);
	}
}



void CloseParent_sFd(Transfer* transfer, int ch){
    Closefd(transfer[0].out[RD], "");
    for (int i = 1; i <= ch; ++i)
    {
       Closefd(transfer[i].in[WR], "");
       Closefd(transfer[i].out[RD], "");
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


	int in = 0;
	int out = 0;
    Transfer* transfer = (Transfer*) calloc(sizeof(Transfer), nch + 1);


    pipe(transfer[0].out);
    if (fcntl(transfer[0].out[RD], F_SETFL, O_RDONLY | O_NONBLOCK) == -1){
            perror("fcntl");
            exit(1);
    }

	pid_t child_id = fork();


	if (child_id == 0){	

        CloseParent_sFd(transfer, 0);
        
		
		in = open(argv[2], O_RDONLY);

		if(lseek(in, 0, SEEK_END) < 0)
			exit(2);

		lseek(in, 0, SEEK_SET);

		while(reallength = splice(in, NULL, transfer[0].out[WR], NULL, PIPE_BUF, 0)) {}; 
      
        Closefd(transfer[0].out[WR], "");
        Closefd(in, "");

		exit(0);                    
	}           

    Closefd(transfer[0].out[WR], "");                 
    transfer[0].in[WR] = -666;

	
	for (int i = 1; i < nch; ++i){

        pipe(transfer[i].in);
        pipe(transfer[i].out);
        if( fcntl(transfer[i].in[WR], F_SETFL, O_WRONLY | O_NONBLOCK) == -1 ||
           fcntl(transfer[i].out[RD], F_SETFL, O_RDONLY | O_NONBLOCK) == -1){
            perror("fcntl");
            exit(1);
        }
        

		child_id = fork();

		if (child_id == 0){         
            
            CloseParent_sFd(transfer, i);

            while(reallength = splice(transfer[i].in[RD], NULL, transfer[i].out[WR], NULL, PIPE_BUF, 0)) {};  
          
            Closefd(transfer[i].out[WR], "");
            Closefd(transfer[i].in[RD], "");    
            exit(0);     
		}

        Closefd(transfer[i].out[WR], "");
        Closefd(transfer[i].in[RD], "");

    }
    
                                                 
    transfer[nch].in[WR] = STDOUT_FILENO;

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
            if(!transfer[i].isfull){
                FD_SET(transfer[i].out[RD], &rd_set);

                if(transfer[i].out[RD] >= nfds)
                    nfds = transfer[i].out[RD] + 1;
            }
            
        }

        for (int i = endwr; i <= nch; ++i)
        {
            if ((transfer[i-1].last_pos - transfer[i-1].cur_pos) || transfer[i-1].isfull){
                FD_SET(transfer[i].in[WR], &wr_set);

                if(transfer[i].in[WR] >= nfds)
                    nfds = transfer[i].in[WR] + 1;
            }

            else if (transfer[i-1].out[RD] == -666)
                {

                    if(i == nch){
                        endwr++;
                        return 0;
                    }
                        Closefd(transfer[i].in[WR], "");
                        transfer[i].in[WR] = -666;
                        endwr++;
                }
        }


        int readytoread = select(nfds, &rd_set, &wr_set, NULL, NULL);
      

        for (int i = endwr - 1; readytoread > 0; ++i)
        {
           
            if(FD_ISSET(transfer[i].out[RD], &rd_set)){
                readytoread--;
                reallength = 1;
               
                int distance = transfer[i].last_pos - transfer[i].cur_pos;
                if(distance > 0)
                {
                    reallength = read(transfer[i].out[RD],
                                      transfer[i].buf + transfer[i].last_pos, 
                                      transfer[i].buf_size - transfer[i].last_pos);

                    transfer[i].last_pos += reallength;
                    if(! (transfer[i].last_pos %= transfer[i].buf_size))
                        transfer[i].isfull = transfer[i].cur_pos ? 0 : 1;
                }
                else if(distance < 0){
                    reallength = read(transfer[i].out[RD],
                                      transfer[i].buf + transfer[i].last_pos, 
                                      transfer[i].cur_pos - transfer[i].last_pos);
                    transfer[i].last_pos += reallength;
                    transfer[i].isfull = transfer[i].cur_pos == transfer[i].last_pos ? 1 : 0;
                }
                else
                {
                    reallength = read(transfer[i].out[RD],
                                      transfer[i].buf, 
                                      transfer[i].buf_size);
                    transfer[i].last_pos = reallength;
                    transfer[i].cur_pos = 0;

                    if( reallength && (! (transfer[i].last_pos %= transfer[i].buf_size )) )
                        transfer[i].isfull = 1;
                }

                if(!reallength){
                        if (i != endrd || transfer[i].in[WR] != -666)
                            exit(21);

                        Closefd(transfer[i].out[RD], "");
                        transfer[i].out[RD] = -666;
                        endrd++;
                    }
            }

            if(FD_ISSET(transfer[i+1].in[WR], &wr_set)){
                readytoread--;


                int distance = transfer[i].last_pos - transfer[i].cur_pos;
                if(distance > 0)
                {
                    transfer[i].cur_pos += write(transfer[i+1].in[WR],
                                                 transfer[i].buf + transfer[i].cur_pos, 
                                                 transfer[i].last_pos - transfer[i].cur_pos);
                    transfer[i].isfull = 0;
                }
                else if(distance < 0){
                    transfer[i].cur_pos += write(transfer[i+1].in[WR],
                                                 transfer[i].buf + transfer[i].cur_pos, 
                                                 transfer[i].buf_size - transfer[i].cur_pos);
                    transfer[i].cur_pos %= transfer[i].buf_size;
                    transfer[i].isfull = 0;
                }
                else if(transfer[i].isfull)
                {
                    transfer[i].cur_pos += write(transfer[i+1].in[WR],
                                                 transfer[i].buf + transfer[i].cur_pos, 
                                                 transfer[i].buf_size - transfer[i].cur_pos);
                    transfer[i].cur_pos %= transfer[i].buf_size;
                    transfer[i].isfull = 0;
                }
            }
        }
    }
     

    

    return 0;
}