#include <stdio.h>
 #include <sys/types.h>
  #include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void* counter(void* n){
	for (int i = 0; i < 10000; i++)
	{(*(int*)n)++;}
	return 0;
}


int main(int argc, char* argv[]){

	if (argc != 2){
		printf("Wrong number of parametrs\n");
		return 1;
	}

	
	int n = 0;

	char* extstr;
	long chnum = strtol(argv[1], &extstr, 0);

	pthread_t* id = (pthread_t*)calloc(chnum, sizeof(pthread_t));

	for(int i = 0; i < chnum; i++){
		if(!pthread_create(id+i, NULL , counter, &n)){
			
		}

		else{
			printf("Failure to start a thread number %d\n", i);
		}
	}

	for(int i = 0; i < chnum; i++){
		pthread_join(*(id+i), 0);
	}

	printf("%d\n", n );
		return 0;
}