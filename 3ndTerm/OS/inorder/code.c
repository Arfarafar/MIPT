#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>




struct M{
	long type;
};


int main(int argc, char* argv[]){
	if (argc != 2){
		printf("Invalid argc\n");
		return 1;
	}

	char* extstr;
	long chnum = strtol(argv[1], &extstr, 0);

	struct M msg = {1};
	
	

	int msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);



	for(long i = 1; i < chnum + 1; i++){

		pid_t ret_code = fork();

		if (ret_code == 0){

			if (!(msgrcv(msgid, &msg, 0, i, 0) + 1)){
				perror("rcv");
				return 1;
			}


			write(STDOUT_FILENO, , sizeoflong);	
			msg.type = i + 1;
			msgsnd(msgid, &msg , 0, 0);
			return 0;

		}
		
	}

	msgsnd(msgid, &msg , 0, 0);
	
	if (!(msgrcv(msgid, &msg, 0, chnum + 1, 0) + 1)){
				perror("rcv");
		}
	if (msgctl(msgid, IPC_RMID, NULL) == -1) {
			printf("не закрыл; id: %d\n", msgid);
	}

	
	return 0;
}