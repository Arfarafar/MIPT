#include <stdio.h>
 #include <sys/types.h>
  #include <unistd.h>
#include <stdlib.h>


int main(int argc, char* argv[]){
	if (argc != 2){
		printf("Invalid argc\n");
	}

	char* extstr;
	long chnum = strtol(argv[1], &extstr, 0);
	printf("m_pid %d; p_pid %d;  \n", getpid(), getppid());

	for(long i = 0; i < chnum; i++){
		pid_t ret_code = fork();

		if (ret_code == 0){
			printf("m_pid %d; p_pid %d; number %ld \n", getpid(), getppid(), i+1);
			return 0;

		}
		else{
						/*int status;
			wait(&status);*/
		}
	}
	return 0;
}