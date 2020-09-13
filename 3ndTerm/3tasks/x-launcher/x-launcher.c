#include <stdio.h>
 #include <sys/types.h>
  #include <unistd.h>
#include <stdlib.h>


int main(int argc, char* argv[]){

	
	int ret_code = execvp(argv[1], argv+1);

	if(ret_code == -1){
		printf("Failure to start a process\n");
	}
		return 0;
}