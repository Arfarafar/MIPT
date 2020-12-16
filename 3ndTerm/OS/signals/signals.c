#include <signal.h>
#include <stdio.h>
 #include <sys/types.h>
  #include <unistd.h>
#include <stdlib.h>
 #include <sys/prctl.h>




#define BITSIZEofCHAR 8
#define firstbitMASK 0b10000000

char bit = 0;


void child(int sig){
	exit(0);
}


void receiver(int sig){
	if (sig == SIGUSR1){
		bit = 1;

	}
	else
		bit = 0;
}

void translator(int sig){

}


int main(int argc, char* argv[]){
	if (argc > 2 || argc == 1){
		printf("Invalid argc\n");
		return 0;
	}

	sigset_t  blockingMask;
	sigemptyset(&blockingMask);
	sigaddset(&blockingMask, SIGUSR1);
	sigaddset(&blockingMask, SIGUSR2);
	sigprocmask(SIG_BLOCK, &blockingMask, NULL);

	struct sigaction act = {};

	act.sa_handler = child;
	sigaction(SIGCHLD, &act, NULL);

	pid_t parent_id = 1;
	pid_t child_id = fork();

	if (child_id == 0){
		
		prctl(PR_SET_PDEATHSIG, SIGCHLD);

		if ((parent_id = getppid()) == 1)
			exit(1);
		
		act.sa_handler = translator;
		sigaction(SIGUSR1, &act, NULL);
		
		FILE* flin = fopen(argv[1], "rb");
		
		sigdelset(&blockingMask, SIGUSR1);
		

		unsigned char buffer ;
		int reallength = 0;
		while(reallength = fread(&buffer, 1, 1, flin)){

			kill(parent_id, SIGUSR1);
			sigsuspend(&blockingMask);
			
			for (int i = 0; i < BITSIZEofCHAR; ++i)
			{
				
				if (buffer & firstbitMASK){
					kill(parent_id, SIGUSR1);
				}
				else{
					kill(parent_id, SIGUSR2);
				} 
				sigsuspend(&blockingMask);
				buffer <<= 1;
				
			
			}
			
		} 
		
		kill(parent_id, SIGUSR2);

		return 0;
	}

	else{
		
		act.sa_handler = receiver;
		sigaction(SIGUSR1, &act, NULL);	
		sigaction(SIGUSR2, &act, NULL);

		sigdelset(&blockingMask, SIGUSR1);
		sigdelset(&blockingMask, SIGUSR2);
		
		

		while(1){
			sigsuspend(&blockingMask);
			
			if(bit == 0){
				exit(0);
			}
			char c = 0;
			kill(child_id, SIGUSR1); 

			for (int i = 0; i < BITSIZEofCHAR; ++i)
			{	
				sigsuspend(&blockingMask);
				c = (c << 1);
				c |= bit;
				
				kill(child_id, SIGUSR1);
			}
			
			write(STDOUT_FILENO, &c , 1);
				
		}

	}
	
	return 0;
}
